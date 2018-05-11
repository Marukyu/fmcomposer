#include "songEditor.hpp"

void SongEditor::updateRecordChannels()
{
	/* Check if select channel is in recording channels*/
	int inSelecteds = 0;
	for (int i = 0; i < FM_ch; i++)
	{
		if (selectedChannel == i && channelHead[i].record.selected == 1)
		{
			inSelecteds = 1;
			break;
		}
	}
	if (!inSelecteds)
	{
		int firstOfSelecteds = 0;
		for (int i = 0; i < FM_ch; i++)
		{
			/* Pre select current recording channel */
			if (!firstOfSelecteds && channelHead[i].record.selected == 1)
			{
				firstOfSelecteds = 1;
				if (selectedChannel == i)
					break;
			}
			channelHead[i].record.selected = 0;
		}
		channelHead[selectedChannel].record.selected = 1;
	}
}

void SongEditor::moveCursor(bool _updateRecordChannels)
{
	focusedElement = &patternView;
	selection.deltaX = 28;
	selection.deltaY = ROW_HEIGHT;
	selected = true;
	selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
	selectedRow = mouseYpat;
	selectedType = mouseXpat % 4;
	selectedChannel = mouseXpat / 4;

	selection.bg.setPosition(clamp(mouseXpat*COL_WIDTH, 0, FM_ch*CH_WIDTH), clamp(selectedRow*ROW_HEIGHT, 0, (fm->patternSize[fm->order] - 1)*ROW_HEIGHT));

	if (_updateRecordChannels)
	{
		updateRecordChannels();
	}

	if (fm->playing)
		selectionDisappear();
}

void SongEditor::doubleClick()
{
	/* Check if the element selected is the same as the one when 1st click */
	if (clickedElem == selectedRow + (selectedType + selectedChannel * 4) * 256)
	{
		mouse.clickg = 0;
		int popups[4] = { POPUP_TRANSPOSE, POPUP_REPLACE_INSTRUMENT, POPUP_FADE, POPUP_EFFECTS };
		if (selectedType == 0)
		{
			if (fm->pattern[fm->order][selectedRow][selectedChannel].note != 255)
			{
				popup->show(POPUP_TRANSPOSE);
			}
			else
			{
				popup->show(POPUP_SETNOTE);
			}
		}
		else
			popup->show(popups[selectedType]);

		/* Effect is already set ? Pre-select it in popup */
		if (selectedType == 3)
		{
			preSelectPopupEffect();

		}
		/* Instrument is already set ? Pre-select it in popup */
		if (selectedType == 1)
		{
			if (fm->pattern[fm->order][selectedRow][selectedChannel].instr != 255)
			{
				popup->lists[0].select(fm->pattern[fm->order][selectedRow][selectedChannel].instr);
			}
		}

		/* Volume is already set + 'set volume' is checked ? Pre-select it in popup */
		if (selectedType == 2)
		{
			if (selection.isSingle())
			{

				if (fm->pattern[fm->order][selectedRow][selectedChannel].vol == 255)
				{
					popup->checkboxes[0].checked = 0;
					popup->checkboxes[1].checked = 0;
					popup->checkboxes[2].checked = 1;
				}
				else
					popup->sliders[3].setValue(fm->pattern[fm->order][selectedRow][selectedChannel].vol);
			}
		}

		/* Note is already set ? Pre-select it in popup */
		if (selectedType == 0 && fm->pattern[fm->order][selectedRow][selectedChannel].note != 255)
		{
			popup->sliders[0].setValue(fm->pattern[fm->order][selectedRow][selectedChannel].note);
		}

		selected = false;
	}
}

void SongEditor::rightMouseEvents()
{
	patternList.update();
	if (isMouseHoverPattern() && !fm->playing)
	{

		updateMousePat();
		if (!selection.isHover(mousePattern.x, mousePattern.y) && (!contextMenu || !patMenu.hover()))
		{
			moveCursor();
			fm_setPosition(fm, fm->order, selectedRow, 0);
			playCursor.setPosition(0, (int)fm->row*ROW_HEIGHT);
		}


		selected = false;
		if (!fm->playing && (!contextMenu || !patMenu.hover()))
			patMenu.show();
	}
	else
	{
		int elem = patternList.getElementHovered();
		if (elem > -1)
		{
			fm_setPosition(fm, elem, 0, 0);
			patListMenu.show();
		}
	}

}

void SongEditor::leftMouseEvents()
{
	patternList.update();
	/* Change cursor position on mouse click */
	if (isMouseHoverPattern())
	{
		updateMousePat();
		if (!keyboard.shift)
		{
			selection.revertIfInverted();
			updateSelectedsFromCursor();
		}

		/* Move selection */
		if (selection.isHover(mousePattern.x, mousePattern.y) && !selection.isSingle() && !contextMenu)
		{
			selection.moveRect.setSize(Vector2f(abs(selection.bg.getSize().x), abs(selection.bg.getSize().y)));
			selection.moving = 1;
			updateSelectedsFromCursor();


			selection.deltaX = mouseXpat - (selection.bg.getPosition().x) / COL_WIDTH;

			selection.deltaY = mouseYpat - selection.bg.getPosition().y / ROW_HEIGHT;
		}

		else
		{
			/* Resize selection */
			if (keyboard.shift)
			{
				selected = 1;
			}
			/* New selection */
			else
			{
				if (contextMenu == NULL || (contextMenu != NULL && !contextMenu->hover()))
				{
					moveCursor();
					if (mouse.dblClick <= 0)
					{
						mouse.dblClick = 15;
						clickedElem = selectedRow + (selectedType + selectedChannel * 4) * 256;
					}
					else if (!fm->playing)
					{
						doubleClick();
					}
				}
			}

		}

	}
	else
	{

		focusedElement = 0;
	}

}


