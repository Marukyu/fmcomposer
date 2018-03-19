#include "operator.hpp"
#include "../../fmengine/fmlib.h"
#include "../../gui/list/list.hpp"
#include "../../gui/drawBatcher.hpp"

extern Vector2i mouseSidebar;
extern fmsynth *phanoo;
extern List* instrList;

Operator::Operator() : bg(Vector2f(OP_SIZE, OP_SIZE)), number("", font, charSize), active(0), active2(0), muted(0)
{
	bg.setFillColor(colors[OPERATORBG]);
	bg.setOutlineThickness(1);
	bg.setOutlineColor(colors[OPERATOROUTLINE]);
	bgValue.setFillColor(colors[OPERATORVUMETER]);

	for (int i = 0; i < 2; i++)
	{
		line[i].color.r = lineBot[i].color.r = colors[OPERATORLINKS].r;
		line[i].color.g = lineBot[i].color.g = colors[OPERATORLINKS].g;
		line[i].color.b = lineBot[i].color.b = colors[OPERATORLINKS].b;
	}

	for (int i = 0; i < 5; i++)
		top[i] = bot[i] = NULL;
}

int Operator::update()
{

	mouse.pos = input_getmouse(instrView);

	if (active || highlighted || hovered)
	{
		bg.setFillColor(colors[OPERATORBGHOVER]);
		number.setColor(colors[muted ? OPERATORBGMUTED : OPERATORTEXTHOVER]);
	}
	else
	{
		bg.setFillColor(colors[muted ? OPERATORBGMUTED : OPERATORBG]);
		number.setColor(colors[muted ? OPERATORTEXTHOVER : OPERATORTEXT]);
	}

	bgValue.setPosition(bg.getPosition().x, bg.getPosition().y + bg.getSize().y);

	float maxVol = 0;

	for (int i = 0; i < FM_ch; i++)
	{
		if (fm->ch[i].instrNumber == instrList->value && fm->ch[i].active)
		{
			if (fm->ch[i].op[id].env>maxVol)
			{
				maxVol = fm->ch[i].op[id].env;
			}
		}
	}

	bgValue.setSize(Vector2f(bg.getSize().x, -(bg.getSize().y)*(maxVol)));
	number.setPosition(bg.getPosition().x + 7, bg.getPosition().y + 2);

	if (mouse.pos.x >= position.x && mouse.pos.x < position.x + bg.getSize().x && mouse.pos.y >= position.y&& mouse.pos.y <= position.y + bg.getSize().y)
	{

		hovered = 1;

		if (mouse.clickd)
		{
			active2 = 1;
			dx = mouse.pos.x - position.x;
			dy = mouse.pos.y - position.y;
		}
		else if (mouse.clickg)
		{
			active = 1;

			dx = mouse.pos.x - position.x;
			dy = mouse.pos.y - position.y;

			return 1;
		}
	}
	else
	{
		hovered = 0;

	}
	if (active2 && !Mouse::isButtonPressed(Mouse::Right))
	{
		active2 = 0;
	}
	if (active)
	{
		position.x = mouse.pos.x - dx;
		position.y = mouse.pos.y - dy;
		updatePosition();
	}
	bg.setPosition((bg.getPosition().x+position.x)*0.5, (bg.getPosition().y+position.y)*0.5);

	return 0;
}

void Operator::updatePosition()
{
	bg.setPosition(position.x, position.y);
}
void Operator::draw()
{
	
	window->draw(bg);
	window->draw(bgValue);
	window->draw(number);
}


void Operator::drawLines()
{
	mouse.pos = input_getmouse(instrView);
	for (int i = 0; i < 5; i++)
	{
		line[0].position.x = bg.getPosition().x + OP_SIZE/2;
		line[0].position.y = bg.getPosition().y;
		if (top[i] && !top[i]->active)
		{
			line[1].position.x = top[i]->bg.getPosition().x + OP_SIZE/2;
			line[1].position.y = top[i]->bg.getPosition().y + OP_SIZE;
			drawBatcher.addLine(&line[0],&line[1]);
		}
		else if (active2)
		{
			line[1].position.x = mouse.pos.x;
			line[1].position.y = mouse.pos.y;
			drawBatcher.addLine(&line[0],&line[1]);
		}
	}
}

int recurFind(Operator* o, Operator* compare)
{
	int val = 0;
	for (int i = 0; i < 5; i++)
	{
		if (o->top[i])
		{
			if (o->top[i] == compare)
				val++;
			val += recurFind(o->top[i], compare);
		}
	}
	return val;
}

void Operator::cleanupBottomLinks()
{
	// cleanup bottom links
	for (int j = 0; j < 5; j++)
	{
		if (this->bot[j])
		{
			for (int i = 0; i < 5; i++)
			{
				// delete children reference to This and move pointers to fill the empty space
				if (this->bot[j]->top[i] == this)
				{
					for (int k = i; k < 4; k++)
					{
						this->bot[j]->top[k] = this->bot[j]->top[k + 1];
					}
					this->bot[j]->top[4] = NULL;
					break;
				}
			}
			this->bot[j] = NULL;
		}
	}
}

int Operator::hasParents()
{
	for (int i = 0; i < 5; i++)
	{
		if (top[i])
		{
			return 1;
		}
	}
	return 0;
}

int Operator::connect(Operator* _bot, int force, int connectLink)
{

	// disable connecting to itself or moving middle/bot tree note into itself
	int alreadyInTree = this == _bot || (bot[0] && hasParents() && recurFind(this, _bot));
	if (!force)
	{
		if (!connectLink && alreadyInTree || _bot->top[0] == this || _bot->top[1] == this || _bot->top[2] == this || _bot->top[3] == this || _bot->top[4] == this)
		{
			int tempId = _bot->id;
			_bot->id = this->id;
			_bot->number.setString(int2str[_bot->id + 1]);
			this->id = tempId;
			this->number.setString(int2str[this->id + 1]);
			return 1;
		}
	}
	if (!connectLink)
	{
		cleanupBottomLinks();
	}

	// connect this bots
	int botIndex = 0;
	while (this->bot[botIndex])
		botIndex++;

	if (botIndex > 4)
		return 0;

	this->bot[botIndex] = _bot;

	// connect bottom tops to this
	for (int i = 0; i < 5; i++)
	{
		if (!_bot->top[i])
		{
			_bot->top[i] = this;
			break;
		}
	}

	return 1;
}

int Operator::hover(Operator* o1)
{
	return !(position.x > o1->position.x + 16 || position.x + 16 < o1->position.x || position.y > o1->position.y + 16 || position.y + 16 < o1->position.y);
}

Operator* Operator::getTopmost(Operator* o)
{
	Operator* ptr = this;
	Operator* good = this;
	while (ptr->top[0] != NULL && ptr->top[0] != o)
	{
		ptr = ptr->top[0];
		if (ptr->position.x / 30 == position.x / 30)
			good = ptr;
		for (int i = 0; i < 5; i++)
		{
			if (o->top[i] == ptr)
			{
				for (int j = 0; j < 5; j++)
				{
					if (o->top[i]->bot[j] == o)
					{
						for (int k = j; k < 4; k++)
						{
							o->top[i]->bot[k] = o->top[i]->bot[k + 1];
						}
						o->top[i]->bot[4] = NULL;

						break;
					}
				}
				o->top[i] = NULL;
				break;
			}
		}

	}

	return good;
}
