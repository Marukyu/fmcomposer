#ifndef OPERATOR_H
#define OPERATOR_H

#include "../../gui/gui.hpp"

#define OP_SIZE 24

class Operator{

	int dx, dy;
	sf::Vertex line[2], lineBot[2];

	public:
	sf::RectangleShape bg;
	sf::RectangleShape bgValue;
	int active, active2, hovered, highlighted, muted;
	unsigned id;
	int depth, isBottom;
	sf::Text number;
	sf::Vector2i position;
	Operator*bot[5], *top[5];
	Operator();
	int update();
	int connect(Operator* bot, int force = 0, int connectLink = 0);
	void draw();
	int hover(Operator* o);
	Operator* getTopmost(Operator* o);
	void cleanupBottomLinks();
	int hasParents();
	void updatePosition();
	void drawLines();
};

#endif