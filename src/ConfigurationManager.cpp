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


#include "ConfigurationManager.h"

int ConfigurationManager::batteryLevel = 100;
bool ConfigurationManager::hasTime = false;
ConfigurationManager::Location ConfigurationManager::currentNav{NeoGPS::Location_t(), ""};

uint8_t ConfigurationManager::LEDStripBrightness = 100;
bool ConfigurationManager::is24Hour = false;
bool ConfigurationManager::useDaylightSavings = true;
int ConfigurationManager::timezoneOffset = -300;

::TimeChangeRule TimeManager::timeDaylight = {"EDT", ::Second, ::Sun, ::Mar, 2, ConfigurationManager::timezoneOffset +
                                                                                (ConfigurationManager::useDaylightSavings
                                                                                 ? 60 : 0)};
::TimeChangeRule TimeManager::timeStandard = {"EST", ::First, ::Sun, ::Nov, 2, ConfigurationManager::timezoneOffset};
::Timezone TimeManager::timezone(TimeManager::timeDaylight, TimeManager::timeStandard);


void ConfigurationManager::save() {

}

void ConfigurationManager::load() {

}
