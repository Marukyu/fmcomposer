#include "list.hpp"
#include "../contextmenu/contextmenu.hpp"
extern void *focusedElement;


List::List(int _x, int _y, int _maxrows, int width, int _paddingleft) : x(_x), y(_y),
paddingleft(_paddingleft), maxrows(_maxrows), squarePing(Vector2f(8, 8)), s(Vector2f(width, 17)), bg(Vector2f(width, 17)),
scroll(0), value(0), selected(0), pressed(0), hovered(0), scrollbar(100, 100, 0, 0.1, _x + width - 8, _y, 17 * _maxrows, true), s2(Vector2f(width, 17))
{
	s.setFillColor(colors[LISTITEMBGFOCUS]);
	s.setPosition(x, y);
	bg.setFillColor(colors[LISTBG]);
	s2.setFillColor(colors[LISTITEMBGFOCUS]);
	bg.setPosition(x, y);
	squarePing.setFillColor(colors[BUTTONTOGGLEDBG]);
}
void List::add(string elem, bool autoUpdate)
{
	text.push_back(Text(elem, font, charSize));
	int strpos = elem.size() - 1;
	while (text[text.size() - 1].getLocalBounds().width > bg.getSize().x - paddingleft - 2)
	{
		text[text.size() - 1].setString(elem.substr(0, strpos));
		strpos--;
	}
	pings.push_back(0);
	text.back().setColor(colors[LISTITEMTEXT]);
	text.back().setPosition(x, y + (text.size() - 1) * 17);
	if (autoUpdate)
		updateSize();
	if (text.size() == 1 && value == 0)
		select(0);
	bg.setOutlineThickness(1);
	scrollbar.setScrollableContent(text.size(), maxrows);
}

void List::updateSize()
{
	bg.setSize(Vector2f(bg.getSize().x, 17 * maxrows));
	pings.resize(text.size());
	scrollbar.setScrollableContent(text.size(), maxrows);
}

bool List::rightClicked()
{
	return clicked(2);
}

bool List::clicked(int mouseButton)
{

	if (focusedElement == this && !contextMenu)
	{
		int newValue = value;
		if (keyboard.up)
		{
			value--;
		}
		if (keyboard.down)
		{
			value++;
		}
		if (keyboard.pageUp)
		{
			if (value <= maxrows)
				value = 0;
			else
				value -= maxrows;
		}

		if (keyboard.pageDown)
		{
			if (value + maxrows >= (int)text.size())
				value = text.size() - 1;
			else
				value += maxrows;
		}
		if (keyboard.up || keyboard.down || keyboard.pageUp || keyboard.pageDown)
		{

			if (value > (int)text.size() - 1)
				value = 0;
			else if (value<0)
			{
				value = (int)text.size() - 1;
			}
			select(value);
			return true;
		}
	}

	if (text.size() > maxrows)
	{

		if (scrollbar.update())
		{
			float scrollpos = (scrollbar.value) *((int)text.size() - maxrows) / 100;

			scroll = clamp((int)round(scrollpos), 0, (int)text.size() - maxrows);
		}
		else
		{
			scrollbar.setValue((float)scroll / ((int)text.size() - maxrows) * 100);
		}
		s.setPosition(x, y + (value - scroll) * 17);
	}



	if (mouse.pos.y >= y &&mouse.pos.y < y + bg.getSize().y &&mouse.pos.x >= x && mouse.pos.x < x + bg.getSize().x )
	{
		
			hovered = 1;
			s2.setPosition(x, y + min(text.size() - 1 - scroll, (mouse.pos.y - y) / 17) * 17);
			if (mouse.scroll != 0)
			{

				scroll = clamp(scroll - mouse.scroll * 5, 0, max(0, (int)text.size() - maxrows));

				return true;
			}

			if (mouse.pos.x < x + bg.getSize().x &&
				(mouse.clickg && (mouseButton == 0 || mouseButton == 1) && !contextMenu || mouse.clickd && (mouseButton == 0 || mouseButton == 2)))
			{
				focusedElement = this;
				prevScroll = scroll;

				if (mouse.pos.x < x + bg.getSize().x - (text.size()>maxrows) * 8 )
					select(min(text.size() - 1, (mouse.pos.y - y) / 17 + scroll), 0);

				selected = 1;
				return true;
			}
		
	}
	else
	{
		hovered = 0;
		if (mouse.clickg || mouse.clickd)
		{
			selected = 0;
		}
	}


	return false;
}

void List::clear()
{
	text.clear();
	pings.clear();
	scroll = value = 0;
	s.setPosition(x, y + (value - scroll) * 17);
}

int List::hover()
{
	return (mouse.pos.y >= y &&mouse.pos.y < y + text.size() * 17 && mouse.pos.x >= x &&mouse.pos.x < x + bg.getSize().x);
}

void List::draw()
{
	if (selected)
	{

		bg.setOutlineColor(colors[FOCUSOUTLINE]);
	}
	else
	{
		bg.setOutlineColor(colors[LISTOUTLINE]);
	}

	window->draw(bg);
	if (pressed)
		window->draw(s2);

	if ((int)(s.getPosition().y + 0.5) >= (int)(bg.getPosition().y + 0.5) && (int)(s.getPosition().y + 0.5) < (int)(bg.getPosition().y + bg.getSize().y + 0.5))
		window->draw(s);

	for (unsigned i = scroll; i<min<int>(scroll + maxrows, text.size()); ++i)
	{
		text[i].setPosition(x + paddingleft, y + (i - scroll) * 17);
		window->draw(text[i]);
		if (pings[i]>0)
		{
			squarePing.setPosition(x + 4, y + (i - scroll) * 17 + 5);
			window->draw(squarePing);
		}

	}
	if (text.size() > maxrows)
		scrollbar.draw();
}

void List::select(int index, bool updateScroll)
{

	value = clamp(index, 0, (int)text.size() - 1);

	if (updateScroll)
	{
		if (text.size() > maxrows)
			scroll = clamp(value - maxrows / 2, 0, (int)text.size() - maxrows);
		else
			scroll = 0;
	}

	s.setPosition(x, y + (value - scroll) * 17);
	for (int i = 0; i < text.size(); i++)
	{
		text[i].setColor(colors[LISTITEMTEXT]);
	}
	if (text.size()>0)
		text[value].setColor(colors[LISTITEMTEXTFOCUS]);
}

void List::remove(int index)
{
	text.erase(text.begin() + index);
	if (value > text.size() - 1)
		value = text.size() - 1;
	s.setPosition(x, y + (value - scroll) * 17);
}

void List::setMaxRows(int rows)
{
	maxrows = rows;
	updateSize();
	scrollbar.setSize(rows * 17);
	scrollbar.setScrollableContent(text.size(), maxrows);
}

void List::movedElement(int *to, int *from)
{
	if (hovered && mouse.clickg)
	{
		pressed = 1;

	}

	if (mouse.clickd || contextMenu)
		pressed = 0;

	int pos = min(text.size() - 1, (mouse.pos.y - y) / 17 + scroll);

	if (hovered && pressed && !contextMenu && scroll == prevScroll && pos != value)
	{

		mouse.cursor = CURSOR_SWAP;

		if (mouse.clickgReleased)
		{
			*to = pos;
			*from = value;

			if (pos < value)
			{
				select(pos);
			}
			else if (pos > value)
			{
				select(pos);
			}
		}

	}

	if (mouse.clickgReleased)
		pressed = 0;

}

void List::setScroll(int pos)
{
	scroll = pos;
	scrollbar.setValue((float)pos / (text.size() - maxrows) * 100);
}