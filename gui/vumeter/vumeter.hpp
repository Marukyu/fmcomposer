#ifndef VUMETER_H
#define VUMETER_H

#include "../gui.hpp"


class VuMeter{
public:
	sf::RectangleShape bar, topbar;
	sf::VertexArray quad, quad2;
	int x;
	sf::Text db, title;
	int value;
	float topValue;
	int topTimer;
	int saturationTimer;
	VuMeter(int x, int y, std::string title);
	void update();
	void draw();
};

class MiniVuMeter{
	sf::RectangleShape bar;

	public:
	int x, y, w, h;
	int value;
	float topValue;
	int topTimer;
	int saturationTimer;
	MiniVuMeter();
	MiniVuMeter(int x, int y);
	void draw();
	void update();
	void setValue(int value);
	void forceValue(int value);
	void setPosition(int x, int y);
};


class StereoVuMeter{
	public:
	VuMeter vuLeft, vuRight;
	sf::RectangleShape dbBars[5];
	sf::Text dbValues[6];
	
	StereoVuMeter(int x, int y);
	void setValue(int left, int right);
	void draw();
	void update();
};

#endif