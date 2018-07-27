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

#include "EffectManager.h"

void EffectManager::show() {
	effectFunction(leds, numLEDs, 0);
	leds->show();
}

void EffectManager::setEffect(void (*effectFunctionIn)(Adafruit_NeoPixel_ZeroDMA *, uint16_t, uint16_t)) {
	effectFunction = effectFunctionIn;
}

void EffectManager::blinker(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t numLEDs, uint16_t offset) {
	// Number of flashes (on and off) per minute
	const int desiredFrequency = 80;
	const uint32_t yellow = Adafruit_NeoPixel_ZeroDMA::Color(255, 255, 0);
	const int cycleTime = 1000 * 60 / desiredFrequency;
	if (millis() % cycleTime > cycleTime / 2) allColor(leds, numLEDs, offset, yellow);
	else allColor(leds, numLEDs);
}

void EffectManager::allColor(Adafruit_NeoPixel_ZeroDMA *leds, uint16_t numLEDs, uint16_t offset, uint32_t color) {
	for (uint16_t i = offset; i < numLEDs; i++) leds->setPixelColor(i, color);
}