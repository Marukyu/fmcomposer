#include "songEditor.hpp"
#include "../settings/configEditor.hpp"
#include <ctype.h>
#include "../../gui/drawBatcher.hpp"
#include "../../gui/sidebar.hpp"


SongEditor *songEditor;



SongEditor::SongEditor() : rowNumbers("", font, charSize), playCursor(Vector2f(CH_WIDTH*FM_ch, ROW_HEIGHT))
, scroll(0), add(0, 52, ICON_MD_ADD,14,0), patText("Patterns", font, charSize)
, patSize(1070, 700, 256, 1, "Pattern size", 128, 150), resize(1230, 700, "Resize"), patSlider(0, 255, 0, 0.1, 1050 - 1, 169, 600, false)
, movePat(-1), selectedRow(0), selectedChannel(0), selectedType(0), scrollX(0),
expand(1070, 730, "Scale x2"), shrink(1150, 730, "Scale /2"), resetMute(0, 89, ICON_MD_CLOSE,14), mouseXpat(0), mouseYpat(0), searched(false),
selected(false), patternList(120, 52), zoom(1),
patHSlider(0, 255, 0, 0.1, 0, 169, 600, false, true)
{
	fm_insertPattern(fm, config->patternSize.value, 0);

	patternList.add(std::to_string(patternList.elementCount()));


	rowNumbers.setPosition(4, 0);
	rowNumbers.setColor(colors[PATTERNROWNUMBERS]);

	for (unsigned ch = 0; ch < FM_ch; ++ch)
	{
		channelHead[ch] = ChannelHead(ch);

		text[ch][0].setCharacterSize(charSize);
		text[ch][1].setCharacterSize(charSize);
		text[ch][2].setCharacterSize(charSize);
		text[ch][3].setCharacterSize(charSize);


		text[ch][0].setFont(font);
		text[ch][1].setFont(font_condensed);
		text[ch][2].setFont(font_condensed);
		text[ch][3].setFont(font_condensed);

		text[ch][0].setPosition(ch*CH_WIDTH, 0);
		text[ch][1].setPosition(ch*CH_WIDTH + 33, 0);
		text[ch][2].setPosition(ch*CH_WIDTH + 50, 0);
		text[ch][3].setPosition(ch*CH_WIDTH + 70, 0);
		text[ch][0].setColor(colors[PATTERNNOTE]);
		text[ch][1].setColor(colors[PATTERNINSTRUMENT]);
		text[ch][2].setColor(colors[PATTERNVOLUME]);
		text[ch][3].setColor(colors[PATTERNEFFECT]);
	}
	channelHead[0].record.selected = 1;

	playCursor.setFillColor(colors[PATTERNSELECTION]);

	patText.setColor(colors[BLOCKTEXT]);
	patText.setPosition(patternList.x - 80, 52);

	buildContextMenus();

	resetHistory();
	playCursor.setPosition(0, fm->row*ROW_HEIGHT);
	setXscroll(0);
	setZoom(0.1*atoi(ini_config.GetValue("config", "patternZoomLevel", "10")));
	bars.setPrimitiveType(sf::Quads);
	
	bars.resize(4*FM_ch+4*(fm_getPatternSize(fm,fm->order)/config->rowHighlight.value));
}

