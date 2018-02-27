

#ifndef OPGUI_H
#define OPGUI_H

#include "../gui.hpp"
#include "../button/button.hpp"
#include "../slider/dataslider.hpp"
#include <SFML/Graphics.hpp>

class OpGUI{
	sf::Text name, mode, volEnv, pitchEnv, scalings, lfo;
	sf::Vector2f mouseOp;
	int basePos[32], baseSize[32];
	int mainBasePos;
	public:
	sf::Sprite waveform;
	int selected, x, y;
	sf::View view;
	sf::RectangleShape bg, lfoOffsetBar;
	std::vector< DataSlider > slider;
	Button fixedFreq, mute, envLoop;
	OpGUI(int x, int y, std::string name);
	void draw();
	bool update();
	int hover();
	void setZoom(float zoom);
};

#endif