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

#ifndef BIKEDISPLAY_UIMANAGER_H
#define BIKEDISPLAY_UIMANAGER_H

#include <U8g2lib.h>

enum UI_TYPES{
    UI_SELECTION,
    UI_MAIN
};
class UIManager{
public:
    explicit UIManager(U8G2 *dispIn){display=dispIn;}
    void setTitle(String titleIn);

    void show();

    void uiSelector(int numPos, String (*pFunction)(int));

    void moveUp();

    void moveDown();

private:
    U8G2 *display;
    String title;
    int selectionCount;
    String (*cb)(int);
    int selectionNum;
    int topItem;
    UI_TYPES uiType = UI_MAIN;
};
#endif //BIKEDISPLAY_UIMANAGER_H
