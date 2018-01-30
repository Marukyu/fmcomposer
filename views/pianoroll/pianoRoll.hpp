#ifndef PIANOROLL_H
#define PIANOROLL_H

#define PIANOROLL_MAX_NOTES 1024

#include "../../fmengine/fmlib.h"
#include "../../state.hpp"
#include "../../gui/contextmenu/contextmenu.hpp"
#include "../../gui/slider/dataslider.hpp"
#include "../../gui/button/button.hpp"

class Pianoroll : public State{
	int y;

	sf::VertexArray vertices;
	RectangleShape cursor, noteBg;
	sf::RenderStates states;
	sf::Color invisible;

	int instrumentToShow;
	
	Button showAllInstruments;
	ListMenu actionsMenu;
	Sprite piano;
	Sprite notePressed[8];
	unsigned char pressedNotes[128];
	RectangleShape cache, bracket;
	vector <RectangleShape> legend;
	Text marker, note, time;
	int subrow;
	int selectedRow, selectedOrder, selectedCh;
	bool isScrolling;
	int deltaX, deltaY,displayNote, moveNote, refNote, hoveredCh, hoveredRow, hoveredOrder, noteInstr, noteVol, oldNote, resizeNote, oldPos,noteMoveLimit;
	int refRow, refOrder, wasPlaying, clickedElem;
	bool saveHistory;
public:
	View view;
	void* callbackFunc;
	vector<Color> instrColors;
	DataSlider diviseur,rowHighlight,defaultPatternSize;
	Pianoroll(int y);
	void updateFromFM();
	void update();
	void draw();
	void drawMarkers(int order, int row);
	void doubleClick();
	void handleEvents();
	void resetView(int width, int height);
	int getPos(int order, int row);
	void handleContextMenu();
	void focusInstrument(int id);
	
};

extern Pianoroll* pianoRoll;


#endif