void SongEditor::updatePatternLines()
{
	int iters = (int)round(ceil((float)fm_getPatternSize(fm,fm->order)/config->rowHighlight.value));
	bars.resize(4*FM_ch+4*iters+4);

	sf::Vertex *quad = &bars[0];
	quad[0].position = Vector2f(0,0);
	quad[1].position = Vector2f(CH_WIDTH*FM_ch,0);
	quad[2].position = Vector2f(CH_WIDTH*FM_ch,fm_getPatternSize(fm, fm->order)*ROW_HEIGHT);
	quad[3].position = Vector2f(0,fm_getPatternSize(fm, fm->order)*ROW_HEIGHT);

	quad[0].color = colors[PATTERNBG];
	quad[1].color = colors[PATTERNBG];
	quad[2].color = colors[PATTERNBG];
	quad[3].color = colors[PATTERNBG];
	
	for (unsigned i = 0; i< iters; ++i)
	{

		sf::Vertex *quad = &bars[i*4+4];
		quad[0].position = Vector2f(0,i*config->rowHighlight.value*ROW_HEIGHT);
		quad[1].position = Vector2f(CH_WIDTH*FM_ch,i*config->rowHighlight.value*ROW_HEIGHT);
		quad[2].position = Vector2f(CH_WIDTH*FM_ch,i*config->rowHighlight.value*ROW_HEIGHT+ROW_HEIGHT);
		quad[3].position = Vector2f(0,i*config->rowHighlight.value*ROW_HEIGHT+ROW_HEIGHT);

		quad[0].color = colors[PATTERNBGODD];
		quad[1].color = colors[PATTERNBGODD];
		quad[2].color = colors[PATTERNBGODD];
		quad[3].color = colors[PATTERNBGODD];
	}

	for (unsigned i = 0; i < FM_ch; ++i)
	{
		sf::Vertex *quad = &bars[4*iters+i*4+4];
		quad[0].position = Vector2f(i*CH_WIDTH,0);
		quad[1].position = Vector2f(i*CH_WIDTH+1,0);
		quad[2].position = Vector2f(i*CH_WIDTH+1,fm_getPatternSize(fm, fm->order)*ROW_HEIGHT);
		quad[3].position = Vector2f(i*CH_WIDTH,fm_getPatternSize(fm, fm->order)*ROW_HEIGHT);

		quad[0].color = colors[PATTERNCOLUMSEPARATOR];
		quad[1].color = colors[PATTERNCOLUMSEPARATOR];
		quad[2].color = colors[PATTERNCOLUMSEPARATOR];
		quad[3].color = colors[PATTERNCOLUMSEPARATOR];
	}
	
}

void SongEditor::draw()
{

	

	static unsigned wasPlaying = 0;

	if (fm->playing != wasPlaying)
	{
		playCursor.setPosition(0, fm->row*ROW_HEIGHT);
		selection.bg.setPosition(selection.bg.getPosition().x, fm->row*ROW_HEIGHT);
		selectedRow = fm->row;
		wasPlaying = fm->playing;
		setScroll(fm->row);
	}

	/* Pattern scrollbar untouched : set to scroll*/
	if (!patSlider.update())
	{
		if (fm->order < fm->patternCount)
			patSlider.setValue(255.0*(scroll - halfPatternHeightView + 6) / (fm_getPatternSize(fm, fm->order) - halfPatternHeightView + 6 - 1));

		
		if (fm->playing)
		{
			subrow = ((float)fm->frameTimer / ((60 / fm->diviseur) * (float)fm->sampleRate / fm->tempo));
			setScroll(fm->row + subrow);
		}
	}
	// playing : view follows the song
	if (fm->playing)
	{


		selection.bg.setPosition(selection.bg.getPosition().x, scroll*ROW_HEIGHT);
		playCursor.setPosition(0, scroll*ROW_HEIGHT);
		selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));

		selectedRow = fm->row;
	}

	// current pattern has changed : display the new pattern
	static unsigned currentPattern = 999;
	if (currentPattern != fm->order)
	{
		patternList.select(fm->order);

		currentPattern = fm->order;
		for (unsigned ch = 0; ch < FM_ch; ++ch)
			updateChannelData(ch);

		patSize.setValue(fm_getPatternSize(fm, fm->order));

		updateRowCount();
	}

	

	window->setView(patternTopView);

	drawBatcher.initialize();
	for (unsigned ch = scrollX; ch < scrollX2; ++ch)
	{
		channelHead[ch].draw();
	}
	drawBatcher.draw();


	window->setView(patternView);


	window->draw(bars);


	for (unsigned ch = scrollX; ch < scrollX2; ++ch)
	for (unsigned j = 0; j < 4; ++j)
		window->draw(text[ch][j]);

	window->draw(playCursor);
	selection.draw();

	window->setView(patNumView);
	
	window->draw(rowNumbers);

	window->setView(globalView);
	patHSlider.draw();
	resetMute.draw();
	add.draw();
	window->draw(patText);
	patternList.draw();
}




