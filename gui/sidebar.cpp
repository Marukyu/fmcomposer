#include "sidebar.hpp"
#include "../views/instrument/instrEditor.hpp"
#include "../views/pattern/songEditor.hpp"
#include "mainmenu.hpp"
#include "drawBatcher.hpp"

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

	instrList = new List(1070, 375, 18, 209, 16);
}

void Sidebar::draw()
{

	/* Update timer in the menu bar */

	static float prevTime=-1;
	static float prevlength=-1;

	float time = fm_getTime(fm);
	float length = fm_getSongLength(fm);

	if (time != prevTime || length != prevlength)
	{
		prevTime = time;
		prevlength = length;

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

	}

	

	sf::Time elapsed = mclock.restart();
	frameTime60 = elapsed.asMilliseconds() /15.0;




	window->setView(globalView);
	menu->draw();
	window->setView(borderView);


	drawBatcher.initialize();
	drawBatcher.addItem(&borderRight);
	drawBatcher.addItem(&currentInstr);
	drawBatcher.addItem(&octave);

	drawBatcher.addItem(&defNoteVol);

	if (state == songEditor)
	{
		drawBatcher.addItem(&songEditor->patSize);
		drawBatcher.addItem(&songEditor->resize);
		drawBatcher.addItem(&songEditor->expand);
		drawBatcher.addItem(&songEditor->shrink);
		drawBatcher.addItem(&songEditor->patSlider);
	}
	else if (state == instrEditor)
	{
		drawBatcher.addItem(&instrEditor->add);
		drawBatcher.addItem(&instrEditor->instrCleanup);
	}

	drawBatcher.addItem(&timer);
	drawBatcher.addItem(&notePreview);

	drawBatcher.addItem(vuMeter);

	drawBatcher.draw();


	

	instrList->draw();

}

void Sidebar::update()
{
	octave.update();
	defNoteVol.update();
	vuMeter->update();
}