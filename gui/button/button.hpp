#ifndef BUTTON_H
#define BUTTON_H

#include "../gui.hpp"

class Button{
	int h, padding, width;
	int textYpadding;
	bool click;
	sf::VertexArray bg2;
	void updatePosition();
	public:
	bool hovered, selected;
	int x, y, w;
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

};

#endif