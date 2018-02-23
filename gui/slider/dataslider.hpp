#ifndef DATASLIDER_H
#define DATASLIDER_H

#include "../gui.hpp"

class DataSlider : public GuiElement{
	int vmin, type, number, updated;
	

	bool selected;
	public:
	sf::RectangleShape bg, bgValue;
	bool focused;
	int x, y, hovered;
	int width;
	sf::Text name;
	sf::Text tvalue;
	int value, vmax;
	DataSlider() {};
	DataSlider(int x, int y, int max, int min, std::string name, int def, int width = 200, int number = 1);
	void draw();
	bool update();
	bool updateOnRelease();
	void setValue(int value);
	void setDisplayedValueOnly(std::string value);
	void setMinMax(int min, int max);
	void setPosition(int x, int y);
	void setSize(int width);
};

extern DataSlider* copiedSlider, pastedSlider;

#endif