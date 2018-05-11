#include "songEditor.hpp"


void SongEditor::udpatePatternFromHistory()
{

	/* Data to restore bigger than actual pattern size : resize pattern */
	if (history[fm->order][currentHistoryPos[fm->order]].data.size() > fm->patternSize[fm->order])
	{
		fm_resizePattern(fm, fm->order, history[fm->order][currentHistoryPos[fm->order]].data.size(), 0);
		patSize.setValue(fm->patternSize[fm->order]);
		updateRowCount();
	}
	/* Data to restore smaller than pattern size : the pattern was resized*/
	else if (history[fm->order][currentHistoryPos[fm->order]].patternSize != fm->patternSize[fm->order])
	{
		fm_resizePattern(fm, fm->order, history[fm->order][currentHistoryPos[fm->order]].patternSize, 0);
		patSize.setValue(fm->patternSize[fm->order]);
		updateRowCount();
		setScroll(scroll);
	}


	if (currentHistoryPos[fm->order] >= 0)
	{

		/* Eviter d'écrire des données en dehors du pattern */
		int maxy = history[fm->order][currentHistoryPos[fm->order]].data.size();
		if (maxy + history[fm->order][currentHistoryPos[fm->order]].y > fm->patternSize[fm->order])
		{
			maxy = -history[fm->order][currentHistoryPos[fm->order]].y + fm->patternSize[fm->order];
		}


		for (unsigned y = 0; y < maxy; y++)
		{
			for (int ch = 0; ch < FM_ch; ch++)
			{
				fm->pattern[fm->order][history[fm->order][currentHistoryPos[fm->order]].y + y][ch] = history[fm->order][currentHistoryPos[fm->order]].data[y][ch];

			}
		}
	}
}

void SongEditor::saveToHistory(int _y, int size)
{

	/* Handle selections outside pattern (ignore) */
	if (_y < 0)
	{
		size -= _y;
		_y = 0;
	}

	if (size<0)
	{
		_y += size;
		size = abs(size);
	}

	if (_y + size >= fm_getPatternSize(fm, fm->order))
	{
		size = fm_getPatternSize(fm, fm->order) - _y;
	}


	while (history[fm->order].size()>0 && currentHistoryPos[fm->order] < history[fm->order].size())
	{
		history[fm->order].pop_back();
	}

	history[fm->order].push_back(historyElem());

	history[fm->order][currentHistoryPos[fm->order]].y = _y;
	history[fm->order][currentHistoryPos[fm->order]].patternSize = fm->patternSize[fm->order];
	history[fm->order][currentHistoryPos[fm->order]].data.resize(size);

	for (unsigned y = _y; y < _y + size; y++)
	{

		history[fm->order][currentHistoryPos[fm->order]].data[y - _y].resize(FM_ch);
		if (y >= fm->patternSize[fm->order])
		{

			break;
		}
		for (int ch = 0; ch <FM_ch; ch++)
			history[fm->order][currentHistoryPos[fm->order]].data[y - _y][ch] = fm->pattern[fm->order][y][ch];
	}

	if (history[fm->order].size()>4 * 1000)
	{
		history[fm->order].erase(history[fm->order].begin() + 0);
	}
	else
	{
		currentHistoryPos[fm->order]++;
	}
}


void SongEditor::addHistory()
{
	int x1, x2, y1, y2;
	selection.getBounds(&x1, &x2, &y1, &y2);

	saveToHistory(y1, y2-y1);
	saveToHistory(y1, y2-y1);
}


void SongEditor::resetHistory()
{
	history.resize(fm->patternCount);
	currentHistoryPos.resize(fm->patternCount);
	for (int i = 0; i < history.size(); i++)
	{
		history[i].clear();
		currentHistoryPos[i] = 0;
	}
}