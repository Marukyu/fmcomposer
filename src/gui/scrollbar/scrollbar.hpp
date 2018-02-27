#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "../gui.hpp"

class Slider
{
	private:
	float vmax, vmin, delta, step;
	
	bool active, mini, horizontal;

	public:
	int x, y;
	sf::RectangleShape bar;
	sf::RectangleShape cursor;
	int size;
	bool upd_cursor;
	Slider(float def, float max, float min, float step, int x, int y, int height, bool mini = false, bool horizontal = false);
	bool update();
	void setValue(float value);
	void draw();
	float value;
	void setSize(int height);
	void setScrollableContent(int content, int bounds);
	void setPosition(int x, int y);
};

#endif