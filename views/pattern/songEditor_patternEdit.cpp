#include "songEditor.hpp"

void SongEditor::patCopy(patternSelection* copiedData)
{

	selection.getBounds(&copiedData->x1, &copiedData->x2, &copiedData->y1, &copiedData->y2);

	copiedData->pattern = fm->order;

	//printf("%d %d\n",copiedData->x1/4, copiedData->decale );
	copiedData->data.resize(copiedData->x2 - copiedData->x1);
	for (unsigned i = 0; i < copiedData->data.size(); i++)
	{
		copiedData->data[i].resize(copiedData->y2 - copiedData->y1);

		switch ((copiedData->x1 + i) % 4)
		{
			case 0:
				for (unsigned j = copiedData->y1; j < copiedData->y2; j++)
				{
					copiedData->data[i][j - copiedData->y1] = fm->pattern[fm->order][j][copiedData->x1 / 4 + (copiedData->x1 % 4 + i) / 4].note;
				}
				break;
			case 1:
				for (unsigned j = copiedData->y1; j < copiedData->y2; j++)
					copiedData->data[i][j - copiedData->y1] = fm->pattern[fm->order][j][copiedData->x1 / 4 + (copiedData->x1 % 4 + i) / 4].instr;
				break;
			case 2:
				for (unsigned j = copiedData->y1; j < copiedData->y2; j++)
					copiedData->data[i][j - copiedData->y1] = fm->pattern[fm->order][j][copiedData->x1 / 4 + (copiedData->x1 % 4 + i) / 4].vol;
				break;
			case 3:
				for (unsigned j = copiedData->y1; j < copiedData->y2; j++)
				{
					copiedData->data[i][j - copiedData->y1] = fm->pattern[fm->order][j][copiedData->x1 / 4 + (copiedData->x1 % 4 + i) / 4].fx;
					copiedData->data[i][j - copiedData->y1] += fm->pattern[fm->order][j][copiedData->x1 / 4 + (copiedData->x1 % 4 + i) / 4].fxdata * 256;
				}
				break;
		}
	}
}

void SongEditor::multipleEdit(int action, patternSelection* copiedData, int param, int param2)
{

	if (copiedData == NULL)
		copiedData = &copiedSelection;

	if (action == 2)
	{
		patCopy(copiedData);
	}
	else if (action == 1)
	{
		patPaste(copiedData, selectedChannel, selectedRow);
		return;
	}
	else if (action == 0)
	{
		patCopy(copiedData);
		return;
	}

	int x1, x2, y1, y2;

	selection.getBounds(&x1, &x2, &y1, &y2);

	for (unsigned i = x1; i < x2; ++i)
	{
		switch (action)
		{
			case 8: // replace instrument
				if (selection.isSingle())
					fm->pattern[fm->order][selectedRow][i / 4].instr = param;
				else if (i % 4 == 1)
				{

					for (unsigned j = y1; j < y2; j++)
					{
						if (fm->pattern[fm->order][j][i / 4].instr != 255)
							fm->pattern[fm->order][j][i / 4].instr = param;
					}

				}
				break;
			case 9:{ // volume fade

					   if (i % 4 == 2)
					   {

						   for (int j = y1; j < y2; j++)
						   {
							   if (selection.isSingle())
							   {
								   fm->pattern[fm->order][j][i / 4].vol = round((param + param2) / 2);
							   }
							   else
							   {
								   float interp = (float)(j - y1) / (y2 - y1 - 1);
								   // fade param-->param2
								   fm->pattern[fm->order][j][i / 4].vol = round((param2*interp) + param*(1 - interp));
							   }
						   }
					   }
			}
				break;
			case 10:{ // effect set

						if (selection.isSingle())
						{
							fm->pattern[fm->order][selectedRow][i / 4].fx = param;
							fm->pattern[fm->order][selectedRow][i / 4].fxdata = param2;
						}
						else
						{
							if (i % 4 == 3)
							{
								for (int j = y1; j < y2; j++)
								{
									fm->pattern[fm->order][j][i / 4].fx = param;
									fm->pattern[fm->order][j][i / 4].fxdata = param2;
								}
							}
						}
			}
				break;
			case 11:{ // volume scale

						if (i % 4 == 2)
						{

							for (int j = y1; j < y2; j++)
							{
								if (fm->pattern[fm->order][j][i / 4].note < 128 || fm->pattern[fm->order][j][i / 4].vol != 255)
									fm->pattern[fm->order][j][i / 4].vol = min(99, round(fm->pattern[fm->order][j][i / 4].vol*param*0.01));
							}
						}
			}
				break;
			case 12:{ // set volume
						if (selection.isSingle())
						{
							fm->pattern[fm->order][selectedRow][i / 4].vol = param;
						}
						else
						{
							if (i % 4 == 2)
							{
								for (int j = y1; j < y2; j++)
								{
									// fade param-->param2
									if (fm->pattern[fm->order][j][i / 4].note != 255 || fm->pattern[fm->order][j][i / 4].vol != 255)
										fm->pattern[fm->order][j][i / 4].vol = param;
								}
							}
						}
			}
				break;
			case 7: // delete
			case 2: // cut
				switch (i % 4)
				{
					case 0:
						for (unsigned j = y1; j < y2; j++)
							fm->pattern[fm->order][j][i / 4].note = 255;
						break;
					case 1:
						for (unsigned j = y1; j < y2; j++)
							fm->pattern[fm->order][j][i / 4].instr = 255;
						break;
					case 2:
						for (unsigned j = y1; j < y2; j++)
							fm->pattern[fm->order][j][i / 4].vol = 255;
						break;
					case 3:
						for (unsigned j = y1; j < y2; j++)
							fm->pattern[fm->order][j][i / 4].fx = 255;
						break;
				}
				break;
			case 3: // transpose
				if (i % 4 == 0)
				for (unsigned j = y1; j < y2; j++)
				if (fm->pattern[fm->order][j][i / 4].note < 128)
					fm->pattern[fm->order][j][i / 4].note = clamp(fm->pattern[fm->order][j][i / 4].note + param, 0, 127);
				break;
			case 4: // note set
				if (selection.isSingle())
				{
					if (fm->pattern[fm->order][selectedRow][i / 4].note == 255 && fm->pattern[fm->order][selectedRow][i / 4].instr == 255)
					{
						fm->pattern[fm->order][selectedRow][i / 4].instr = instrList->value;
					}
					fm->pattern[fm->order][selectedRow][i / 4].note = param;
				}
				else
				{
					if (i % 4 == 0)
					for (unsigned j = y1; j < y2; j++)
					if (fm->pattern[fm->order][j][i / 4].note < 128)
						fm->pattern[fm->order][j][i / 4].note = param;
				}
				break;
		}

	}
	for (unsigned i = x1 / 4; i < x2 / 4 + 1; i++)
	if (i < FM_ch)
		updateChannelData(i);

	if (action == 7 || action == 10 || action == 2)
		songModified(1);
	else
		songModified(1);
}

