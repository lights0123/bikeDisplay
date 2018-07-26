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

//#define SERIAL_BUFFER_SIZE 1536
#include <Arduino.h>
#include <U8g2lib.h>
#include "UIManager.h"
#include "Keypad.h"
#include <NMEAGPS.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include "FastLED.h"
//#include "EEPROMManager.h"

NMEAGPS gps; // This parses the GPS characters
gps_fix fix; // This holds on to the latest values


CRGBPalette16 currentPalette;
TBlendType currentBlending = LINEARBLEND;
#define NUM_LEDS 20
#define NEOPIXEL_DATA_PIN 11
//CRGB leds[NUM_LEDS];
Adafruit_NeoPixel_ZeroDMA strip(NUM_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB);
#ifdef U8X8_HAVE_HW_SPI

#include <SPI.h>

#endif
#ifdef U8X8_HAVE_HW_I2C

#include <Wire.h>

#endif
#define gps_port Serial1
#define Serial SerialUSB
//#define CAPTOUCH
#define KEYPAD
#define CAPTOUCH_THRESHOLD 3
#define BUTTON_LONGPRESS 800
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
UIManager ui(&u8g2);
//EEPROManager e;
enum buttons {
	noButtons = -1,
	leftPin = A0,
	rightPin = A1,
	upPin = A2,
	downPin = 8,
	selectPin = 9
};
#ifdef KEYPAD
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
		{'1', '2', '3'},
		{'4', '5', '6'},
		{'7', '8', '9'},
		{'*', '0', '#'}
};
byte rowPins[ROWS] = {7, 8, 9, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
#endif
#if defined(CAPTOUCH) && defined(ARDUINO_ARCH_SAMD)
Adafruit_FreeTouch touchController[5];
int touchCal[5];
#endif
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeRam() {
	char top;
#ifdef __arm__
	return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

int freeSRAM;
unsigned long millStart = 0;
unsigned long millLast = 0;
unsigned long millEnd;
int t;

buttons getButtons();

uint8_t readCapacitivePin(int pinToMeasure);

float speedMPH = 0;

void setup() {
	Serial.begin(115200);
	gps_port.begin(115200);
	u8g2.begin();
	u8g2.setFontMode(0);
	//LEDS.addLeds<WS2812B, NEOPIXEL_DATA_PIN, GRB>(leds, NUM_LEDS);
	//LEDS.setBrightness(84);
	strip.begin();
	strip.setBrightness(84);
	CRGB yellow = CHSV(HUE_YELLOW, 255, 255);
	CRGB black = CRGB::Black;

	currentPalette = CRGBPalette16(
			yellow, yellow, black, black,
			yellow, yellow, black, black,
			yellow, yellow, black, black,
			yellow, yellow, black, black);
#ifndef CAPTOUCH
	pinMode(leftPin, INPUT_PULLUP);
	pinMode(rightPin, INPUT_PULLUP);
	pinMode(upPin, INPUT_PULLUP);
	pinMode(downPin, INPUT_PULLUP);
	pinMode(selectPin, INPUT_PULLUP);
#elif defined(ARDUINO_ARCH_SAMD)
	touchController[0]=Adafruit_FreeTouch(leftPin, OVERSAMPLE_4, RESISTOR_0, FREQ_MODE_NONE);
	touchController[1]=Adafruit_FreeTouch(rightPin, OVERSAMPLE_4, RESISTOR_0, FREQ_MODE_NONE);
	touchController[2]=Adafruit_FreeTouch(upPin, OVERSAMPLE_4, RESISTOR_0, FREQ_MODE_NONE);
	touchController[3]=Adafruit_FreeTouch(downPin, OVERSAMPLE_4, RESISTOR_0, FREQ_MODE_NONE);
	touchController[4]=Adafruit_FreeTouch(selectPin, OVERSAMPLE_4, RESISTOR_0, FREQ_MODE_NONE);
	if (!touchController[0].begin())
	{Serial.print("Failed to begin qt on pin ");Serial.println(leftPin);}
	if (!touchController[1].begin())
	{Serial.print("Failed to begin qt on pin ");Serial.println(rightPin);}
	if (!touchController[2].begin())
	{Serial.print("Failed to begin qt on pin ");Serial.println(upPin);}
	if (!touchController[3].begin())
	{Serial.print("Failed to begin qt on pin ");Serial.println(downPin);}
	if (!touchController[4].begin())
	{Serial.print("Failed to begin qt on pin ");Serial.println(selectPin);}
	for(int i = 0; i < 5; i++){
		touchCal[i] = touchController[i].measure()+50;
	}
#endif
	ui.setTitle("Locations");
	ui.uiSelector(5, [](int pos) {
		switch (pos) {
			case 0:
				return String(String("Speed: ") + String(speedMPH) + String(" MPH"));
			case 1:
				return String(String("Free RAM: ") + String(t));
			case 2:
				return String(String("millis: ") + String(millLast));
			case 3:
				return String("Filler");
			case 4:
				return String("Filler 2");
			default:
				return String("Error");
		}
	});
	ui.show();
}

unsigned long startTouch;
buttons lastButton;
bool wasTouched = false;

void loop() {
	//while(Serial.available()) gps_port.write(Serial.read());
	//while(gps_port.available()) Serial.write(gps_port.read());
	t = freeRam();
	millLast = millEnd - millStart;
	millStart = millis();
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
			ui.moveUp();
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
			ui.moveDown();
			ui.show();
			break;
		case selectPin:
			break;
		default:
			wasTouched = false;
			break;
	}
	//ui.show();
	while (gps.available(gps_port)) {
		fix = gps.read();
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
	}
	static uint8_t startIndex = 0;
	static unsigned long oldMil = millis();
	startIndex += (millis() - oldMil) / 12; /* motion speed */
	if ((millis() - oldMil) / 12 >= 1) {
		uint8_t colorIndex = startIndex;
		uint8_t brightness = 255;
		for (int i = 0; i < NUM_LEDS; i++) {
			CRGB color = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
			colorIndex += 3;
			//leds[i]=color;
			strip.setPixelColor(i, color.r, color.g, color.b);
		}
		oldMil = millis();
	}
	strip.show();
	millEnd = millis();
}

#ifdef ARDUINO_ARCH_AVR
uint8_t readCapacitivePin(int pinToMeasure) {
	// Variables used to translate from Arduino to AVR pin naming

	volatile uint8_t *port;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;

	// Here we translate the input pin number from
	//  Arduino pin number to the AVR PORT, PIN, DDR,
	//  and which bit of those registers we care about.

	byte bitmask;

	port = portOutputRegister(digitalPinToPort(pinToMeasure));
	ddr = portModeRegister(digitalPinToPort(pinToMeasure));
	bitmask = digitalPinToBitMask(pinToMeasure);
	pin = portInputRegister(digitalPinToPort(pinToMeasure));

	// Discharge the pin first by setting it low and output

	*port &= ~(bitmask);
	*ddr |= bitmask;
	delay(1);
	uint8_t SREG_old = SREG; //back up the AVR Status Register

	// Prevent the timer IRQ from disturbing our measurement

	noInterrupts();

	// Make the pin an input with the internal pull-up on

	*ddr &= ~(bitmask);
	*port |= bitmask;

	// Now see how long the pin to get pulled up. This manual unrolling of the loop
	// decreases the number of hardware cycles between each read of the pin,
	// thus increasing sensitivity.

	uint8_t cycles = 17;
	if (*pin & bitmask) { cycles = 0; }
	else if (*pin & bitmask) { cycles = 1; }
	else if (*pin & bitmask) { cycles = 2; }
	else if (*pin & bitmask) { cycles = 3; }
	else if (*pin & bitmask) { cycles = 4; }
	else if (*pin & bitmask) { cycles = 5; }
	else if (*pin & bitmask) { cycles = 6; }
	else if (*pin & bitmask) { cycles = 7; }
	else if (*pin & bitmask) { cycles = 8; }
	else if (*pin & bitmask) { cycles = 9; }
	else if (*pin & bitmask) { cycles = 10; }
	else if (*pin & bitmask) { cycles = 11; }
	else if (*pin & bitmask) { cycles = 12; }
	else if (*pin & bitmask) { cycles = 13; }
	else if (*pin & bitmask) { cycles = 14; }
	else if (*pin & bitmask) { cycles = 15; }
	else if (*pin & bitmask) { cycles = 16; }

	// End of timing-critical section; turn interrupts back on if they were on before, or leave them off if they were off before

	SREG = SREG_old;

	// Discharge the pin again by setting it low and output
	//  It's important to leave the pins low if you want to
	//  be able to touch more than 1 sensor at a time - if
	//  the sensor is left pulled high, when you touch
	//  two sensors, your body will transfer the charge between
	//  sensors.
	*port &= ~(bitmask);
	*ddr |= bitmask;

	return cycles;
}
#endif

buttons getButtons() {
#ifdef CAPTOUCH
#ifdef ARDUINO_ARCH_SAMD
	if (touchController[0].measure() > touchCal[0]) return leftPin;
	if (touchController[1].measure() > touchCal[1]) return rightPin;
	if (touchController[2].measure() > touchCal[2]) return upPin;
	if (touchController[3].measure() > touchCal[3]) return downPin;
	if (touchController[4].measure() > touchCal[4]) return selectPin;
	return noButtons;
#else
	if (readCapacitivePin(leftPin) > CAPTOUCH_THRESHOLD) return leftPin;
	if (readCapacitivePin(rightPin) > CAPTOUCH_THRESHOLD) return rightPin;
	if (readCapacitivePin(upPin) > CAPTOUCH_THRESHOLD) return upPin;
	if (readCapacitivePin(downPin) > CAPTOUCH_THRESHOLD) return downPin;
	if (readCapacitivePin(selectPin) > CAPTOUCH_THRESHOLD) return selectPin;
	return noButtons;
#endif
#elif defined(KEYPAD)
	keypad.getKeys();
	if (keypad.key[0].kstate == PRESSED || keypad.key[0].kstate == HOLD) {
		switch (keypad.key[0].kchar) {
			case '4':
				return leftPin;
			case '5':
				return selectPin;
			case '6':
				return rightPin;
			case '2':
				return upPin;
			case '8':
				return downPin;
			default:
				return noButtons;
		}
	}
#else
	if (digitalRead(leftPin) == LOW) return leftPin;
	if (digitalRead(rightPin) == LOW) return rightPin;
	if (digitalRead(upPin) == LOW) return upPin;
	if (digitalRead(downPin) == LOW) return downPin;
	if (digitalRead(selectPin) == LOW) return selectPin;
	return noButtons;
#endif
}