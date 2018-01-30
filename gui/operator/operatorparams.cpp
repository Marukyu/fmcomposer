#include "opgui.hpp"


// GUI for one operator in the instrument editor
OpGUI::OpGUI(int _x, int _y, string _name) :
bg(Vector2f(484, 210)),
name(_name, font, charSize),
mode("Frequency", font, charSize),
waveform(*tileset),
fixedFreq(271, 29, "Ratio"),
mute(160, 5, ICON_MD_VOLUME_OFF,15,0,16),
envLoop(0, 130, ICON_MD_LOOP,15,0,16),
pitchEnv("Pitch envelope", font, charSize),
volEnv("Volume envelope", font, charSize),
scalings("Keyboard scalings", font, charSize),
x(_x),
y(_y),
selected(0),
lfoOffsetBar(Vector2f(1, 26)),
lfo("LFO", font, charSize)
{
	mainBasePos = _x;
	lfoOffsetBar.setFillColor(colors[WAVEFORMOFFSETBAR]);
	bg.setFillColor(colors[BLOCKBG]);
	bg.setOutlineColor(colors[FOCUSOUTLINE]);
	bg.setPosition(-5, 0);
	name.setColor(colors[TITLE]);
	name.setPosition(0, 4);
	view.setCenter(x, y);
	slider.insert(slider.end(), {
		DataSlider(180, 5, 99, 0, "Volume", 99, 130),
		DataSlider(160, 50, 40, 0, "Mult", 1, 150), // mult
		DataSlider(236, 70, 100, -100, "Fine", 0, 74), // detune
		DataSlider(0, 90, 99, 1, "Attack", 99, 150), // a
		DataSlider(20, 130, 99, 0, "Decay", 38, 130), // d
		DataSlider(0, 150, 99, 0, "Sustain", 0, 150), // s
		DataSlider(0, 170, 99, -99, "Release", 38, 150), // r
		DataSlider(321, 90, 127, -127, "Volume scale", 0, 150), // keyboard volume scaling
		DataSlider(351, 180, 99, 0, "FM", 0, 59),
		DataSlider(411, 180, 99, 0, "AM", 0, 60),
		DataSlider(321, 5, 7, 0, "Wave", 0, 60),

		DataSlider(321, 110, 48, -48, "Attack", 0, 75), // envelope rate scaling
		DataSlider(397, 110, 48, -48, "Decay", 0, 74),
		DataSlider(321, 130, 99, 0, "VelSense", 99, 150), // keyboard volume scaling
		DataSlider(0, 110, 80, 0, "Hold", 0, 150), // hold
		DataSlider(0, 50, 70, 0, "Delay", 0, 150), // delay
		DataSlider(0, 70, 99, 0, "Initial level", 0, 150), // initial level
		DataSlider(160, 70, 24, 0, "¼tone", 0, 75),
		DataSlider(321, 70, 120, 0, "Center note", 40, 150, 2), // envelope rate scaling
		DataSlider(397, -900, 40, -40, "Pitch", 0, 74), // envelope rate scaling
		DataSlider(160, 120, 99, -99, "Initial freq", 0, 150), // envelope rate scaling
		DataSlider(160, 140, 99, 0, "Decay time", 0, 150), // envelope rate scaling
		DataSlider(160, 160, 99, -99, "Release freq", 0, 150), // envelope rate scaling
		DataSlider(160, 180, 99, 0, "Release time", 0, 150), // envelope rate scaling
		DataSlider(321 + 61, 5, 31, 0, "Offs.", 0, 60)
	});

	for (int i = 0; i < slider.size(); i++)
	{
		basePos[i] = slider[i].x;
		baseSize[i] = slider[i].width;
	}


	waveform.setPosition(443, 0);
	lfoOffsetBar.setPosition(waveform.getPosition().x + slider[24].value*36.0 / 32, waveform.getPosition().y + 6);
	mode.setPosition(160, 30);
	mode.setColor(colors[SUBTITLE]);

	pitchEnv.setPosition(160, 100);
	pitchEnv.setColor(colors[SUBTITLE]);
	volEnv.setPosition(0, 30);
	volEnv.setColor(colors[SUBTITLE]);
	scalings.setPosition(320, 50);
	scalings.setColor(colors[SUBTITLE]);
	lfo.setPosition(320, 180);
	lfo.setColor(colors[SUBTITLE]);
}

