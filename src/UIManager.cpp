//
// Created by Ben Schattinger on 8/5/17.
//
#include <Arduino.h>
#include "UIManager.h"

#define listFont u8g2_font_7x14_tr
#define listFontHeight 10
#define listEntries ((display->getDisplayHeight() - 16) / (listFontHeight + 2))

void UIManager::setTitle(String titleIn) {
	title = titleIn;
}

void UIManager::show() {
	int progressBarStart = 0, progressBarLength = 0;
	switch (uiType) {
		case UI_SELECTION: {
			float progress = (float) selectionNum / (float) (selectionCount - 1);
			float unitLength = (float) 1 / (float) selectionCount;
			progressBarLength = (display->getDisplayHeight() - 17) / 2.4;
			progressBarStart = progress * (display->getDisplayHeight() - 16 - progressBarLength);
			break;
		}
		case UI_MAIN:
			break;
	}
	display->clearBuffer();
	{
		//Serial.println("Page");
		display->setFont(u8g2_font_crox3h_tr);
		int width = display->getUTF8Width(title.c_str());
		display->setCursor((display->getDisplayWidth() - width) / 2, 16);
		display->setDrawColor(1);
		display->print(title);
		switch (uiType) {
			case UI_SELECTION: {
				display->setFont(listFont);
				for (int i = topItem; i < selectionCount && i < topItem + listEntries; i++) {
					int top = 16 + (listFontHeight + 2) * ((i - topItem) + 1) - 1;
					display->setDrawColor(1);
					if (i == selectionNum) {
						display->drawBox(0, top - 1 - listFontHeight, display->getDisplayWidth() - 3,
										 listFontHeight + 2);
						display->setDrawColor(0);
					}
					display->setCursor(0, top);
					display->print(cb(i));
				}
				/*display->setDrawColor(1);
				display->drawBox(display->getDisplayWidth() - 3, 16,
								 3, display->getDisplayHeight() - 16);
				display->setDrawColor(0);
				display->drawPixel(display->getDisplayWidth() - 3, 16);
				display->drawPixel(display->getDisplayWidth() - 1, 16);
				display->drawPixel(display->getDisplayWidth() - 3, display->getDisplayHeight() - 1);
				display->drawPixel(display->getDisplayWidth() - 1, display->getDisplayHeight() - 1);
				display->drawBox(display->getDisplayWidth() - 2, 17,
								 1, display->getDisplayHeight() - 18);*/
				display->setDrawColor(1);
				display->drawBox(display->getDisplayWidth() - 3, progressBarStart + 16,
								 3, progressBarLength);
				break;
			}
			case UI_MAIN:
				break;
		}
	}
	display->sendBuffer();
}

void UIManager::uiSelector(int numPos, String (*pFunction)(int)) {
	uiType = UI_SELECTION;
	topItem = 0;
	selectionCount = numPos;
	selectionNum = 0;
	cb = pFunction;
}

void UIManager::moveDown() {
	if (uiType == UI_SELECTION && selectionNum + 1 < selectionCount) {
		selectionNum++;
		if (topItem <= selectionNum - listEntries) topItem++;
	}
}

void UIManager::moveUp() {
	if (uiType == UI_SELECTION && selectionNum > 0) {
		selectionNum--;
		if (topItem > selectionNum) topItem--;
	}
}
