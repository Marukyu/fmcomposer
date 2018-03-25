#include "songEditor.hpp"


int SongEditor::search(int searchWhat, unsigned char searchValues[5], int searchIn, int replaceWhat, unsigned char replaceValues[6])
{
	int orderBegin, orderEnd;
	int rowBegin, rowEnd;
	int chBegin, chEnd;
	int found;
	int occurence = 0;
	// search in whole song
	if (searchIn == 0)
	{
		orderBegin = 0;
		orderEnd = fm->patternCount;
	}
	// search in current pattern
	else
	{
		orderBegin = fm->order;
		orderEnd = fm->order + 1;
	}

	if (!replaceWhat)
	{
		orderBegin = fm->order;
	}


	// search in selection
	if (searchIn == 2)
	{
		chBegin = (selectedChannel * 4 + selectedType) / 4;
		chEnd = ceil((selectedChannel * 4 + selectedType + selection.bg.getSize().x / COL_WIDTH) / 4); // ceil because need to take channel even if selection is smaller than a channel width
	}
	else
	{
		chBegin = 0;
		chEnd = FM_ch;
	}

	for (unsigned i = orderBegin; i < orderEnd; i++)
	{
		// search in selection
		if (searchIn == 2)
		{
			rowBegin = selectedRow;
			rowEnd = selectedRow + (selection.bg.getSize().y + 1) / ROW_HEIGHT;
		}
		else
		{
			rowBegin = 0;
			rowEnd = fm->patternSize[i];
			if (!replaceWhat && i == orderBegin)
			{
				rowBegin = fm->row;
			}
		}

		for (unsigned j = rowBegin; j < rowEnd; j++)
		{
			/* Search from the current cursor position (does not apply if Replace checked) */
			if (!replaceWhat && j == rowBegin && i == orderBegin)
			{
				chBegin = selectedChannel + 1;
				if (chBegin >= FM_ch)
				{
					chBegin = 0;
					continue;
				}
			}
			else
			{
				chBegin = 0;
			}
			for (unsigned ch = chBegin; ch < chEnd; ch++)
			{
				found = 0;

				/* Note found */
				if (searchWhat & 1 && (searchValues[0] != 255 && fm->pattern[i][j][ch].note == searchValues[0] || fm->pattern[i][j][ch].note < 128 && searchValues[0] == 255))
				{
					found++;
				}

				/* Volume found */
				if (searchWhat & 2 && (searchValues[1] != 255 && fm->pattern[i][j][ch].vol == searchValues[1] || fm->pattern[i][j][ch].vol < 100 && searchValues[1] == 255))
				{
					found += 2;
				}

				/* Instrument found */
				if (searchWhat & 4 && (searchValues[2] != 255 && fm->pattern[i][j][ch].instr == searchValues[2] || fm->pattern[i][j][ch].instr < 255 && searchValues[2] == 255))
				{
					found += 4;
				}

				/* Effect found */
				if (searchWhat & 8 && (searchValues[3] != 255 && fm->pattern[i][j][ch].fx == searchValues[3] || fm->pattern[i][j][ch].fx < 255 && searchValues[3] == 255))
				{
					found += 8;

					/* Effect value found */
					if (searchWhat & 16 && (searchValues[4] != 255 && fm->pattern[i][j][ch].fxdata == searchValues[4] || searchValues[4] == 255))
					{
						found += 16;
					}
				}
				/* Searched items were found */
				if (found == (searchWhat & 1) + (searchWhat & 2) + (searchWhat & 4) + (searchWhat & 8) + (searchWhat & 16))
				{
					occurence++;

					/* 'Replace' checked */
					if (replaceWhat)
					{
						if (replaceWhat & 1)
						{
							fm->pattern[i][j][ch].note = replaceValues[0];
						}
						if (replaceWhat & 2)
						{
							fm->pattern[i][j][ch].vol = replaceValues[1];
						}
						if (replaceWhat & 4)
						{
							fm->pattern[i][j][ch].instr = replaceValues[2];
						}
						if (replaceWhat & 8)
						{
							fm->pattern[i][j][ch].fx = replaceValues[3];
						}
						if (replaceWhat & 16)
						{
							fm->pattern[i][j][ch].fxdata = replaceValues[4];
						}
						if (replaceWhat & 32)
						{
							fm->pattern[i][j][ch].note = clamp(fm->pattern[i][j][ch].note + (char)replaceValues[5], 0, 127);
						}
					}
					/* No replace, update display to show the next search occurence */
					else
					{
						if (i > fm->order || i == fm->order && j > fm->row || i == fm->order && j == fm->row && ch > selectedChannel)
						{
							searched = true;
							fm_setPosition(fm, i, j, 2);
							updateFromFM();
							moveY(j);

							setX(ch * 4);
							return 1;
						}
					}
				}
			}
		}
	}

	if (replaceWhat)
	{
		if (occurence > 0)
		{
			for (unsigned ch = chBegin; ch < chEnd; ++ch)
				updateChannelData(ch);
			songModified(1);
		}
		return occurence;
	}
	else
	{
		return occurence;
	}
}