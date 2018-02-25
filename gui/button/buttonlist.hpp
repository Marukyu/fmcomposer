#ifndef BUTTONLIST_H
#define BUTTONLIST_H

#include "../gui.hpp"
#include "button.hpp"


class ButtonList{
	vector<Button> buttons;
	int maxId;
	RectangleShape bgfocus;
	int selectedIndex;
	public:
	int scroll, x, y, selected;
	ButtonList(int x, int y);
	void draw();
	void select(int index);
	void add(string text);
	void erase(int index);
	void move(int indexA, int indexB);
	void clear();
	int isElementHovered(int index);
	int getElementHovered();
	unsigned elementCount();
	bool clicked();
	void setX(int x);
	void setText(std::string text);
	bool hover();
	void setPosition(int x, int y);
	void insert(int index, string text);
	void update();
	void selectAll();
	void updateButtonPos();

};

#endif