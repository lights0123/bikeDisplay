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
#include "UI.h"
#include "Config.h"
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
	explicit Drawable(UI *ui) : ui(ui) {};

	UI *ui;

	vl::Func<void()> exit;
};

class LEDStripBriSetting : public Drawable {
public:
	explicit LEDStripBriSetting(UI *ui) : Drawable(ui), slider(
			UI::UISlider(ui, "Strip Brightness", Config::LEDStripBrightness)) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UI::UISlider slider;
};

class Preferences : public Drawable {
public:
	explicit Preferences(UI *ui) : Drawable(ui), Selector(
			UI::UISelector(ui, 3, [](int pos) -> UI::UISelector::ListItem {
				switch (pos) {
					case 0:
						return UI::UISelector::ListReturn();
					case 1:
						return UI::UISelector::ListItem("Strip Brightness");
					case 2:
						return UI::UISelector::ListItem(Config::is24Hour ? "24 Hour" : "12 Hour");
					default:
						return UI::UISelector::ListItem("");
				}
			})), l(ui) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UI::UISelector Selector;
	enum class CurrentDisplay {
		MENU,
		LED_STRIP
	};
	CurrentDisplay currentDisplay = CurrentDisplay::MENU;
	LEDStripBriSetting l;
};

class Locations : public Drawable {
public:
	explicit Locations(UI *ui) : Drawable(ui), Selector(
			UI::UISelector(ui, Config::getLocationCount() + 1, getLocation)) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UI::UISelector Selector;

	static UI::UISelector::ListItem getLocation(int pos) {
		if (pos == 0)return UI::UISelector::ListReturn();
		if (Config::currentNav) {
			if (pos == 1) return UI::UISelector::ListItem("\xD7 Cancel Nav");
			pos--;
		}
		pos--;
		const auto loc = Config::getLocation(pos);
		const bool isSelected = loc == Config::currentNav;
		return UI::UISelector::ListItem(String(isSelected ? "\xBB" : "") + loc.name,
		                                Config::fix.valid.location ? loc.distanceString(Config::fix.location) : "");
	}
};

class MainScreen : public Drawable {
public:
	explicit MainScreen(UI *ui) : Drawable(ui), screen(ui), locationScreen(ui), preferencesScreen(ui) {};

	void enter(vl::Func<void()> exitCB) override;

	void buttonEvent(ButtonManager::Button b) override;

private:
	UI::UIMain screen;
	Locations locationScreen;
	Preferences preferencesScreen;
	enum class CurrentDisplay {
		MAIN,
		LOCATIONS,
		PREFERENCES
	};
	CurrentDisplay currentDisplay = CurrentDisplay::MAIN;
};