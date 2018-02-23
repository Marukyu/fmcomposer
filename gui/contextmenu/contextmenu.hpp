#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "../gui.hpp"

class ListMenu{
	sf::RectangleShape s, bg, shadow;
	View *relative;
	int x, y, scroll;
	public:
	std::vector<sf::Text> text;
	int value;
	ListMenu();
	void add(std::string elem);
	void insert(std::string elem);
	void setElement(int index, std::string elem);
	int clicked();
	void draw();
	bool hover();
	void setPosition(int x, int y);
	void show(int x = mouseGlobal.x, int y = mouseGlobal.y);
	void show(View *relative);
	void remove(int index);
	void updateSize(int fromIndex);
	void update();
};

extern ListMenu* contextMenu;
extern ListMenu *recentSongs;
extern ListMenu *copyPasta;

#endif