#include "../../globalFunctions.hpp"
#include "../../input/input.hpp"
#include "../../gui/popup/popup.hpp"
#include "../../libs/tinyfiledialogs/tinyfiledialogs.h"
#include "../instrument/instrEditor.hpp"
#include "../pattern/songEditor.hpp"
#include "../pianoroll/pianoRoll.hpp"
#include "../general/generalEditor.hpp"
#include "../../gui/mainmenu.hpp"
#include "../settings/configEditor.hpp"

extern CSimpleIniA ini_gmlist;
string saveAs;
string songLoadedRequest;


int song_saveas()
{
	mouse.clickLock2 = 1;
	mouse.clickg = 0;
	static const char * filters[1] = { "*.fmcs" };
	const char *fileName = tinyfd_saveFileDialog("Save song", NULL, 1, filters, "FMComposer song");
	if (fileName)
	{
		mclock.restart();
		songDir = dirnameOf(fileName);
		string fileNameOk = forceExtension(fileName, "fmcs");
		if (!fm_saveSong(fm, fileNameOk.c_str()))
		{
			popup->show(POPUP_SAVEFAILED);
			return 0;
		}
		else
		{
			//popup->show(POPUP_SAVED);
			setWindowTitle(fileNameOk.c_str());
			saveAs = lastSongOpened = fileNameOk.c_str();
			config->lastSongRotate();
			songModified(0);
		}

		return 1;
	}
	return 0;
}

int song_save()
{
	if (saveAs == "")
	{
		return song_saveas();
	}
	else
	{
		if (!fm_saveSong(fm, saveAs.c_str()))
		{
			popup->show(POPUP_SAVEFAILED);
			return 0;
		}
		else
		{
			//popup->show(POPUP_SAVED);
			songModified(0);
		}
		return 1;
	}
}



void song_load(const char* filename, bool fromAutoReload)
{
	if (isSongModified && !fromAutoReload)
	{
		songLoadedRequest = string(filename);
		popup->show(POPUP_OPENCONFIRM);
		return;
	}

	int playing = fm->playing;
	int opened = 0;

	song_stop();
	if (checkExtension(filename, "fmcs"))
	{
		if ((opened = fm_loadSong(fm, filename)) == 0)
			saveAs = filename;
	}
	else if (checkExtension(filename, "mid") || checkExtension(filename, "smf") || checkExtension(filename, "rmi"))
	{
		if ((opened = midiImport(filename)) == 0)
			saveAs = "";
	}
	if (opened == FM_ERR_FILEIO && !fromAutoReload)
	{
		popup->show(POPUP_OPENFAILED);
		config->lastSongRemove(string(filename));
	}
	else if (opened == FM_ERR_FILEVERSION)
	{
		popup->show(POPUP_WRONGVERSION);
		config->lastSongRemove(string(filename));
	}
	else if (opened == 0 || opened == FM_ERR_FILECORRUPTED)
	{
		if (opened == FM_ERR_FILECORRUPTED)
		{
			popup->show(POPUP_FILECORRUPTED);
		}
		if (!fromAutoReload)
		{
			lastSongOpened = string(filename);
			config->lastSongRotate();
		}

		instrEditor->reset();
		pianoRoll->updateFromFM();

		songEditor->reset();
		generalEditor->updateFromFM();

		fm_setPosition(fm, 0, 0, 2);
		if (playing)
			song_play();

		setWindowTitle(filename);

		songModified(0);

		if (popup->savedState.size() >= POPUP_STREAMEDEXPORT && popup->savedState[POPUP_STREAMEDEXPORT].size()>=2){
			popup->savedState[POPUP_STREAMEDEXPORT][2] = fm->patternCount;
		}
	}
	else
	{
		config->lastSongRemove(string(filename));
	}
}


void song_open()
{


	mouse.clickLock2 = 1;
	contextMenu = NULL;
	static const char * filters[4] = { "*.fmcs", "*.mid", "*.rmi", "*.smf" };
	const char *fileName = tinyfd_openFileDialog("Open a file", songDir.c_str(), 4, filters, "FMComposer or MIDI files", false);
	if (fileName)
	{
		songDir = dirnameOf(fileName);

		song_load(fileName, 0);
	}
}


void song_clear()
{
	mouse.clickLock2 = 1;
	song_stop();
	fm_clearSong(fm);
	fm_setVolume(fm, config->defaultVolume.value);
	fm_insertPattern(fm, config->patternSize.value, 0);
	fm_setPosition(fm, 0, 0, 2);
	fm_resizeInstrumentList(fm, 1);
	if (fm_loadInstrument(fm, string("instruments/" + config->defaultPreloadedSound + ".fmci").c_str(), 0) < 0)
	{
		fm_loadInstrument(fm, string(string("instruments/") + ini_gmlist.GetValue("melodic", "default", "0") + string(".fmci")).c_str(), 0);
	}
	

	instrEditor->reset();
	generalEditor->updateFromFM();

	pianoRoll->updateFromFM();
	lastSongOpened = "";
	songEditor->reset();
	setWindowTitle("New song");
	saveAs = "";
	menu->button[5].setTextureRect(IntRect(100, 32, 32, 32));
	songModified(0);
}