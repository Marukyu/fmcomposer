#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include "../gui.hpp"

class TextInput{
	
	int x, y, vmax, lineMax, currentLine, selectionBegin, selectionCount, pos2, sIndex, selected, mouseLock;
	
	std::vector<int> charCountLine;
	void recalcCursorPos();
	void cutSelection();
	int getCursorIndex(int &stringPosLastLine);
	void moveCursorX(int delta);
	int getSelectionStart();
	void newLine();
	void removeLine();
	void createNewLine();
	public:
	float cursorBlink;
	sf::RectangleShape bg, cursor, selection;
	sf::Text title;
	bool multiline, editing;
	sf::Text text;
	TextInput(int x, int y, int max, std::string name, bool multiline = false, int lineMax = 1, int width = -1);
	bool modified();
	void draw();
	bool hover();
	void setText(std::string text);
	void unselect();
};

#endif