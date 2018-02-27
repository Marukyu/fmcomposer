#ifndef CHANNELHEAD_H
#define CHANNELHEAD_H

#include "gui.hpp"
#include "button/button.hpp"
#include "slider/dataslider.hpp"
#include "vumeter/vumeter.hpp"

class ChannelHead{

	Text channelName;


	sf::RectangleShape channelSelector;
	int channelIndex;
	public:
	DataSlider pan, vol, rev;
	MiniVuMeter vu;
	Button mute, solo, record;
	bool selected, pressed;
	sf::RectangleShape bg;
	ChannelHead();
	ChannelHead(int channelIndex);
	void draw();
	void update(unsigned *paramChanged, int *mutedChanged, int *channelSwapped);
	void updateFromFM();
	bool hover();
	void updateZoom();
};

#endif