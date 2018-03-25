#ifndef SONGEDITOR_H
#define SONGEDITOR_H



#include "../../fmengine/fmlib.h"
#include <vector>
#include "../../globalFunctions.hpp"
#include "../settings/configEditor.hpp"
#include <math.h>
#include <algorithm>    // std::move (ranges)
#include <utility> 
#include "../../gui/popup/popup.hpp"
#include "../../gui/vumeter/vumeter.hpp"
#include "../../state.hpp"
#include "../../gui/button/buttonlist.hpp"
#include "../../gui/channelHead.hpp"
#include "../../gui/contextmenu/contextmenu.hpp"
#include "patternSelection.hpp"

extern List *instrList;
extern int prevNote;

extern View borderView, patternTopView, patNumView;
extern void *focusedElement;

typedef struct historyElem{
	vector< vector< Cell >>  data;
	int y;
	unsigned char patternSize; 
};


typedef struct{
	int x1, x2, y1, y2, pattern, decale;
	vector< vector < unsigned int > > data;
}patternSelection;



/*
	1:paste
	2:cut
	3:transpose
	4:note set
	5:-
	6:-
	7:delete
	8:replace instrument
	9:volume fade
	10:{ // effect set
	11:{ // volume scale
	12:{ // set volume
*/
enum patternActions{
	PATTERN_PASTE=1,
	PATTERN_CUT=2,
	PATTERN_TRANSPOSE=3,
	PATTERN_SETNOTE=4,
	PATTERN_DELETE=7,
	PATTERN_SETINSTR=8,
	PATTERN_VOLFADE=9,
	PATTERN_SETEFFECT=10,
	PATTERN_VOLSCALE=11,
	PATTERN_SETVOL=12

};

class SongEditor : public State{
	bool selected;
	
	int clickedElem; /* Keep trace of which element is clicked, to detect double click on them */
	bool searched;
	int halfPatternHeightView;
	int patternHeightView;
	string s0,s1,s2,s3;
	Vector2i mousePattern;
	Vector2i patternViewBaseSize;
	
public:
	VertexArray bars;
	PatternSelection selection;
	float subrow;
	float zoom;
	ButtonList patternList;
	ChannelHead channelHead[FM_ch];
	Text text[FM_ch][4], rowNumbers, patText;
	RectangleShape playCursor;
	int selectedRow, selectedType, selectedChannel;
	Button add;
	Button resize, expand, shrink, resetMute;

	vector<vector<Cell>> copiedPattern;
	DataSlider patSize;

	patternSelection copiedSelection;
	patternSelection movedSelection;

	vector< vector < unsigned int > > movedData2;

	int movePat, mouseXpat, mouseYpat,mouseXpat2,mouseYpat2;
	Slider patSlider, patHSlider;
	Vector2f mousePat;
	
	ListMenu patMenu, patListMenu, editTools;
	
	vector< vector< historyElem>  > history;
	
	vector<int> currentHistoryPos;

	float scroll;
	int scrollX, scrollX2, scrollXsmooth;

	SongEditor();

	void draw();
	void updatePatternLines();
	void update();
	void updateChannelData(int channel);
	void updateFromFM();
	void patCopy(patternSelection* copiedData);
	void patPaste(patternSelection* copiedData, int channel, int ypos);
	void multipleEdit(int action,patternSelection* copiedData=NULL, int param=0,int param2=0, bool saveHistory=true);

	void updateMutedChannels();
	
	void recordFromKeyboard(int note, int volume, int channel,int isFromMidi);
	void moveY(int pos);
	void setX(int channel);
	void moveXrelative(int offset);
	void handleShortcuts();
	void updateRowCount();
	void leftMouseEvents();
	void rightMouseEvents();
	void leftMouseRelease();
	void wrappingValue();
	void selectionDisappear();
	void saveToHistory(int _y, int size);
	void udpatePatternFromHistory();
	void moveCursor(bool updateRecordChannels=true);
	void setXscroll(int value, bool updateSlider=true);
	void reset();

	void addHistory();
	void resetHistory();

	void handlePatternListContextMenu();
	void handlePatternContextMenu();
	int search(int searchWhat, unsigned char searchValues[5], int searchIn, int replaceWhat, unsigned char replaceValues[6]);
	void doubleClick();
	void buildContextMenus();

	void preSelectPopupEffect();

	void updateSelectedsFromCursor();
	bool isMouseHoverPattern();
	void updateMousePat();
	void updateRecordChannels();


	void pattern_copy();
	void pattern_paste(int insertAfter=0);
	void pattern_cut();
	void pattern_duplicate();
	void pattern_delete();
	void pattern_insert();
	void pattern_move(int from, int to);

	void pattern_insertrows(int count);
	void pattern_deleterows(int count);

	void resetView(int width, int height);
	void setScroll(float pos);
	void updateScrollbar();

	void handleChannelHeads();
	void moveCursorAfterDataEntered();
	void setZoom(float zoom);
	void handleEvents();

	void undo();
	void redo();
};

extern SongEditor *songEditor;

#endif