void SongEditor::update()
{

	if (mouse.pos.y >= 168 && mouse.pos.x < windowWidth - 215)
	{
		if (mouse.scroll != 0)
		{
			// mouse wheel scroll
			if (keyboard.shift)
				setXscroll(scrollXsmooth + mouse.scroll*CH_WIDTH);
			else if (keyboard.ctrl)
			{
				setZoom(zoom + mouse.scroll*0.1);
			}
			else if (!fm->playing && mouse.pos.y < windowHeight - 16)
			{
				setScroll(max(halfPatternHeightView - 6, scroll - mouse.scroll * 2));
			}
			else if (mouse.pos.y >= windowHeight - 16)
			{
				setXscroll(scrollXsmooth + mouse.scroll*CH_WIDTH);
			}
		}
		else if (mouse.hscroll != 0)
		{
			setXscroll(scrollXsmooth + mouse.hscroll*CH_WIDTH);
		}
	}

	if ((selection.moving || Mouse::isButtonPressed(Mouse::Left) && selected))
	{
		// scroll right
		if (mouse.pos.x > +(int)windowWidth - 231)
		{
			setXscroll(scrollXsmooth + CH_WIDTH);
		}
		// scroll left
		else if (mouse.pos.x < 32)
		{
			setXscroll(scrollXsmooth - CH_WIDTH);
		}

		// scroll down
		if (mouse.pos.y >(int)windowHeight - 20)
		{
			setScroll(scroll + (mouse.pos.y - (int)windowHeight + 20) / 20);
		}
		// scroll up
		else if (mouse.pos.y < 170)
		{
			setScroll(scroll + (mouse.pos.y - 170) / 20);
		}
	}

	mousePattern = input_getmouse(patternView);


	// move selection
	if (selection.moving)
	{

		updateMousePat();
		selection.moveRect.setPosition(((mouseXpat - selection.deltaX) / 4)*CH_WIDTH + selectedType*COL_WIDTH, (mouseYpat - selection.deltaY)*ROW_HEIGHT);
	}
	// resize selection
	else if (Mouse::isButtonPressed(Mouse::Left) && selected &&focusedElement == &patternView && !fm->playing)
	{ // selection

		selection.resizeAbsolute(selectedChannel * 4 + selectedType, selectedRow, mousePattern.x, mousePattern.y);

	}


	// add pattern
	if (add.clicked())
	{
		fm_insertPattern(fm, patSize.value, fm->patternCount);
		fm_setPosition(fm, fm->patternCount - 1, 0, 0);
		moveY(0);
		history.push_back(vector<historyElem>());
		currentHistoryPos.push_back(0);
		patternList.add(std::to_string(patternList.elementCount()));
		patternList.select((int)patternList.elementCount() - 1);
		songModified(1);
	}
	else if (resetMute.clicked())
	{
		for (unsigned ch = 0; ch < FM_ch; ++ch)
		{
			channelHead[ch].mute.selected = 0;
			channelHead[ch].solo.selected = 0;
			updateMutedChannels();
		}

	}




	if (contextMenu == &patMenu)
	{
		handlePatternContextMenu();
	}
	else if (contextMenu == &patListMenu)
	{
		handlePatternListContextMenu();
	}
	else if (contextMenu == &editTools)
	{
		int choice = contextMenu->clicked();
		switch (choice)
		{
			case 0:
				undo();
				break;
			case 1:
				redo();
				break;
			case 2:
				popup->show(POPUP_SEARCH);
				break;
			case 3:
				setZoom(zoom + 0.2);
				break;
			case 4:
				setZoom(zoom - 0.2);
				break;
		}
	}

	

	int patternListHovered = patternList.getElementHovered();
	if (!contextMenu && patternListHovered > -1 && Mouse::isButtonPressed(Mouse::Left) && movePat == -1)
	{
		movePat = patternListHovered;
	}
	if (Mouse::isButtonPressed(Mouse::Left) && patternList.selected && patternListHovered > -1 && movePat != patternListHovered && movePat != -1)
	{
		mouse.cursor = CURSOR_SWAP;
	}

	
	patternList.update();


	// top pattern events
	mouse.pos = input_getmouse(patternTopView);

	handleChannelHeads();

	mouse.pos = input_getmouse(globalView);
	if (patHSlider.update())
	{
		setXscroll(patHSlider.value*(FM_ch*CH_WIDTH - ((int)(windowWidth - 231))+32) / 255, false);
	}

	// border events
	mouse.pos = input_getmouse(borderView);

	/* Pattern scrollbar moved: set scroll*/
	if (patSlider.update())
	{
		setScroll(patSlider.value*(fm_getPatternSize(fm, fm->order) - halfPatternHeightView + 6 - 1) / 255 + halfPatternHeightView - 6);
	}




	patSize.update();
	if (resize.clicked())
	{ // resize pattern
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		fm_resizePattern(fm, fm->order, patSize.value, 0);

		updateScrollbar();
		updateFromFM();
		songModified(1);
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
	}



	if (expand.clicked() && fm_getPatternSize(fm, fm->order) <= 128)
	{
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		fm_resizePattern(fm, fm->order, fm_getPatternSize(fm, fm->order) * 2, 1);

		updateScrollbar();
		updateFromFM();
		songModified(1);
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
	}
	else if (shrink.clicked() && fm_getPatternSize(fm, fm->order) >= 2)
	{
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));

		fm_resizePattern(fm, fm->order, fm_getPatternSize(fm, fm->order) / 2, 1);
		updateFromFM();
		updateScrollbar();
		songModified(1);
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
		saveToHistory(0, fm_getPatternSize(fm, fm->order));
	}
}

