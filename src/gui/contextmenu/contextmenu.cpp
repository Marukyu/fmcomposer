#include "contextmenu.hpp"
#include "../popup/popup.hpp"

ListMenu* contextMenu;


ListMenu::ListMenu() : scroll(0), relative(&globalView)
{
	bg.setFillColor(colors[CONTEXTMENUBG]);
	s.setFillColor(colors[CONTEXTMENUBGHOVER]);
	shadow.setFillColor(colors[POPUPSHADOW]);
}
void ListMenu::add(string elem)
{
	text.push_back(Text(elem, font, charSize));
	updateSize(text.size()-1);
	text[text.size() - 1].setPosition(x + 4, y + (text.size() - 1 - scroll) * 24 + 1);
}

void ListMenu::setElement(int index, string elem)
{
	text[index] = Text(elem, font, charSize);
	updateSize(index);
}

void ListMenu::insert(string elem)
{
	text.insert(text.begin(), Text(elem, font, charSize));
	updateSize(0);
	text[0].setPosition(x + 4, y + (text.size() - 1 - scroll) * 24 + 1);
}
int ListMenu::clicked()
{
	mouse.pos = input_getmouse(*relative);
	if (hover())
	{
		if (mouse.clickgReleased || mouse.clickdReleased)
		{
			mouse.clickgReleased = mouse.clickdReleased = false;
			contextMenu = NULL;
			return (mouse.pos.y - y) / 24 + scroll;
		}
	}
	else
	{
		s.setPosition(-500, -500);
		if ((mouse.clickg || mouse.clickd) && contextMenu)
		{

			contextMenu = NULL;
		}
	}
	return -1;
}

void ListMenu::remove(int index)
{
	text.erase(text.begin() + index);
	int maxSize = 0;
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i].getLocalBounds().width>maxSize)
			maxSize = text[i].getLocalBounds().width;
	}

	bg.setSize(Vector2f(maxSize, text.size() * 24));
}

bool ListMenu::hover()
{

	mouse.pos = input_getmouse(*relative);
	for (unsigned i = scroll; i < min<int>(scroll + 14, text.size()); ++i)
	{
		text[i].setColor(colors[CONTEXTMENUTEXT]);
	}
	s.setPosition(x, -100);
	if (mouse.pos.y >= y && mouse.pos.y < y + text.size() * 24 && mouse.pos.x >= x && mouse.pos.x < x + bg.getSize().x)
	{
		s.setPosition(x, y + ((int)(mouse.pos.y - y) / 24 + scroll) * 24);
		text[(mouse.pos.y - y) / 24 + scroll].setColor(colors[CONTEXTMENUTEXTHOVER]);
		return true;
	}

	return false;
}

void ListMenu::show(int x, int y)
{
	if (contextMenu && hover())
		return;

	relative=&globalView;
	contextMenu = this;
	mouse.clickg = mouse.clickd = 0;

	/* Keep the menu inside the screen ! */
	if (y + text.size() * 24 > windowHeight)
	{
		setPosition(x + 4, windowHeight - text.size() * 24);
	}
	else
		setPosition(x + 4, y + 4);
}

void ListMenu::update()
{
	if (!hover() && (mouse.clickd || mouse.clickg))
		contextMenu=0;
}

void ListMenu::show(View *_relative)
{
	relative=_relative;
	mouse.pos = input_getmouse(*relative);
	x=mouse.pos.x;
	y=mouse.pos.y;
	if (contextMenu && hover())
		return;

	contextMenu = this;
	mouse.clickg = mouse.clickd = 0;

	/* Keep the menu inside the screen ! */
	if (y + text.size() * 24 > windowHeight)
	{
		setPosition(x + 4, windowHeight - text.size() * 24);
	}
	else
		setPosition(x + 4, y + 4);
}

void ListMenu::draw()
{
	

	window->setView(*relative);
	
	window->draw(shadow);
	window->draw(bg);
	window->draw(s);
	for (unsigned i = scroll; i < min<int>(scroll + 14, text.size()); ++i)
	{
		window->draw(text[i]);
	}
}

void ListMenu::setPosition(int _x, int _y)
{
	x = _x;
	y = _y;
	bg.setPosition(x, y);
	shadow.setPosition(x + 10, y + 10);
	for (unsigned i = 0; i < text.size(); ++i)
		text[i].setPosition(x + 4, y + i * 24 + 1);
}

void ListMenu::updateSize(int fromIndex)
{
	if (fromIndex>=text.size())
		return;

	bg.setSize(Vector2f((bg.getSize().x < text[fromIndex].getLocalBounds().width + 8) ? text[fromIndex].getLocalBounds().width + 8 : bg.getSize().x, text.size() * 24));
	s.setSize(Vector2f(bg.getSize().x, 24));
	shadow.setSize(Vector2f(bg.getSize().x, bg.getSize().y));
}