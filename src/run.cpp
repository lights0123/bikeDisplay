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

#define NMEAGPS_INTERRUPT_PROCESSING

#include <NMEAGPS_cfg.h>

#undef LAST_SENTENCE_IN_INTERVAL
#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GST

#include <NMEAGPS.h>
#include "UI.h"
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
#include "wiring_private.h"
#include "LEDController.h"
#include "Config.h"
#include "UIMenu.h"

NMEAGPS gps;
volatile bool hasFix = false;

Uart GPSPort(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM2_Handler() {
	// If data is available for reading...
	if (sercom2.availableDataUART()) {
		// Read it and decode. If finished decoding a sentence, then...
		if (gps.decode(sercom2.readDataUART()) == NMEAGPS::DECODE_COMPLETED) {
			// Add the received sentence to the GPS data
			static gps_fix fixStore;
			fixStore |= gps.fix();
			// If it's the last sentence in the block, then
			if (gps.nmeaMessage == LAST_SENTENCE_IN_INTERVAL) {
				using Config::fix;
				// Copy the internal data to what's accessed externally
				fix = fixStore;
				// If the time & date are correct from the GPS, update them
				// (fix.dateTime_cs is used to make sure that the clock is only updated once per second).
				if (fix.valid.time && fix.valid.date && fix.dateTime_cs == 0) Time::setTime(fix.dateTime);
				// And reset the internal data
				fixStore.init();
				// Mark the data as ready
				hasFix = true;
			}
		}
	}
	GPSPort.IrqHandler();
}

#define NUM_LEDS 40
#define NEOPIXEL_DATA_PIN 11
//CRGB leds[NUM_LEDS];
Adafruit_NeoPixel_ZeroDMA strip(NUM_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB);
LEDController leftBlinker(&strip, 20);
LEDController rightBlinker(&strip, 20,20);

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
UI ui(&u8g2);

MainScreen l(&ui);

void setup() {
	GPSPort.begin(115200);
	pinPeripheral(3, PIO_SERCOM_ALT);
	pinPeripheral(4, PIO_SERCOM_ALT);
	u8g2.begin();
	u8g2.setFontMode(0);
	//LEDS.addLeds<WS2812B, NEOPIXEL_DATA_PIN, GRB>(leds, NUM_LEDS);
	//LEDS.setBrightness(84);
	strip.begin();
	strip.setBrightness(Config::LEDStripBrightness);
	rightBlinker.setEffect(LEDController::blinker);
	leftBlinker.setEffect(LEDController::rainbow);
	ButtonManager::init();

	ui.setTitle("Locations");
	l.enter([](){});
	ui.show();
}

void loop() {
//	while(SerialUSB.available()) GPSPort.write(SerialUSB.read());
//	while(GPSPort.available()) SerialUSB.write(GPSPort.read());
	if (hasFix) {
		using namespace Config;
		SerialUSB.print(F("Location: "));
		if (fix.valid.location) {
			SerialUSB.print(fix.latitude(), 6);
			SerialUSB.print(',');
			SerialUSB.print(fix.longitude(), 6);
		}
		SerialUSB.print(F(", Altitude: "));
		if (fix.valid.altitude)
			SerialUSB.print(fix.altitude());
		SerialUSB.print(F(", Speed: "));
		if (fix.valid.speed) SerialUSB.print(fix.speed_mph());
		SerialUSB.print(F(", Heading: "));
		if (fix.valid.heading) SerialUSB.print(fix.heading());
		SerialUSB.println();
		SerialUSB.println(Config::formatAsTracklog(fix));
		hasFix = false;
	}
	ui.show();
	leftBlinker.show();
	rightBlinker.show();
	if (Config::hasTime) ui.setTitle(Time::formatTime());
	strip.setBrightness(Config::LEDStripBrightness);
	l.buttonEvent(ButtonManager::getButtons());
}