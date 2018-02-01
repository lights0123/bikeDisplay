//
// Created by Ben Schattinger on 8/6/17.
//
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