#ifndef BUTTON_H
#define BUTTON_H

#include "../gui.hpp"

class Button{
	int padding, width;
	int textYpadding;
	bool click;
	
	void updatePosition();
	public:
	sf::VertexArray bg2;
	bool hovered, selected;
	int x, y, w, h;
	sf::Text text;
	Button();
	Button(int x, int y, std::string text, int width = -1, int padding = 0);
	Button(int x, int y, std::wstring text, int width = -1, int padding = 0,int fontSize=charSize);
	void draw();
	bool clicked();
	void setText(std::string text);
	void setText(std::wstring text);
	bool hover();
	void setPosition(int x, int y);
	void construct(int x, int y, int width = -1, int padding = 0);
	void setSelected(bool selected);
	void updateStyle();

};

#endif