void SongEditor::updateMutedChannels()
{
	int oneSolo = 0;
	for (unsigned ch = 0; ch < FM_ch; ++ch)
	{
		if (channelHead[ch].solo.selected)
		{
			oneSolo = 1;
			break;
		}
	}
	if (oneSolo)
	{
		for (unsigned ch = 0; ch < FM_ch; ++ch)
		{
			fm->ch[ch].muted = !channelHead[ch].solo.selected;
		}
	}
	else
	{
		for (unsigned ch = 0; ch < FM_ch; ++ch)
		{
			fm->ch[ch].muted = channelHead[ch].mute.selected;
		}
	}

}

void SongEditor::setXscroll(int value, bool updateSlider)
{
	scrollXsmooth=clamp(value,0,FM_ch*CH_WIDTH - ((int)(windowWidth - 231))+32*zoom);
	scrollX = scrollXsmooth/CH_WIDTH;
	scrollX2 = min(FM_ch, scrollX + (windowWidth - 231) / CH_WIDTH + 2);

	patternView.setCenter((float)windowWidth / 2 + scrollXsmooth, patternView.getCenter().y);
	patternTopView.setCenter((float)windowWidth / 2 + scrollXsmooth, (float)windowHeight / 2 - 85);


	if (updateSlider)
		patHSlider.setValue((float)scrollXsmooth / (FM_ch*CH_WIDTH - ((int)(windowWidth - 231))+32*zoom) * 255);

	//setXscroll(patHSlider.value*(FM_ch-((int)windowWidth-231)/CH_WIDTH)/255);
}

void SongEditor::recordFromKeyboard(int note, int volume, int channel, int isFromMidi)
{
	//channel+=selectedChannel;

	if (channel >= FM_ch)
		return;

	if ((!isFromMidi && !Keyboard::isKeyPressed(Keyboard::LControl) || isFromMidi) && selectedType == 0 && focusedElement != instrList)
	{
		selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
		//selectCursor.setPosition(channel*100,fm->row*ROW_HEIGHT);

		if (note < 128 || note == 128 && fm->pattern[fm->order][fm->row][channel].note == 255 && isFromMidi)
		{
			moveCursorAfterDataEntered();

			saveToHistory(fm->row, 1);
			saveToHistory(fm->row, 1);

			unsigned char newVol = fm->pattern[fm->order][fm->row][channel].vol;

			if (newVol == 255)
			{
				newVol = note == 128 ? 255 : (isFromMidi ? volume / 1.282828 : volume);
			}

			fm_write(fm, fm->order, fm->row, channel, {
				note,
				note == 128 ? 255 : instrList->value,
				newVol,
				255,
				255
			});

			saveToHistory(fm->row, 1);
			saveToHistory(fm->row, 1);
			songModified(1);

			if (sidebar->editingStep.value != 0)
			{
				int diff = fm->row + sidebar->editingStep.value - fm_getPatternSize(fm, fm->order);
				if (diff>=0)
				{
					if (fm->order < fm->patternCount-1)
					{
						fm_setPosition(fm, fm->order+1, diff, 0);
						moveY( fm->row);
					}

				}
				else
				{
					moveY( fm->row+sidebar->editingStep.value);
				}
				
			}

		}

		updateChannelData(channel);

	}
}

