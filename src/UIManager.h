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
#include <functional-vlpp.h>
#include <GPSfix.h>

enum UI_TYPES {
	UI_SELECTION,
	UI_MAIN
};

class UIManager {
public:

	class UIType {
	public:
		explicit UIType(UIManager *manager) : manager(manager), display(manager->display),
		                                      displayWidth(display->getDisplayWidth()),
		                                      displayHeight(display->getDisplayHeight()) {};

		virtual void show() {};
	protected:
		UIManager *manager;
		U8G2 *display;
		int displayWidth, displayHeight;
	};

	explicit UIManager(U8G2 *display) : display(display), displayWidth(display->getDisplayWidth()),
	                                    displayHeight(display->getDisplayHeight()) {}

	void setTitle(String titleIn);

	void setType(UIType *type);

	void show();

	class UISelector : public UIType {
	public:
		struct ListItem {
			String leftSide, rightSide = "";

			ListItem(String leftSide) : leftSide(leftSide) {};

			ListItem(String leftSide, String rightSide) : leftSide(leftSide), rightSide(rightSide) {};
		};

		/**
		 * Represents a selector that can be navigated
		 *
		 * @param manager A pointer to the UIManager
		 * @param positions Number of positions in the selector
		 * @param cb A callback that is given a zero-indexed integer that returns a String of the desired output
		 */
		UISelector(UIManager *manager, int positions, vl::Func<ListItem(int)> cb) : UIType(manager),
		                                                                            positions(positions), cb(cb) {};

		void show() override;

		void moveUp();

		void moveDown();

		int getPosition() {
			return currentPosition;
		}

	private:
		int positions;
		int currentPosition = 0;
		int topItem = 0;

		vl::Func<ListItem(int)> cb;
	};

	class UISlider : public UIType {
	public:
		UISlider(UIManager *manager, String name, int value) : UIType(manager),
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
		UISlider &onChange(vl::Func<void(int)> cbChangeNew);

		void show() override;

		void increase(int amount = 1);

		void decrease(int amount = 1);

	private:
		int min = 0;
		int max = 100;
		int value = 0;
		String name;
		String suffix = "%";

		vl::Func<void(int)> cbChange;

	};

private:
	U8G2 *display;
	UIType *currentDisplay;
	String title;
	int displayWidth, displayHeight;
};
