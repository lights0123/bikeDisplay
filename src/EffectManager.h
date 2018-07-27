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
#include <FastLED.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>

#pragma once


class EffectManager {
public:
	EffectManager(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t numLEDs, uint16_t offset = 0) : leds(leds),
	                                                                                        numLEDs(numLEDs),
	                                                                                        offset(offset) {
		// Prevent calling an undefined pointer if setEffect is not called
		effectFunction = [](Adafruit_NeoPixel_ZeroDMA *, uint16_t, uint16_t) {};
	};

	void show();

	void setEffect(void (*effectFunction)(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t, uint16_t));

	// Effects
	static void blinker(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t numLEDs, uint16_t offset = 0);

	static void allColor(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t numLEDs, uint16_t offset = 0,
	                     uint32_t color = Adafruit_NeoPixel_ZeroDMA::Color(0, 0, 0));

private:
	Adafruit_NeoPixel_ZeroDMA *leds;
	uint16_t numLEDs;
	uint16_t offset;

	void (*effectFunction)(Adafruit_NeoPixel_ZeroDMA *, uint16_t, uint16_t);
};

