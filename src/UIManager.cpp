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
#include "ConfigurationManager.h"

#define titleHeight 16
#define UIFont u8g2_font_7x14_mr
#define UIFontHeight 10
#define listEntries ((display->getDisplayHeight() - titleHeight) / (UIFontHeight + 2))

void UIManager::setTitle(String titleIn) {
	title = titleIn;
}

void UIManager::show() {
	display->clearBuffer();
	display->setFont(u8g2_font_crox3h_tr);
	display->setDrawColor(1);
	display->setCursor(0, titleHeight);
	display->print(title);
	display->setCursor(displayWidth - display->getUTF8Width((String(ConfigurationManager::batteryLevel) + '%').c_str()),
	                   titleHeight);
	display->print(String(ConfigurationManager::batteryLevel) + '%');
	display->setFont(UIFont);
	currentDisplay->show();
	display->sendBuffer();
}

void UIManager::setType(UIManager::UIType *type) {
	currentDisplay = type;
}

void UIManager::UISelector::show() {
	const float progress = (float) currentPosition / (float) (positions - 1);
	const int progressBarLength = (displayHeight - 17) / 2.4;
	const int progressBarStart = progress * (displayHeight - titleHeight - progressBarLength);
	const int progressBarWidth = 3;

	auto drawPosition = [this](int position, bool isSelected = false) {
		const u8g2_uint_t top = titleHeight + (UIFontHeight + 2) * ((position - topItem) + 1);
		display->setDrawColor(1);
		if (isSelected) {
			display->drawBox(0, top - 2 - UIFontHeight, displayWidth - progressBarWidth, UIFontHeight + 4);
			display->setDrawColor(0);
		}
		display->setCursor(0, top);
		ListItem l = cb(position);
		display->print(l.leftSide);
		if (l.rightSide != "") {
			display->setCursor(displayWidth - progressBarWidth - display->getUTF8Width(l.rightSide.c_str()), top);
			display->print(l.rightSide);
		}
	};

	for (int i = topItem; i < positions && i < topItem + listEntries; i++) {
		if (i == currentPosition) continue;
		drawPosition(i);
	}
	drawPosition(currentPosition, true);

	display->setDrawColor(1);
	display->drawBox(displayWidth - progressBarWidth, progressBarStart + 16, progressBarWidth, progressBarLength);
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

UIManager::UISlider &UIManager::UISlider::setSuffix(String newSuffix) {
	suffix = newSuffix;
	return *this;
}


UIManager::UISlider &UIManager::UISlider::onChange(vl::Func<void(int)> cbChangeNew) {
	cbChange = cbChangeNew;
	return *this;
}

void UIManager::UISlider::show() {
	const double screenPercentage = 0.8;

	// Divide by 2, round, and multiply by 2 so that the output is an even number
	const int barWidth = round(displayWidth * screenPercentage / 2) * 2;
	const int barHeight = 6;
	const int barDistanceFromTop = displayHeight / 2 - barHeight / 2 + titleHeight / 2;
	const int barDistanceFromLeft = displayWidth / 2 - barWidth / 2;
	const int valueWidth = (value - min) / (double) (max - min) * barWidth;

	const int nameWidth = display->getUTF8Width(name.c_str());
	const int nameDistanceFromTop = barDistanceFromTop / 2 + titleHeight / 2 + UIFontHeight / 2;

	String valueString = String(value) + suffix;

	const int valueStringWidth = display->getUTF8Width(valueString.c_str());
	const int valueStringDistanceFromTop = (barDistanceFromTop + barHeight + UIFontHeight + displayHeight) / 2;

	display->setCursor((displayWidth - nameWidth) / 2, nameDistanceFromTop);
	display->print(name);

	display->drawFrame(barDistanceFromLeft, barDistanceFromTop, barWidth, barHeight);

	display->drawBox(barDistanceFromLeft, barDistanceFromTop, valueWidth, barHeight);

	display->setCursor((displayWidth - valueStringWidth) / 2, valueStringDistanceFromTop);
	display->print(valueString);
}

void UIManager::UISlider::increase(int amount) {
	int oldVal = value;
	value = min(max, value + amount);
	if (value != oldVal) cbChange(value);
}

void UIManager::UISlider::decrease(int amount) {
	int oldVal = value;
	value = max(min, value - amount);
	if (value != oldVal) cbChange(value);
}