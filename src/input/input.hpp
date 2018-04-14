#ifndef INPUT_H
#define INPUT_H
#include <SFML/Graphics.hpp>

typedef struct keyboard_{
	int up, del, down, shift, ctrl, left, right, plus, minus, c, v, x, pageUp, pageDown, equal, multiply, divide, a;
}keyboard_;

typedef struct mouse_{
	sf::Vector2i pos;
	int scroll, hscroll;
	int leftLeft, leftTop, leftRight, leftDown;
	int clickg, clickgReleased, clickdReleased, clickd, clickdLock, clickgLock, clickLock2;
	float dblClick;
	int cursor;
}mouse_;

extern keyboard_ keyboard;
extern mouse_ mouse;

extern sf::Vector2i mouseSidebar, mouseGlobal;

extern int textEnteredCount;
void input_update();

void handleNotePreview(int canPreview);

void handleUnconditionalEvents();
sf::Vector2i input_getmouse(sf::View &view);

#endif