void SongEditor::reset()
{
	resetHistory();
	for (int ch = 0; ch < FM_ch; ch++)
	{
		channelHead[ch].solo.selected = channelHead[ch].mute.selected = 0;
	}
	updateMutedChannels();
	setScroll(0);
	updateScrollbar();
	selected = 0;
	selection.moving = 0;
	selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
	updateFromFM();
	patternList.select(fm->order);

}




bool SongEditor::isMouseHoverPattern()
{
	return mouseGlobal.y >= 168 && mouseGlobal.y < windowHeight - 16 && mouseGlobal.x < windowWidth - 231;
}

void SongEditor::moveY(int pos)
{

	fm_setPosition(fm, fm->order, pos, fm->playing ? 2 : 0);

	selectedRow = fm->row;
	mouseYpat = selectedRow;
	selection.bg.setPosition(mouseXpat*COL_WIDTH, selectedRow*ROW_HEIGHT);
	selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
	setScroll(fm->row);

	playCursor.setPosition(0, (int)fm->row*ROW_HEIGHT);
}

void SongEditor::setX(int channel)
{
	int pos = clamp(channel, 0, FM_ch * 4 - 1);

	mouseXpat = pos;
	selectedChannel = pos / 4;
	selectedType = pos % 4;
	selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
	selection.bg.setPosition(selectedChannel*CH_WIDTH + selectedType*COL_WIDTH, fm->row*ROW_HEIGHT);
	updateRecordChannels();
	setXscroll((selectedChannel - ((int)windowWidth - 215 - 32) / CH_WIDTH / 2)*CH_WIDTH);
}

void SongEditor::moveXrelative(int offset)
{
	setX(selectedChannel * 4 + selectedType + offset);
	moveY(selectedRow);
}

void SongEditor::resetView(int width, int height)
{
	patternView.reset(FloatRect(0.f, 0.f, width, height));
	patternViewBaseSize.x = width;
	patternViewBaseSize.y = height;
	patternView.setViewport(FloatRect((32 * zoom) / width, 0.22f * 768 / height, 1, 1));

	patNumView.reset(FloatRect(0.f, 0.f, width, height));
	patNumView.setViewport(FloatRect(0, 0.22f * 768 / height, 1, 1));
	patternTopView.reset(FloatRect( 0.f, 0.f, width, height));
	patternTopView.setViewport(FloatRect((32 * zoom) / width, 0, 1, 1));
	setXscroll(scrollXsmooth);

	resize.setPosition(1230, instrList->bg.getPosition().y + instrList->maxrows * 17 + 20);
	expand.setPosition(1070, instrList->bg.getPosition().y + instrList->maxrows * 17 + 50);
	shrink.setPosition(1150, instrList->bg.getPosition().y + instrList->maxrows * 17 + 50);
	patSize.setPosition(1070, instrList->bg.getPosition().y + instrList->maxrows * 17 + 20);

	patSlider.setSize(windowHeight - 169);
	patHSlider.setSize(windowWidth - 231);
	patHSlider.setPosition(0, windowHeight - 16);
	updateScrollbar();

}

void SongEditor::setScroll(float pos)
{
	scroll = clamp(pos, 0, fm_getPatternSize(fm, fm->order) - 1);
	patternView.setCenter(patternView.getCenter().x, (int)round(max(-6, scroll - halfPatternHeightView)*ROW_HEIGHT) + (float)windowHeight / 2);
	patNumView.setCenter((float)windowWidth / 2, (int)round(max(-6, scroll - halfPatternHeightView)*ROW_HEIGHT) + (float)windowHeight / 2);
}

