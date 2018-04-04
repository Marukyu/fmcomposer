#include "checkbox.hpp"

Checkbox::Checkbox(int x, int y, std::string _title) :title(_title, font, charSize), bg(Vector2f(24, 24)), visible(true)
{
	bg.setPosition(x, y);
	bg.setFillColor(colors[TEXTINPUTBG]);
	bg.setOutlineThickness(1);
	v.setTexture(*tileset);
	v.setTextureRect(IntRect(0, 0, 24, 22));
	v.setPosition(x + 2, y + 2);
	title.setPosition(x + 32, (int)(y + 11 - title.getLocalBounds().height/2));
	title.setColor(colors[BLOCKTEXT]);
	checked = 0;

}

void Checkbox::draw()
{
	if (visible) {
		window->draw(bg);
		window->draw(title);
		if (checked)
		{
			window->draw(v);
		}
	}
}

int Checkbox::clicked()
{
	if (!visible)
		return 0;

	int hovered = (mouse.pos.x >= bg.getPosition().x && mouse.pos.x < title.getPosition().x + title.getLocalBounds().width && mouse.pos.y >= min(title.getPosition().y, bg.getPosition().y-1) && mouse.pos.y < max(title.getPosition().y + title.getLocalBounds().height, bg.getPosition().y+bg.getSize().y+1));

	if (hovered)
	{
		bg.setOutlineColor(colors[FOCUSOUTLINE]);
		if (mouse.clickg)
		{
			checked = !checked;
			focusedElement = this;

			return 1;
		}
	}
	else
	{
		bg.setOutlineColor(colors[TEXTINPUTOUTLINE]);
	}
	return 0;
}