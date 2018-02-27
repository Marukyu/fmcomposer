#include "generalEditor.hpp"
#include "../../gui/drawBatcher.hpp"

GeneralEditor* generalEditor;

GeneralEditor::GeneralEditor() :
tempo(50, 50, 255, 0, "Tempo (BPM)", 120),
songName(100, 160, 63, "Title :"),
author(100, 190, 63, "Author :"),
comments(100, 220, 255, "Comments :", true, 7, 483),
globalVolume(300, 50, 99, 0, "Global Volume", 60),
diviseur(140, 100, 32, 1, "", 8, 100),
diviseurText("1 quarter note =", font, charSize),
reverbLength(550, 50, 40, 0, "Room reverberation", 4),
roomSize(550, 80, 40, 1, "Room size", 5),
transpose(800, 50, 12, -12, "Transpose (semitones)", 0),
rows("rows", font, charSize)
{
	fm->tempo = tempo.value;
	reverb.setPosition(650, 325);
	rows.setPosition(245, 100);
	rows.setColor(colors[BLOCKTEXT]);
	diviseurText.setPosition(20, 100);
	diviseurText.setColor(colors[BLOCKTEXT]);
	updateFromFM();
}

void GeneralEditor::draw()
{
	window->setView(globalView);

	drawBatcher.initialize();

	drawBatcher.addItem(&tempo);
	
	drawBatcher.addItem(&songName);
	drawBatcher.addItem(&author);
	drawBatcher.addItem(&comments);

	drawBatcher.addItem(&diviseur);
	drawBatcher.addItem(&globalVolume);
	drawBatcher.addItem(&reverbLength);
	drawBatcher.addItem(&roomSize);
	drawBatcher.addItem(&transpose);

	drawBatcher.addItem(&rows);
	drawBatcher.addItem(&diviseurText);

	drawBatcher.draw();
}

void GeneralEditor::update()
{

	if (mouse.pos.x > (int)windowWidth - 215)
		return;

	static int tempoUpdated = 0;

	if (tempo.update())
	{
		tempoUpdated = 1;
	}
	//printf("rev %f\n", fm->reverbLength);
	if ((mouse.clickgReleased || mouse.scroll) && tempoUpdated)
	{
		tempoUpdated = 0;
		fm_setTempo(fm, tempo.value);
		songModified(1);

		fm_buildStateTable(fm, 0, fm->patternCount, 0, FM_ch);

	}


	else if (globalVolume.update() || diviseur.update() || transpose.update() || reverbLength.update())
	{
		fm_setVolume(fm, globalVolume.value);
		fm->diviseur = diviseur.value;
		fm->transpose = transpose.value;

		fm->initialReverbLength = fm->reverbLength = 0.5 + (float)reverbLength.value*0.0125;
		songModified(1);
	}

	if (songName.modified() || author.modified() || comments.modified())
	{
		strncpy(fm->songName, songName.text.getString().toAnsiString().c_str(), 64);
		strncpy(fm->author, author.text.getString().toAnsiString().c_str(), 64);
		strncpy(fm->comments, comments.text.getString().toAnsiString().c_str(), 256);
		songModified(1);
	}

	if (roomSize.update())
	{
		fm_initReverb(fm, roomSize.value*0.025);
		fm->initialReverbRoomSize = roomSize.value*0.025;
	}


	/*
	for(unsigned ch = 0; ch< FM_chCount; ++ch){
	if(surround[ch].update())
	fm->surround[ch] = surround[ch].value*-2+1;
	}*/
}

void GeneralEditor::updateFromFM()
{
	songName.setText(fm->songName);
	songName.unselect();
	author.setText(fm->author);
	author.unselect();
	comments.setText(fm->comments);
	comments.unselect();
	globalVolume.setValue(fm->_globalVolume);
	diviseur.setValue(fm->diviseur);
	tempo.setValue(fm->initial_tempo);
	transpose.setValue(fm->transpose);
	reverbLength.setValue((fm->initialReverbLength - 0.5) * 80 + 0.5);
	roomSize.setValue(fm->initialReverbRoomSize * 40 + 0.5);
	/*for(unsigned ch = 0; ch< FM_chCount; ++ch){
		surround[ch].setValue(fm->surround[ch]*-1);
		}*/


}

void GeneralEditor::handleEvents()
{
	handleNotePreview(!songName.editing && !author.editing && !comments.editing);
}