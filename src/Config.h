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

namespace Config {

	struct Location {
		NeoGPS::Location_t location;
		String name;

		float milesTo(NeoGPS::Location_t otherLocation) const {
			return location.DistanceMiles(otherLocation);
		}

		float feetTo(NeoGPS::Location_t otherLocation) const {
			return milesTo(otherLocation) * 5280;
		}

		float kilometersTo(NeoGPS::Location_t otherLocation) const {
			return location.DistanceKm(otherLocation);
		}

		float metersTo(NeoGPS::Location_t otherLocation) const {
			return kilometersTo(otherLocation) * 1000;
		}

		int degreesTo(NeoGPS::Location_t otherLocation) const {
			return location.BearingToDegrees(otherLocation);
		}

		String distanceString(NeoGPS::Location_t otherLocation) const {
			const float miles = milesTo(otherLocation);
			if (miles >= 10) return String(miles, 0) + " mi";
			if (miles >= 0.5) return String(miles, 1) + " mi";
			const int feet = miles * 5280;
			if (feet >= 500) return String(round(feet / 100) * 100) + " ft";
			if (feet >= 100) return String(round(feet / 50) * 50) + " ft";
			return String(round(feet / 10) * 10) + " ft";
		}

		explicit operator bool() const {
			return name != "";
		}

		bool operator==(Location otherLocation) const {
			return location.lat() == otherLocation.location.lat() && location.lon() == otherLocation.location.lon();
		}
	};

	// Runtime
	extern bool hasTime;
	extern int batteryLevel;
	extern Location currentNav;
	extern gps_fix fix;
	// Saved
	extern bool is24Hour;
	extern bool useDaylightSavings;
	extern uint8_t LEDStripBrightness;
	extern int timezoneOffset;

	void save();

	void load();

	static int getLocationCount() {
		// TODO: read from SD card
		return 4;
	}

	static Location getLocation(int loc) {
		// TODO: read from SD card
		const Location temp[] = {Location{NeoGPS::Location_t(407127000L, -740059000L), "New York City"},
		                         Location{NeoGPS::Location_t(404381311L, -38196229L), "Madrid"},
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
		s += fix.valid.location ? (String(fix.latitude(), 6) + c + String(fix.longitude(), 6)) : ",";
		s += ',';
		s += fix.valid.altitude ? String(fix.altitude_ft()) : "";
		s += ',';
		s += fix.valid.speed ? String(fix.speed_mph()) : "";
		s += ',';
		s += fix.valid.heading ? String(fix.heading()) : "";
		return s;
	}
};

namespace Time {
	extern ::TimeChangeRule timeDaylight;
	extern ::TimeChangeRule timeStandard;
	extern ::Timezone timezone;

	static void updateRules() {
		timeDaylight.offset =
				Config::timezoneOffset + (Config::useDaylightSavings ? 60 : 0);
		timeStandard.offset = Config::timezoneOffset;
		timezone.setRules(timeDaylight, timeStandard);
	}

	static void setTime(NeoGPS::time_t dt) {
		::setTime(dt.hours, dt.minutes, dt.seconds, dt.date, dt.month, dt.year);
		Config::hasTime = true;
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
		if (Config::is24Hour) {
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