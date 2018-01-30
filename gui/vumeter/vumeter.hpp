#ifndef VUMETER_H
#define VUMETER_H

#include "../gui.hpp"


class VuMeter{
	sf::RectangleShape bar, topbar;
	sf::VertexArray quad, quad2;
	int x;
	sf::Text db, title;
	public:
	int value;
	float topValue;
	int topTimer;
	int saturationTimer;
	VuMeter(int x, int y, std::string title);
	void draw();
};

class MiniVuMeter{
	sf::RectangleShape bar;

	public:
	int value;
	float topValue;
	int topTimer;
	int saturationTimer;
	MiniVuMeter();
	MiniVuMeter(int x, int y);
	void draw();
	void setValue(int value);
	void forceValue(int value);
	void setPosition(int x, int y);
};


class StereoVuMeter{
	VuMeter vuLeft, vuRight;
	sf::RectangleShape dbBars[5];
	sf::Text dbValues[6];
	public:
	StereoVuMeter(int x, int y);
	void setValue(int left, int right);
	void draw();
};

#endif