void SongEditor::selectionDisappear()
{
	selected = false;

	setScroll(fm->row);

	playCursor.setPosition(0, (int)fm->row*ROW_HEIGHT);

	/* Clicking on the sides automatically scroll right/left */
	if (mouse.pos.x >= windowWidth - 233 - 100)
	{
		setXscroll(scrollXsmooth + CH_WIDTH);
	}
	else if (mouse.pos.x < 100 + 32)
	{
		setXscroll(scrollXsmooth - CH_WIDTH);
	}

}


void SongEditor::leftMouseRelease()
{
	if (contextMenu)
		return;
	selected = false;
	if (selection.moving)
	{
		selection.moving = false;
		// click on selection without moving it


		if (selectedRow == selection.getMovedRow() && selectedChannel * 4 + selectedType == selection.getMovedChannel() * 4 + selection.getMovedType())
		{
			selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));
			selectedRow = mouseYpat;
			fm_setPosition(fm, fm->order, selectedRow, 0);
			selectedChannel = mouseXpat / 4;
			selectedType = mouseXpat % 4;
			selection.bg.setPosition(min<int>(selectedChannel * CH_WIDTH + selectedType * COL_WIDTH, FM_ch * CH_WIDTH), selectedRow*ROW_HEIGHT);
			selectionDisappear();
		}
		else
		{
			/* Copy data */
			saveToHistory(selectedRow, selection.bg.getSize().y / ROW_HEIGHT);
			multipleEdit(2, &movedSelection,0,0,false);
			saveToHistory(selectedRow, selection.bg.getSize().y / ROW_HEIGHT);

			/* Get row/channel from selection position. do -0.5/+0.5 to correct float-to-int value rounding */
			selectedRow = selection.getMovedRow();
			selectedChannel = selection.getMovedChannel();

			/* Keep current row/channel before they are bringed in range*/
			int oldRow = selectedRow;
			int oldChannel = selectedChannel;

			/* Resize selection when it goes outside pattern area */

			if (selectedRow < 0)
			{
				selection.bg.setSize(Vector2f(selection.bg.getSize().x, selection.bg.getSize().y + selectedRow*ROW_HEIGHT));
				movedSelection.y1 += selectedRow;
				movedSelection.y2 += selectedRow;
				selectedRow = 0;
			}
			
			if (selectedChannel<0)
			{
				
				selection.bg.setSize(Vector2f(selection.bg.getSize().x + selectedChannel*CH_WIDTH + selectedType*COL_WIDTH, selection.bg.getSize().y));

				selectedChannel = selectedType = 0;
			}

			int exceedSizeY = selectedRow + (selection.bg.getSize().y + 0.5) / ROW_HEIGHT - fm->patternSize[fm->order];

			if (exceedSizeY > 0)
			{
				selection.bg.setSize(Vector2f(selection.bg.getSize().x, selection.bg.getSize().y - exceedSizeY*ROW_HEIGHT));
				movedSelection.y2 -= exceedSizeY;
			}

			int exceedSizeX = selectedChannel + (selection.bg.getSize().x + 0.5) / CH_WIDTH - FM_ch;

			if (exceedSizeX > 0)
			{
				selection.bg.setSize(Vector2f(selection.bg.getSize().x - exceedSizeX*CH_WIDTH, selection.bg.getSize().y));
				movedSelection.x2 -= exceedSizeX;
			}


			for (int i = 0; i < FM_ch; i++)
			{
				channelHead[i].record.selected = 0;
			}
			channelHead[selectedChannel].record.selected = 1;

			fm_setPosition(fm, fm->order, selectedRow, 0);
			selection.bg.setPosition(min<int>(selectedChannel*CH_WIDTH + selectedType*COL_WIDTH, FM_ch*CH_WIDTH), selectedRow*ROW_HEIGHT);
			setScroll(fm->row);
			playCursor.setPosition(0, (int)fm->row*ROW_HEIGHT);
			mouseXpat = selectedChannel*4+selectedType;
			mouseYpat=selectedRow;

			saveToHistory(selectedRow, selection.bg.getSize().y / ROW_HEIGHT);

			patPaste(&movedSelection, oldChannel, oldRow);
			saveToHistory(selectedRow, selection.bg.getSize().y / ROW_HEIGHT);
			fm->channelStatesDone = 0;
		}
	}
	else if (selection.isSingle() && isMouseHoverPattern() && !fm->playing && focusedElement == &patternView)
	{
		fm_setPosition(fm, fm->order, selectedRow, 0);
		selectionDisappear();
	}

	/* Move patterns */
	int patternListHovered = patternList.getElementHovered();

	if (patternListHovered > -1 && patternList.selected)
	{
		if (movePat > -1 && patternListHovered != movePat && !patternList.isElementHovered(movePat))
		{
			pattern_move(movePat, patternListHovered);
		}

		fm_setPosition(fm, patternListHovered, fm->playing ? 0 : fm->row, 2);
		selection.bg.setSize(Vector2f(selection.bg.getSize().x, min(selection.bg.getSize().y, fm->patternSize[fm->order] * ROW_HEIGHT - selection.bg.getPosition().y)));
	}


	movePat = -1;
	selected = false;
}


void SongEditor::updateSelectedsFromCursor()
{
	selectedChannel = selection.getChannel();
	selectedType = selection.getType();
	selectedRow = selection.getRow();
}

void SongEditor::updateMousePat()
{
	mouseXpat = clamp((int)round(mousePattern.x / COL_WIDTH), 0, FM_ch * 4 - 1);
	mouseYpat = clamp((int)round(mousePattern.y / ROW_HEIGHT), 0, (int)fm->patternSize[fm->order] - 1);
}