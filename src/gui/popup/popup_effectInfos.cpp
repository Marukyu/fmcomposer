#include "popup.hpp"

#include "../../views/pattern/songEditor.hpp"
extern SongEditor* songEditor;

/* Displays appropriate infos and sliders for each effect in the effect popup */


unsigned char findPrevious(int type, int _order, int _row, int channel, int*foundOrder, int*foundRow)
{
	while (_row < 0)
	{
		_order--;
		if (_order < 0)
		{
			_row = 0;
			_order = 0;
			break;
		}
		_row += fm_getPatternSize(fm, _order);
	}
	int row = _row;
	int order = _order;

	while (1)
	{
		if (type == 0 && fm->pattern[order][row][channel].note < 128
			|| type == 1 && fm->pattern[order][row][channel].instr != 255)
			break;
		row--;
		if (row < 0)
		{
			order--;
			if (order < 0)
			{
				order = 0;
				row = 0;
				break;
			}
			row = fm_getPatternSize(fm, order) - 1;
		}
	}
	if (foundOrder)
		*foundOrder = order;
	if (foundRow)
		*foundRow = row;

	if (type == 0)
		return fm->pattern[order][row][channel].note;
	else
		return fm->pattern[order][row][channel].instr;

}

void Popup::updateEffectDescription()
{
	sliders[1].setVisible(false);
	switch (lists[0].value)
	{
		default:
			texts[0].setString("");
			texts[1].setString("");
			break;
		case 0:{ // Arpeggio
				   sliders[0].setMinMax(0, 255);

				   int pos = fm->order * 128 + fm->row;
				   int maxBackward = 128;
				   while (maxBackward-- > 0 && pos > 0 && fm->pattern[pos / 128][pos % 128][songEditor->selectedChannel].note > 127)
				   {
					   pos--;
				   }
				   if (fm->pattern[pos / 128][pos % 128][songEditor->selectedChannel].note < 128)
				   {
					   int baseNote = fm->pattern[pos / 128][pos % 128][songEditor->selectedChannel].note;
					   texts[0].setString(L"Play a fast triplet arpeggio :\n>" + noteName(baseNote) + L"\n> " + noteName(baseNote + sliders[0].value % 16) + " (" + intervals[sliders[0].value % 16] + L")\n> " + noteName(baseNote + sliders[0].value / 16) + " (" + intervals[sliders[0].value / 16] + ") ");
				   }
				   else
				   {
					   texts[0].setString(L"Play a fast triplet arpeggio :\n> Base note\n> note+" + int2str[sliders[0].value % 16] + " (" + intervals[sliders[0].value % 16] + ") " + L"\n> note+" + int2str[sliders[0].value / 16] + " (" + intervals[sliders[0].value / 16] + ") ");
				   }
				   sliders[0].name.setString("Arpeggio notes");
				   texts[1].setString("");
				   sprites[0].setColor(Color(0, 0, 0, 0));
		}
			break;
		case 1: // B - Pattern jump
			texts[0].setString("Jump to another pattern");
			texts[1].setString("");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Pattern number");
			sprites[0].setColor(Color(0, 0, 0, 0));
			break;
		case 2: // C - Row jump
			texts[0].setString("Jump to another row");
			texts[1].setString("");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Row number");
			sprites[0].setColor(Color(0, 0, 0, 0));
			break;
		case 3: // D - Delay
			texts[0].setString("Play the note with precise timing (1/8ths of a row)");
			texts[1].setString("No delay								Max delay");
			sliders[0].setMinMax(0, 7);
			sliders[0].name.setString("");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 4: // E - Portamento up
			texts[0].setString("Pitch slide up");
			texts[1].setString("Slow												Fast");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Speed");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 5: // F - Portamento down
			texts[0].setString("Pitch slide down");
			texts[1].setString("Slow												Fast");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Speed");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 6:{ // G - Glissando

				   texts[0].setString("Pitch slide between two notes");

				   int order, row;
				   unsigned char note1 = findPrevious(0, fm->order, fm->row, songEditor->selectedChannel, &order, &row);

				   if (note1 < 128)
				   {

					   unsigned char note2 = findPrevious(0, order, row - 1, songEditor->selectedChannel, &order, &row);

					   if (note2 < 128)
					   {
						   texts[0].setString("Pitch slide between " + noteName(note2) + " and " + noteName(note1));
					   }

				   }


				   texts[1].setString("Slow												Fast");
				   sliders[0].setMinMax(0, 255);
				   sliders[0].name.setString("Speed");
				   sprites[0].setColor(Color(255, 255, 255, 255));
		}
			break;
		case 7: // Vibrato
			sliders[1].setVisible(true);
			sliders[0].setMinMax(0, 15);
			sliders[1].setMinMax(0, 15);
			texts[0].setString("Add vibrato");
			texts[1].setString("");
			sliders[0].name.setString("Speed");
			sliders[1].name.setString("Depth");
			sprites[0].setColor(Color(255, 255, 255, 0));
			break;

		case 8: // I Pitch bend
			sliders[0].setMinMax(0, 255);
			texts[0].setString("Set the pitch bend wheel (-2/+2 semitones range)");
			texts[1].setString("-2							0						 +2");
			sliders[0].name.setString("");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 9: // J Tremolo
			sliders[1].setVisible(true);
			sliders[0].setMinMax(0, 15);
			sliders[1].setMinMax(0, 15);
			texts[0].setString("Add tremolo");
			texts[1].setString("");
			sliders[0].name.setString("Speed");
			sliders[1].name.setString("Depth");
			sprites[0].setColor(Color(255, 255, 255, 0));
			break;
		case 10:{ // K FM parameter select

					unsigned char instr = findPrevious(1, fm->order, fm->row, songEditor->selectedChannel, NULL, NULL);

					if (instr >= fm->instrumentCount)
						instr = 0;

					if (fm->instrument[instrList->value].kfx / 32 == 0)
						texts[0].setString("Current instrument : " + string(fm->instrument[instr].name) + "\nSet Global " + kfx_globalParams[fm->instrument[instrList->value].kfx % 32]);
					else
						texts[0].setString("Current instrument : " + string(fm->instrument[instr].name) + "\nSet Operator " + std::to_string(fm->instrument[instrList->value].kfx / 32) + " : " + kfx_operatorParams[fm->instrument[instrList->value].kfx % 32]);



					texts[1].setString("");
					sliders[0].name.setString("Value");
					sprites[0].setColor(Color(255, 255, 255, 255));



					break;


		}
		case 11: // M - Channel volume
			texts[1].setString("Quiet											  Loud");
			texts[0].setString("Set the channel volume");
			sliders[0].setMinMax(0, 99);
			sliders[0].name.setString("Volume");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 12:{ // N - Channel volume slide
					sliders[0].setMinMax(0, 255);
					string s;
					if (sliders[0].value < 127)
					{
						s = "Decrease ";
						s += sliders[0].value < 64 ? "fastly" : "slowly";
					}
					else if (sliders[0].value == 127)
					{
						s = "No change";
					}
					else
					{
						s = "Increase ";
						s += sliders[0].value < 192 ? "slowly" : "fastly";
					}
					texts[0].setString("Smooth channel volume slide :\n" + s);

					texts[1].setString("Decrease	   No change        Increase");
					sliders[0].name.setString("");
					sprites[0].setColor(Color(255, 255, 255, 255));
		}
			break;
		case 13:{ // P Panning slide
					sliders[0].setMinMax(0, 255);
					string s;
					if (sliders[0].value < 127)
					{
						s = "To left, ";
						s += sliders[0].value < 64 ? "fast" : "slow";
					}
					else if (sliders[0].value == 127)
					{
						s = "No change";
					}
					else
					{
						s = "To right, ";
						s += sliders[0].value < 192 ? "slow" : "fast";
					}
					texts[0].setString("Smooth panning slide :\n" + s);
					texts[1].setString("To left			No change	      To right");
					sliders[0].name.setString("Speed/direction");
					sprites[0].setColor(Color(255, 255, 255, 255));
		}
			break;
		case 14:{ // Q Note retrigger
					sliders[0].setMinMax(0, 8);
					texts[0].setString("Faslty repeat the note on the same row");
					texts[1].setString("");
					sliders[0].name.setString("Times per row");
					sprites[0].setColor(Color(255, 255, 255, 255));
		}
			break;
		case 15: // R - Reverb send
			texts[0].setString("Set the channel's reverb amount");
			texts[1].setString("No reverb							 Max reverb");
			sliders[0].setMinMax(0, 99);
			sliders[0].name.setString("Amount");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 16:{ // S - Global reverb params

					string s;
					if (sliders[0].value <= 40)
					{
						s = "Room reveberation : " + std::to_string(sliders[0].value);
					}
					else
					{
						s = "Room size : " + std::to_string(clamp(sliders[0].value - 40, 1, 40));
					}
					texts[0].setString("Set reverb parameters :\n" + s);

					texts[1].setString("");
					sliders[0].setMinMax(0, 80);
					sliders[0].name.setString("Value");
					sprites[0].setColor(Color(255, 255, 255, 255));
					break;
		}
		case 17: // T - Set tempo
			texts[0].setString("Set the tempo (BPM)");
			texts[1].setString("Slow												Fast");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Tempo");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 18: // V - Global volume
			texts[0].setString("Set the global volume");
			texts[1].setString("Quiet											  Loud");
			sliders[0].setMinMax(0, 99);
			sliders[0].name.setString("Volume");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 19:{ // V - Global volume slide

					sliders[0].setMinMax(0, 255);
					string s;
					if (sliders[0].value < 127)
					{
						s = "Decrease ";
						s += sliders[0].value < 64 ? "fastly" : "slowly";
					}
					else if (sliders[0].value == 127)
					{
						s = "No change";
					}
					else
					{
						s = "Increase ";
						s += sliders[0].value < 192 ? "slowly" : "fastly";
					}
					texts[0].setString("Smooth global volume slide :\n" + s);
					texts[1].setString("Decrease	   No change        Increase");
					sliders[0].name.setString("");
					sprites[0].setColor(Color(255, 255, 255, 255));
					break; }
		case 20: // X - Channel panning
			texts[0].setString("Set channel panning");
			texts[1].setString("Left				   Center				  Right");
			sliders[0].setMinMax(0, 255);
			sliders[0].name.setString("Pan");
			sprites[0].setColor(Color(255, 255, 255, 255));
			break;
		case 21: // Y - Sync marker
			sliders[0].setMinMax(0, 255);
			texts[0].setString("Place a marker that will trigger a callback function");
			sliders[0].name.setString("Marker ID");
			texts[1].setString("");
			sprites[0].setColor(Color(0, 0, 0, 0));
			break;
	}
}
