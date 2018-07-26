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

#ifdef ARDUINO_ARCH_AVR
#include <Arduino.h>
#include <EEPROM.h>
#include "EEPROMManager.h"

byte EEPROManager::getEntries() {
    return EEPROM.read(1);
}

bool EEPROManager::isInitialized() {
    if (getEntries() > EEPROM.length() / sizeof(location)) return false;
    return EEPROM.read(0) != 123;
}

void EEPROManager::initialize() {
    EEPROM.write(0, 123);
    EEPROM.write(1, 0);
}

location EEPROManager::read(int loc) {
    location ret;
    EEPROM.get(2 + loc * sizeof(location), ret);
    return ret;
}

void EEPROManager::write(int loc, location place) {
    EEPROM.put(2+loc*sizeof(location), place);
}

unsigned int EEPROManager::push_back(location place) {
    unsigned int space = spaceLeft();
    if (space) {
        write(getEntries(),place);
        EEPROM.write(1,getEntries()+1);
        return space;
    }
    return 0;
}

unsigned int EEPROManager::spaceLeft() {
    return spaceAvailable() - getEntries() / sizeof(location);
}

unsigned int EEPROManager::spaceAvailable() {
    return (EEPROM.length() - 2) / sizeof(location);
}
#endif