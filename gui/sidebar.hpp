#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "gui.hpp"
#include "slider/dataslider.hpp"
#include "vumeter/vumeter.hpp"

class Sidebar
{
	public:
	DataSlider octave, defNoteVol, editingStep;
	Text currentInstr;
	StereoVuMeter *vuMeter;
	RectangleShape borderRight;
	Text timer, notePreview;

	Sidebar();
	void update();
	void draw();
};

extern Sidebar *sidebar;

#endif