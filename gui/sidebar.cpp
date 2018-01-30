#include "sidebar.hpp"
#include "../views/instrument/instrEditor.hpp"
#include "../views/pattern/songEditor.hpp"
#include "mainmenu.hpp"


Sidebar *sidebar;

Sidebar::Sidebar() :
borderRight(Vector2f(300, 1000)),
octave(1070, 240, 7, 0, "Octave", 4),
currentInstr("Instrument list", font, charSize),
timer("", font, charSize),
notePreview("", font, charSize),
defNoteVol(1070, 220, 99, 0, "Volume", 70)
{
	vuMeter = new StereoVuMeter(1100, 50);

	currentInstr.setColor(colors[TITLE]);
	currentInstr.setPosition(1070, 355);

	timer.setColor(colors[BLOCKTEXT]);
	timer.setPosition(1200, 50);


	notePreview.setColor(colors[BACKGROUND]);
	notePreview.setPosition(1242, 6);

	borderRight.setPosition(1065, 32);
	borderRight.setFillColor(colors[SIDEBAR]);

	instrList = new List(1070, 375, 18, 200, 16);
}

void Sidebar::draw()
{

	/* Update timer in the menu bar */

	float time = fm_getTime(fm);
	float length = fm_getSongLength(fm);


	string currentTime = "";
	/* Do we REALLY need hours ? */
	if (time / 60 >= 60)
	{
		currentTime = std::to_string((int)time / 3600) + ":";
	}
	currentTime += std::to_string(((int)time / 60) % 60) + ":" + std::to_string((int)time % 60) + "'" + std::to_string((int)(time * 100) % 100);

	string totalTime = "";
	/* Do we REALLY need hours ? */
	if (length / 60 >= 60)
	{
		totalTime = std::to_string((int)length / 3600) + ":";
	}
	totalTime += std::to_string(((int)length / 60) % 60) + ":" + std::to_string((int)length % 60) + "'" + std::to_string((int)(length * 100) % 100);


	timer.setString(currentTime + "\n" + totalTime + "\n\n" + to_string(fm->tempo) + " BPM");

	sf::Time elapsed = mclock.restart();
	frameTime60 = elapsed.asMilliseconds() /15.0;




	window->setView(globalView);
	menu->draw();
	window->setView(borderView);

	window->draw(borderRight);
	window->draw(currentInstr);
	if (state == songEditor)
	{
		songEditor->patSlider.draw();
	}
	octave.draw();
	defNoteVol.draw();
	instrList->draw();
	if (state == songEditor)
	{
		songEditor->patSize.draw();
		songEditor->resize.draw();
		songEditor->expand.draw();
		songEditor->shrink.draw();
	}
	if (state == instrEditor)
	{
		instrEditor->add.draw();
		instrEditor->instrCleanup.draw();
	}
	vuMeter->draw();

	window->draw(timer);
	window->draw(notePreview);
}

void Sidebar::update()
{
	octave.update();
	defNoteVol.update();
}