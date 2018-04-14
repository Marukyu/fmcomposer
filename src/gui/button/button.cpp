#include "button.hpp"
#include "../contextmenu/contextmenu.hpp"

Button::Button() {}

void Button::construct(int _x, int _y, int _width, int _padding)
{
	x = _x;
	y = _y;
	padding = _padding;
	width = _width;

	// manual or automatic width
	if (width != -1)
	{
		w = width + 4 + padding * 2;
		h = 19 + padding * 2;
	}
	else
	{
		w = text.getGlobalBounds().width + 4 + padding * 2;
		h = 19 + padding * 2;
	}

	hover();
	hovered = 0;
	updatePosition();
}

Button::Button(int _x, int _y, string _text, int _width, int _padding) : text(_text, font, charSize), selected(false), click(false), bg2(Quads, 4), textYpadding(0)
{
	construct( _x, _y, _width, _padding);
}

Button::Button(int _x, int _y, wstring _text, int _width, int _padding,int fontSize) : text(_text, font_symbols, fontSize), selected(false), click(false), bg2(Quads, 4), textYpadding(2)
{
	construct( _x, _y, _width, _padding);
}

void Button::draw()
{

	
	window->draw(bg2);
	window->draw(text);

}

bool Button::clicked()
{
	if (contextMenu)
		return 0;
	if (hover())
	{
		if (mouse.clickg)
		{
			click = true;
		}
		if (mouse.clickgReleased && click)
		{
			click = false;
			return true;
		}
	}
	else
	{
		if (mouse.clickgReleased)
			click = false;
	}

	return false;
}

void Button::setPosition(int _x, int _y)
{
	x = _x;
	y = _y;
	updatePosition();
}

void Button::setText(string _text)
{
	text.setString(_text);
	if (width != -1)
	{
		w = width + 4 + padding * 2;
	}
	else
	{
		w = text.getGlobalBounds().width + 4 + padding * 2;
	}
}

void Button::setText(wstring _text)
{
	text.setString(_text);
	if (width != -1)
	{
		w = width + 4 + padding * 2;
	}
	else
	{
		w = text.getGlobalBounds().width + 4 + padding * 2;
	}
}

bool Button::hover()
{
	hovered = (!contextMenu && mouse.pos.x >= x - padding && mouse.pos.x - padding <= x - 2 * padding + w && mouse.pos.y >= y - padding && mouse.pos.y <= y - padding + h);

	updateStyle();
	return hovered;
}

void  Button::setSelected(bool _selected)
{
	selected=_selected;
	updateStyle();
}

void Button::updatePosition()
{
	text.setPosition(x + 2, y+textYpadding);

	bg2[0].position = sf::Vector2f(x - padding, y - padding);
	bg2[1].position = sf::Vector2f(x - padding + w, y - padding);
	bg2[2].position = sf::Vector2f(x - padding + w, y - padding + h);
	bg2[3].position = sf::Vector2f(x - padding, y - padding + h);
}

void Button::updateStyle()
{
	text.setColor((colors[selected ? BUTTONTOGGLEDTEXT : BUTTONTEXT]));
	if (hovered)
	{
		if (click)
		{
			bg2[0].color = colors[BUTTONBGCLICKEDA];
			bg2[1].color = colors[BUTTONBGCLICKEDA];
			bg2[2].color = colors[BUTTONBGCLICKEDB];
			bg2[3].color = colors[BUTTONBGCLICKEDB];
		}
		else if (selected)
		{
			bg2[0].color = colors[BUTTONTOGGLEBGHOVER];
			bg2[1].color = colors[BUTTONTOGGLEBGHOVER];
			bg2[2].color = colors[BUTTONTOGGLEBGHOVER];
			bg2[3].color = colors[BUTTONTOGGLEBGHOVER];
		}
		else
		{
			bg2[0].color = colors[BUTTONBGHOVERA];
			bg2[1].color = colors[BUTTONBGHOVERA];
			bg2[2].color = colors[BUTTONBGHOVERB];
			bg2[3].color = colors[BUTTONBGHOVERB];
		}
	}
	else
	{
		if (selected)
		{
			bg2[0].color = colors[BUTTONTOGGLEDBG];
			bg2[1].color = colors[BUTTONTOGGLEDBG];
			bg2[2].color = colors[BUTTONTOGGLEDBG];
			bg2[3].color = colors[BUTTONTOGGLEDBG];
		}
		else
		{
			bg2[0].color = colors[BUTTONBGA];
			bg2[1].color = colors[BUTTONBGA];
			bg2[2].color = colors[BUTTONBGB];
			bg2[3].color = colors[BUTTONBGB];
		}
	}
}