#include "channelHead.hpp"
#include "../fmengine/fmlib.h"
#include "contextmenu/contextmenu.hpp"
#include "drawBatcher.hpp"

extern fmsynth *phanoo;

ChannelHead::ChannelHead() {};

ChannelHead::ChannelHead(int _channelIndex) : mute(CH_WIDTH*_channelIndex + 6, 4, "M", 13), solo(18 + CH_WIDTH*_channelIndex + 6, 4, "S", 13), vu(CH_WIDTH*_channelIndex + 1, 4),
pan(_channelIndex*CH_WIDTH + 6, 24, 255, 0, "Pan", 127, 94), vol(_channelIndex*CH_WIDTH + 6, 44, 99, 0, "Vol", 99, 94), rev(_channelIndex*CH_WIDTH + 6, 64, 99, 0, "Rev", 99, 94), channelSelector(Vector2f(100, 81)),
channelName(std::to_string(_channelIndex + 1), font, charSize), pressed(false), record(2 * 18 + CH_WIDTH*_channelIndex + 6, 4, "R", 13)
{
	channelIndex = _channelIndex;
	channelSelector.setOutlineColor(colors[FOCUSOUTLINE]);
	channelSelector.setOutlineThickness(1.f);
	channelSelector.setFillColor(colors[TOPBARBG]);
	channelSelector.setPosition(100 * channelIndex + 5, 3);

	channelName.setPosition(3 * 18 + 4 + 100 * channelIndex + 5, 4);
	channelName.setColor(colors[TITLE]);
	selected = 0;
}

void ChannelHead::draw()
{

	vu.update();

	
	if (selected)
		drawBatcher.addItem(&channelSelector);

	drawBatcher.addItem(&mute);

	


	drawBatcher.addItem(&pan);
	drawBatcher.addItem(&vol);
	drawBatcher.addItem(&rev);
	drawBatcher.addItem(&vu);

	drawBatcher.addItem(&mute);
	drawBatcher.addItem(&solo);
	drawBatcher.addItem(&record);

	drawBatcher.addItem(&channelName);
	//window->draw(channelName);

	
	
}

void ChannelHead::update(unsigned *paramChanged, int *mutedChanged, int *channelSwapped)
{
	if (mouse.clickg && mouseGlobal.x >= windowWidth - 215)
	{
		return;
	}

	

	if (pan.update())
	{
		fm_setChannelPanning(fm, channelIndex, pan.value);
		*paramChanged = 1;
	}
	if (vol.update())
	{
		fm_setChannelVolume(fm, channelIndex, vol.value);
		*paramChanged = 2;
	}
	if (rev.update())
	{
		fm_setChannelReverb(fm, channelIndex, rev.value);
		*paramChanged = 3;
	}
	if (mute.clicked())
	{

		mute.selected = !mute.selected;

		if (mute.selected)
			solo.selected = 0;

		*mutedChanged = 1;
	}
	if (solo.clicked())
	{

		solo.selected = !solo.selected;
		if (solo.selected)
			mute.selected = 0;

		*mutedChanged = 1;
	}
	if (record.clicked())
	{

		record.selected = !record.selected;

	}
	if (!contextMenu)
	{
		if (mouse.clickg)
		{
			if (hover() && *paramChanged == 0 && !mute.hover() && !solo.hover() && !record.hover())
			{
				selected = 1;
				pressed = 1;
			}
			else
			{
				selected = 0;
				pressed = 0;
			}
		}

		if (!hover() && pressed && Mouse::isButtonPressed(Mouse::Left))
		{
			mouse.cursor = CURSOR_SWAP;
		}
		if (mouse.clickgReleased && pressed)
		{
			pressed = 0;
			if (!hover() && *paramChanged == 0 && (mouse.pos.x < channelSelector.getPosition().x || mouse.pos.x > channelSelector.getPosition().x + channelSelector.getSize().x))
			{
				*channelSwapped = clamp(mouse.pos.x / CH_WIDTH, 0, FM_ch - 1);
			}
		}
	}

}

bool ChannelHead::hover()
{
	return (mouse.pos.x >= channelSelector.getPosition().x && mouse.pos.x < channelSelector.getPosition().x + channelSelector.getSize().x &&
		mouse.pos.y >= channelSelector.getPosition().y && mouse.pos.y < channelSelector.getPosition().y + channelSelector.getSize().y);
}

void ChannelHead::updateFromFM()
{
	pan.setValue(fm->ch[channelIndex].initial_pan);
	vol.setValue(fm->ch[channelIndex].initial_vol);
	rev.setValue(fm->ch[channelIndex].initial_reverb);
	vu.forceValue(0);
}

void ChannelHead::updateZoom()
{
	pan.setSize(94 - (100 - CH_WIDTH));
	vol.setSize(94 - (100 - CH_WIDTH));
	rev.setSize(94 - (100 - CH_WIDTH));

	pan.setPosition(channelIndex*CH_WIDTH + 6, 24);
	vol.setPosition(channelIndex*CH_WIDTH + 6, 44);
	rev.setPosition(channelIndex*CH_WIDTH + 6, 64);

	channelSelector.setPosition(CH_WIDTH*channelIndex + 5, 3);
	mute.setPosition(CH_WIDTH*channelIndex + 6, 4);
	solo.setPosition(18 + CH_WIDTH*channelIndex + 6, 4);
	record.setPosition(2 * 18 + CH_WIDTH*channelIndex + 6, 4);

	vu.setPosition(CH_WIDTH*channelIndex + 1, 4);

	channelSelector.setSize(Vector2f(CH_WIDTH, 81));
	channelName.setPosition(3 * 18 + 4 + CH_WIDTH*channelIndex + 5, 4);

	if (CH_WIDTH < 80)
	{
		channelName.setString(std::to_string((channelIndex + 1) % 10));
	}
	else
	{
		channelName.setString(std::to_string(channelIndex + 1));
	}
}
