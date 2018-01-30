#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "gui.hpp"
#include "slider/dataslider.hpp"
#include "vumeter/vumeter.hpp"

class Sidebar
{
	private:

	public:
	DataSlider octave, defNoteVol;
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