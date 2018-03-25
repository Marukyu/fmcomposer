#include "songEditor.hpp"




void SongEditor::handleShortcuts()
{//printf("%d,",evt.key.code);

	switch (evt.key.code)
	{
		case Keyboard::F3:
			popup->show(POPUP_SEARCH);
			if (searched)
			{
				popup->close(true);
			}
			break;
	}

	if (patternList.selected)
	{

		if (keyboard.ctrl)
		{
			switch (evt.key.code)
			{
				case Keyboard::C: // copier
					pattern_copy();
					break;
				case Keyboard::D: // dupliquer
					pattern_duplicate();
					break;
				case Keyboard::X: // couper
					pattern_cut();
					break;
				case Keyboard::V:// coller
					pattern_paste(0);
					break;
			}
		}
		else
		{
			switch (evt.key.code)
			{
				case Keyboard::Delete:
					pattern_delete();
					break;

			}
		}


		return;
	}


	switch (evt.key.code)
	{
		case Keyboard::F3:
			if (searched)
			{
				popup->show(POPUP_SEARCH);
				popup->close(true);
			}
			break;
		case Keyboard::Add:
			if (selection.isSingle())
			{

				if (selectedType == 1)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].instr = min(instrList->text.size() - 1, fm->pattern[fm->order][selectedRow][selectedChannel].instr + 1);
					addHistory();
					songModified(1);
				}
				else if (selectedType == 2)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].vol = min(99, fm->pattern[fm->order][selectedRow][selectedChannel].vol + 1);
					addHistory();
					songModified(1);
				}
				else if (selectedType == 3)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].fxdata = min(255, fm->pattern[fm->order][selectedRow][selectedChannel].fxdata + 1);
					addHistory();
					songModified(1);
				}
				else
				{
					multipleEdit(3, 0, 1);
				}
				if (selectedType != 0)
				{
					updateChannelData(selectedChannel);
				}
			}
			else
				multipleEdit(3, 0, 1);
			
			break;
		case Keyboard::Subtract:

			if (selection.isSingle())
			{
				if (selectedType == 1)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].instr = max(0, fm->pattern[fm->order][selectedRow][selectedChannel].instr - 1);
					addHistory();
					songModified(1);
				}
				else if (selectedType == 2)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].vol = max(0, fm->pattern[fm->order][selectedRow][selectedChannel].vol - 1);
					addHistory();
					songModified(1);
				}
				else if (selectedType == 3)
				{
					addHistory();
					fm->pattern[fm->order][selectedRow][selectedChannel].fxdata = max(0, fm->pattern[fm->order][selectedRow][selectedChannel].fxdata - 1);
					addHistory();
					songModified(1);
				}
				else
				{
					multipleEdit(3, 0, -1);
				}
				if (selectedType != 0)
				{
					updateChannelData(selectedChannel);
				}
			}
			else
				multipleEdit(3, 0, -1);
			break;
		case Keyboard::Delete:

			multipleEdit(7);
			break;
		case Keyboard::Equal:
			if (selectedType == 0)
			{ // stop a note
				addHistory();
				fm->pattern[fm->order][selectedRow][selectedChannel].note = 128;
				fm->pattern[fm->order][selectedRow][selectedChannel].instr = 255;
				fm->pattern[fm->order][selectedRow][selectedChannel].vol = 255;
				updateChannelData(selectedChannel);
				addHistory();
			}
			break;
		case Keyboard::PageUp: // scroll to pattern top
			if (focusedElement == &patternView || focusedElement == NULL)
			{
				if (keyboard.shift)
				{
					selection.resizeRelative(0, -patternHeightView);
					setScroll(scroll - patternHeightView);

				}
				else
					moveY(selectedRow - patternHeightView);
			}
			break;

		case Keyboard::PageDown: // scroll to pattern bottom
			if (focusedElement == &patternView || focusedElement == NULL)
			{
				if (keyboard.shift)
				{
					selection.resizeRelative(0, patternHeightView);
					setScroll(scroll + patternHeightView);
				}
				else
					moveY(selectedRow + patternHeightView);
			}
			break;
		case Keyboard::Up: // move cursor up
			if (focusedElement == &patternView || focusedElement == NULL)
			{

				if (keyboard.shift)
				{

					selection.resizeRelative(0, -1);

					if (selection.bg.getPosition().y + selection.bg.getSize().y < (scroll - halfPatternHeightView)*ROW_HEIGHT)
					{
						setScroll(scroll - 1);
					}

				}
				else
					moveY(selectedRow - 1);
			}
			break;
		case Keyboard::Down: // move cursor down
			if (focusedElement == &patternView || focusedElement == NULL)
			{

				if (keyboard.shift)
				{
					if ((selection.bg.getPosition().y + selection.bg.getSize().y) / ROW_HEIGHT < fm->patternSize[fm->order])
					{
						selection.resizeRelative(0, 1);

						if (selection.bg.getPosition().y + selection.bg.getSize().y>(scroll + halfPatternHeightView)*ROW_HEIGHT)
						{
							setScroll(scroll + 1);
						}
					}
				}
				else
					moveY(selectedRow + 1);
			}
			break;
		case Keyboard::Left: // move cursor left

			if (keyboard.shift)
			{
				if ((selection.bg.getPosition().x + selection.bg.getSize().x) / COL_WIDTH > 0)
				{
					selection.resizeRelative(-1, 0);
					if (selection.bg.getPosition().x + selection.bg.getSize().x < scrollX*CH_WIDTH)
					{
						setXscroll(scrollX - 1);
					}
				}
			}
			else
			{
				moveXrelative(-1);

			}
			break;
		case Keyboard::Right: // move cursor right

			if (keyboard.shift)
			{
				if ((selection.bg.getPosition().x + selection.bg.getSize().x) / COL_WIDTH < FM_ch * 4)
				{
					selection.resizeRelative(1, 0);
					if (selection.bg.getPosition().x + selection.bg.getSize().x> scrollX*CH_WIDTH + (windowWidth - 270))
					{
						setXscroll(scrollX + 1);
					}
				}
			}
			else
			{
				moveXrelative(1);

			}
			break;
		case Keyboard::Tab: // move cursor right (1 channel)

			if (keyboard.shift)
				moveXrelative(-4);
			else
				moveXrelative(4);
			break;
	}

	if (keyboard.shift)
	{
		switch (evt.key.code)
		{
			case Keyboard::E: // note +12

				multipleEdit(3, 0, 12);
				break;
			case Keyboard::D: // note -12

				multipleEdit(3, 0, -12);
				break;
			case Keyboard::R: // note +1

				multipleEdit(3, 0, 1);
				break;
			case Keyboard::F: // note -1

				multipleEdit(3, 0, -1);
				break;
		}
	}
	if (keyboard.ctrl)
	{
		switch (evt.key.code)
		{
			case Keyboard::Z: // undo
				undo();
				break;
			case Keyboard::Y: // redo

				redo();
				break;
			case Keyboard::C: // copier

				patCopy(&copiedSelection);
				break;
			case Keyboard::F: // recherche
				popup->show(POPUP_SEARCH);
				break;
			case Keyboard::X: // couper
				
				patCopy(&copiedSelection);
				multipleEdit(2);
				break;


			case Keyboard::V:// coller
				if (copiedSelection.data.size())
				{
					saveToHistory(fm->row, copiedSelection.data[0].size());
					saveToHistory(fm->row, copiedSelection.data[0].size());
					patPaste(&copiedSelection, selectedChannel, selectedRow);
					saveToHistory(fm->row, copiedSelection.data[0].size());
					saveToHistory(fm->row, copiedSelection.data[0].size());
				}
				break;
			case Keyboard::A:// tout sélectionner
				selectedRow = selectedType = selectedChannel = 0;
				selection.bg.setPosition(0, 0);
				selection.bg.setSize(Vector2f(FM_ch*CH_WIDTH, fm->patternSize[fm->order] * ROW_HEIGHT));
				break;

		}
	}
}

void SongEditor::undo()
{
	if (currentHistoryPos[fm->order] > 0)
	{
		currentHistoryPos[fm->order]--;
		udpatePatternFromHistory();

		currentHistoryPos[fm->order]--;
		udpatePatternFromHistory();
		currentHistoryPos[fm->order]--;
		udpatePatternFromHistory();
		currentHistoryPos[fm->order]--;
		udpatePatternFromHistory();
		for (unsigned ch = 0; ch < FM_ch; ++ch)
		{
			updateChannelData(ch);
		}
		songModified(1);
	}
}
void SongEditor::redo()
{
	if (currentHistoryPos[fm->order] < (int)history[fm->order].size() - 1)
	{

		udpatePatternFromHistory();
		currentHistoryPos[fm->order]++;
		udpatePatternFromHistory();
		currentHistoryPos[fm->order]++;
		udpatePatternFromHistory();
		currentHistoryPos[fm->order]++;
		udpatePatternFromHistory();
		currentHistoryPos[fm->order]++;
		for (unsigned ch = 0; ch < FM_ch; ++ch)
		{
			updateChannelData(ch);
		}
		songModified(1);

	}
}