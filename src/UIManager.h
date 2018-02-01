//
// Created by Ben Schattinger on 8/5/17.
//

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
