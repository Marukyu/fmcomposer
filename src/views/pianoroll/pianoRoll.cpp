#include "pianoRoll.hpp"
#include "../pattern/songEditor.hpp"
#include "../../gui/mainmenu.hpp"
#include "../../input/noteInput.hpp"

extern fmsynth* phanoo;
extern Texture *tileset;

extern List *instrList;
extern SongEditor *songEditor;
Pianoroll* pianoRoll;

extern CSimpleIniA ini_theme;



Pianoroll::Pianoroll(int _y) :cache(Vector2f(64, 2500)), marker("", font, (int)(1.5*charSize)), note("", font, charSize), time("", font, charSize), clickedElem(0),
moveNote(0), displayNote(0), oldNote(-1), y(_y), subrow(0), isScrolling(false), instrumentToShow(-1), showAllInstruments(10, 89, ICON_MD_REMOVE_RED_EYE, 13), wasPlaying(0), resizeNote(0),
bracket(Vector2f(1,18))
{
	bracket.setFillColor(colors[BLOCKTEXT]);
	bracket.setOutlineColor(colors[BACKGROUND]);
	bracket.setOutlineThickness(1);
	cursor.setFillColor(colors[PATTERNSELECTION]);
	cursor.setSize(Vector2f(1, 4000));
	noteBg.setFillColor(colors[BACKGROUND]);

	cache.setFillColor(colors[BACKGROUND]);
	marker.setColor(colors[BLOCKTEXT]);
	time.setColor(colors[BLOCKTEXT]);
	note.setColor(colors[BLOCKTEXT]);
	piano.setTexture(*tileset);
	piano.setTextureRect(IntRect(0, 64, 57, 96));

	invisible = sf::Color(0,0,0,0);

	vertices.setPrimitiveType(sf::Quads);
    vertices.resize(PIANOROLL_MAX_NOTES * 4);

	for (int i = 0; i < 8; i++)
	{
		notePressed[i].setTexture(*tileset);
	}
	notePressed[6].setTextureRect(IntRect(0, 160, 57, 14));
	notePressed[5].setTextureRect(IntRect(0, 174, 57, 14));
	notePressed[4].setTextureRect(IntRect(0, 188, 57, 15));
	notePressed[3].setTextureRect(IntRect(0, 203, 57, 13));
	notePressed[2].setTextureRect(IntRect(0, 216, 57, 14));
	notePressed[1].setTextureRect(IntRect(0, 229, 57, 14));
	notePressed[0].setTextureRect(IntRect(0, 243, 57, 14));
	notePressed[7].setTextureRect(IntRect(0, 257, 38, 8));

	memset(pressedNotes, 0, 128);


	unsigned colorCount = atoi(ini_theme.GetValue("pianoRollColors", "colorCount", "13"));

	instrColors.resize(colorCount);

	for (unsigned i = 0; i < colorCount; i++)
	{
		instrColors[i] = string2color(ini_theme.GetValue("pianoRollColors", std::to_string(i).c_str(), "127,127,127"));
	}

	

	actionsMenu.add("Show only this instrument");
}


void Pianoroll::updateFromFM()
{
	displayNote = 0;
	oldNote = -1;
	legend.resize(fm->instrumentCount);
	for (int i = 0; i < legend.size(); i++)
	{
		legend[i].setFillColor(instrColors[i%instrColors.size()]);
		legend[i].setSize(Vector2f(17, 16));
		legend[i].setOutlineColor(colors[BLOCKTEXT]);
	}

	instrumentToShow= -1;
}

void Pianoroll::doubleClick()
{

	if (oldNote >= 0 && clickedElem == oldNote + 127 * hoveredRow + 32385 * noteInstr && clickedElem > 0)
	{
		menu->goToPage(PAGE_SONG);
		fm_setPosition(fm, hoveredOrder, hoveredRow, 0);

		songEditor->updateFromFM();
		songEditor->setX(hoveredCh * 4);
		songEditor->moveY(hoveredRow);
		mouse.clickLock2 = 1;
	}
	else if (oldNote >= 0)
	{
		moveNote = 1;
		fm_playNote(fm, noteInstr, oldNote, 0, noteVol);
	}
	clickedElem = -1;
}

void getOrderRowFromPos(int *order, int *row, int pos)
{
	*row = 0;
	*order = 0;
	while (pos > 0 && *order < fm->patternCount - 1 && pos >= fm->patternSize[*order])
	{
		pos -= fm->patternSize[*order];
		*row = 0;
		(*order)++;
	}

	if (pos > 0)
	{
		(*row) += pos;
	}
}


int Pianoroll::getPos(int order, int row)
{
	int pos = 0;
	for (int i = 0; i < order; i++)
	{
		pos += fm->patternSize[i];
	}
	pos += row;
	return pos;
}

