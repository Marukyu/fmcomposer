#include "songEditor.hpp"

void SongEditor::updateRowCount()
{
	// pattern size has changed : update row numbers

	string s;
	s.reserve(fm->patternSize[fm->order] * 3);

	for (unsigned i = 0; i < fm->patternSize[fm->order]; ++i)
	{
		s += int2str[i] + "\n";
	}
	rowNumbers.setString(s);
	updateScrollbar();
}


void SongEditor::updateFromFM()
{

	fm->order = fm->order;

	for (unsigned ch = 0; ch < FM_ch; ++ch)
	{
		updateChannelData(ch);
		channelHead[ch].updateFromFM();
	}
	if (patternList.elementCount() != fm->patternCount)
	{
		patternList.clear();
		for (unsigned i = 0; i < fm->patternCount; ++i)
			patternList.add(std::to_string(patternList.elementCount()));
	}
	patSize.setValue(fm->patternSize[fm->order]);
	updateRowCount();

	selectedRow = fm->row;
	selection.bg.setPosition(mouseXpat*COL_WIDTH, selectedRow*ROW_HEIGHT);
	selection.bg.setSize(Vector2f(COL_WIDTH, ROW_HEIGHT));

	playCursor.setPosition(0, (int)fm->row*ROW_HEIGHT);
}

void SongEditor::updateChannelData(int channel)
{
	if (fm->patternCount == 0)
		return;

	
	s0.clear();
	s1.clear();
	s2.clear();
	s3.clear();

	size_t pSize = fm->patternSize[fm->order];

	s0.reserve(5 * pSize);
	s1.reserve(5 * pSize);
	s2.reserve(5 * pSize);
	s3.reserve(5 * pSize);


	for (unsigned i = 0; i < pSize; ++i)
	{
		if (fm->pattern[fm->order][i][channel].note < 255)
			s0 += noteName(fm->pattern[fm->order][i][channel].note);
		s0 += "\n";
		if (fm->pattern[fm->order][i][channel].instr < 255)
			s1 += int2str[(int)fm->pattern[fm->order][i][channel].instr];
		s1 += "\n";
		if (fm->pattern[fm->order][i][channel].vol < 255)
			s2 += int2str[(int)fm->pattern[fm->order][i][channel].vol];
		s2 += "\n";
		if (fm->pattern[fm->order][i][channel].fx < 255)
		{
			s3 += fm->pattern[fm->order][i][channel].fx;
			s3 += int2str[(int)fm->pattern[fm->order][i][channel].fxdata];
		}
		s3 += "\n";
	}
	text[channel][0].setString(s0);
	text[channel][1].setString(s1);
	text[channel][2].setString(s2);
	text[channel][3].setString(s3);
}