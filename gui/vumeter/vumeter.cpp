#include "vumeter.hpp"
#include <math.h>

MiniVuMeter::MiniVuMeter() {};

MiniVuMeter::MiniVuMeter(int _x, int _y) :bar(Vector2f(0, 0)), value(0), x(_x), y(_y), w(4)
{

	bar.setFillColor(colors[VUMETERBARLOW]);
	bar.setPosition(x, y + 79);
}


void MiniVuMeter::update()
{
	h = -min(79, abs(value / (32768 / 512)));
	bar.setSize(Vector2f(w, h));
	// value *= 0.85, but better going for an FPS-independant function
	value *= pow(0.85, frameTime60);
}

void MiniVuMeter::draw()
{
	window->draw(bar);
}

void MiniVuMeter::setValue(int _value)
{
	if (abs(_value) > value)
		value = abs(_value);
}

void MiniVuMeter::setPosition(int _x, int _y)
{
	x=_x;
	y=_y;
	bar.setPosition(x, y + 79);
}

void MiniVuMeter::forceValue(int _value)
{
	value = _value;
}


VuMeter::VuMeter(int x, int y, string _title) :bar(Vector2f(30, 0)), topbar(Vector2f(30, 1)), db("", font, charSize), quad(Quads, 4), quad2(Quads, 4), title(_title, font, charSize), value(0), topValue(0)
{
	this->x = x;

	title.setPosition(x + 10, y - 16);
	title.setColor(colors[VUMETERTEXT]);
	quad[0].position = sf::Vector2f(x + 1, y + 64);
	quad[1].position = sf::Vector2f(x + 1 + 30, y + 64);
	quad[2].position = sf::Vector2f(x + 1 + 30, y + 128);
	quad[3].position = sf::Vector2f(x + 1, y + 128);

	quad[0].color = colors[VUMETERBARMEDIUM];
	quad[1].color = colors[VUMETERBARMEDIUM];
	quad[2].color = colors[VUMETERBARLOW];
	quad[3].color = colors[VUMETERBARLOW];

	quad2[0].position = sf::Vector2f(x + 1, y + 0);
	quad2[1].position = sf::Vector2f(x + 1 + 30, y + 0);
	quad2[2].position = sf::Vector2f(x + 1 + 30, y + 64);
	quad2[3].position = sf::Vector2f(x + 1, y + 64);

	quad2[0].color = colors[VUMETERBARHIGH];
	quad2[1].color = colors[VUMETERBARHIGH];
	quad2[2].color = colors[VUMETERBARMEDIUM];
	quad2[3].color = colors[VUMETERBARMEDIUM];

	bar.setPosition(x + 1, y);

	bar.setFillColor(colors[VUMETERBG]);
	topbar.setFillColor(colors[VUMETERTOPBAR]);
	topbar.setPosition(bar.getPosition().x, y + 128);
	db.setPosition(x, y + 128);
	db.setColor(colors[VUMETERTEXT]);
}

void VuMeter::update()
{
	bar.setSize(Vector2f(30, 128 - abs(value / (32768 / 128))));

	if (value > topValue)
	{
		topValue = value;
		topTimer = 0;
	}
	topTimer++;

	topValue <= 0 ? db.setString(L"-∞") : db.setString(float2string(20 * log10(abs(topValue) / 32768.0), 1));

	db.setPosition((int)(x + 16 - db.getLocalBounds().width / 2), db.getPosition().y);


	topbar.setPosition(bar.getPosition().x, bar.getPosition().y + 128 - abs(topValue / (32768 / 128)));

	if (topTimer > 40)
	{
		topValue -= 200;
		if (topValue<0)
			topValue = 0;
	}

	if (value == 32768)
	{

		saturationTimer = 60;
		db.setColor(colors[VUMETERTEXTSATURATED]);
	}

	if (saturationTimer > 0)
	{
		saturationTimer--;
		if (saturationTimer == 0)
		{
			db.setColor(colors[VUMETERTEXT]);
		}
	}
	// value *= 0.85, but better going for an FPS-independant function
	value *= pow(0.85, frameTime60);

}

void VuMeter::draw()
{
	window->draw(title);
	window->draw(quad);
	window->draw(quad2);
	window->draw(bar);
	window->draw(topbar);
	window->draw(db);
}
