#include "midi.h"
#include <fstream>

vector<int> midiExportAssoc;
vector<int> midiExportAssocChannels;

void WriteVarLen(register unsigned long value, ofstream* f)
{
	register unsigned long buffer;
	buffer = value & 0x7F;
	while ((value >>= 7))
	{
		buffer <<= 8;
		buffer |= ((value & 0x7F) | 0x80);
	}
	while (1)
	{
		f->write((char*)&buffer, 1);
		if (buffer & 0x80)
			buffer >>= 8;
		else
			break;
	}
}


void midi_writeTempo(ofstream *file, int tempo)
{
	tempo = 60000000 / (tempo*fm->diviseur / 4);

	unsigned char tempo1 = (tempo & 0x00FF0000) >> 16;
	unsigned char tempo2 = (tempo & 0x0000FF00) >> 8;
	unsigned char tempo3 = (tempo & 0x000000FF);
	file->put(0xFF);
	file->put(0x51);
	file->put(0x03);
	file->write((char*)&tempo1, 1);
	file->write((char*)&tempo2, 1);
	file->write((char*)&tempo3, 1);
}


void midiExport(const char* filename)
{
	int chunksize, cpt = 0, channels2 = 0x0100;
	int header_size = (6 >> 24) | (6 << 24);
	int file_format = 0;
	int delta_time_ticks = 32;
	delta_time_ticks = (delta_time_ticks >> 8) | (delta_time_ticks << 8);
	vector <int> lastnote(FM_ch, -1), lastinstr(FM_ch, -1), lastvol(FM_ch, -1);

	int bufferChVol[FM_ch], bufferChPan[FM_ch], bufferChPitchbend[FM_ch];

	for (int i = 0; i < FM_ch; i++)
	{
		bufferChVol[i] = fm->ch[i].initial_vol;
		bufferChPan[i] = fm->ch[i].initial_pan;
		bufferChPitchbend[i] = -1;
	}


	ofstream file(filename, ios::out | ios::binary);
	file.write("MThd", 4);
	file.write((char*)&header_size, 4);
	file.write((char*)&file_format, 2);
	file.write((char*)&channels2, 2);
	file.write((char*)&delta_time_ticks, 2);
	file.write("MTrk", 4);
	file.write("", 4);


	// write time signature
	file.put(0x00);
	file.put(0xFF);
	file.put(0x58);
	file.put(4);
	file.put(4);
	file.put(2);
	file.put(25);
	file.put(8);
	file.put(0);

	midi_writeTempo(&file, fm->initial_tempo);

	// write song name
	if (strlen(fm->songName))
	{
		file.put(0x00);
		file.put(0xFF);
		file.put(0x03);
		WriteVarLen(strlen(fm->songName), &file);
		file.write(fm->songName, strlen(fm->songName));
	}

	// write author
	if (strlen(fm->author))
	{
		file.put(0x00);
		file.put(0xFF);
		file.put(0x02);
		WriteVarLen(strlen(fm->author), &file);
		file.write(fm->author, strlen(fm->author));
	}

	// write comments
	if (strlen(fm->comments))
	{
		file.put(0x00);
		file.put(0xFF);
		file.put(0x01);
		WriteVarLen(strlen(fm->comments), &file);
		file.write(fm->comments, strlen(fm->comments));
	}


	for (unsigned int k = 0; k < fm->patternCount; k++)
	{ // for each order
		for (unsigned int j = 0; j < fm->patternSize[k]; j++)
		{ // for each row
			for (int i = 0; i<FM_ch; i++)
			{
				if (fm->pattern[k][j][i].note != 255)
				{

					// note off
					if (lastnote[i] != -1 && lastinstr[i]>-1)
					{
						WriteVarLen(cpt * 8, &file);
						file.put(128 + midiExportAssocChannels[lastinstr[i]]);
						file.write((char*)&lastnote[i], 1);
						file.write((char*)&lastvol[i], 1);
						cpt = 0;
						lastnote[i] = -1;
					}
					// program change
					if (fm->pattern[k][j][i].note < 128 && lastinstr[i] != fm->pattern[k][j][i].instr && fm->pattern[k][j][i].instr != 255)
					{
						WriteVarLen(cpt * 8, &file);
						file.put(192 + midiExportAssocChannels[fm->pattern[k][j][i].instr]);
						file.put(midiExportAssoc[fm->pattern[k][j][i].instr]);
						lastinstr[i] = fm->pattern[k][j][i].instr;
						cpt = 0;
					}
					// note on
					if (fm->pattern[k][j][i].note < 128)
					{
						if (bufferChVol[i] != -1)
						{
							WriteVarLen(cpt * 8, &file);
							file.put(0xB0 + midiExportAssocChannels[fm->pattern[k][j][i].instr]);
							file.put(0x07);
							file.put(bufferChVol[i]);
							cpt = 0;
							bufferChVol[i] = -1;
						}
						if (bufferChPan[i] != -1)
						{
							WriteVarLen(cpt * 8, &file);
							file.put(0xB0 + midiExportAssocChannels[fm->pattern[k][j][i].instr]);
							file.put(0x0A);
							file.put(bufferChPan[i]);
							cpt = 0;
							bufferChPan[i] = -1;
						}
						if (bufferChPitchbend[i] != -1)
						{
							WriteVarLen(cpt * 8, &file);
							file.put(0xE0 + midiExportAssocChannels[fm->pattern[k][j][i].instr]);
							file.put(0x00); // ignore pitch bend fine byte
							file.put(bufferChPitchbend[i]);
							cpt = 0;
							bufferChPitchbend[i] = -1;
						}

						// percussion
						if (fm->pattern[k][j][i].instr < 255 && midiExportAssocChannels[fm->pattern[k][j][i].instr] == 9 ||
							lastinstr[i] != -1 && midiExportAssocChannels[lastinstr[i]] == 9)
						{
							WriteVarLen(cpt * 8, &file);
							file.put(144 + 9);
							lastnote[i] = 23 + midiExportAssoc[fm->pattern[k][j][i].instr]; // convert instrument to drumkit note
						}
						// melodic
						else
						{
							WriteVarLen(cpt * 8, &file);
							if (fm->pattern[k][j][i].instr<255)
							lastinstr[i] = fm->pattern[k][j][i].instr;
							
							file.put(144 + midiExportAssocChannels[lastinstr[i]]);
							lastnote[i] = fm->pattern[k][j][i].note;
						}

						if (fm->pattern[k][j][i].vol != 255)
							lastvol[i] = fm->pattern[k][j][i].vol*1.282828;
						else if (lastvol[i] == -1) // no volume = assume max
							lastvol[i] = 127;

						file.write((char*)&lastnote[i], 1);
						file.write((char*)&lastvol[i], 1);
						cpt = 0;
					}


					switch (fm->pattern[k][j][i].fx)
					{
						case 'T': // tempo
							WriteVarLen(cpt * 8, &file);
							midi_writeTempo(&file, fm->pattern[k][j][i].fxdata);
							cpt = 0;
							break;
						case 'M': // ch volume
							if (lastinstr[i] != -1)
							{
								WriteVarLen(cpt * 8, &file);
								file.put(0xB0 + midiExportAssocChannels[lastinstr[i]]);
								file.put(0x07);
								file.put(fm->pattern[k][j][i].fxdata*1.282828);
								cpt = 0;
							}
							else
								bufferChVol[i] = fm->pattern[k][j][i].fxdata*1.282828;
							break;
						case 'X': // ch panning
							if (lastinstr[i] != -1)
							{
								WriteVarLen(cpt * 8, &file);
								file.put(0xB0 + midiExportAssocChannels[lastinstr[i]]);
								file.put(0x0A);
								file.put(fm->pattern[k][j][i].fxdata / 2);
								cpt = 0;
							}
							else
								bufferChPan[i] = fm->pattern[k][j][i].fxdata / 2;
							break;
						case 'I': // pitch bend
							if (lastinstr[i] != -1)
							{
								WriteVarLen(cpt * 8, &file);
								file.put(0xE0 + midiExportAssocChannels[lastinstr[i]]);
								file.put(0x00); // ignore pitch bend fine byte
								file.put(fm->pattern[k][j][i].fxdata / 2);
								cpt = 0;
							}
							else
								bufferChPitchbend[i] = fm->pattern[k][j][i].fxdata / 2;
							break;
					}



				}
			}
			cpt++;
		}
		for (int i = 0; i < FM_ch; i++)
		{
			if (lastnote[i] != -1)
			{
				WriteVarLen(cpt * 8, &file);
				file.put(128 + midiExportAssocChannels[lastinstr[i]]);
				file.write((char*)&lastnote[i], 1);
				file.write((char*)&lastvol[i], 1);
				cpt = 0;
			}
		}

	}


	file.put(0x00);


	file.put(0xFF);
	file.put(0x2F);
	file.put(0x00);
	chunksize = (int)file.tellp() - 22;
	file.seekp(18);
	chunksize = (chunksize >> 24) | ((chunksize & 0x00ff0000) >> 8) | ((chunksize & 0x0000ff00) << 8) | (chunksize << 24);
	file.write((char*)&chunksize, 4);
	file.close();
}
