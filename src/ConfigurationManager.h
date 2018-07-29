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
#include <NeoTime.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <GPSfix.h>

namespace ConfigurationManager {

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
	// Runtime
	extern bool hasTime;
	extern int batteryLevel;
	extern Location currentNav;
	// Saved
	extern bool is24Hour;
	extern bool useDaylightSavings;
	extern uint8_t LEDStripBrightness;
	extern int timezoneOffset;

	void save();

	void load();

	static int getLocationCount() {
		return 4;
	}

	static Location getLocation(int loc) {
		// TODO: read from SD card
		const Location temp[] = {Location{NeoGPS::Location_t(404381311L, -38196229L), "Madrid"},
		                         Location{NeoGPS::Location_t(407127000L, -740059000L), "New y"},
		                         Location{NeoGPS::Location_t(404381311L, -38196229L), "Madrid"},
		                         Location{NeoGPS::Location_t(407127000L, -740059000L), "New Yor"}};
		return temp[loc];
	}

	static String formatAsTracklog(gps_fix fix) {
		// To be read by http://www.gpsvisualizer.com
		// type,year,month,day,hour,minute,seconds,latitude,longitude,altitude(ft),speed(mph),course
		String s = "T,";
		String c = ",";
		s += fix.valid.date ? (fix.dateTime.year + c + fix.dateTime.month + c + fix.dateTime.day) : ",,";
		s += ',';
		s += fix.valid.time ? (fix.dateTime.hours + c + fix.dateTime.minutes + c + fix.dateTime.seconds) : ",,";
		s += ',';
		s += fix.valid.location ? (String(fix.latitude(), 6) + c + String(fix.longitude(),6)) : ",";
		s += ',';
		s += fix.valid.altitude ? String(fix.altitude_ft()) : "";
		s += ',';
		s += fix.valid.speed ? String(fix.speed_mph()) : "";
		s += ',';
		s += fix.valid.heading ? String(fix.heading()) : "";
		return s;
	}
};

namespace TimeManager {
	extern ::TimeChangeRule timeDaylight;
	extern ::TimeChangeRule timeStandard;
	extern ::Timezone timezone;

	static void updateRules() {
		timeDaylight.offset =
				ConfigurationManager::timezoneOffset + (ConfigurationManager::useDaylightSavings ? 60 : 0);
		timeStandard.offset = ConfigurationManager::timezoneOffset;
		timezone.setRules(timeDaylight, timeStandard);
	}

	static void setTime(NeoGPS::time_t dt) {
		::setTime(dt.hours, dt.minutes, dt.seconds, dt.date, dt.month, dt.year);
		ConfigurationManager::hasTime = true;
	};

	static int hours() {
		return ::hour(timezone.toLocal(::now()));
	}

	static int hours12() {
		return ::hourFormat12(timezone.toLocal(::now()));
	}

	static int minutes() {
		return ::minute(timezone.toLocal(::now()));
	}

	static int seconds() {
		return ::second(timezone.toLocal(::now()));
	}

	static bool isAM() {
		return ::isAM(timezone.toLocal(::now()));
	}

	static bool isPM() {
		return ::isPM(timezone.toLocal(::now()));
	}

	static int year() {
		return ::year(timezone.toLocal(::now()));
	}

	static int month() {
		return ::month(timezone.toLocal(::now()));
	}

	static int date() {
		return ::day(timezone.toLocal(::now()));
	}

	static int day() {
		return ::weekday(timezone.toLocal(::now()));
	}

	static String formatTime(time_t time) {
		if (ConfigurationManager::is24Hour) {
			return String(hours()) + String(":") + String(minutes() < 10 ? "0" : "") + String(minutes());
		} else {
			return String(String(hours12()) + String(":") + String(minutes() < 10 ? "0" : "") + String(minutes()) +
			              String(isAM() ? " AM" : " PM"));
		}
	};

	static String formatTime() {
		return formatTime(timezone.toLocal(::now()));
	};
};