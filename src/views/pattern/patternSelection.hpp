#ifndef SONGEDITORSELECTION_H
#define SONGEDITORSELECTION_H

#include "../../globalFunctions.hpp"




class PatternSelection{

	public:
	RectangleShape bg, moveRect;
	int deltaX, deltaY;
	bool moving;
	PatternSelection();
	void draw();
	bool isHover(int x, int y);
	void getBounds(int *x1, int *x2, int *y1, int *y2);
	void resizeRelative(int x, int y);
	void resizeAbsolute(int xOrigin, int yOrigin, int x, int y);
	void revertIfInverted();
	int getChannel();
	int getRow();
	int getType();
	int getMovedChannel();
	int getMovedRow();
	int getMovedType();
	bool isSingle();

};

#endif