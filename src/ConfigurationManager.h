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

#pragma once

#include <Arduino.h>
#include <Location.h>

namespace ConfigurationManager {
	// Runtime
	static bool hasTime = false;
	// Saved
	static bool is24Hour = false;
	extern uint8_t LEDStripBrightness;

	void save();

	void load();

	struct Location {
		NeoGPS::Location_t location;
		char name[19];

		float milesTo(NeoGPS::Location_t otherLocation) {
			return location.DistanceMiles(otherLocation);
		}

		float kilometersTo(NeoGPS::Location_t otherLocation) {
			return location.DistanceKm(otherLocation);
		}
	};
};