void SongEditor::patPaste(patternSelection* copiedData, int channel, int ypos)
{

	if (copiedData->data.size() == 0)
		return;

	if (selection.isSingle() && copiedData->data.size() == 1 && copiedData->data[0].size() == 1 && copiedData->data[0][0] == 255)
	{
		switch (selectedType)
		{
			case 0:
				fm->pattern[fm->order][ypos][channel].note = 255;
				break;
			case 1:
				fm->pattern[fm->order][ypos][channel].instr = 255;
				break;
			case 2:
				fm->pattern[fm->order][ypos][channel].vol = 255;
				break;
			case 3:
				fm->pattern[fm->order][ypos][channel].fx = 255;
				break;
			case 4:
				fm->pattern[fm->order][ypos][channel].fxdata = 255;
				break;
		}

	}
	else
	{
		for (unsigned i = 0; i < copiedData->data.size(); i++)
		{

			for (unsigned j = 0; j < copiedData->data[i].size(); j++)
			{

				if (ypos + j < 0 || ypos + j >= fm->patternSize[fm->order] || channel + (copiedData->x1 % 4 + i) / 4 < 0 || channel + (copiedData->x1 % 4 + i) / 4 >= FM_ch)continue;


				switch ((copiedData->x1 + i) % 4)
				{
					case 0:
						fm->pattern[fm->order][ypos + j][channel + (copiedData->x1 % 4 + i) / 4].note = copiedData->data[i][j];

						break;
					case 1:

						fm->pattern[fm->order][ypos + j][channel + (copiedData->x1 % 4 + i) / 4].instr = copiedData->data[i][j];

						break;
					case 2:

						fm->pattern[fm->order][ypos + j][channel + (copiedData->x1 % 4 + i) / 4].vol = copiedData->data[i][j];

						break;
					case 3:

						fm->pattern[fm->order][ypos + j][channel + (copiedData->x1 % 4 + i) / 4].fx = copiedData->data[i][j] % 256;
						fm->pattern[fm->order][ypos + j][channel + (copiedData->x1 % 4 + i) / 4].fxdata = copiedData->data[i][j] / 256;

						break;
				}

			}

		}
	}
	for (unsigned i = 0; i< FM_ch; i++)
	{
		updateChannelData(i);
	}

	/* Set selection size to copied element. don't do that when the selection  is moved */
	if (copiedData == &copiedSelection)
		selection.bg.setSize(Vector2f(copiedData->data.size()*COL_WIDTH, copiedData->data[0].size()*ROW_HEIGHT));

	if (selectedRow + copiedData->data[0].size()>fm->patternSize[fm->order])
	{
		selection.bg.setSize(Vector2f(copiedData->data.size()*COL_WIDTH, (fm->patternSize[fm->order] - selectedRow)*ROW_HEIGHT));
	}


	selection.bg.setPosition(selectedChannel*CH_WIDTH + selectedType*COL_WIDTH, selection.bg.getPosition().y);

	if (selectedChannel * 4 + selectedType + copiedData->data.size() > 4 * FM_ch)
	{
		selection.bg.setSize(Vector2f((4 * FM_ch - (selectedChannel * 4 + selectedType))*COL_WIDTH, selection.bg.getSize().y));
	}

	songModified(1);
}

