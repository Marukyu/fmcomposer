#ifndef GENERALEDITOR_H
#define GENERALEDITOR_H


#include "../../gui/slider/dataslider.hpp"
#include "../../gui/textfield/textfield.hpp"

#include "../../fmengine/fmlib.h"
#include "../../state.hpp"

class GeneralEditor : public State{
	DataSlider tempo, globalVolume/*, surround[FM_chCount]*/, reverbLength, transpose;

	Text reverb, effects, echo, /*chsurround,*/ effectsvol /*chn, bitcrush*/, rows;
	DataSlider chDry[FM_ch], roomSize, damping, stereoWidth, bits, rate;

	DataSlider diviseur;
	Text diviseurText;
	public:
	TextInput songName, author, comments;
	GeneralEditor();
	void draw();
	void update();
	void updateFromFM();
	void handleEvents();
};

extern GeneralEditor* generalEditor;


#endif