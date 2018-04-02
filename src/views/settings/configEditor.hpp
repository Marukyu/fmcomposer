#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include "../../gui/button/button.hpp"
#include "../../gui/list/list.hpp"

#include "../../gui/slider/dataslider.hpp"
#include "../../gui/textfield/textfield.hpp"
#include "../../gui/checkbox/checkbox.hpp"

#include "../../fmengine/fmlib.h"
#include "../../state.hpp"
#include "../../gui/contextmenu/contextmenu.hpp"

const int sampleRates[6] = { 24000, 32000, 44100, 48000, 88200, 96000 };
extern int noteMappings[110];
extern CSimpleIniA ini_config, ini_theme, ini_gmlist, ini_keyboards;

class ConfigEditor : public State{
	DataSlider buffer;
	Checkbox openLastSong;

	string lastRunVersion;
	


	Button refreshMidiDevicesButton;
	Button defaultAzerty, defaultQwerty, defaultQwertz;
	Text midiInText, midiQuantizeText, soundDeviceText;
	vector<int> soundDeviceIds;
	TextInput themeFile;
	Text diviseurText, rowHighlightText, sampleRateError, themeText, midiImport, keyAssoc, keyAssocHelp, startup, keyPreset, display, rowHighlightText2;
	ListMenu keyMappingReset;
	string hostnames[15];
	public:
	int directXdevicesCount;
	int approvedSampleRate, approvedDeviceId, currentLatency;
	string approvedSoundDeviceName;
	Checkbox subquantize;
	List midiDevicesList, soundDevicesList;
	List noteList, keyList;
	int currentSoundDeviceId;
	int maxRecentSongCount;
	DataSlider diviseur, rowHighlight, patternSize, samplerate, latency, defaultVolume;
	DataSlider previewReverb;
	string defaultPreloadedSound;
	ConfigEditor();
	void draw();
	void update();
	void refresh();
	void refreshMidiDevices();
	void midiEvents();
	void midiSelectDevice(int index);
	int selectSoundDevice(int index, int samplerate = 48000, int latency = 0, bool force = false);
	void selectBestSoundDevice();
	void loadRecentSongs();
	void saveRecentSongs();
	void updateKeyboardMappingDisplay();

	void save();
	void lastSongRotate();
	void lastSongRemove(string filename);
	void loadLastSong();
	void loadKeyboardMappings(string keyboard);
	void handleKeyNoteMapping();
	void handleEvents();
	void updateRowHighlightText();
};

extern ConfigEditor *config;

void iniparams_load();

#endif