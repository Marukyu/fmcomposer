#ifndef LIST_H
#define LIST_H



#include "../gui.hpp"
#include "../scrollbar/scrollbar.hpp"

class List{

	
	public:
	int x, y, paddingleft, width;
	bool multiple;
	sf::RectangleShape s, bg, squarePing, s2;
	std::vector<sf::Text> text;
	std::vector<bool> selecteds;
	std::vector<sf::RectangleShape> selecteds_s;
	std::vector<int> pings;
	Slider scrollbar;
	int scroll, maxrows;
	int value, selected, hovered, pressed, prevScroll;
	List(int x, int y, int maxrows, int width = 200, int paddingleft = 4, bool multiple=false);
	void add(std::string elem, bool autoUpdate = 1);
	bool clicked(int mouseButton = 0);
	void draw();
	void unselectAll();
	void selectAll();
	void select(int index, bool updateScroll = true, bool hold=false);
	void remove(int index);
	void updateSize();
	void clear();
	int hover();
	void setMaxRows(int rows);
	void movedElement(int *a, int *b);
	void setScroll(int pos);
	bool rightClicked();
	bool selectionVisible();

	void updateView();
};

extern List *instrList;


#endif