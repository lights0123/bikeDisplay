/*
    BikeDisplay
    Copyright (C) 2018 Ben Schattinger

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include "UIManager.h"
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <FastLED.h>
#include "freeRAM.h"
#include "wiring_private.h"
#include "EffectManager.h"

#define NMEAGPS_INTERRUPT_PROCESSING

#include <NMEAGPS_cfg.h>

#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GST

#include <NMEAGPS.h>
//#include "EEPROMManager.h"

NMEAGPS gps; // This parses the GPS characters
gps_fix fix;
gps_fix fixStore;
volatile bool hasFix = false;

Uart GPSPort(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM2_Handler() {
	// If data is available for reading...
	if (sercom2.availableDataUART()) {
		// Read it and decode. If finished decoding, then...
		if (gps.decode(sercom2.readDataUART()) == NMEAGPS::DECODE_COMPLETED) {
			// Add the received sentence to the GPS data
			fixStore |= gps.fix();
			// If it's the last sentence in the block, then
			if (gps.nmeaMessage == LAST_SENTENCE_IN_INTERVAL) {
				// Copy the internal data to what's accessed externally
				fix = fixStore;
				// And reset the internal data
				fixStore.init();
				// Mark the data as ready
				hasFix = true;
			}
		}
	}
	GPSPort.IrqHandler();
}

CRGBPalette16 currentPalette;
TBlendType currentBlending = LINEARBLEND;
#define NUM_LEDS 20
#define NEOPIXEL_DATA_PIN 11
//CRGB leds[NUM_LEDS];
Adafruit_NeoPixel_ZeroDMA strip(NUM_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB);
EffectManager e(&strip, NUM_LEDS);
#ifdef U8X8_HAVE_HW_SPI

#include <SPI.h>

#endif
#ifdef U8X8_HAVE_HW_I2C

#include <Wire.h>

#endif

#define Serial SerialUSB
#define BUTTON_LONGPRESS 800
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
UIManager ui(&u8g2);
//EEPROManager e;
enum buttons {
	noButtons = -1,
	leftPin = 5,
	rightPin = 6,
	upPin = 0,
	downPin = 2,
	selectPin = 1
};

unsigned long millStart = 0;
unsigned long millLast = 0;
unsigned long millEnd;
int t;

buttons getButtons();

float speedMPH = 0;

unsigned long startTouch = 0;
buttons lastButton;
bool wasTouched = false;
UIManager::UISelector sel(&ui, 5, [](int pos) {
	switch (pos) {
		case 0:
			return String(String("Speed: ") + String(speedMPH) + String(" MPH"));
		case 1:
			return String(String("Free RAM: ") + String(t));
		case 2:
			return String(String("millis: ") + String(millLast));
		case 3:
			return String(millis() - startTouch);
		case 4:
			return String("Filler 2");
		default:
			return String("Error");
	}
});

void setup() {
	GPSPort.begin(115200);
	pinPeripheral(3, PIO_SERCOM_ALT);
	pinPeripheral(4, PIO_SERCOM_ALT);
	u8g2.begin();
	u8g2.setFontMode(0);
	//LEDS.addLeds<WS2812B, NEOPIXEL_DATA_PIN, GRB>(leds, NUM_LEDS);
	//LEDS.setBrightness(84);
	strip.begin();
	strip.setBrightness(84);
	e.setEffect(EffectManager::blinker);

	pinMode(leftPin, INPUT_PULLUP);
	pinMode(rightPin, INPUT_PULLUP);
	pinMode(upPin, INPUT_PULLUP);
	pinMode(downPin, INPUT_PULLUP);
	pinMode(selectPin, INPUT_PULLUP);

	ui.setTitle("Locations");
	ui.setType(&sel);
	ui.show();
}

void loop() {
//	while(Serial.available()) GPSPort.write(Serial.read());
//	while(GPSPort.available()) Serial.write(GPSPort.read());
	if (hasFix) {
		Serial.print(F("Location: "));
		if (fix.valid.location) {
			Serial.print(fix.latitude(), 6);
			Serial.print(',');
			Serial.print(fix.longitude(), 6);
		}
		Serial.print(F(", Altitude: "));
		if (fix.valid.altitude)
			Serial.print(fix.altitude());
		Serial.print(F(", Speed: "));
		if (fix.valid.speed) {
			speedMPH = fix.speed_mph();
			Serial.print(speedMPH);
		}
		Serial.println();
		hasFix = false;
	}
	millLast = millEnd - millStart;
	millStart = millis();
	t = freeRAM();
	switch (getButtons()) {
		case leftPin:
			break;
		case rightPin:
			break;
		case upPin:
			if (lastButton == upPin && wasTouched) {
				if (millis() - startTouch < BUTTON_LONGPRESS) break;
			} else {
				lastButton = upPin;
				wasTouched = true;
				startTouch = millis();
			}
			sel.moveUp();
			ui.show();
			break;
		case downPin:
			if (lastButton == downPin && wasTouched) {
				if (millis() - startTouch < BUTTON_LONGPRESS) break;
			} else {
				lastButton = downPin;
				wasTouched = true;
				startTouch = millis();
			}
			sel.moveDown();
			ui.show();
			break;
		case selectPin:
			break;
		default:
			wasTouched = false;
			break;
	}
	ui.show();
	e.show();
}

buttons getButtons() {
	if (digitalRead(leftPin) == LOW) return leftPin;
	if (digitalRead(rightPin) == LOW) return rightPin;
	if (digitalRead(upPin) == LOW) return upPin;
	if (digitalRead(downPin) == LOW) return downPin;
	if (digitalRead(selectPin) == LOW) return selectPin;
	return noButtons;
}