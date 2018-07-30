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


#include "UIMenu.h"

void ButtonManager::init() {
	pinMode((uint32_t) Button::leftPin, INPUT_PULLUP);
	pinMode((uint32_t) Button::rightPin, INPUT_PULLUP);
	pinMode((uint32_t) Button::upPin, INPUT_PULLUP);
	pinMode((uint32_t) Button::downPin, INPUT_PULLUP);
	pinMode((uint32_t) Button::selectPin, INPUT_PULLUP);
}

ButtonManager::Button ButtonManager::getButtons() {
	if (digitalRead((uint32_t) Button::leftPin) == LOW) return Button::leftPin;
	if (digitalRead((uint32_t) Button::rightPin) == LOW) return Button::rightPin;
	if (digitalRead((uint32_t) Button::upPin) == LOW) return Button::upPin;
	if (digitalRead((uint32_t) Button::downPin) == LOW) return Button::downPin;
	if (digitalRead((uint32_t) Button::selectPin) == LOW) return Button::selectPin;
	return Button::noButton;
}

ButtonManager::ButtonPress ButtonManager::longPressHelper(Button button) {
	static Button lastButton;
	static unsigned long startTouch = 0;
	ButtonPress b = {Button::noButton, startTouch};

	if (button == Button::noButton) {
		lastButton = Button::noButton;
		return b;
	}
	if (lastButton == button) {
		if (millis() - startTouch < BUTTON_LONGPRESS) return b;
	} else {
		lastButton = button;
		startTouch = millis();
		b.startTouch = startTouch;
	}
	b.button = button;
	return b;
}

void LEDStripBriSetting::enter(vl::Func<void()> exitCB) {
	exit = exitCB;
	slider
			.setValue(Config::LEDStripBrightness)
			.setBounds(0, 255)
			.setSuffix("")
			.onChange([](int val) {
				Config::LEDStripBrightness = val;
			});
	ui->setType(&slider);
}

void LEDStripBriSetting::buttonEvent(ButtonManager::Button b) {
	using namespace ButtonManager;

	ButtonPress buttonPress = ButtonManager::longPressHelper(b);
	Button button = buttonPress.button;
	unsigned long changeFreq = ceil((millis() - buttonPress.startTouch + 1) / 1000.0);

	if (button == Button::noButton) return;
	if (button == Button::rightPin || button == Button::upPin) slider.increase(changeFreq);
	else if (button == Button::leftPin || button == Button::downPin) slider.decrease(changeFreq);
	else {
		exit();
		return;
	}
}

void Preferences::enter(vl::Func<void()> exitCB) {
	exit = exitCB;
	ui->setType(&Selector);
}

void Preferences::buttonEvent(ButtonManager::Button b) {
	using namespace ButtonManager;

	switch (currentDisplay) {
		case CurrentDisplay::LED_STRIP:
			return l.buttonEvent(b);
		default:
			break;
	}
	Button button = ButtonManager::longPressHelper(b).button;

	if (button == Button::noButton) return;
	if (button == Button::upPin) Selector.moveUp();
	else if (button == Button::downPin) Selector.moveDown();
	else if (button == Button::selectPin) {
		switch (Selector.getPosition()) {
			case 0:
				currentDisplay = CurrentDisplay::LED_STRIP;
				l.enter([this]() {
					currentDisplay = CurrentDisplay::MENU;
					ui->setType(&Selector);
				});
				break;
			case 1:
				Config::is24Hour = !Config::is24Hour;
				break;
			default:
				break;
		}
	} else {
		exit();
		return;
	}
}

void Locations::enter(vl::Func<void()> exitCB) {
	exit = exitCB;
	ui->setType(&Selector);
}

void Locations::buttonEvent(ButtonManager::Button b) {
	using namespace ButtonManager;

	Button button = ButtonManager::longPressHelper(b).button;

	if (button == Button::noButton) return;
	if (button == Button::upPin) Selector.moveUp();
	else if (button == Button::downPin) Selector.moveDown();
	else if (button == Button::selectPin) {
		exit();
		return;
	}
}
