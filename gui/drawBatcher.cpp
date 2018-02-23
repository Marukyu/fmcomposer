#include "drawBatcher.hpp"
#include "../globalFunctions.hpp"

DrawBatcher drawBatcher;


DrawBatcher::DrawBatcher() :itemCount(0), textCount(0)
{
	items.setPrimitiveType(sf::Quads);
	items.resize(3000);
	texts.resize(400);
}

void DrawBatcher::addItemSingleColor(float x, float y, float w, float h, sf::Color color)
{
	items.resize(itemCount+4);
	sf::Vertex *v = &items[itemCount];
	v[0].position.x = x;
	v[0].position.y = y;
	v[1].position.x = x + w;
	v[1].position.y = y;
	v[2].position.x = x + w;
	v[2].position.y = y+h;
	v[3].position.x = x;
	v[3].position.y = y+h;

	v[0].color=color;
	v[1].color=color;
	v[2].color=color;
	v[3].color=color;

	itemCount+=4;
}


void DrawBatcher::addItemBicolor(float x, float y, float w, float h, sf::Color color, sf::Color color2)
{

	items.resize(itemCount+4);
	sf::Vertex *v = &items[itemCount];
	v[0].position.x = x;
	v[0].position.y = y;
	v[1].position.x = x + w;
	v[1].position.y = y;
	v[2].position.x = x + w;
	v[2].position.y = y+h;
	v[3].position.x = x;
	v[3].position.y = y+h;

	v[0].color=color;
	v[1].color=color;
	v[2].color=color2;
	v[3].color=color2;

	itemCount+=4;
}

void DrawBatcher::addItem(Operator *o)
{
	addItemSingleColor(o->bg.getPosition().x-1, o->bg.getPosition().y-1, OP_SIZE+2, OP_SIZE+2, o->bg.getOutlineColor());
	addItemSingleColor(o->bg.getPosition().x, o->bg.getPosition().y, OP_SIZE, OP_SIZE, o->bg.getFillColor());
	addItemSingleColor(o->bgValue.getPosition().x, o->bgValue.getPosition().y, OP_SIZE, o->bgValue.getSize().y, o->bgValue.getFillColor());
	addItem(&o->number);
}

void DrawBatcher::addItem(Button *b)
{
	addItemBicolor(b->bg2[0].position.x, b->bg2[0].position.y, b->w, b->h, b->bg2[0].color, b->bg2[2].color);
	addItem(&b->text);
}

void DrawBatcher::addItem(DataSlider *d)
{
	if (d->visible)
	{
		if (d->focused)
		{
			addItemSingleColor(d->x, d->y, d->width, 19, colors[SLIDEROUTLINE]);
			addItemSingleColor(d->x+1, d->y+1, d->width-2, 17, d->focused ? colors[SLIDERBGHOVER] : colors[SLIDERBG]);
		}
		else
		{
			addItemSingleColor(d->x, d->y, d->width, 19, d->focused ? colors[SLIDERBGHOVER] : colors[SLIDERBG]);
		}
	

		addItemSingleColor(d->bgValue.getPosition().x, d->y, d->bgValue.getSize().x, 19, d->focused ? colors[SLIDERBARHOVER] : colors[SLIDERBAR]);
		addItem(&d->name);

		addItem(&d->tvalue);
	}
}

void DrawBatcher::addItem(MiniVuMeter *v)
{
	addItemSingleColor(v->x, v->y+ 79, v->w, v->h, colors[VUMETERBARLOW]);
}

void DrawBatcher::addItem(TextInput *t)
{
	addItem(&t->bg);
	addItem(&t->text);
	addItem(&t->title);
	addItem(&t->selection);
	if(t->cursorBlink < 32)
		addItem(&t->cursor);
}

void DrawBatcher::addItem(Slider *s)
{
	addItemSingleColor(s->x, s->y, s->bar.getSize().x, s->bar.getSize().y, s->bar.getFillColor());
	addItemSingleColor(s->cursor.getPosition().x, s->cursor.getPosition().y, s->cursor.getSize().x, s->cursor.getSize().y, s->cursor.getFillColor());
}


void DrawBatcher::addItem(sf::VertexArray *v)
{
	items.resize(itemCount+4);

	items[itemCount] = (*v)[0];
	items[itemCount+1] = (*v)[1];
	items[itemCount+2] = (*v)[2];
	items[itemCount+3] = (*v)[3];
	itemCount+=4;
}


void DrawBatcher::addItem(StereoVuMeter *v)
{
	addItem(&v->vuLeft);
	addItem(&v->vuRight);

	for (int i = 0; i < 5; i++)
	{
		addItem(&v->dbBars[i]);
	}

	for (int i = 0; i < 6; i++)
	{
		addItem(&v->dbValues[i]);
	}
}

void DrawBatcher::addItem(VuMeter *v)
{
	addItem(&v->title);
	addItem(&v->quad);
	addItem(&v->quad2);
	addItem(&v->bar);
	addItem(&v->topbar);
	addItem(&v->db);
}

void DrawBatcher::addItem(RectangleShape *r)
{
	if (r->getOutlineThickness() > 0.5f)
	{
		addItemSingleColor(r->getPosition().x-1, r->getPosition().y-1, r->getSize().x+2, r->getSize().y+2, r->getOutlineColor());
		addItemSingleColor(r->getPosition().x, r->getPosition().y, r->getSize().x, r->getSize().y, r->getFillColor());
	}
	else
	{
		addItemSingleColor(r->getPosition().x, r->getPosition().y, r->getSize().x, r->getSize().y, r->getFillColor());
	}
	
}


void DrawBatcher::initialize()
{
	items.resize(0);
	texts.resize(0);
	itemCount=0;
	textCount=0;
}

void DrawBatcher::draw()
{//printf("========================\n");

	items.resize(itemCount);
	texts.resize(textCount);

	window->draw(items);
	for (unsigned i=0; i<textCount; i++)
		window->draw(*texts[i]);
	
}

void DrawBatcher::addItem(sf::Text* text)
{
	texts.resize(textCount+1);
	//printf("%s\n",text->getString().toAnsiString().c_str());
	texts[textCount]=text;
	textCount++;
}