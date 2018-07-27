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

#include <Arduino.h>
#include "UIManager.h"

#define listFont u8g2_font_7x14_tr
#define listFontHeight 10
#define listEntries ((display->getDisplayHeight() - 16) / (listFontHeight + 2))

void UIManager::setTitle(String titleIn) {
	title = titleIn;
}

void UIManager::show() {
	display->clearBuffer();
	display->setFont(u8g2_font_crox3h_tr);
	int width = display->getUTF8Width(title.c_str());
	display->setCursor((display->getDisplayWidth() - width) / 2, 16);
	display->setDrawColor(1);
	display->print(title);
	currentDisplay->show();
	display->sendBuffer();
}

void UIManager::setType(UIManager::UIType *type) {
	currentDisplay = type;
}

void UIManager::UISelector::show() {
	int progressBarStart = 0, progressBarLength = 0;
	float progress = (float) currentPosition / (float) (positions - 1);
	float unitLength = (float) 1 / (float) positions;
	progressBarLength = (display->getDisplayHeight() - 17) / 2.4;
	progressBarStart = progress * (display->getDisplayHeight() - 16 - progressBarLength);

	display->setFont(listFont);
	for (int i = topItem; i < positions && i < topItem + listEntries; i++) {
		int top = 16 + (listFontHeight + 2) * ((i - topItem) + 1) - 1;
		display->setDrawColor(1);
		if (i == currentPosition) {
			display->drawBox(0, top - 1 - listFontHeight, display->getDisplayWidth() - 3,
			                 listFontHeight + 2);
			display->setDrawColor(0);
		}
		display->setCursor(0, top);
		display->print(cb(i));
	}
	display->setDrawColor(1);
	display->drawBox(display->getDisplayWidth() - 3, progressBarStart + 16,
	                 3, progressBarLength);
}

void UIManager::UISelector::moveUp() {
	if (currentPosition > 0) {
		currentPosition--;
		if (topItem > currentPosition) topItem--;
	}
}

void UIManager::UISelector::moveDown() {
	if (currentPosition + 1 < positions) {
		currentPosition++;
		if (topItem <= currentPosition - listEntries) topItem++;
	}
}

UIManager::UISlider &UIManager::UISlider::setBounds(int newMin, int newMax) {
	min = newMin;
	max = newMax;
	return *this;
}

UIManager::UISlider &UIManager::UISlider::setValue(int newValue) {
	value = newValue;
	return *this;
}

UIManager::UISlider &UIManager::UISlider::setName(String newName) {
	name = newName;
	return *this;
}


UIManager::UISlider &UIManager::UISlider::onChange(void (*cbChangeNew)(int)) {
	cbChange = cbChangeNew;
	return *this;
}

UIManager::UISlider &UIManager::UISlider::onSave(void (*cbSaveNew)(int)) {
	cbSave = cbSaveNew;
	return *this;
}

void UIManager::UISlider::show() {

}

void UIManager::UISlider::increase(int amount) {
	if (value + amount <= max) {
		value += amount;
		cbChange(value);
	}
}

void UIManager::UISlider::decrease(int amount) {
	if (value - amount >= min) {
		value -= amount;
		cbChange(value);
	}
}