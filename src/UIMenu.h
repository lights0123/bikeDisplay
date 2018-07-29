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



#pragma once

#include <Arduino.h>
#include <functional-vlpp.h>
#include "UIManager.h"
#include "ConfigurationManager.h"
//#include "freeRAM.h"

namespace ButtonManager {
	const int BUTTON_LONGPRESS = 800;
	enum class Button {
		noButton = -1,
		leftPin = 6,
		rightPin = 5,
		upPin = 0,
		downPin = 2,
		selectPin = 1
	};

	struct ButtonPress {
		Button button;
		unsigned long startTouch;
	};

	void init();

	Button getButtons();

	ButtonPress longPressHelper(Button button);
};

class Drawable {
public:
	virtual void enter(vl::Func<void()> exitCB) = 0;

	virtual void buttonEvent(ButtonManager::Button b) = 0;

protected:
	explicit Drawable(UIManager *ui) : ui(ui) {};

	UIManager *ui;

	vl::Func<void()> exit;
};

class LEDStripBriSetting : public Drawable {
public:
	explicit LEDStripBriSetting(UIManager *ui) : Drawable(ui), slider(
			UIManager::UISlider(ui, "LED Strip Brightness", ConfigurationManager::LEDStripBrightness)) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UIManager::UISlider slider;
};

class Preferences : public Drawable {
public:
	explicit Preferences(UIManager *ui) : Drawable(ui), Selector(
			UIManager::UISelector(ui, 2, [](int pos) -> String {
				switch (pos) {
					case 0:
						return "LED Strip";
					case 1:
						return ConfigurationManager::is24Hour ? "24 Hour" : "12 Hour";
					default:
						return "";
				}
			})), l(ui) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UIManager::UISelector Selector;
	enum class CurrentDisplay {
		MENU,
		LED_STRIP
	};
	CurrentDisplay currentDisplay = CurrentDisplay::MENU;
	LEDStripBriSetting l;
};

class Locations : public Drawable {
public:
	explicit Locations(UIManager *ui) : Drawable(ui), Selector(
			UIManager::UISelector(ui, ConfigurationManager::getLocationCount(), [](int pos) {
				return UIManager::UISelector::ListItem{ConfigurationManager::getLocation(pos).name, "5451mi"};
			})), l(ui) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UIManager::UISelector Selector;
	enum class CurrentDisplay {
		MENU,
		LED_STRIP
	};
	CurrentDisplay currentDisplay = CurrentDisplay::MENU;
	LEDStripBriSetting l;
};