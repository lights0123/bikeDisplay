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

#ifndef BIKEDISPLAY_EEPROMMANAGER_H
#define BIKEDISPLAY_EEPROMMANAGER_H

#include <Location.h>
struct location{
    NeoGPS::Location_t loc;
    char name[15];
};
class EEPROManager{
public:
    EEPROManager() = default;
    byte getEntries();
    bool isInitialized();
    unsigned int spaceLeft();
    unsigned int spaceAvailable();
    void initialize();
    location read(int loc);
    void write(int loc, location place);
    unsigned int push_back(location place);
};
#endif //BIKEDISPLAY_EEPROMMANAGER_H
