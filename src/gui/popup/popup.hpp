#ifndef POPUP_H
#define POPUP_H

enum popups{
	POPUP_SAVED,
	POPUP_WORKING,
	POPUP_ABOUT,
	POPUP_REPLACE_INSTRUMENT,
	POPUP_FADE,
	POPUP_EFFECTS,
	POPUP_SEARCH,
	POPUP_QUITCONFIRM,
	POPUP_NEWFILE,
	POPUP_MIDIEXPORT,
	POPUP_SAVEFAILED,
	POPUP_REPLACERESULT,
	POPUP_DELETEINSTRUMENT,
	POPUP_TRANSPOSE,
	POPUP_SETNOTE,
	POPUP_STREAMEDEXPORT,
	POPUP_TEMPERAMENT,
	POPUP_INSERTROWS,
	POPUP_REMOVEROWS,
	POPUP_FILECORRUPTED,
	POPUP_OPENCONFIRM,
	POPUP_FIRSTSTART,
	POPUP_OPENFAILED,
	POPUP_WRONGVERSION
};


#include "../../midi/midi.h"

extern List* instrList;

class Popup{
	RectangleShape bg, titlebar, shadow;
	Text title;
	float delay;
	int x, y, moving, deltaX, deltaY;
	std::vector<std::vector<int>> savedState;

	public:
	int type;
	View view;
	int visible, w, h;
	vector<Button> buttons;
	vector<Sprite> sprites;
	vector<Texture> textures;
	vector<Text> texts;
	vector<List> lists;
	vector<Checkbox> checkboxes;
	vector<DataSlider> sliders;
	vector<RectangleShape> shapes;
	Popup();

	void show(int type, int param = 0);
	void draw();
	void handleEvents();
	void setSize(int w, int h);
	void move(int x, int y);
	void setMelodicList();
	void setPercussionList();
	void updateEffectDescription();
	void close(bool pressOK = false);
	void buttonActions(int buttonID);
	void updateIntervalDescription();
	void updateBitDepthDescription();
	void updateExportSliders();
	void updateWindow();
	//void show(string text, int delay);
};

extern Popup *popup;

#endif