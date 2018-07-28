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

#include <U8g2lib.h>

enum UI_TYPES {
	UI_SELECTION,
	UI_MAIN
};

class UIManager {
public:

	class UIType {
	public:
		UIType() = default;

		virtual void prepareShow() {};

		virtual void show() {};
	};

	explicit UIManager(U8G2 *display) : display(display) {}

	void setTitle(String titleIn);

	void setType(UIType *type);

	void show();

	class UISelector : public UIType {
	public:
		UISelector(UIManager *manager, int positions, String (*cb)(int)) : manager(manager), display(manager->display),
		                                                                   positions(positions), cb(cb) {};

		void show() override;

		void moveUp();

		void moveDown();

	private:
		UIManager *manager;
		U8G2 *display;
		int positions;
		int currentPosition = 0;
		int topItem = 0;

		String (*cb)(int);
	};

	class UISlider : public UIType {
	public:
		UISlider(UIManager *manager, String name, int value) : manager(manager), display(manager->display),
		                                                       value(value), name(name), cbChange([](int) {}) {};

		/**
		 * Sets the boundaries for the slider.
		 * @param newMin New minimum to set
		 * @param newMax New maximum to set
		 * @return itself, for a fluent-style interface
		 */
		UISlider &setBounds(int newMin, int newMax);

		UISlider &setValue(int newValue);

		UISlider &setName(String newName);

		UISlider &setSuffix(String newSuffix);

		/**
		 * Sets the callback for when the value is changed in real-time
		 * @param cbChangeNew New callback that accepts an integer
		 * @return itself, for a fluent-style interface
		 */
		UISlider &onChange(void (*cbChangeNew)(int));

		void show() override;

		void increase(int amount = 1);

		void decrease(int amount = 1);

	private:
		UIManager *manager;
		U8G2 *display;
		int min = 0;
		int max = 100;
		int value = 0;
		String name;
		String suffix = "%";

		void (*cbChange)(int);

	};

private:
	U8G2 *display;
	UIType *currentDisplay;
	String title;
};
