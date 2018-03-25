#include "songEditor.hpp"

void SongEditor::buildContextMenus()
{

	/* Pattern edition menu */

	patMenu.add("Copy");
	patMenu.add("Paste");
	patMenu.add("Cut");
	patMenu.add("Transpose...");
	patMenu.add("Instrument...");
	patMenu.add("Volume...");
	patMenu.add("Effects...");
	patMenu.add("Insert rows...");
	patMenu.add("Remove rows...");

	/* Pattern buttons menu */

	patListMenu.add("Insert");
	patListMenu.add("Remove");
	patListMenu.add("Copy");
	patListMenu.add("Paste");
	patListMenu.add("Cut");


	editTools.add("Undo");
	editTools.add("Redo");
	editTools.add("Search");
	editTools.add("Zoom +");
	editTools.add("Zoom -");
}


void SongEditor::handlePatternListContextMenu()
{
	int choice = contextMenu->clicked();


	if (choice > -1)
	{
		movePat = -1;
	}

	switch (choice)
	{
		/* insert */
		case 0:
			pattern_insert();
			break;
			/* remove */
		case 1:
			pattern_delete();

			break;
			/* copy pattern */
		case 2:
			pattern_copy();
			break;
			/* paste pattern (insert) */
		case 3:
			pattern_paste(1);
			break;
			/* cut pattern */
		case 4:
			pattern_cut();
			break;
	}
}


void SongEditor::handlePatternContextMenu()
{
	switch (contextMenu->clicked())
	{

		case 0:
			multipleEdit(0);
			break;

		case 1:
			saveToHistory(fm->row, copiedSelection.data[0].size());
			saveToHistory(fm->row, copiedSelection.data[0].size());
			patPaste(&copiedSelection, selectedChannel, selectedRow);
			saveToHistory(fm->row, copiedSelection.data[0].size());
			saveToHistory(fm->row, copiedSelection.data[0].size());
			break;

		case 2:
			multipleEdit(2);
			break;

		case 3:
			popup->show(POPUP_TRANSPOSE);
			break;

		case 4:
			popup->show(POPUP_REPLACE_INSTRUMENT);
			if (fm->pattern[fm->order][selectedRow][selectedChannel].instr != 255)
			{
				popup->lists[0].select(fm->pattern[fm->order][selectedRow][selectedChannel].instr);
			}
			break;

		case 5:
			popup->show(POPUP_FADE);
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
			break;

		case 6:
			popup->show(POPUP_EFFECTS);
			preSelectPopupEffect();
			break;

		case 7:
			popup->show(POPUP_INSERTROWS);
			break;

		case 8:
			popup->show(POPUP_REMOVEROWS);
			if (!selection.isSingle())
			{
				int x1, x2, y1, y2;
				selection.getBounds(&x1, &x2, &y1, &y2);
				popup->sliders[0].setValue(y2 - y1);
			}
			break;
	}

}


void SongEditor::preSelectPopupEffect()
{
	// pre-select effect
	if (selectedType == 3 && fm->pattern[fm->order][fm->row][selectedChannel].fx != 255)
	{
		int fxLetterToList[26] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22 };

		if (fm->pattern[fm->order][fm->row][selectedChannel].fx - 65 >= 0 && fm->pattern[fm->order][fm->row][selectedChannel].fx - 65 < 26)
			popup->lists[0].select(fxLetterToList[fm->pattern[fm->order][fm->row][selectedChannel].fx - 65]);

		/* vibrato/tremolo : two sliders*/
		if (popup->lists[0].value == 7 || popup->lists[0].value == 9)
		{
			popup->sliders[0].setValue(fm->pattern[fm->order][fm->row][selectedChannel].fxdata / 16);
			popup->sliders[1].setValue(fm->pattern[fm->order][fm->row][selectedChannel].fxdata % 16);
		}
		else
		{
			popup->sliders[0].setValue(fm->pattern[fm->order][fm->row][selectedChannel].fxdata);
		}

	}
	popup->updateEffectDescription();
}