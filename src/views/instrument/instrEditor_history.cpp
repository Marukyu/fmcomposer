#include "instrEditor.hpp"

void InstrEditor::undo()
{
	// needed if value changed with mousescroll (no click)
	addToUndoHistory();

	if (currentHistoryPos[instrList->value] > 1)
	{
		currentHistoryPos[instrList->value] -= 2;


		fm->instrument[instrList->value] = history[instrList->value][currentHistoryPos[instrList->value]];
		currentHistoryPos[instrList->value]++;
		updateFromFM();
		updateAlgoFromFM();
		updateInstrListFromFM();
		updateToFM();
	}
}

void InstrEditor::redo()
{

	if (currentHistoryPos[instrList->value] < history[instrList->value].size())
	{

		fm->instrument[instrList->value] = history[instrList->value][currentHistoryPos[instrList->value]];
		currentHistoryPos[instrList->value]++;
		updateFromFM();
		updateAlgoFromFM();
		updateInstrListFromFM();
		updateToFM();
	}
}

void InstrEditor::addToUndoHistory()
{

	if (valueChanged)
	{

		while (history[instrList->value].size() > 0 && currentHistoryPos[instrList->value] < history[instrList->value].size())
		{
			history[instrList->value].pop_back();
		}

		history[instrList->value].push_back(fm->instrument[instrList->value]);

		if (history[instrList->value].size()>1000)
		{
			history[instrList->value].erase(history[instrList->value].begin() + 0);
		}
		else
		{
			currentHistoryPos[instrList->value]++;
		}

		valueChanged = 0;
		songModified(1);
	}
}