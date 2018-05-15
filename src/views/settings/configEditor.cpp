#include "configEditor.hpp"
#include "../../input/noteInput.hpp"
#include "../pattern/songEditor.hpp"
#include "../../libs/portmidi/portmidi.h"
#include "../../midi/midi.h"
#include "../pattern/songFileActions.hpp"
#include "../../gui/sidebar.hpp"
#include "../../gui/drawBatcher.hpp"

extern List *instrList;

int noteMappings[110];

ConfigEditor *config;

CSimpleIniA ini_config, ini_theme, ini_gmlist, ini_keyboards;




ConfigEditor::ConfigEditor() :
refreshMidiDevicesButton(295, 80, ICON_MD_REFRESH, 15,0,16),
midiDevicesList(14, 100, 10, 300),
midiInText("MIDI in device", font, charSize),
midiQuantizeText("MIDI import quantization", font, charSize),
diviseur(14, 350, 32, 1, "1 quarter note =", 8, 150),
diviseurText("rows", font, charSize),
rowHighlightText("Highlight pattern rows every", font, charSize),
rowHighlightText2("", font, charSize),
rowHighlight(210, 590, 8, 1, "", 4, 40),
soundDevicesList(420, 100, 10, 360),
soundDeviceText("Sound device", font, charSize),
patternSize(14, 380, 256, 1, "Pattern size", 8, 200),
samplerate(420, 280, 5, 0, "Sample Rate (hz)", 3, 170),
sampleRateError("", font, charSize),
openLastSong(14, 510, "Reopen last song")
, themeFile(420, 400, 20, "Theme file : themes/"),
themeText(" (restart FM Composer to apply changes)", font, charSize),
midiImport("MIDI import", font, charSize),
subquantize(14, 310, "Preserve unquantized notes"),
approvedSampleRate(-1), approvedDeviceId(-1),
noteList(800, 120, 16, 90), keyList(894, 120, 16, 90),
keyAssoc("Keyboard-Note Mappings", font, charSize),
keyAssocHelp("Select note then press a key", font, charSize),
defaultQwerty(884, 438, "QWERTY", 62, 4),
defaultAzerty(884, 410, "AZERTY", 62, 4),
defaultQwertz(884, 466, "QWERTZ", 62, 4),
latency(420, 300, 256, 0, "Desired latency (ms)", 0, 170),
previewReverb(420, 480, 99, 0, "Note preview reverb", 10),
startup("At startup", font, charSize),
keyPreset("Presets :", font, charSize),
defaultVolume(14, 410, 99, 0, "Song volume", 10),
display("Display", font, charSize),
directXdevicesCount(0),
wasapiExclusive(420, 325, "Exclusive mode")
{
	startup.setPosition(14, 480);
	display.setPosition(14, 560);
	hostnames[0] = "";
	hostnames[1] = "DirectSound";
	hostnames[2] = "MME";
	hostnames[3] = "ASIO";
	hostnames[4] = "SoundManager";
	hostnames[5] = "CoreAudio";
	hostnames[6] = "";
	hostnames[7] = "OSS";
	hostnames[8] = "ALSA";
	hostnames[9] = "AL";
	hostnames[10] = "BeOS";
	hostnames[11] = "WDMKS";
	hostnames[12] = "JACK";
	hostnames[13] = "WASAPI";
	hostnames[14] = "AudioScienceHPI";

	wasapiExclusive.visible=false;


	midiInText.setPosition(Vector2f(14, 80));
	midiQuantizeText.setPosition(Vector2f(14, 280));
	rowHighlightText.setPosition(Vector2f(14, 590));
	rowHighlightText2.setPosition(Vector2f(14+250, 590));
	keyMappingReset.add("Reset this key");

	patternSize.setValue(atoi(ini_config.GetValue("config", "defaultPatternSize", "128")));
	defaultVolume.setValue(atoi(ini_config.GetValue("config", "defaultSongVolume", "60")));
	maxRecentSongCount = atoi(ini_config.GetValue("recentSongs", "max", "10"));

	lastRunVersion = ini_config.GetValue("config", "lastRunVersion", "1.4");

	openLastSong.checked = atoi(ini_config.GetValue("config", "openLastFileAtStart", "1"));
	themeFile.setText(ini_config.GetValue("config", "theme", "dark.ini"));
	themeText.setPosition(420, 425);

	defaultPreloadedSound = ini_config.GetValue("config", "defaultPreloadedSound", "keyboards/piano");

	sidebar->defNoteVol.setValue(atoi(ini_config.GetValue("config", "defaultNoteVolume", "70")));
	sidebar->editingStep.setValue(atoi(ini_config.GetValue("config", "editingStep", "0")));

	wasapiExclusive.checked = atoi(ini_config.GetValue("config", "wasapiExclusive", "0"));

	diviseur.setValue(atoi(ini_config.GetValue("config", "rowsPerQuarterNote", "8")));
	previewReverb.setValue(atoi(ini_config.GetValue("config", "notePreviewReverb", "14")));

	rowHighlight.setValue(atoi(ini_config.GetValue("config", "patternRowHighlight", "4")));
	subquantize.checked = atoi(ini_config.GetValue("config", "preserveUnquantizedNotes", "1"));

	if (atoi(ini_config.GetValue("window", "maximized", "1")))
	{
		/* Show window maximized at start */
#ifdef _WIN32
		ShowWindow(GetActiveWindow(), SW_MAXIMIZE);
#endif
	}
	else
	{
		unsigned ww = atoi(ini_config.GetValue("window", "width", "0"));
		unsigned wh = atoi(ini_config.GetValue("window", "height", "0"));
		if (ww && wh)
		{
			window->setSize(Vector2u(ww, wh));
		}
		sf::VideoMode vm = VideoMode::getDesktopMode();
		window->setPosition(Vector2i((vm.width - ww) / 2, (vm.height - wh) / 2));
	}

	updateRowHighlightText();

	songDir = ini_config.GetValue("config", "lastSongFolder", "");
	instrDir = ini_config.GetValue("config", "lastInstrumentFolder", "");


	int sampleRate = atoi(ini_config.GetValue("config", "sampleRate", "48000"));

	latency.setValue(atoi(ini_config.GetValue("config", "desiredLatency", "70")));

	string notes[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	string octaves[3] = { "", " +1 oct", "+2 oct" };
	for (int i = 0; i < 36; i++)
	{
		noteList.add(notes[i % 12] + octaves[i / 12]);
		keyList.add("");
	}



	if (equalsIgnoreCase(ini_config.GetValue("config", "firstStart", "1"), "1"))
	{
#ifdef _WIN32
		HKL lang = GetKeyboardLayout(0);
		switch (HIWORD(lang) & 0xff)
		{
			case 0x7:
				loadKeyboardMappings("QWERTZ");
				break;
			case 0xC:
				loadKeyboardMappings("AZERTY");
				break;
			case 0xA:
				loadKeyboardMappings("QWERTY-Spain");
				break;
			case 0x9:
				if (HIWORD(lang) >> 8 == 4)
					loadKeyboardMappings("QWERTY-Canada");
				else
					loadKeyboardMappings("QWERTY");
				break;
			default:
				loadKeyboardMappings("QWERTY");
				break;
		}
#else 

#endif
	}
	else
	{
		for (int i = 0; i < 110; i++)
		{
			noteMappings[i] = atoi(ini_config.GetValue("keyboardMappings", std::to_string(i).c_str(), "-1"));
		}
		updateKeyboardMappingDisplay();
	}





	for (int i = 0; i < 6; i++)
	{
		if (sampleRates[i] == sampleRate)
		{
			samplerate.setValue(i);
		}
	}


	

	diviseurText.setPosition(200, 350);
	soundDeviceText.setPosition(420, 80);
	sampleRateError.setPosition(600, 280);

	midiImport.setPosition(14, 280);

	keyAssoc.setPosition(800, 80);
	keyAssocHelp.setPosition(800, 100);
	keyPreset.setPosition(800, 410);
	/* Set text colors */

	keyAssocHelp.setColor(colors[BLOCKTEXT]);
	keyPreset.setColor(colors[BLOCKTEXT]);
	midiImport.setColor(colors[TITLE]);
	soundDeviceText.setColor(colors[TITLE]);
	midiInText.setColor(colors[TITLE]);
	startup.setColor(colors[TITLE]);
	display.setColor(colors[TITLE]);

	themeText.setColor(colors[BLOCKTEXT]);
	keyAssoc.setColor(colors[TITLE]);
	midiQuantizeText.setColor(colors[BLOCKTEXT]);
	rowHighlightText.setColor(colors[BLOCKTEXT]);
	diviseurText.setColor(colors[BLOCKTEXT]);

	refresh();




}

void ConfigEditor::updateRowHighlightText()
{
	if (rowHighlight.value == 1)
	{
		rowHighlightText2.setString("rows (disabled)");
	}
	else
	{
		rowHighlightText2.setString("rows");
	}
}



void ConfigEditor::draw()
{
	window->setView(globalView);
	samplerate.setDisplayedValueOnly(to_string(sampleRates[samplerate.value]));

	drawBatcher.initialize();


	drawBatcher.addItem(&midiDevicesList);
	drawBatcher.addItem(&soundDevicesList);
	drawBatcher.addItem(&refreshMidiDevicesButton);
	drawBatcher.addItem(&midiInText);

	drawBatcher.addItem(&soundDeviceText);

	drawBatcher.addItem(&diviseur);
	drawBatcher.addItem(&patternSize);
	drawBatcher.addItem(&samplerate);
	drawBatcher.addItem(&latency);

	drawBatcher.addItem(&sampleRateError);
	drawBatcher.addItem(&themeText);
	drawBatcher.addItem(&midiImport);
	drawBatcher.addItem(&themeFile);
	drawBatcher.addItem(&defaultVolume);
	drawBatcher.addItem(&diviseurText);
	drawBatcher.addItem(&rowHighlightText);
	drawBatcher.addItem(&rowHighlightText2);
	drawBatcher.addItem(&startup);
	drawBatcher.addItem(&keyPreset);
	drawBatcher.addItem(&display);
	drawBatcher.addItem(&rowHighlight);
	drawBatcher.addItem(&noteList);
	drawBatcher.addItem(&keyList);
	drawBatcher.addItem(&previewReverb);
	drawBatcher.addItem(&keyAssoc);
	drawBatcher.addItem(&keyAssocHelp);
	drawBatcher.addItem(&defaultAzerty);
	drawBatcher.addItem(&defaultQwerty);
	drawBatcher.addItem(&defaultQwertz);
	drawBatcher.draw();

	subquantize.draw();
	openLastSong.draw();
	wasapiExclusive.draw();
}

#ifdef _WIN32
#include "pa_win_wasapi.h"
#endif

static const string hostnames[15];


void ConfigEditor::refresh()
{

	vector<string> *deviceList = midi_refreshDevices();

	midiDevicesList.text.clear();
	midiDevicesList.add("No device");

	for (int i = 0; i < deviceList->size(); i++)
	{
		midiDevicesList.add((*deviceList)[i]);
	}
	if (deviceList->size() == 1)
		midiDevicesList.select(1);
	else
		midiDevicesList.select(0);

	soundDevicesList.text.clear();
	int nbdevices = Pa_GetDeviceCount();
	soundDeviceIds.clear();

	bool skipDirectXdevices=false;
	if (equalsIgnoreCase(lastRunVersion.c_str(), "1.4"))
	{
		skipDirectXdevices=true;
	}

	int soundDeviceId = atoi(ini_config.GetValue("config", "soundDeviceId", "0"));

	for (int i = 0; i < nbdevices; i++)
	{

		const PaDeviceInfo* p = Pa_GetDeviceInfo(i);
		const PaHostApiInfo *phost = Pa_GetHostApiInfo(p->hostApi);

		/* DirectX device */
		if (skipDirectXdevices && phost->type==1 && i <= soundDeviceId+directXdevicesCount)
			directXdevicesCount++;

		if (p->maxOutputChannels>0)
		{
			soundDevicesList.add(string(hostnames[phost->type]) + " " + utf8_to_string(p->name, locale("")));

			

			soundDeviceIds.push_back(i);
		}
	}
}

void ConfigEditor::refreshMidiDevices()
{

	vector<string> *deviceList = midi_refreshDevices();

	midiDevicesList.text.clear();
	midiDevicesList.add("No device");

	for (int i = 0; i < deviceList->size(); i++)
	{
		midiDevicesList.add((*deviceList)[i]);
	}
	if (deviceList->size() == 1)
		midiDevicesList.select(1);
	else
		midiDevicesList.select(0);

}


void ConfigEditor::update()
{


	themeFile.modified();
	if (contextMenu == &keyMappingReset)
	{
		switch (contextMenu->clicked())
		{
			case 0:
				for (int i = 0; i < 110; i++)
				{
					if (noteMappings[i] == noteList.value)
					{
						noteMappings[i] = -1;
					}
				}
				updateKeyboardMappingDisplay();
				break;
		}
	}
	if (mouse.pos.x > (int)windowWidth - 215)
		return;


	if (refreshMidiDevicesButton.clicked())
	{
		refreshMidiDevices();
	}

	if (midiDevicesList.clicked())
	{
		midi_selectDevice(midiDevicesList.value);
	}

	handleKeyNoteMapping();


	defaultVolume.update();
	previewReverb.update();

	diviseur.update();
	if (rowHighlight.update())
	{
		songEditor->updatePatternLines();
		updateRowHighlightText();
	}

	static int paramChanged = 0;

	if (soundDevicesList.clicked() || samplerate.update() || latency.update())
	{
		paramChanged = 1;
	}

	if (paramChanged && !Mouse::isButtonPressed(Mouse::Left))
	{
		paramChanged = 0;
		selectSoundDevice(soundDeviceIds[soundDevicesList.value], sampleRates[samplerate.value], latency.value);
	}



	patternSize.update();
	openLastSong.clicked();
	subquantize.clicked();

	if (wasapiExclusive.clicked())
	{
		selectSoundDevice(soundDeviceIds[soundDevicesList.value], sampleRates[samplerate.value], latency.value, true);
	}
}


void ConfigEditor::save()
{
	ini_config.SetValue("window", "width", std::to_string(window->getSize().x).c_str());
	ini_config.SetValue("window", "height", std::to_string(window->getSize().y).c_str());
	ini_config.SetValue("window", "maximized", std::to_string(isWindowMaximized()).c_str());

	ini_config.SetValue("config", "defaultPatternSize", std::to_string(patternSize.value).c_str());
	ini_config.SetValue("config", "rowsPerQuarterNote", std::to_string(diviseur.value).c_str());
	ini_config.SetValue("config", "patternRowHighlight", std::to_string(rowHighlight.value).c_str());
	ini_config.SetValue("config", "openLastFileAtStart", std::to_string(openLastSong.checked).c_str());
	ini_config.SetValue("config", "preserveUnquantizedNotes", std::to_string(subquantize.checked).c_str());
	ini_config.SetValue("config", "defaultNoteVolume", std::to_string(sidebar->defNoteVol.value).c_str());
	ini_config.SetValue("config", "notePreviewReverb", std::to_string(previewReverb.value).c_str());
	ini_config.SetValue("config", "defaultPreloadedSound", defaultPreloadedSound.c_str());
	ini_config.SetValue("config", "defaultVolume", std::to_string(defaultVolume.value).c_str());
	ini_config.SetValue("config", "editingStep", std::to_string(sidebar->editingStep.value).c_str());
	ini_config.SetValue("config", "lastRunVersion", VERSION);

	ini_config.SetValue("config", "patternZoomLevel", std::to_string((int)round(songEditor->zoom * 10)).c_str());
	ini_config.SetValue("config", "theme", themeFile.text.getString().toAnsiString().c_str());
	ini_config.SetValue("config", "lastSongFolder", songDir.c_str());
	ini_config.SetValue("config", "lastInstrumentFolder", instrDir.c_str());
	ini_config.SetValue("config", "wasapiExclusive",std::to_string( (int)wasapiExclusive.checked).c_str());
	ini_config.SetValue("config", "firstStart", "0");
	ini_config.SetValue("recentSongs", "max", std::to_string(maxRecentSongCount).c_str());

	for (int i = 0; i < 110; i++)
	{
		ini_config.SetValue("keyboardMappings", std::to_string(i).c_str(), std::to_string(noteMappings[i]).c_str());
	}


	int sd = 0;
	for (int i = 0; i < soundDevicesList.text.size(); i++)
	{
		if (soundDeviceIds[i] == approvedDeviceId)
		{
			sd = i;
			break;
		}
	}

	ini_config.SetValue("config", "soundDeviceId", std::to_string(approvedDeviceId).c_str());
	ini_config.SetValue("config", "sampleRate", std::to_string(approvedSampleRate).c_str());
	ini_config.SetValue("config", "desiredLatency", std::to_string(latency.value).c_str());

	saveRecentSongs();

	ini_config.SaveFile((appconfigdir + pathSeparator + "config.ini").c_str());
}



void iniparams_load()
{



	// load config from application directory if exists
	if (ini_config.LoadFile(string(appconfigdir + "config.ini").c_str()) < 0)
	{
		error("Can't load " + appconfigdir + "config.ini");
	}

	if (ini_theme.LoadFile(string(appdir + "themes" + pathSeparator + ini_config.GetValue("config", "theme", "dark.ini")).c_str()) < 0)
	{
		error("Can't load theme file (" + appdir + "themes" + pathSeparator + string(ini_config.GetValue("config", "theme", "dark.ini")) + ")");
	}
	if (ini_gmlist.LoadFile(string(appdir + "gmlist.ini").c_str()) < 0)
	{
		error("Can't load gmlist.ini");
	}
	if (ini_keyboards.LoadFile(string(appdir + "defaultkeyboards.ini").c_str()) < 0)
	{
		error("Can't load defaultkeyboards.ini");
	}


	loadColors(&ini_theme);
}

void ConfigEditor::handleEvents()
{
	handleNotePreview(!themeFile.editing && !noteList.selected && !keyList.selected);
	switch (evt.type)
	{

		case Event::KeyPressed:
			if (noteList.selected || keyList.selected)
			{
				// don't allow up/down arrow as note shortcut since it conflicts with list up/down shortcuts arrow
				if (evt.key.code == 74 || evt.key.code == 73)
					break;

				for (int i = 0; i < 110; i++)
				{
					if (noteMappings[i] == noteList.value)
					{
						noteMappings[i] = noteMappings[evt.key.code + 1];
					}
				}

				noteMappings[evt.key.code + 1] = noteList.value;
				updateKeyboardMappingDisplay();
			}
			break;
	}
}
