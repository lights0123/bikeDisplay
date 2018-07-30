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


#include "Config.h"

int Config::batteryLevel = 100;
bool Config::hasTime = false;
Config::Location Config::currentNav{NeoGPS::Location_t(), ""};
gps_fix Config::fix;

uint8_t Config::LEDStripBrightness = 100;
bool Config::is24Hour = false;
bool Config::useDaylightSavings = true;
int Config::timezoneOffset = -300;

::TimeChangeRule Time::timeDaylight = {"EDT", ::Second, ::Sun, ::Mar, 2, Config::timezoneOffset +
                                                                                (Config::useDaylightSavings
                                                                                 ? 60 : 0)};
::TimeChangeRule Time::timeStandard = {"EST", ::First, ::Sun, ::Nov, 2, Config::timezoneOffset};
::Timezone Time::timezone(Time::timeDaylight, Time::timeStandard);


void Config::save() {

}

void Config::load() {

}
