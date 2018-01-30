#ifndef MENU_H
#define MENU_H

#include "gui.hpp"

#define MAX_MENUITEMS 18

class Menu{

	sf::RectangleShape bg, select, hover;
	int last, selected, oneHovered;
	
	RenderStates states;
	public:
	VertexArray items;
	sf::Sprite button[MAX_MENUITEMS];
	Menu(int def);
	void draw();
	int clicked();
	int hovered();
	void goToPage(int page);
	void setVertexPos(sf::Vertex *v, int x, int y, int w);
	void setVertexRect(sf::Vertex *v, int x, int y, int w);
};

extern Menu *menu;

#endif