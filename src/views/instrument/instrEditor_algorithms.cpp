#include "instrEditor.hpp"

/*
	Set operator's positions according to their place in the tree
	*/
void InstrEditor::updateDepths(Operator* o)
{

	for (int i = 0; i < 5; i++)
	{
		int count = 0;
		if (o->top[i])
		{

			int maxBotDepth = 0, xAvgSum = 0, xAvgDiv = 0;

			//
			for (int j = 0; j < 5; j++)
			{
				if (o->top[i]->bot[j])
				{
					xAvgSum += o->top[i]->bot[j]->position.x;
					xAvgDiv++;
					if (o->top[i]->bot[j]->depth>maxBotDepth)
						maxBotDepth = o->top[i]->bot[j]->depth;
				}
			}

			if (o->depth + 1 > maxBotDepth)
			{
				o->top[i]->depth = o->depth + 1;
				o->top[i]->position.y = (connector.getPosition().y - 12) - (o->depth) * 37;
			}


			// has more than 1 children : set x pos to average of children
			o->top[i]->position.x = xAvgDiv > 1 ? 996 + (int)((xAvgSum) / xAvgDiv - 986) / 32 * 32 : o->position.x + count * 32;

			// try to correct overlapping operators
			// do FOUR passes to correct more eventual overlaps (TODO: find a better algorithm ? try to get the positions right at the start?)
			for (int k = 0; k < 4; k++)
			{
				for (int j = 0; j < 6; j++)
				{
					if (!o->isBottom && &fmop[j] != o->top[i] && o->top[i]->position.x == fmop[j].position.x && o->top[i]->position.y == fmop[j].position.y)
					{
						o->top[i]->position.x += 32;
					}
				}
			}

			
			updateDepths(o->top[i]);
			count++;
		}

	}
}

void InstrEditor::rearrangeOps()
{

	// need to set all op positions to something impossible to avoid conflicts with the op overlap algorithm
	for (int i = 0; i < 6; i++)
	{
		fmop[i].position.y = -99;
	}
	for (int i = 0; i < 6; i++)
	{
		updateDepths(&outs[i]);
	}
	for (int i = 0; i < 6; i++)
	{
		updateDepths(&outs[i]);
	}
}

void InstrEditor::linkOps()
{

	// check if the mixer is already used (an operator with more than 2 bottom connections)
	int mixFull = 0;
	for (int i = 0; i < 6; i++)
	{
		int count = 0;
		for (int j = 0; j < 5; j++)
		{

			if (fmop[i].top[j])
			{
				count++;
			}
			if (count >= 3)
			{
				mixFull = 1;
			}
		}
	}


	for (int i = 0; i < 6; i++)
	{

		if (fmop[i].active2)
		{
			for (int j = 0; j < 6; j++)
			{
				if (fmop[j].hovered && i != j)
				{
					Operator* a, *b;
					if (fmop[i].depth >= fmop[j].depth)
					{
						a = &fmop[i];
						b = &fmop[j];
					}
					else if (fmop[i].depth < fmop[j].depth)
					{
						b = &fmop[i];
						a = &fmop[j];
					}
					else
					{
						return;
					}
					int count = 0;
					for (int k = 0; k < 5; k++)
					{
						if (b->top[k])
						{
							count++;
						}
					}

					// "Mix" already full, cant create more links
					if (mixFull && count >= 2)
						return;



					a->connect(b, 0, 1);

					updateAlgoToFM();
					break;
				}
			}
		}
	}
}

void InstrEditor::updateAlgoToFM()
{
	for (int i = 0; i < 6; i++)
	{
		fm->instrument[instrList->value].op[i].connect = -1;
		fm->instrument[instrList->value].op[i].connect2 = -1;
		fm->instrument[instrList->value].op[i].connectOut = -1;

	}
	for (int i = 0; i < 4; i++)
	{
		fm->instrument[instrList->value].toMix[i] = -1;
	}
	for (int i = 0; i < 6; i++)
	{
		if (outs[i].top[0] != NULL)
			fm->instrument[instrList->value].op[i].connectOut = outs[i].top[0]->id;
	}

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (fmop[i].top[j])
			{
				if (fm->instrument[instrList->value].op[fmop[i].id].connect == -1)
				{
					fm->instrument[instrList->value].op[fmop[i].id].connect = fmop[i].top[j]->id;
				}
				else if (fm->instrument[instrList->value].op[fmop[i].id].connect2 == -1)
				{

					fm->instrument[instrList->value].op[fmop[i].id].connect2 = fmop[i].top[j]->id;
				}
				else
				{
					if (fm->instrument[instrList->value].toMix[0] == -1)
					{

						fm->instrument[instrList->value].toMix[0] = fm->instrument[instrList->value].op[fmop[i].id].connect2;
						fm->instrument[instrList->value].op[fmop[i].id].connect2 = 6;
					}
					int adder = 0;
					while (fm->instrument[instrList->value].toMix[adder] >= 0 && adder < 3)
						adder++;
					fm->instrument[instrList->value].toMix[adder] = fmop[i].top[j]->id;

				}
			}
		}
	}
	for (int i = 0; i < FM_ch; i++)
	{
		if (fm->ch[i].instrNumber == instrList->value)
			fm->ch[i].cInstr = 0;
	}
}


void InstrEditor::updateAlgoFromFM()
{
	algo.setValue(35);
	algo.setDisplayedValueOnly("Custom");
	customPreset = fm->instrument[instrList->value];
	// reset op connections
	for (int i = 0; i < 6; i++)
	{
		fmop[i].id = i;
		fmop[i].number.setString(int2str[fmop[i].id + 1]);
		for (int j = 0; j < 5; j++)
		{
			fmop[i].bot[j] = NULL;
			fmop[i].top[j] = NULL;
			outs[i].top[j] = NULL;
			outs[i].bot[j] = NULL;
		}
	}

	// connect to outs
	for (int i = 0; i < 6; i++)
	{
		if (fm->instrument[instrList->value].op[i].connectOut >= 0)
		{
			fmop[fm->instrument[instrList->value].op[i].connectOut].connect(&outs[i]);
		}
	}

	// links
	for (int i = 0; i < 6; i++)
	{
		if (fm->instrument[instrList->value].op[i].connect >= 0)
		{
			fmop[fm->instrument[instrList->value].op[i].connect].connect(&fmop[i], 1, 1);
		}

		if (fm->instrument[instrList->value].op[i].connect2 >= 0)
		{
			if (fm->instrument[instrList->value].op[i].connect2 > 5)
			{
				for (int j = 0; j < 4; j++)
				{
					if (fm->instrument[instrList->value].toMix[j] >= 0)
					{
						fmop[fm->instrument[instrList->value].toMix[j]].connect(&fmop[i], 1, 1);

					}
				}

			}
			else
			{
				fmop[fm->instrument[instrList->value].op[i].connect2].connect(&fmop[i], 1, 1);
			}
		}


	}

	rearrangeOps();

	for (int i = 0; i < 6; i++)
		fmop[i].updatePosition();
}