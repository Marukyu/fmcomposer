#ifndef MENU_H
#define MENU_H

#include "gui.hpp"

#define MAX_MENUITEMS 18

class Menu{

	sf::RectangleShape select, hover;
	int last, selected;
	
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
	void setVertexRect(int vertexId, int x, int y, int w);
	void update();
	bool isPage(int buttonIndex);
};

extern Menu *menu;

#endif