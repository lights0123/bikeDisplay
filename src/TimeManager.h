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
#include <NeoTime.h>
#include "ConfigurationManager.h"
#include <TimeLib.h>
#include <Timezone.h>

namespace TimeManager {
	::TimeChangeRule timeDaylight = {"EDT", ::Second, ::Sun, ::Mar, 2, -240};
	::TimeChangeRule timeStandard = {"EST", ::First, ::Sun, ::Nov, 2, -300};
	::Timezone timezone(timeDaylight, timeStandard);

	void setTime(NeoGPS::time_t dt) {
		::setTime(dt.hours, dt.minutes, dt.seconds, dt.date, dt.month, dt.year);
		ConfigurationManager::hasTime = true;
	};

	int hours() {
		return ::hour(timezone.toLocal(::now()));
	}

	int hours12() {
		return ::hourFormat12(timezone.toLocal(::now()));
	}

	int minutes() {
		return ::minute(timezone.toLocal(::now()));
	}

	int seconds() {
		return ::second(timezone.toLocal(::now()));
	}

	bool isAM() {
		return ::isAM(timezone.toLocal(::now()));
	}

	bool isPM() {
		return ::isPM(timezone.toLocal(::now()));
	}

	int year() {
		return ::year(timezone.toLocal(::now()));
	}

	int month() {
		return ::month(timezone.toLocal(::now()));
	}

	int date() {
		return ::day(timezone.toLocal(::now()));
	}

	int day() {
		return ::weekday(timezone.toLocal(::now()));
	}

	String formatTime(time_t time) {
		if (ConfigurationManager::is24Hour) {
			return String(hours()) + String(":") + String(minutes());
		} else {
			return String(String(hours12()) + String(":") + String(minutes()) + String(isAM() ? "AM" : "PM"));
		}
	};

	String formatTime() {
		formatTime(timezone.toLocal(::now()));
	};
};