int OpGUI::hover()
{
	mouse.pos = input_getmouse(view);
	return (mouse.pos.x >= bg.getPosition().x && mouse.pos.x < bg.getPosition().x+bg.getSize().x && mouse.pos.y >= bg.getPosition().y && mouse.pos.y < bg.getPosition().y+bg.getSize().y);
}

bool OpGUI::update()
{
	mouse.pos = input_getmouse(view);
	
	int itemsHovered=0;

	bool modified=false;

	if (envLoop.clicked())
	{
		envLoop.selected = !envLoop.selected;
		selected=false;
		modified= true;
	}
	if (mute.clicked())
	{
		mute.selected = !mute.selected;
		if (mute.selected)
		{
			mute.setText(ICON_MD_VOLUME_OFF);
		}
		else
		{
			mute.setText(ICON_MD_VOLUME_UP);
		}
		selected=false;
		modified= true;
	}
	if (fixedFreq.clicked())
	{
		fixedFreq.selected = !fixedFreq.selected;

		if (fixedFreq.selected)
		{
			slider[1].name.setString("Frequency");
			slider[1].setMinMax(1, 255);
			fixedFreq.setText("Fixed");
		}
		else
		{
			slider[1].name.setString("Ratio");
			slider[1].setMinMax(0, 40);
			fixedFreq.setText("Ratio");
		}
		selected=false;
		modified= true;
	}
	waveform.setTextureRect(IntRect(810, 32 * slider[10].value, 36, 32));
	lfoOffsetBar.setPosition(waveform.getPosition().x + slider[24].value*36.0 / 32, waveform.getPosition().y + 6);

	itemsHovered = envLoop.hovered+mute.hovered+mute.hovered+fixedFreq.hovered;


	for (unsigned i = 0; i < slider.size(); ++i)
	{
		if (slider[i].update())
		{
			if (i == 0)
			{
				mute.selected = slider[0].value == 0;
			}
			selected=false;
			modified=true;
		}
		else
		{
			if (slider[i].hovered)
			{
				itemsHovered++;
			}
		}
	}

	if (modified)
		return true;


	if (itemsHovered==0 && (mouse.clickg || mouse.clickd))
	{
		mouse.pos = input_getmouse(view);
		selected = hover();
		selected ? bg.setFillColor(colors[BLOCKBGHOVER]) : bg.setFillColor(colors[BLOCKBG]);
	}
	return false;
}

void OpGUI::draw()
{
	window->setView(view);
	bg.setOutlineThickness(selected);
	window->draw(bg);
	window->draw(name);
	for (unsigned i = 0; i < slider.size(); ++i)
		slider[i].draw();
	window->draw(waveform);
	window->draw(lfoOffsetBar);
	fixedFreq.draw();
	window->draw(mode);
	mute.draw();
	envLoop.draw();
	window->draw(pitchEnv);
	window->draw(volEnv);
	window->draw(scalings);
	window->draw(lfo);
}

void OpGUI::setZoom(float zoom)
{
	for (int i = 0; i < slider.size(); i++)
	{
		slider[i].setSize(baseSize[i] * zoom);
		if (slider[i].x > 0)
		{
			slider[i].setPosition(basePos[i] * zoom, slider[i].y);
		}
	}
	bg.setSize(Vector2f(484 * zoom, 210));
	waveform.setPosition((int)(484 * zoom - 41), 0);
	lfoOffsetBar.setPosition((int)(waveform.getPosition().x + slider[24].value*36.0 / 32), waveform.getPosition().y + 6);
	mode.setPosition((int)(160 * zoom), 30);
	mute.setPosition((int)(160 * zoom), mute.y);

	pitchEnv.setPosition((int)(160 * zoom), 100);
	
	scalings.setPosition((int)(320 * zoom), 50);
	lfo.setPosition((int)(320 * zoom), 180);

	slider[0].setPosition((int)(mute.x+20), slider[0].y);
	slider[0].setSize(slider[3].width-20);

	fixedFreq.setPosition((int)(slider[0].x+slider[0].width-fixedFreq.w), 29);

	slider[4].setPosition((int)(envLoop.x+20), slider[4].y);
	slider[4].setSize(slider[3].width-20);

}