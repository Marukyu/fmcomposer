#ifndef INSTREDITOR_H
#define INSTREDITOR_H




#include "../../gui/operator/operator.hpp"
#include "../../gui/operator/opgui.hpp"
#include "../../fmengine/fmlib.h"
#include <vector>
#include "../../globalFunctions.hpp"
#include "../../gui/popup/popup.hpp"
#include "../../state.hpp"

extern char appDir[256];
extern List *instrList;
extern int prevNote;

extern View borderView;



class InstrEditor : public State{
	public:
	fm_instrument customPreset;
	DataSlider algo, feedback, feedbackSource, lfoSpeed, lfoA, lfoDelay, lfoWaveform, lfoOffset, volume, tuning, transpose, k_fx1, k_fx2;
	vector<OpGUI> op;
	Sprite imgalgo, waveform, connector;
	Button save, load, add, envReset, phaseReset, lfoReset, instrCleanup, temperament, smoothTransition;
	TextInput instrName;
	Checkbox transposable;
	RectangleShape adsr, lfoBG, lfoOffsetBar;
	Vertex ah[2], hd[2], ds[2], sr[2];
	fm_instrument copiedInstr;
	bool copied;
	Text lfo, newNote, kfx_text;
	ListMenu operatorMenu, instrumentMenu, editTools;
	char copiedEnvelope[7];
	int opSelected, opCopied, valueChanged;
	Operator outs[6], fmop[6];

	vector<vector <fm_instrument> > history;
	vector<int> currentHistoryPos;

	float zoom;

	InstrEditor(int y);
	void draw();
	void update();
	void updateFromFM();
	void updateToFM();
	void opEnvPaste();
	void opEnvCopy();
	void opPaste();
	void updateAlgoToFM();
	void updateAlgoFromFM();
	void rearrangeOps();
	void linkOps();
	void updateDepths(Operator* o);
	void updateInstrListFromFM();
	void leftMouseClickEvents();
	void addToUndoHistory();
	void loadAlgoPreset(int id, Operator* fmop, Operator* outs);
	void reset();
	void removeInstrument();
	void handleGlobalEvents();

	void resetView(int width, int height);

	void undo();
	void redo();

	void instrument_load(const char *filename);
	void instrument_open();
	void instrument_save();
	void cleanupInstruments();
	int loadInstrument(string filename, int slot);
	void setZoom(float zoom);
	void handleEvents();
};

extern InstrEditor* instrEditor;

#endif