void SongEditor::wrappingValue()
{
	textEnteredCount--;
	if (selectedType > 0 && !keyboard.ctrl && (focusedElement == &patternView || focusedElement == NULL))
	{
		if (textEntered[textEnteredCount] > 45 && textEntered[textEnteredCount] < 58)
		{
			addHistory();
			if (selectedType == 1)
			{ // user changes instrument number
				wrapValue(&fm->pattern[fm->order][selectedRow][selectedChannel].instr, textEntered[textEnteredCount], instrList->text.size());
			}
			else if (selectedType == 2)
			{ // user changes volume
				wrapValue(&fm->pattern[fm->order][selectedRow][selectedChannel].vol, textEntered[textEnteredCount], 99);
			}
			else
			{ // user changes fx data

				int maxValue;

				switch (fm->pattern[fm->order][selectedRow][selectedChannel].fx)
				{
					case 'D':
						maxValue = 7;
						break;
					case 'Q':
						maxValue = 8;
						break;
					case 'M':
					case 'R':
					case 'V':
						maxValue = 99;
						break;
					case 'S':
						maxValue = 80;
						break;
					default:
						maxValue = 255;
						break;
				}

				wrapValue(&fm->pattern[fm->order][selectedRow][selectedChannel].fxdata, textEntered[textEnteredCount], maxValue);

			}

			updateChannelData(selectedChannel);

			moveCursorAfterDataEntered();

			/* Updating note, vol or instr doesn't need to regenerate channel status */
			if (selectedType <3)
				songModified(1);
			else
				songModified(1);

			fm->channelStatesDone = 0;
			addHistory();
		}
		else if (textEntered[textEnteredCount] >64 && textEntered[textEnteredCount] <91 // uppercase
			|| textEntered[textEnteredCount] >96 && textEntered[textEnteredCount] < 123)
		{ // lowercase
			if (selectedType == 3)
			{ // effect shortcuts
				addHistory();
				moveCursorAfterDataEntered();
				if (fm->pattern[fm->order][selectedRow][selectedChannel].fx == 255)
				{
					fm->pattern[fm->order][selectedRow][selectedChannel].fxdata = 0;
				}
				fm->pattern[fm->order][selectedRow][selectedChannel].fx = toupper(textEntered[textEnteredCount]);

				updateChannelData(selectedChannel);

				songModified(1);
				fm->channelStatesDone = 0;
				addHistory();
			}
		}
	}
}

void SongEditor::pattern_insertrows(int count)
{
	saveToHistory(0, fm->patternSize[fm->order]);
	saveToHistory(0, fm->patternSize[fm->order]);
	if (fm->patternSize[fm->order] + count > 256)
	{
		count = 256 - fm->patternSize[fm->order];
	}

	fm_insertRows(fm, fm->order, selectedRow, count);

	updateFromFM();
	songModified(1);

	saveToHistory(0, fm->patternSize[fm->order]);
	saveToHistory(0, fm->patternSize[fm->order]);
}

void SongEditor::pattern_deleterows(int count)
{
	saveToHistory(0, fm->patternSize[fm->order]);
	saveToHistory(0, fm->patternSize[fm->order]);

	if (selectedRow + count > fm->patternSize[fm->order])
	{
		count = clamp((int)fm->patternSize[fm->order] - selectedRow, 0, fm->patternSize[fm->order] - 1);
	}

	fm_removeRows(fm, fm->order, selectedRow, count);
	updateFromFM();
	songModified(1);

	saveToHistory(0, fm->patternSize[fm->order]);
	saveToHistory(0, fm->patternSize[fm->order]);
}