#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "fmengine/fmlib.h"
#include "globalFunctions.hpp"
#include "views/pianoroll/pianoRoll.hpp"
#include "views/instrument/instrEditor.hpp"
#include "views/pattern/songEditor.hpp"
#include "views/general/generalEditor.hpp"
#include "views/settings/configEditor.hpp"
#include "midi/midi.h"
#include "portaudio.h"
#include <fstream>

#include "libs/simpleini/SimpleIni.h"
#include <fenv.h>
#include "libs/tinyfiledialogs/tinyfiledialogs.h"
#include "input/input.hpp"
#include "state.hpp"
#include "gui/mainmenu.hpp"
#include "views/pattern/songFileActions.hpp"
#include "gui/sidebar.hpp"


Uint32 textEntered[32];

//char appDir[256];
Texture *tileset;

View patternView, patternTopView, borderView, globalView, patNumView, instrView;
void *focusedElement;

State *state;
int windowFocus = 1;


int main(int argc, char *argv[])
{

	appdir = dirnameOf(string(argv[0]));


	global_initialize();

	gui_initialize();

	sidebar = new Sidebar();
	menu = new Menu(14);


	/* Create pages */
	config = new ConfigEditor();
	songEditor = new SongEditor();
	generalEditor = new GeneralEditor();
	instrEditor = new InstrEditor(46);
	popup = new Popup();
	pianoRoll = new Pianoroll(0);

	updateViews(WindowWidth, WindowHeight);

	/* Start the app on the song editor */
	menu->goToPage(PAGE_SONG);

	/* Configure the sound output */
	config->selectBestSoundDevice();

	

	// reads all GM-mapped instruments .. to do things on them

	/*extern CSimpleIniA ini_gmlist;
	int wstats[7]={0,0,0,0,0,0,0};
	int icount=0;
	for (int i = 0; i < 143; i++){
	if (fm_loadInstrument(fm, string(string("instruments/") + ini_gmlist.GetValue("melodic", int2str[i].c_str(), "0") + string(".fmci")).c_str(), 0)<0){
	printf(string(string(string("instruments/") + ini_gmlist.GetValue("melodic", int2str[i].c_str(), "0") + string(".fmci\n")).c_str()).c_str());
	continue;
	}
	fm_saveInstrument(fm, string(string("instruments/")+ini_gmlist.GetValue("melodic", int2str[i].c_str(),"0")+string(".fmci")).c_str(),0);
	for (int j = 0; j < 6; j++){
	wstats[fm->instrument[0].waveform[j]]++;
	if (fm->instrument[0].waveform[j]==4)
	printf("%s\n",fm->instrument[0].name);
	}
	}

	for (int i = 23; i < 88; i++){
	if (fm_loadInstrument(fm, string(string("instruments/") + ini_gmlist.GetValue("percussion", int2str[i].c_str(), "0") + string(".fmci")).c_str(), 0)<0){
	printf(string(string(string("instruments/") + ini_gmlist.GetValue("percussion", int2str[i].c_str(), "0") + string(".fmci\n")).c_str()).c_str());
	continue;
	}
	fm_saveInstrument(fm, string(string("instruments/")+ini_gmlist.GetValue("percussion", int2str[i].c_str(),"0")+string(".fmci")).c_str(),0);
	for (int j = 0; j < 6; j++){
	wstats[fm->instrument[0].waveform[j]]++;
	if (fm->instrument[0].waveform[j]==4)
	printf("%s\n",fm->instrument[0].name);
	}
	}
	return 0;*/
	//printf("stats: %d %d %d %d %d %d %d\n",wstats[0],wstats[1],wstats[2],wstats[3],wstats[4],wstats[5],wstats[6]);

	config->loadRecentSongs();

	if (argc > 1)
	{
		song_load(argv[1], false);
	}
	else
	{
		config->loadLastSong();
	}

	while (window->isOpen())
	{

		mouse.cursor = CURSOR_NORMAL;
		midi_getEvents();

		input_update();





		if (!windowFocus || mouse.clickLock2 > 0)
		{

			if (mouse.clickLock2 > 0)
				mouse.clickLock2--;
			goto drawing;
		}


		if (contextMenu)
			contextMenu->update();

		if (contextMenu == recentSongs)
		{
			int value = contextMenu->clicked();
			if (value >= 0)
			{
				lastSongOpened = recentSongs->text[value].getString().toAnsiString();
				song_load(recentSongs->text[value].getString().toAnsiString().c_str());
			}
		}
		if (contextMenu == copyPasta)
		{
			
			int value = contextMenu->clicked();
			if (value >= 0)
			{
				
			}
			switch(value)
			{
				case 0:
					copiedSliderValueOk = copiedSliderValue;
					break;
				case 1:
					copiedSlider->setValue(copiedSliderValueOk);
					break;
			}
		}
		
		if (!popup->visible)
		{



			if (!mouse.clickLock2)
			{
				static float dropdowntimer=0, dropdownclosetimer=0;
				if (menu->hovered() == 1)
				{
					dropdowntimer+=frameTime60;
					if (contextMenu!=recentSongs  && !mouse.clickg && dropdowntimer>20)
						recentSongs->show(menu->items[1*4].position.x - 4, 28);
				}
				else if (menu->hovered() == 17)
				{
					if (state == songEditor)
						songEditor->editTools.show(menu->items[17*4].position.x - 4, 28);
					else if (state == instrEditor)
						instrEditor->editTools.show(menu->items[17*4].position.x - 4, 28);
				}
				else if ( (contextMenu == recentSongs && !recentSongs->hover()
					|| contextMenu == &songEditor->editTools && !songEditor->editTools.hover()
					|| contextMenu == &instrEditor->editTools && !instrEditor->editTools.hover()
					))
				{
					dropdownclosetimer+=frameTime60;
					if (dropdownclosetimer > 15) {
						contextMenu = NULL;
						dropdownclosetimer=0;
					}
				}
				else
				{
					dropdowntimer=0;
					dropdownclosetimer=0;
				}
				int menuItem = menu->clicked();
				if (menuItem > 0)
				{
					contextMenu = NULL;
				}
				switch (menuItem)
				{
					case 0: // new
						isSongModified ? popup->show(POPUP_NEWFILE) : song_clear();
						break;
					case 1: // open
						song_open();
						break;
					case 2: // save
						song_save();
						break;
					case 3: // back
						fm_setPosition(fm, 0, 0, 2);
						songEditor->moveY(0);
						break;
					case 4: // top pattern
						songEditor->moveY(0);
						break;
					case 5: // play
						song_playPause();
						break;
					case 6: // stop
						menu->button[5].setTextureRect(IntRect(100, 32, 32, 32));
						fm_stop(fm, 1);
						break;
					case 10: // wave/mp3 export
						popup->show(POPUP_STREAMEDEXPORT);
						break;
					case 9: // midi export
						popup->show(POPUP_MIDIEXPORT);
						break;
					case 11: // save as
						song_saveas();
						break;
					case 12: // general
						menu->goToPage(PAGE_GENERAL);
						break;
					case 13: // song
						menu->goToPage(PAGE_SONG);
						break;
					case 14: // instrument
						menu->goToPage(PAGE_INSTRUMENT);
						break;
					case 7: // config
						menu->goToPage(PAGE_CONFIG);
						break;
					case 15: // piano roll
						menu->goToPage(PAGE_PIANOROLL);
						break;
					case 16: // piano roll
						popup->show(POPUP_ABOUT);
						break;

				}
				// re check clicklock in case it was set by a menu action
				if (!mouse.clickLock2)
				{

					state->update();
					instrEditor->handleGlobalEvents();
					mouse.pos = input_getmouse(borderView);
					sidebar->update();
				}


			}

		}

		popup->handleEvents();



		

	drawing:
		window->clear(colors[BACKGROUND]);

		state->draw();

		sidebar->draw();
		window->setView(globalView);

		showInstrumentLights();

		
		popup->draw();

		if (contextMenu) { contextMenu->draw(); }


		window->display();

		updateMouseCursor();

	}

	global_exit();



	return(0);
}