void Pianoroll::update()
{
	if (contextMenu == &actionsMenu)
	{
		handleContextMenu();
	}

	if (resizeNote && selectedCh>=0)
	{
		mouse.pos = input_getmouse(view);
		mouse.cursor = CURSOR_HRESIZE;
		int pos = (int)round(mouse.pos.x/8)+(resizeNote==2);

		if (pos != oldPos && (resizeNote==1 && pos < noteMoveLimit && oldPos < noteMoveLimit || resizeNote==2 && pos > noteMoveLimit && oldPos > noteMoveLimit))
		{
			//printf("%d %d\n", pos, noteMoveLimit);
			int oldRow, oldOrder;
			getOrderRowFromPos(&oldOrder, &oldRow, oldPos);
			int newRow, newOrder;
			getOrderRowFromPos(&newOrder, &newRow, pos);

			if(fm->pattern[newOrder][newRow][selectedCh].note==128)
				fm->pattern[newOrder][newRow][selectedCh].note=255;

			Cell temp = fm->pattern[newOrder][newRow][selectedCh];
			fm->pattern[newOrder][newRow][selectedCh] = fm->pattern[oldOrder][oldRow][selectedCh];
			fm->pattern[oldOrder][oldRow][selectedCh]=temp;
			//memset(&fm->pattern[oldOrder][oldRow][noteCh], 255, sizeof(Cell));
			oldPos = pos;
		}


		

	}
	if (Mouse::isButtonPressed(Mouse::Left))
	{


		if (moveNote && !resizeNote)
		{
			mouse.pos = input_getmouse(view);
			oldNote = fm->pattern[selectedOrder][selectedRow][selectedCh].note;
			fm->pattern[selectedOrder][selectedRow][selectedCh].note = clamp((y + 950 - mouse.pos.y - 0.5) / 8 + 1,0,127);

			if (oldNote != fm->pattern[selectedOrder][selectedRow][selectedCh].note)
			{
				oldNote = fm->pattern[selectedOrder][selectedRow][selectedCh].note;
				configurePreviewChannel(0);
				fm_playNote(fm, noteInstr, fm->pattern[selectedOrder][selectedRow][selectedCh].note, 0, noteVol);
			}
		}
		else if (isScrolling && oldNote == -1 && mouse.pos.y > 32 && mouse.pos.y < windowHeight && mouse.pos.x < windowWidth - 250)
		{

			if (fm->playing)
			{
				song_pause();
				wasPlaying = 1;
			}
			y = mouse.pos.y - deltaY;




			int row, order, newPos = getPos(refOrder, refRow) - (mouse.pos.x - deltaX) / 8;

			subrow = ((deltaX - mouse.pos.x) % 8);

			getOrderRowFromPos(&order, &row, newPos);

			fm_setPosition(fm, order, row, 0);
		}
	}

	if (mouse.scroll != 0 && !instrList->hover())
	{
		if (keyboard.shift)
		{
			int row, order, pos = getPos(fm->order, fm->row) + mouse.scroll * 4;

			getOrderRowFromPos(&order, &row, pos);

			fm_setPosition(fm, order, row, 0);
		}
		else
			y += mouse.scroll * 20;

	}

	if (showAllInstruments.clicked())
	{
		instrumentToShow=-1;
		for (int i = 0; i < legend.size(); i++)
		{
			legend[i].setOutlineThickness(0);
		}
	}
}




void Pianoroll::focusInstrument(int id)
{
	if (id < legend.size())
	{
		for (unsigned i = 0; i < legend.size(); i++)
		{
			legend[i].setOutlineThickness(0);
		}
		instrumentToShow=id;
		legend[id].setOutlineThickness(1);
		instrList->select(id);
	}
}

void Pianoroll::handleEvents()
{
	handleNotePreview(1);
	switch (evt.type)
	{
		case Event::MouseButtonPressed:
			if (evt.mouseButton.button == Mouse::Left)
			{
				
				if (mouseGlobal.y > 32 && mouseGlobal.x > 57 && mouseGlobal.x < windowWidth - 215)
				{
					deltaX = mouse.pos.x + subrow;
							deltaY = mouse.pos.y - y;
							refRow = fm->row;
							refOrder = fm->order;
					isScrolling=true;
				}

				/* Click on colours to select an instrument to show */
				if (mouseGlobal.y > legend[0].getPosition().y && mouseGlobal.x > windowWidth - 250 && mouseGlobal.x < windowWidth - 215)
				{
					int instrumentIndex = (mouseGlobal.y - legend[0].getPosition().y)/17+instrList->scroll;
					focusInstrument(instrumentIndex);
				}


				

				if (mouse.dblClick <= 0)
				{
					mouse.dblClick = 15;
					clickedElem = oldNote + 127 * hoveredRow + 32385 * noteInstr;
				}
				else
				{
					doubleClick();
				}

				
			}
			break;
		case Event::MouseButtonReleased:
			if (evt.mouseButton.button == Mouse::Left)
			{
				resizeNote=0;
				isScrolling=false;
				if (wasPlaying)
				{
					song_play();
					wasPlaying = 0;
				}
				if (moveNote)
				{
					moveNote = 0;
					fm_stopNote(fm, 0);
				}
			}
			break;

	}
}

void Pianoroll::resetView(int width, int height)
{
	view.reset(FloatRect(0.f, 0.f, width, height));
	view.setViewport(FloatRect(0, 0, 1, 1));
	showAllInstruments.setPosition(windowWidth - 233, 375 -20);

}