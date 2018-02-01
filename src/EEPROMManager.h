//
// Created by Ben Schattinger on 8/6/17.
//

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