void SongEditor::updateScrollbar()
{
	patternHeightView = (int)round(windowHeight - 169) / ROW_HEIGHT;
	halfPatternHeightView = (int)round((windowHeight - 169)*0.33) / ROW_HEIGHT;
	patSlider.setScrollableContent((fm_getPatternSize(fm, fm->order) + 6)*ROW_HEIGHT + (windowHeight - 169) / 2, windowHeight - 169);

	patHSlider.setScrollableContent(CH_WIDTH*FM_ch, windowWidth - 231);
	patSize.setValue(fm_getPatternSize(fm, fm->order));
	if (selectedRow > fm_getPatternSize(fm, fm->order) - 1)
	{
		fm_setPosition(fm, fm->order, fm_getPatternSize(fm, fm->order) - 1, 0);
		selectedRow = fm->row;
		selection.bg.setPosition(selection.bg.getPosition().x, fm->row*ROW_HEIGHT);
		playCursor.setPosition(0, fm->row*ROW_HEIGHT);
	}

	setScroll(scroll);
	updatePatternLines();
}

void SongEditor::moveCursorAfterDataEntered()
{
	if (!fm->playing)
		fm_setPosition(fm, fm->order, selectedRow, 0);

	moveCursor(false);
	moveY(selectedRow);
	selected = 0;
	if (Mouse::isButtonPressed(Mouse::Left))
		selectionDisappear();
}

void SongEditor::setZoom(float _zoom)
{
	/* This rounding is done to avoid float imprecision leading to x.x9999 values sometimes... so we round to 1 digit precision */
	zoom = clamp(round(_zoom*10)*0.1, 0.7, 1.5);
	int selectionW = (int)round(selection.bg.getSize().x / COL_WIDTH);
	int selectionH = (int)round(selection.bg.getSize().y / ROW_HEIGHT);

	ROW_HEIGHT = font.getLineSpacing(charSize*zoom);
	CH_WIDTH = (int)round(100 * zoom) / 4 * 4;
	COL_WIDTH = round(CH_WIDTH / 4);
	for (unsigned ch = 0; ch < FM_ch; ++ch)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			text[ch][i].setCharacterSize(charSize*zoom);
		}
		text[ch][0].setPosition(ch*CH_WIDTH, 0);
		text[ch][1].setPosition(round(ch*CH_WIDTH + (int)(1.25*COL_WIDTH)), 0);
		text[ch][2].setPosition(round(ch*CH_WIDTH + (int)(2 * COL_WIDTH)), 0);
		text[ch][3].setPosition(round(ch*CH_WIDTH + 2.75*COL_WIDTH), 0);
		channelHead[ch].updateZoom();
	}
	rowNumbers.setCharacterSize(charSize*zoom);


	playCursor.setSize(Vector2f(CH_WIDTH*FM_ch, ROW_HEIGHT));
	playCursor.setPosition(0, fm->row*ROW_HEIGHT);

	patternView.setViewport(FloatRect((32 * zoom) / windowWidth, 0.22f * 768 / windowHeight, 1, 1));
	patternTopView.setViewport(FloatRect((32 * zoom) / windowWidth, 0, 1, 1));
	updateScrollbar();
	setXscroll(scrollXsmooth);
	selection.bg.setPosition(clamp((mouseXpat+(selectionW<0))*COL_WIDTH, 0, FM_ch*CH_WIDTH), clamp((selectedRow+(selectionH<0))*ROW_HEIGHT, 0, (fm->patternSize[fm->order] - 1)*ROW_HEIGHT));
	selection.bg.setSize(Vector2f(COL_WIDTH*selectionW, selectionH*ROW_HEIGHT));

}

void SongEditor::handleEvents()
{

	mousePattern = input_getmouse(patternView);

	handleNotePreview((selectedType == 0 || focusedElement == instrList) && !keyboard.shift);

	// edit pattern data
	if (textEnteredCount > 0)
	{
		wrappingValue();
	}

	// keyboard shortcuts
	if (evt.type == Event::KeyPressed && !fm->playing)
	{
		handleShortcuts();
	}

	if (evt.type == Event::MouseButtonPressed)
	{

		if (evt.mouseButton.button == Mouse::Right)
		{
			rightMouseEvents();
		}
		if (evt.mouseButton.button == Mouse::Left)
		{
			leftMouseEvents();
		}
	}
	if (evt.type == Event::MouseButtonReleased)
	{

		if (evt.mouseButton.button == Mouse::Left)
		{
			leftMouseRelease();
		}
	}

}