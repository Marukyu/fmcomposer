#include "instrEditor.hpp"
#include "../pattern/songEditor.hpp"

#include "../pianoroll/pianoRoll.hpp"

void InstrEditor::handleGlobalEvents()
{

	/* Switch instrument by clicking the list */



	if ((!contextMenu || contextMenu && !contextMenu->hover()))
	{
		mouse.pos = input_getmouse(borderView);
		if (instrList->clicked())
		{

			updateFromFM();
			if (state==pianoRoll)
				pianoRoll->focusInstrument(instrList->value);
		}
	}
	mouse.pos = input_getmouse(borderView);
	/* Handle list swapping elements */

	int to = -1, from = -1;
	instrList->movedElement(&to, &from);

	if (mouse.clickd && !popup->visible && instrList->hover())
	{
		instrumentMenu.show();
	}

	if (to >= 0)
	{

		vector<int> instrIds;
		instrIds.resize(instrList->text.size());
		for (int i = 0; i<instrIds.size(); i++)
			instrIds[i] = i;

		/* Move instruments by pairs until the elements are in the right position */
		for (int i = from; i >to; i--)
		{
			fm_instrument temp = fm->instrument[i];
			fm->instrument[i] = fm->instrument[i - 1];
			fm->instrument[i - 1] = temp;
			/* Keep trace of the new instrument index for easy replace in the patterns afterwards */
			instrIds[i] -= (i - to);
			instrIds[i - 1] += (i - to);
		}

		for (int i = from; i < to; i++)
		{
			fm_instrument temp = fm->instrument[i];
			fm->instrument[i] = fm->instrument[i + 1];
			fm->instrument[i + 1] = temp;
			instrIds[i] -= (i - to);
			instrIds[i + 1] += (i - to);
		}


		for (unsigned i = 0; i < fm->patternCount; i++)
		{
			for (unsigned j = 0; j < fm->patternSize[i]; j++)
			{
				for (unsigned ch = 0; ch < FM_ch; ch++)
				{
					if (fm->pattern[i][j][ch].instr != 255)
					{
						fm->pattern[i][j][ch].instr = instrIds[fm->pattern[i][j][ch].instr];
					}
				}
			}
		}

		songModified(1);
		updateFromFM();
		updateInstrListFromFM();

		if (state == songEditor)
			songEditor->updateFromFM();
	}



	/* Instrument list contextual menu */

	if (contextMenu == &instrumentMenu)
	{
		switch (contextMenu->clicked())
		{
			// copy
			case 0:
				copiedInstr = fm->instrument[instrList->value];
				copied = true;
				break;
				// paste
			case 1:
				if (copied)
				{
					fm->instrument[instrList->value] = copiedInstr;
					valueChanged = 1;
					addToUndoHistory();
					updateFromFM();
					updateInstrListFromFM();
				}
				break;
				// remove
			case 2:
				popup->show(POPUP_DELETEINSTRUMENT);
				break;
		}
	}
}