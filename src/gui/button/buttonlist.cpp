#include "buttonlist.hpp"
#include "../../fmengine/fmlib.h"
#include "../contextmenu/contextmenu.hpp"
#include "../drawBatcher.hpp"

extern fmsynth *phanoo;
extern ListMenu *contextMenu;

ButtonList::ButtonList(int _x, int _y) :selected(0), maxId(0)
{
	x = _x;
	y = _y;
	scroll = 0;
	bgfocus.setOutlineThickness(2);
	bgfocus.setOutlineColor(colors[FOCUSOUTLINE]);
	bgfocus.setFillColor(colors[FOCUSOUTLINE]);
	bgfocus.setPosition(_x, _y);
}

void ButtonList::updateButtonPos()
{

	scroll = clamp(selectedIndex - 23, 0, (int)buttons.size());
	int xpos = buttons[scroll].x;

	for (unsigned i = scroll; i < min<int>(buttons.size(), scroll + 46); ++i)
	{
		buttons[i].hover();
		buttons[i].selected = (selectedIndex == i);
		buttons[i].setPosition(xpos - (buttons[scroll].x - buttons[0].x), y);
		xpos += buttons[i].w + 1;
	}

	xpos = buttons[scroll].x;

	for (unsigned i = scroll; i < min<int>(buttons.size(), scroll + 46); ++i)
	{
		buttons[i].setPosition(xpos - (buttons[scroll].x - buttons[0].x), y);
		xpos += buttons[i].w + 1;
	}

	bgfocus.setSize(Vector2f(xpos - buttons[0].x, 19));
	
}

void ButtonList::select(int index)
{
	selectedIndex = index;
	updateButtonPos();
}

void ButtonList::draw()
{

	
	drawBatcher.initialize();
	
	if (selected)
	{
		drawBatcher.addItem(&bgfocus);
	}

	for (unsigned i = scroll; i < min<int>(buttons.size(), scroll + 46); ++i)
	{
		drawBatcher.addItem(&buttons[i]);
	}
	drawBatcher.draw();

}

void ButtonList::add(string text)
{

	buttons.push_back(Button(x + buttons.size() * 24, y, std::to_string(maxId), maxId < 100 ? 19 : 25));

	maxId++;
	updateButtonPos();
}

int ButtonList::getElementHovered()
{
	int elem = -1;
	for (unsigned i = max<int>(0, scroll); i < min<int>(buttons.size(), scroll + 46); ++i)
	{

		if (buttons[i].hover())
		{
			
			elem = i;
		}
	}

	return elem;
}

void ButtonList::update()
{
	mouse.pos = input_getmouse(globalView);
	if (mouse.clickg || mouse.clickd)
	{
		if (mouse.pos.x >= x && mouse.pos.y >= y && mouse.pos.x < x + buttons.size() * 24 && mouse.pos.y < y + 19)
		{
			focusedElement = this;
			selected = 1;
		}
		else if (!contextMenu || !contextMenu->hover())
		{
			selected = 0;
		}
	}
}

unsigned ButtonList::elementCount()
{
	return buttons.size();
}

void ButtonList::erase(int index)
{
	buttons.erase(buttons.begin() + index);
	for (unsigned i = 0; i<buttons.size(); ++i)
		buttons[i].setPosition(x + i * 24, buttons[i].y);
	updateButtonPos();
}

void ButtonList::move(int indexA, int indexB)
{
	buttons.insert(buttons.begin() + indexB + (indexB>indexA), buttons[indexA]);
	buttons.erase(buttons.begin() + indexA + (indexB <= indexA));
	updateButtonPos();

}

void ButtonList::clear()
{
	buttons.clear();
	maxId = 0;

}

int ButtonList::isElementHovered(int index)
{

	if (index < 0 || index >= buttons.size())
		return 0;
	return buttons[index].hover();
}

void ButtonList::insert(int index, string text)
{
	buttons.insert(buttons.begin() + index, Button(x + index * 24, y, std::to_string(maxId), maxId < 100 ? 19 : 25));
	maxId++;
	updateButtonPos();
}

void ButtonList::selectAll()
{
	for (int i = 0; i < fm->patternCount; i++)
	{
		buttons[i].selected = 1;
	}
}