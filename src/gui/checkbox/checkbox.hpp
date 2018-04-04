#ifndef CHECKBOX_H
#define CHECKBOX_H


#include "../gui.hpp"

class Checkbox{
	sf::RectangleShape bg;
	sf::Text title;
	sf::Sprite v;
	
	public:
	int checked;
	bool visible;
	Checkbox(int x, int y, std::string title);
	void draw();
	int clicked();
};

#endif