#include "songEditor.hpp"

void SongEditor::pattern_copy()
{
	copiedPattern.resize(fm->patternSize[fm->order]);
	for (int i = 0; i < copiedPattern.size(); i++)
	{
		copiedPattern[i].resize(FM_ch);
		for (int ch = 0; ch <FM_ch; ch++)
			copiedPattern[i][ch] = fm->pattern[fm->order][i][ch];
	}
}

void SongEditor::pattern_paste(int insertAfter)
{
	if (copiedPattern.size()>0)
	{
		patternList.insert(fm->order + insertAfter, std::to_string(patternList.elementCount()));
		fm_insertPattern(fm, copiedPattern.size(), fm->order);
		for (int i = 0; i < copiedPattern.size(); i++)
		{
			for (int ch = 0; ch < FM_ch; ch++)
				fm->pattern[fm->order][i][ch] = copiedPattern[i][ch];
		}
		updateFromFM();
		songModified(1);
		history.insert(history.begin() + fm->order, vector<historyElem>());
		currentHistoryPos.insert(currentHistoryPos.begin() + fm->order, 0);
		fm_setPosition(fm, fm->order + insertAfter, fm->row, 2);
	}
}
void SongEditor::pattern_delete()
{
	if (fm->patternCount > 1)
	{

		patternList.erase(fm->order);
		history.erase(history.begin() + fm->order);
		currentHistoryPos.erase(currentHistoryPos.begin() + fm->order);

		fm_removePattern(fm, fm->order);


	}
	else
	{
		fm_removePattern(fm, 0);
		history[0].clear();
		currentHistoryPos[0] = 0;
	}
	fm->order = clamp(fm->order, 0, fm->patternCount - 1);
	updateFromFM();
	songModified(1);
}

void SongEditor::pattern_cut()
{

	copiedPattern.resize(fm->patternSize[fm->order]);
	for (int i = 0; i < copiedPattern.size(); i++)
	{
		copiedPattern[i].resize(FM_ch);
		for (int ch = 0; ch < FM_ch; ch++)
			copiedPattern[i][ch] = fm->pattern[fm->order][i][ch];
	}

	pattern_delete();
}

void SongEditor::pattern_duplicate()
{
	pattern_copy();
	pattern_paste(1);
}

void SongEditor::pattern_insert()
{
	patternList.insert(fm->order, std::to_string(patternList.elementCount()));
	fm_insertPattern(fm, patSize.value, fm->order);
	updateFromFM();
	history.insert(history.begin() + fm->order, vector<historyElem>());
	currentHistoryPos.insert(currentHistoryPos.begin() + fm->order, 0);
	songModified(1);
}

void SongEditor::pattern_move(int from, int to)
{
	fm_movePattern(fm, from, to);

	songModified(1);


	patternList.move(from, to);

	history.insert(history.begin() + to+(to>from), history[from]);
	history.erase(history.begin() + from + (to <= from));

	currentHistoryPos.insert(currentHistoryPos.begin() + to+(to>from), currentHistoryPos[from]);
	currentHistoryPos.erase(currentHistoryPos.begin() + from + (to <= from));

}