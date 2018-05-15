#include "configEditor.hpp"
#include "../pattern/songFileActions.hpp"
#include "../../gui/popup/popup.hpp"

extern ListMenu *recentSongs;

void ConfigEditor::loadRecentSongs()
{
	for (int i = 0; i < maxRecentSongCount; i++)
	{
		string rfname = ini_config.GetValue("recentSongs", string(int2str[i]).c_str(), "");
		if (rfname.size())
		{
			recentSongs->add(rfname);
		}
	}
}

void ConfigEditor::saveRecentSongs()
{
	for (int i = 0; i < maxRecentSongCount; i++)
	{
		if (i < recentSongs->text.size())
			ini_config.SetValue("recentSongs", string(int2str[i]).c_str(), recentSongs->text[i].getString().toAnsiString().c_str());
		else
			ini_config.SetValue("recentSongs", string(int2str[i]).c_str(), "");
	}
}


void ConfigEditor::loadLastSong()
{
	if (equalsIgnoreCase(ini_config.GetValue("config", "firstStart", "1"), "1"))
	{

		song_load(string(appdir + "songs" + pathSeparator + "AnotherThing.fmcs").c_str(), true);
		popup->show(POPUP_FIRSTSTART);
	}

	if (equalsIgnoreCase(ini_config.GetValue("config", "openLastFileAtStart", "1"), "1") && recentSongs->text.size() > 0)
	{
		song_load(recentSongs->text[0].getString().toAnsiString().c_str(), true);
	}
}

void ConfigEditor::lastSongRemove(string filename)
{
	for (int i = 0; i < recentSongs->text.size(); i++)
	{
		if (recentSongs->text[i].getString().toAnsiString() == filename)
		{
			recentSongs->remove(i);
			break;
		}
	}
}

void ConfigEditor::lastSongRotate()
{

	for (int i = 0; i < recentSongs->text.size(); i++)
	{
		if (recentSongs->text[i].getString().toAnsiString() == lastSongOpened)
		{
			recentSongs->text.erase(recentSongs->text.begin() + i);
			recentSongs->insert(lastSongOpened);
			return;
		}
	}

	if (recentSongs->text.size()<maxRecentSongCount)
	{
		recentSongs->insert(lastSongOpened);
		return;
	}


	for (int i = recentSongs->text.size() - 1; i >0; i--)
	{
		recentSongs->setElement(i, recentSongs->text[i - 1].getString());
	}
	recentSongs->setElement(0, lastSongOpened);
}
