#include "popup.hpp"
#include "../contextmenu/contextmenu.hpp"
#include "../../streamed/streamedExport.h"

void setEffectList(List* list)
{
	list->add("A - Arpeggio");
	list->add("B - Pattern jump");
	list->add("C - Row jump");
	list->add("D - Delay");
	list->add("E - Portamento up");
	list->add("F - Portamento down");
	list->add("G - Glissando");
	list->add("H - Vibrato");
	list->add("I - Pitch bend");
	list->add("J - Tremolo");
	list->add("K - Instrument control");
	list->add("M - Channel volume");
	list->add("N - Channel volume slide");
	list->add("P - Panning slide");
	list->add("Q - Retrigger note");
	list->add("R - Channel reverb");
	list->add("S - Global reverb params");
	list->add("T - Tempo");
	list->add("V - Global volume");
	list->add("W - Global volume slide");
	list->add("X - Channel panning");
	list->add("Y - Sync marker");
}

void Popup::show(int _type, int param)
{
	if (visible)
	{
		close();
	}
	contextMenu = NULL;
	texts.clear();
	type = _type;
	textures.clear();
	sliders.clear();
	lists.clear();
	sprites.clear();
	buttons.clear();
	shapes.clear();
	checkboxes.clear();
	delay = -1;
	visible = 1;
	switch (type)
	{
		case POPUP_SAVED:{
							 setSize(250, 100);
							 title.setString("Info");

							 Sprite s;
							 s.setTexture(*tileset);
							 s.setTextureRect(IntRect(0, 0, 24, 22));
							 sprites.push_back(s);
							 sprites[0].setPosition(80, 34);


							 delay = 1;
							 texts.push_back(Text("Saved !", font, charSize));
							 texts[0].setColor(colors[BLOCKTEXT]);
							 texts[0].setPosition(110, 40);
							 break;
		}
		case POPUP_WORKING:
			setSize(500, 200);
			title.setString("Info");
			texts.push_back(Text("Rendering, please wait...", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 20);
			sliders.push_back(DataSlider(80, 70, 99, 0, "%", 99));
			buttons.push_back(Button(50, h - 50, "Abort", -1, 8));
			break;
		case POPUP_ABOUT:{
			setSize(740, 400);
			title.setString("About");

			Sprite s;
			s.setTexture(*tileset);
			s.setTextureRect({ 260, 0, 256, 256 });
			sprites.push_back(s);
			sprites[0].setPosition(10, 10);

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(20, h - 50, "Go to the website", -1, 8));
			buttons.push_back(Button(180, h - 50, "Check for updates", -1, 8));
			texts.push_back(Text(L"FM Composer, © 2017-2018 Stéphane Damo\n\n--------- Credits ---------\n\nTesting, help and advices :\n		Klairzaki Fil-Xter, Masami Komuro, Isaac Zuniga\n\nLibraries authors :\n		Laurent Gomila & SFML contributors (SFML lib)\n		Guillaume Vareille (tinyfiledialogs lib)\n		Brodie Thiesfield (SimpleIni lib)\n		Ross Bencina/Phil Burk/Roger B. Dannenberg (PortMidi/Audio lib)\n		Yann Collet (LZ4)\n		The LAME MP3 encoder team\n		The Google team (Material Icons)\n		Josh Coalson & Xiph.org foundation (FLAC encoder)", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(286, 20);

			texts.push_back(Text(string("Version ") + VERSION + " (" + VERSION_DATE + ")", font, charSize));
			texts[1].setColor(colors[BLOCKTEXT]);
			texts[1].setPosition(100, 276);

		}
			break;

		case POPUP_REPLACE_INSTRUMENT:
			setSize(300, 400);
			title.setString("Change instrument");
			lists.push_back(List(10, 10, 17, 230));

			lists[0].text = instrList->text;
			lists[0].updateSize();

			buttons.push_back(Button(w - 90, h - 50, "OK", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));
			break;
		case POPUP_TRANSPOSE:
			setSize(500, 250);
			title.setString("Change note");

			checkboxes.push_back(Checkbox(20, 20, "Set note"));
			checkboxes.push_back(Checkbox(20, 80, "Transpose"));

			checkboxes[0].checked = 1;

			sliders.push_back(DataSlider(160, 20, 128, 0, "", 0, 200, 2));
			sliders.push_back(DataSlider(160, 80, 24, -24, "Semitones", 0));


			texts.push_back(Text("", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(160, 110);

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));

			updateIntervalDescription();

			break;
		case POPUP_SETNOTE:
			setSize(300, 150);
			title.setString("Set note");
			sliders.push_back(DataSlider(20, 20, 128, 0, "", 0, 200, 2));
			sliders[0].setValue(60);

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));

			break;
		case POPUP_FADE:
			setSize(500, 350);
			title.setString("Volume");

			checkboxes.push_back(Checkbox(20, 80, "Scale volume"));



			checkboxes.push_back(Checkbox(20, 140, "Fade in/out"));

			texts.push_back(Text("From\n\nTo", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(160, 140);

			sliders.push_back(DataSlider(210, 140, 99, 0, "", 99));
			sliders.push_back(DataSlider(210, 180, 99, 0, "", 99));

			sliders.push_back(DataSlider(160, 80, 200, 0, "%", 100));

			checkboxes.push_back(Checkbox(20, 20, "Set volume"));

			sliders.push_back(DataSlider(160, 20, 99, 0, "", 99));

			checkboxes[2].checked = 1;

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));
			break;
		case POPUP_EFFECTS:{

							   Sprite s;
							   s.setTexture(*tileset);
							   s.setTextureRect({ 270, 257, 257, 18 });
							   sprites.push_back(s);
							   sprites[0].setPosition(249, 150);

							   setSize(600, 450);
							   title.setString("Add effect");

							   texts.push_back(Text("Effect type", font, charSize));
							   texts[0].setColor(colors[BLOCKTEXT]);
							   texts[0].setPosition(10, 10);

							   lists.push_back(List(20, 20, 18, 200));
							   setEffectList(&lists[0]);

							   buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
							   buttons.push_back(Button(20, h - 50, "Cancel", -1, 8));

							   sliders.push_back(DataSlider(250, 168, 255, 0, "Value", 0, 255));
							   sliders.push_back(DataSlider(250, 188, 255, 0, "", 0, 255));
							   sliders[1].setVisible(false);

							   texts.push_back(Text("", font, charSize));
							   texts[0].setColor(colors[BLOCKTEXT]);
							   texts[0].setPosition(250, 20);

							   texts.push_back(Text("", font, charSize));
							   texts[1].setColor(colors[BLOCKTEXT]);
							   texts[1].setPosition(250, 128);
		}break;

		case POPUP_SEARCH:
			setSize(800, 530);
			title.setString("Search/Replace");

			texts.push_back(Text("Search for :", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(20, 20);

			checkboxes.push_back(Checkbox(20, 100, "Note"));
			sliders.push_back(DataSlider(150, 100, 128, -1, "", 64, 200, 2));

			checkboxes.push_back(Checkbox(20, 140, "Volume"));
			sliders.push_back(DataSlider(150, 140, 99, -1, "", 0, 200));

			checkboxes.push_back(Checkbox(20, 180, "Instrument"));
			lists.push_back(List(150, 180, 6, 200));
			lists[0].add("Any");
			for (int i = 0; i < instrList->text.size(); i++)
			{
				lists[0].add(instrList->text[i].getString().toAnsiString());
			}

			lists[0].updateSize();

			checkboxes.push_back(Checkbox(20, 300, "Effect"));
			lists.push_back(List(150, 300, 5, 200));
			lists[1].add("Any");
			setEffectList(&lists[1]);

			checkboxes.push_back(Checkbox(20, 410, "Effect value"));

			checkboxes.push_back(Checkbox(220, h - 50, "The whole song"));
			checkboxes[5].checked = 1;
			checkboxes.push_back(Checkbox(400, h - 50, "This pattern"));
			checkboxes.push_back(Checkbox(550, h - 50, "Selection"));


			sliders.push_back(DataSlider(150, 410, 255, -1, "", 0, 200));

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));

			shapes.push_back(RectangleShape(Vector2f(1, 410)));
			shapes[0].setPosition(394, 56);
			shapes[0].setFillColor(colors[BLOCKBG]);

			// replace
			checkboxes.push_back(Checkbox(382, 20, "Replace with"));

			checkboxes.push_back(Checkbox(420, 100, "Note"));
			sliders.push_back(DataSlider(550, 100, 128, -1, "", 64, 200, 2));

			checkboxes.push_back(Checkbox(420, 140, "Volume"));
			sliders.push_back(DataSlider(550, 140, 99, -1, "", 0, 200));

			checkboxes.push_back(Checkbox(420, 180, "Instrument"));
			lists.push_back(List(550, 180, 6, 200));
			lists[2].add("None");
			for (int i = 0; i < instrList->text.size(); i++)
			{
				lists[2].add(instrList->text[i].getString().toAnsiString());
			}
			lists[2].updateSize();

			checkboxes.push_back(Checkbox(420, 300, "Effect"));
			lists.push_back(List(550, 300, 5, 200));
			lists[3].add("None");
			setEffectList(&lists[3]);


			checkboxes.push_back(Checkbox(420, 410, "Effect value"));
			sliders.push_back(DataSlider(550, 410, 255, -1, "", 0, 200));

			texts.push_back(Text("Search in :", font, charSize));
			texts[1].setColor(colors[BLOCKTEXT]);
			texts[1].setPosition(120, h - 50);

			for (int i = 0; i < sliders.size(); i++)
			{
				if (i < 3 && sliders[i].value == -1)
				{
					sliders[i].setDisplayedValueOnly("Any");
				}
				else if (i >= 3 && sliders[i].value == -1)
				{
					sliders[i].setDisplayedValueOnly("None");
				}
			}

			checkboxes.push_back(Checkbox(420, 60, "Transposed note"));
			sliders.push_back(DataSlider(600, 60, 24, -24, "Semitones", 64, 150));

			break;

		case POPUP_REPLACERESULT:
			setSize(300, 130);
			title.setString("Search/Replace");
			texts.push_back(Text(std::to_string(param) + " occurence(s) replaced", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(20, 20);
			delay = 60;
			break;

		case POPUP_SAVEFAILED:
			setSize(460, 150);
			title.setString("Save action failed");
			texts.push_back(Text("Can't write data. Check if this file is writable.", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			break;
		case POPUP_OPENFAILED:
			setSize(460, 150);
			title.setString("Open action failed");
			texts.push_back(Text("Can't open file. Check if the file exists.", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			break;

		case POPUP_QUITCONFIRM:
			setSize(450, 150);
			title.setString("Confirmation");
			texts.push_back(Text("You have unsaved changes", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 160, h - 50, "Quit without saving", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));
			buttons.push_back(Button(130, h - 50, "Save and quit", -1, 8));

			break;

		case POPUP_OPENCONFIRM:
			setSize(450, 150);
			title.setString("Confirmation");
			texts.push_back(Text("You have unsaved changes", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 160, h - 50, "Discard changes", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));
			buttons.push_back(Button(130, h - 50, "Save", -1, 8));
			break;

		case POPUP_DELETEINSTRUMENT:
			setSize(500, 150);
			title.setString("Confirmation");
			texts.push_back(Text("Notes using this instrument will be removed from the song. Proceed ?", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 80, h - 50, "Yes", -1, 8));
			buttons.push_back(Button(30, h - 50, "No", -1, 8));
			break;

		case POPUP_NEWFILE:
			setSize(450, 150);
			title.setString("Confirmation");
			texts.push_back(Text("You have unsaved changes", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 160, h - 50, "Discard changes", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));
			buttons.push_back(Button(130, h - 50, "Save", -1, 8));
			break;

		case POPUP_MIDIEXPORT:{
			int oldSize = midiExportAssoc.size();
			midiExportAssoc.resize(fm->instrumentCount, 0);

			midiExportAssocChannels.resize(fm->instrumentCount, 0);
			for (int i = oldSize; i < midiExportAssocChannels.size(); i++)
			{
				midiExportAssocChannels[i] = i % 16;
				if (i == 9)
					midiExportAssocChannels[i]++;
			}
			setSize(850, 510);
			title.setString("MIDI export");

			texts.push_back(Text("Associate the song's instruments to MIDI channels and MIDI instruments.", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);

			texts.push_back(Text("Song instrument", font, charSize));
			texts[1].setColor(colors[TITLE]);
			texts[1].setPosition(10, 45);

			texts.push_back(Text("MIDI channel", font, charSize));
			texts[2].setColor(colors[TITLE]);
			texts[2].setPosition(300, 45);

			texts.push_back(Text("MIDI instrument", font, charSize));
			texts[3].setColor(colors[TITLE]);
			texts[3].setPosition(600, 45);



			lists.push_back(List(10, 70, 20, 200));
			lists[0].text = instrList->text;
			lists[0].updateSize();

			lists.push_back(List(600, 70, 20, 200));
			setMelodicList();

			lists.push_back(List(300, 70, 20, 200));
			lists[2].add("Ch1 Melodic");
			lists[2].add("Ch2 Melodic");
			lists[2].add("Ch3 Melodic");
			lists[2].add("Ch4 Melodic");
			lists[2].add("Ch5 Melodic");
			lists[2].add("Ch6 Melodic");
			lists[2].add("Ch7 Melodic");
			lists[2].add("Ch8 Melodic");
			lists[2].add("Ch9 Melodic");
			lists[2].add("Ch10 Percussion");
			lists[2].add("Ch11 Melodic");
			lists[2].add("Ch12 Melodic");
			lists[2].add("Ch13 Melodic");
			lists[2].add("Ch14 Melodic");
			lists[2].add("Ch15 Melodic");
			lists[2].add("Ch16 Melodic");


			buttons.push_back(Button(w - 90, h - 50, "Export", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));

			break;
		}
		case POPUP_STREAMEDEXPORT:
			setSize(600, 380);
			title.setString("Streamed audio export");

			texts.push_back(Text("From", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(20, 20);

			texts.push_back(Text("To", font, charSize));
			texts[1].setColor(colors[BLOCKTEXT]);
			texts[1].setPosition(240, 20);

			checkboxes.push_back(Checkbox(20, 100, "Export as Wave"));
			checkboxes.push_back(Checkbox(20, 150, "Export as MP3"));

			checkboxes.push_back(Checkbox(170, 150, "VBR"));
			checkboxes.push_back(Checkbox(170, 190, "CBR"));

			checkboxes[0].checked = 1;

			sliders.push_back(DataSlider(240, 170, 10, 0, "Quality (0=best, 9=worst)", 0, 200));


			sliders.push_back(DataSlider(60, 20, fm->patternCount, 0, "Pattern", 0, 150));
			sliders.push_back(DataSlider(270, 20, fm->patternCount, 0, "Pattern", fm->patternCount, 150));


			sliders.push_back(DataSlider(60, 50, 10, 0, "Loop", 0, 80));
			texts.push_back(Text("Times", font, charSize));
			texts[2].setColor(colors[BLOCKTEXT]);
			texts[2].setPosition(150, 50);

			checkboxes.push_back(Checkbox(20, 240, "Export as FLAC"));
			sliders.push_back(DataSlider(170, 242, 8, 0, "Compression level", 0, 200));

			buttons.push_back(Button(w - 90, h - 50, "Export", -1, 8));
			buttons.push_back(Button(50, h - 50, "Cancel", -1, 8));
			break;
		case POPUP_TEMPERAMENT:
			setSize(320, 400);
			title.setString("Edit temperament");
			texts.push_back(Text("Tuning in cents for each note", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(20, 40);
			sliders.push_back(DataSlider(20, 80, 100, -100, "C", 0));
			sliders.push_back(DataSlider(20, 100, 100, -100, "C#", 0));
			sliders.push_back(DataSlider(20, 120, 100, -100, "D", 0));
			sliders.push_back(DataSlider(20, 140, 100, -100, "D# / Eb", 0));
			sliders.push_back(DataSlider(20, 160, 100, -100, "E", 0));
			sliders.push_back(DataSlider(20, 180, 100, -100, "F", 0));
			sliders.push_back(DataSlider(20, 200, 100, -100, "F#", 0));
			sliders.push_back(DataSlider(20, 220, 100, -100, "G", 0));
			sliders.push_back(DataSlider(20, 240, 100, -100, "G# / Ab", 0));
			sliders.push_back(DataSlider(20, 260, 100, -100, "A", 0));
			sliders.push_back(DataSlider(20, 280, 100, -100, "A# / Bb", 0));
			sliders.push_back(DataSlider(20, 300, 100, -100, "B", 0));

			

			buttons.push_back(Button(w - 65, h - 50, "Close", -1, 8));

			buttons.push_back(Button(250, 88, "Reset", -1, 8));

			break;
		case POPUP_INSERTROWS:
			setSize(240, 300);
			title.setString("Insert rows");

			sliders.push_back(DataSlider(20, 40, 128, 1, "Number of rows", config->rowHighlight.value));

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));
			break;

		case POPUP_REMOVEROWS:
			setSize(240, 300);
			title.setString("Remove rows");

			sliders.push_back(DataSlider(20, 40, 128, 1, "Number of rows", config->rowHighlight.value));

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(30, h - 50, "Cancel", -1, 8));
			break;

		case POPUP_FILECORRUPTED:
			setSize(460, 150);
			title.setString(":(");
			texts.push_back(Text("The file you opened seems corrupted.\n\nBe careful when editing, it may crash the program.\n\nUse a clean backup if you have one.", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(10, 10);
			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			break;
		case POPUP_FIRSTSTART:
		{
			setSize(800, 300);
			title.setString("First start");

			Sprite s;
			s.setTexture(*tileset);
			s.setTextureRect({ 260, 0, 256, 256 });
			sprites.push_back(s);
			sprites[0].setPosition(10, 10);

			buttons.push_back(Button(w - 80, h - 50, "Close", -1, 8));
			buttons.push_back(Button(490, 101, "Online tutorial", -1, 8));
			texts.push_back(Text("Welcome to FM Composer !\n\nIt seems to be the first time you launch this program.\n\n\nClick here to learn the basics :\n\n\nA demo song was just loaded so you can see how a song is made and how\nsome FM Composer features are used.\nThere are other demos in the song folder.\n\n\nHave fun !", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(276, 20);


		}
			break;
		case POPUP_WRONGVERSION:
			setSize(600, 200);
			title.setString("Newer file version");

			buttons.push_back(Button(w - 50, h - 50, "OK", -1, 8));
			buttons.push_back(Button(28, 150, "Go to the download page", -1, 8));
			texts.push_back(Text("This file has been created with a newer version of FM Composer.\n\nPlease download the latest version and try again.", font, charSize));
			texts[0].setColor(colors[BLOCKTEXT]);
			texts[0].setPosition(20, 20);
			break;

	}
	view.reset(FloatRect(0.f, 0.f, windowWidth, windowHeight));
	view.setViewport(FloatRect(0, 0, 1, 1));
	view.setCenter((float)windowWidth / 2 - (int)windowWidth / 2 + (float)w / 2, (float)windowHeight / 2 - (int)windowHeight / 2 + (float)h / 2);
	shadow.setSize(Vector2f(w, h + 32));

	// restore widget states if this popup has been opened before
	if (type<savedState.size() && savedState[type].size()>0)
	{
		int item = 0;
		for (int i = 0; i < lists.size(); i++)
		{
			lists[i].select(savedState[type][item]);
			item++;
		}
		for (int i = 0; i < sliders.size(); i++)
		{
			sliders[i].setValue(savedState[type][item]);
			item++;
			if (type == POPUP_SEARCH)
			{
				if (i < 3 && sliders[i].value == -1)
				{
					sliders[i].setDisplayedValueOnly("Any");
				}
				else if (i >= 3 && sliders[i].value == -1)
				{
					sliders[i].setDisplayedValueOnly("None");
				}
			}
		}
		for (int i = 0; i < checkboxes.size(); i++)
		{
			checkboxes[i].checked = savedState[type][item];
			item++;
		}
	}

	switch (type)
	{
		case POPUP_EFFECTS:
			updateEffectDescription();
			break;
		case POPUP_TRANSPOSE:
			updateIntervalDescription();
			break;
		case POPUP_STREAMEDEXPORT:
			updateExportSliders();
			break;
	}


}

void Popup::updateExportSliders()
{
	if (checkboxes[3].checked)
	{
		sliders[0].name.setString("Bitrate (Kbps)");
		sliders[0].setMinMax(0, 15);
		sliders[0].setDisplayedValueOnly(std::to_string(mp3_bitrates[sliders[0].value]));
	}
	else
	{
		sliders[0].name.setString("Quality (0=best, 9=worst)");
		sliders[0].setMinMax(0, 9);
	}
}

void Popup::updateWindow()
{
	view.reset(FloatRect(0.f, 0.f, windowWidth, windowHeight));
	view.setViewport(FloatRect(0, 0, 1, 1));
	view.setCenter((float)windowWidth / 2 - (int)windowWidth / 2 + (float)w / 2, (float)windowHeight / 2 - (int)windowHeight / 2 + (float)h / 2);
}