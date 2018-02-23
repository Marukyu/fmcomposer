#include "mainmenu.hpp"
#include "../fmengine/fmlib.h"
#include "../views/settings/configEditor.hpp"
#include "../views/general/generalEditor.hpp"
#include "../views/instrument/instrEditor.hpp"
#include "../views/pattern/songEditor.hpp"
#include "../views/pianoroll/pianoRoll.hpp"
#include "../gui/popup/popup.hpp"
#include "../views/pattern/songFileActions.hpp"

Menu *menu;

void Menu::setVertexRect(int vertexId, int x, int y, int w)
{
	sf::Vertex *v = &items[vertexId];
	v[0].texCoords.x = x;
	v[0].texCoords.y = y;
	v[1].texCoords.x = x + w;
	v[1].texCoords.y = y;
	v[2].texCoords.x = x + w;
	v[2].texCoords.y = y+32;
	v[3].texCoords.x = x;
	v[3].texCoords.y = y + 32;
}

void Menu::setVertexPos(sf::Vertex *v, int x, int y, int w)
{
	v[0].position = sf::Vector2f(x, y);
    v[1].position = sf::Vector2f(x+w, y);
    v[2].position = sf::Vector2f(x+w, y+32);
    v[3].position = sf::Vector2f(x, y+32);
}

// create main menu bar and its buttons
Menu::Menu(int def)
{
	items.setPrimitiveType(sf::Quads);
	items.resize(4*(MAX_MENUITEMS+1));
	
	states.texture=tileset;

	select.setFillColor(colors[MENUITEMSELECT]);

	for (unsigned i = 0; i < MAX_MENUITEMS; ++i)
		button[i].setTexture(*tileset);

	sf::Vertex *v = &items[0];
	v[0].position = sf::Vector2f(0,0);
    v[1].position = sf::Vector2f(8000,0);
    v[2].position = sf::Vector2f(8000,32);
    v[3].position = sf::Vector2f(0,32);

	setVertexRect(0, 0, 27, 32);

	for (unsigned i = 0; i < 12; ++i)
	{
		setVertexRect(4+i*4, 68 + (i % 4) * 32, i / 4 * 32, 32);
	}
	last = def;

	setVertexRect(13*4, 68, 96, 128);
	setVertexRect(14*4, 68, 128, 128);
	setVertexRect(15*4, 68, 160, 128);
	setVertexRect(16*4, 196, 96, 32);
	setVertexRect(17*4, 36, 0, 32);// about
	setVertexRect(18*4, 196, 64, 32);// edit tools


	Vector2f buttonPositions[18] = {
		{0, 0},
		{32, 0},
		{64, 0},
		{192, 0},
		{224, 0},
		{256, 0},
		{288, 0},
		{686 + 32 * 12, 0},
		{-100, 0},
		{964, 0},
		{996, 0},
		{96, 0},
		{430 - 32, 0},
		{558 - 32, 0},
		{718 - 32, 0},
		{686 - 32, 0},
		{686 + 32 * 13, 0},
		{32 * 27, 0}
		
	};

	for (unsigned i = 0; i < MAX_MENUITEMS; i++)
	{
		if (i==12 || i==13 || i==14)
			setVertexPos(&items[4+i*4],buttonPositions[i].x, buttonPositions[i].y,128 );
		else
			setVertexPos(&items[4+i*4],buttonPositions[i].x, buttonPositions[i].y,32 );
	}

	selected = 13;
	select.setSize(Vector2f(128, 2));
	select.setPosition(items[14*4].position.x, 30);

}
void Menu::draw()
{
	window->draw(items, states);

	if (isPage(selected))
	{
		window->draw(select);
	}

}
int Menu::clicked()
{

	for (unsigned i = 1; i < MAX_MENUITEMS+1; ++i)
	{
		sf::Vertex *v = &items[i*4];
		if (mouse.pos.y >= 0 && mouse.pos.y < 32 && mouse.pos.x >= items[i*4].position.x && mouse.pos.x < items[i*4+1].position.x)
		{
			
	
			 v[0].color = colors[MENUITEMHOVER];
			 v[1].color = colors[MENUITEMHOVER];
			 v[2].color = colors[MENUITEMHOVER];
			 v[3].color = colors[MENUITEMHOVER];

			if (mouse.clickg)
			{ 
				mouse.clickg = 0;
				return i-1;
			}
		}
		else
		{
			v[0].color = Color(255,255,255,255);
			v[1].color = Color(255,255,255,255);
			v[2].color = Color(255,255,255,255);
			v[3].color = Color(255,255,255,255);
		}
	}

	return -1;
}

int Menu::hovered()
{
	for (unsigned i = 1; i < MAX_MENUITEMS+1; ++i)
	{
		if (mouse.pos.y >= 0 && mouse.pos.y < 32 && mouse.pos.x >= items[i*4].position.x && mouse.pos.x < items[i*4+1].position.x)
			return i-1;
	}
	return -1;
}

bool Menu::isPage(int buttonIndex)
{
	return buttonIndex == PAGE_CONFIG || buttonIndex == PAGE_GENERAL || buttonIndex == PAGE_SONG || buttonIndex == PAGE_INSTRUMENT || buttonIndex == PAGE_PIANOROLL;
}

void Menu::goToPage(int page)
{
	/* Pages have a bar below them showing which is currently displayed */
	if (isPage(page))
	{
		select.setPosition(items[4+page*4].position.x, 30);
		selected = page;
		
		/* Some pages have bigger buttons */
		if (page == PAGE_GENERAL || page == PAGE_SONG || page == PAGE_INSTRUMENT)
		{
			select.setSize(Vector2f(128, 2));
		}
		else
		{
			select.setSize(Vector2f(32, 2));
		}
	}

	/* Avoid buffered keypresses to stay */
	textEnteredCount = 0;


	switch (page)
	{
		case PAGE_CONFIG:
			state = config;
			break;
		case PAGE_GENERAL:
			state = generalEditor;
			break;
		case PAGE_SONG:
			state = songEditor;
			break;
		case PAGE_INSTRUMENT:
			state = instrEditor;
			break;
		case PAGE_PIANOROLL:
			state = pianoRoll;
			break;
	}

	state->updateFromFM();


}

void Menu::update()
{
	if (!mouse.clickLock2)
	{
		int menuItem = menu->clicked();
		if (menuItem > 0)
		{
			contextMenu = NULL;
		}
		switch (menuItem)
		{
			case 0: // new
				isSongModified ? popup->show(POPUP_NEWFILE) : song_clear();
				break;
			case 1: // open
				song_open();
				break;
			case 2: // save
				song_save();
				break;
			case 3: // back
				fm_setPosition(fm, 0, 0, 2);
				songEditor->moveY(0);
				break;
			case 4: // top pattern
				songEditor->moveY(0);
				break;
			case 5: // play
				song_playPause();
				break;
			case 6: // stop
				song_stop();
				break;
			case 10: // wave/mp3 export
				popup->show(POPUP_STREAMEDEXPORT);
				break;
			case 9: // midi export
				popup->show(POPUP_MIDIEXPORT);
				break;
			case 11: // save as
				song_saveas();
				break;
			case 12: // general
				goToPage(PAGE_GENERAL);
				break;
			case 13: // song
				goToPage(PAGE_SONG);
				break;
			case 14: // instrument
				goToPage(PAGE_INSTRUMENT);
				break;
			case 7: // config
				goToPage(PAGE_CONFIG);
				break;
			case 15: // piano roll
				goToPage(PAGE_PIANOROLL);
				break;
			case 16:
				popup->show(POPUP_ABOUT);
				break;
	
		}
	}
}