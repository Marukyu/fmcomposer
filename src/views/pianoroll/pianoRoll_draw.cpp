#include "pianoRoll.hpp"
#include "../settings/configEditor.hpp"
#include "../../gui/popup/popup.hpp"
#include "../../input/noteInput.hpp"
#include "../../gui/sidebar.hpp"

#define PIANOROLL_MAX_WALKBACK 256

void Pianoroll::drawMarkers(int order, int row)
{
	/* Draw beat lines */
	if (row%config->rowHighlight.value == 0)
	{
		int pos = getPos(order, row);
		cursor.setPosition(pos * 8, 0);

		if (config->rowHighlight.value > 1)
		{
			window->draw(cursor);
		}

		if (row == 0)
		{
			window->draw(cursor);
			time.setPosition((pos - row) * 8 + 4, 64 + (int)(charSize*1.5));
			time.setString(std::to_string((int)fm->channelStates[order][row].time / 60) + ":" + std::to_string((int)fm->channelStates[order][row].time % 60));
			window->draw(time);
		}
	}
	/* Draw new pattern line */
	if (row == 0)
	{
		marker.setString(std::to_string(order));
		marker.setPosition(getPos(order, 0) * 8 + 4, 64);
		window->draw(marker);
	}
}



void Pianoroll::draw()
{
	if (!moveNote)
	{
		displayNote = 0;
		oldNote = -1;
	}

	window->setView(view);
	hoveredCh=-1;
	hoveredOrder=-1;
	hoveredRow=-1;


	int row = (int)fm->row - PIANOROLL_MAX_WALKBACK;
	unsigned order = fm->order;

	if (!fm->playing && !popup->visible && mouseGlobal.x < 58 && mouseGlobal.y>=32)
	{
		int n = (int)(y + 950 - mouse.pos.y - 0.5) / 8 + 1;
		if (n >=0 && n<128)
		{
			int n = (int)(y + 950 - mouse.pos.y - 0.5) / 8 + 1;
			note.setString(noteName(n));
			displayNote = 1;
			if (mouse.clickg)
			{
				fm_playNote(fm, instrList->value, n, 0, sidebar->defNoteVol.value);
			}
		}
	}


	if (fm->playing)
		subrow = ((double)fm->frameTimer / ((60 / fm->diviseur) * (double)fm->sampleRate / fm->tempo)) * 8;

	int calcScrollX = windowWidth / 2 + 8 * getPos(order, row) + subrow + PIANOROLL_MAX_WALKBACK * 8 -57;

	int scrollX = max(windowWidth / 2 - 56, calcScrollX);
	int windowLeftX = scrollX - windowWidth / 2 + 57;

	view.setCenter(Vector2f(scrollX, (float)windowHeight / 2));

	mouse.pos = input_getmouse(view);
	while (row < 0)
	{
		if (order == 0)
		{
			row = 0;
			break;
		}
		order--;

		row += fm->patternSize[order];
	}
	
	unsigned squareCount = 0;

	while (order < fm->patternCount)
	{
		for (unsigned ch = 0; ch < FM_ch; ch++)
		{

			if (squareCount >= PIANOROLL_MAX_NOTES)
				goto end;

			if (fm->pattern[order][row][ch].note >= 128)
				continue;

			int noteLength = 0;
			int tempRow = row;
			int tempOrder = order;

			do
			{
				tempRow++;
				noteLength += 8;
				if (tempRow >= fm->patternSize[tempOrder])
				{
					tempRow = 0;
					tempOrder++;
					if (tempOrder >= fm->patternCount || noteLength > PIANOROLL_MAX_WALKBACK * 8)
						break;
				}
			} while (fm->pattern[tempOrder][tempRow][ch].note == 255);




			int sX = getPos(order, row) * 8;

			/* Don't display notes finishing before the window left side */
			if (sX + noteLength < windowLeftX)
				continue;

			int sY = y + 950 - (int)fm->pattern[order][row][ch].note * 8;

			sf::Vertex *quad = &vertices[squareCount * 4];

			quad[0].position=sf::Vector2f(sX, sY);
			quad[1].position=sf::Vector2f(sX+noteLength, sY);
			quad[2].position=sf::Vector2f(sX+noteLength, sY+7);
			quad[3].position=sf::Vector2f(sX, sY+7);



			Color squareColor = instrColors[(fm->pattern[order][row][ch].instr)%instrColors.size()];
				
			if (instrumentToShow != -1 && fm->pattern[order][row][ch].instr != instrumentToShow)
			{
				squareColor.a = 30;
			}
			
			quad[0].color = squareColor;
			quad[1].color = squareColor;
			quad[2].color = squareColor;
			quad[3].color = squareColor;


			

			if (!fm->playing && !popup->visible)
			{
				
				if (mouseGlobal.x >= 58 && !contextMenu && mouse.pos.x >= sX && mouse.pos.x < sX + noteLength && mouseGlobal.x < (int)windowWidth - 250)
				{
					if ((resizeNote || moveNote || mouse.pos.y >= sY  && mouse.pos.y < sY + 8) && ch==hoveredCh &&  !contextMenu && mouse.pos.x >= sX && mouse.pos.x < sX + noteLength )
					{
						bracket.setPosition(sX, sY-5);
						window->draw(bracket);
						bracket.setPosition(sX+noteLength, sY-5);
						window->draw(bracket);
					}

					if (mouse.pos.y >= sY  && mouse.pos.y < sY + 8)
					{

					
						if (instrumentToShow == -1 || instrumentToShow == fm->pattern[order][row][ch].instr) {
							/* Resize the note */
							if (mouse.pos.x<sX + 3 || mouse.pos.x > sX + noteLength - 4)
							{
								mouse.cursor = CURSOR_HRESIZE;
							
								if (mouse.clickg)
								{
									/* Resize left side */
									if (mouse.pos.x < sX + 3) {
										resizeNote=1;
										oldPos = getPos(order, row);
										noteMoveLimit = oldPos + noteLength/8;
									}
									/* Resize right side */
									else {
										resizeNote=2;
										oldPos = getPos(order, row)+noteLength/8;
										noteMoveLimit = getPos(order, row);
									}

								
								}
							}

						

							/* Highlight the note */
							
							note.setString(noteName(fm->pattern[order][row][ch].note)+" vol "+to_string(fm->pattern[order][row][ch].vol));
							if (!displayNote)
							{

								oldNote = fm->pattern[order][row][ch].note;

								displayNote = 1;
								if (!fm->playing && (hoveredRow != row || hoveredCh != ch))
								{
									hoveredCh = ch;
									hoveredRow = row;
									hoveredOrder = order;
									noteInstr = fm->pattern[order][row][ch].instr;
									noteVol = fm->pattern[order][row][ch].vol;
								}
							}

							if (mouse.clickg)
							{
								selectedRow = row;
								selectedOrder = order;
								selectedCh = ch;
								deltaX = mouse.pos.x + subrow;
								deltaY = mouse.pos.y - y;
								refRow = fm->row;
								refOrder = fm->order;
								if (oldNote >= 0 && !fm->playing && clickedElem == oldNote + 127 * selectedRow + 32385 * noteInstr)
								{
									moveNote = 1;
									configurePreviewChannel(0);
									fm_playNote(fm, noteInstr, oldNote, 0, noteVol);
								}
							}

							/* Delete the note */
							if (keyboard.del || keyboard.equal)
							{
								fm->pattern[order][row][ch].note = 128;
							}
							/* Show context menu */
							if (mouse.clickd)
							{
								actionsMenu.show();
							}

							if (!resizeNote || (selectedCh == ch)) {
								bracket.setPosition(sX, sY-5);
								window->draw(bracket);
								bracket.setPosition(sX+noteLength, sY-5);
								window->draw(bracket);
								
							}

							
							
							
						}
					}

					
				}
				if ( selectedCh == ch && (instrumentToShow == -1 || instrumentToShow == fm->pattern[order][row][ch].instr))
				{
					quad[0].color = colors[BLOCKTEXT];
					quad[1].color = colors[BLOCKTEXT];
					quad[2].color = colors[BLOCKTEXT];
					quad[3].color = colors[BLOCKTEXT];
				}

			}
			

	
			if (sX < scrollX -(int)windowWidth/2 + 57 && (instrumentToShow==-1 || instrumentToShow == fm->pattern[order][row][ch].instr))
			{
				pressedNotes[(int)(950 + y - sY) / 8] = 1;
			}

			squareCount++;
		}

		
		drawMarkers(order, row);

		if (++row >= fm->patternSize[order])
		{
			row = 0;
			order++;
		}

	}
end:

	for (unsigned i = squareCount; i < PIANOROLL_MAX_NOTES; i++)
	{
		sf::Vertex *quad = &vertices[i * 4];
		quad[0].color=invisible;
		quad[1].color=invisible;
		quad[2].color=invisible;
		quad[3].color=invisible;
	}

	window->draw(vertices);

	window->setView(globalView);
	mouse.pos = input_getmouse(globalView);


	for (int i = 0; i < 10; i++)
	{
		piano.setPosition(0, y + i * 96);
		window->draw(piano);
	}



	static const int keyData[12 * 2] = {/*note types*/0, 7, 1, 7, 2, 3, 7, 4, 7, 5, 7, 6,/*note y offsets*/14, 16, 28, 32, 41, 54, 56, 69, 72, 83, 88, 97 };

	for (unsigned i = 0; i < 128; i++)
	{
		if (pressedNotes[i])
		{
			pressedNotes[i] = 0;
			notePressed[keyData[i % 12]].setPosition(0, y + 960 - (i / 12) * 96 - keyData[12 + i % 12]);
			window->draw(notePressed[keyData[i % 12]]);

		}
	}

	if (displayNote && !popup->visible)
	{
		noteBg.setSize(Vector2f(note.getLocalBounds().width + 2, charSize + 2));
		noteBg.setPosition(mouse.pos.x + 16 - 1, mouse.pos.y + 16);
		note.setPosition(mouse.pos.x + 16, mouse.pos.y + 16);
		window->draw(noteBg);
		window->draw(note);

	}

	
	cache.setPosition(windowWidth - 250, 0);
	window->draw(cache);
	for (int i = instrList->scroll; i < min<int>(instrList->scroll + instrList->maxrows, instrList->text.size()); i++)
	{

		legend[i].setPosition(windowWidth - 233, 375 + (i - instrList->scroll) * 17);
		window->draw(legend[i]);
	}

	showAllInstruments.draw();
}