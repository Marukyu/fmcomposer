#ifndef DRAWBATCHER_H
#define DRAWBATCHER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "button\button.hpp"
#include "slider/dataslider.hpp"
#include "vumeter\vumeter.hpp"
#include "scrollbar\scrollbar.hpp"
#include "textfield\textfield.hpp"
#include "operator/operator.hpp"

class DrawBatcher{
	sf::VertexArray items;
	std::vector<sf::Text*> texts;
	int itemCount ,textCount;

	public:
		DrawBatcher();
		void draw();
		void initialize();
		void addItemSingleColor(float x, float y, float w, float h, sf::Color color);
		void addItemBicolor(float x, float y, float w, float h, sf::Color color, sf::Color color2);
		void addItem(Button *b);
		void addItem(DataSlider *b);
		void addItem(Slider *s);
		void addItem(TextInput *t);
		void addItem(MiniVuMeter *v);
		void addItem(Operator *o);
		void addItem(VuMeter *v);
		void addItem(StereoVuMeter *v);
		void addItem(RectangleShape *r);

		void addItem(sf::Text* text);
		void addItem(sf::VertexArray *v);
};

extern DrawBatcher drawBatcher;


#endif