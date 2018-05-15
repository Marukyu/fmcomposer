#include "input.hpp"
#include "../globalFunctions.hpp"
#include "noteInput.hpp"
#include "../views/instrument/instrEditor.hpp"
#include "../views/pattern/songEditor.hpp"
#include "../gui/mainmenu.hpp"
#include "../views/pattern/songFileActions.hpp"
#include "../gui/sidebar.hpp"

keyboard_ keyboard;
mouse_ mouse;

Vector2i mouseSidebar, mouseGlobal;

extern View globalView, borderView;
extern Text notePreview;

int textEnteredCount = 0;

void input_update()
{


	memset(&keyboard, 0, sizeof(keyboard));
	mouse.scroll = mouse.hscroll = mouse.clickg = mouse.clickd = mouse.clickgReleased = mouse.clickdReleased = 0;
	mouse.cursor = CURSOR_NORMAL;


	mouse.pos = input_getmouse(globalView);
	mouseGlobal = mouse.pos;
	mouseSidebar = input_getmouse(borderView);
	keyboard.shift = Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift);
	keyboard.ctrl = Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl);

	if (mouse.dblClick > 0)
		mouse.dblClick -= frameTime60;

	while (window->pollEvent(evt))
	{
		handleUnconditionalEvents();
		if (!popup->visible)
		{
			state->handleEvents();
		}
		else
		{
			/* Popups usually never allows other events when they are opened, but for temperament popup it's useful to be able to play notes */
			if (state == instrEditor && popup->type==POPUP_TEMPERAMENT)
			{
				handleNotePreview(1);
			}
		}
	}
}

Vector2i input_getmouse(View &view)
{
	Vector2f m = window->mapPixelToCoords(Mouse::getPosition(*window), view);
	return Vector2i((int)round(m.x), (int)round(m.y));
}

void handleUnconditionalEvents()
{
	switch (evt.type)
	{
		case Event::LostFocus:
			windowFocus = 0;
			break;
		case Event::GainedFocus:
			windowFocus = 1;
			break;
		case Event::Closed:
			isSongModified ? popup->show(POPUP_QUITCONFIRM) : window->close();
			break;
		case Event::Resized:
			updateViews(evt.size.width, evt.size.height);

			break;
			#ifdef FMC_MODIFIED_SFML
		case Event::FileDropped:
			if (checkExtension(evt.file.path, "fmcs") || checkExtension(evt.file.path, "mid") || checkExtension(evt.file.path, "rmi") || checkExtension(evt.file.path, "smf"))
			{
				song_load(evt.file.path);
			}
			else if (checkExtension(evt.file.path, "fmci") && state == instrEditor)
			{
				instrEditor->instrument_load(evt.file.path);
			}

			free(evt.file.path);
			break;
			#endif
	}

	if (windowFocus)
	{
		switch (evt.type)
		{
			case Event::KeyPressed:

				switch (evt.key.code)
				{


					case Keyboard::Delete:
						keyboard.del = 1;

						break;
					case Keyboard::Add:
						keyboard.plus = 1;

						break;
					case Keyboard::Subtract:
						keyboard.minus = 1;
						break;
					case Keyboard::Up:
						keyboard.up = 1;

						break;
					case Keyboard::Down:
						keyboard.down = 1;
						break;
					case Keyboard::Left:
						keyboard.left = 1;

						break;
					case Keyboard::Right:
						keyboard.right = 1;
						break;

					case Keyboard::C:
						keyboard.c = 1;
						break;
					case Keyboard::A:
						keyboard.a = 1;
						break;
					case Keyboard::V:
						keyboard.v = 1;
						break;
					case Keyboard::X:
						keyboard.x = 1;
						break;
					case Keyboard::PageUp:
						keyboard.pageUp = 1;
						break;
					case Keyboard::PageDown:
						keyboard.pageDown = 1;
						break;
					case Keyboard::Equal:
						keyboard.equal = 1;
						break;
					case Keyboard::Multiply:
						keyboard.multiply = 1;
						break;
					case Keyboard::Divide:
						keyboard.divide = 1;
						break;
					case Keyboard::Home:
						fm_setPosition(fm, 0, 0, 2);
						songEditor->moveY(0);
						break;
					case Keyboard::Return: // play / stop
					case Keyboard::Space:
						if (focusedElement == NULL || focusedElement == &patternView)
						{
							song_playPause();
						}
						break;
					case Keyboard::S:
						if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))
							song_save();

						break;
					case Keyboard::O:
						if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))
							song_open();

						break;

					case Keyboard::Escape:
						popup->close();
						contextMenu=0;
						break;
				}

				break;

			case Event::MouseButtonPressed:
				if (evt.mouseButton.button == Mouse::Left)
				{
					mouse.clickg = 1;
				}
				if (evt.mouseButton.button == Mouse::Right)
				{
					mouse.clickd = 1;
				}
				break;
			case Event::MouseButtonReleased:
				if (evt.mouseButton.button == Mouse::Left)
				{
					mouse.clickgReleased = 1;
				}
				if (evt.mouseButton.button == Mouse::Right)
				{
					mouse.clickdReleased = 1;
				}
				break;
			case Event::KeyReleased:
				{
					int note = keyboard2note(evt.key.code);
					if (note > -1)
					{
						previewNoteStop(sidebar->octave.value * 12 + keyboard2note(evt.key.code), 0);
						sidebar->notePreview.setString("");
					}
					break;
				}


			case Event::MouseLeft:
				if (mouseGlobal.x < 0)
					mouse.leftLeft = 1;
				else if (mouseGlobal.x >= windowWidth)
					mouse.leftRight = 1;
				if (mouseGlobal.y < 0)
					mouse.leftTop = 1;
				else if (mouseGlobal.y >= windowHeight)
					mouse.leftDown = 1;

				break;
			case Event::MouseWheelScrolled:
				if (evt.mouseWheelScroll.wheel == Mouse::VerticalWheel)
					mouse.scroll = evt.mouseWheelScroll.delta;
				else if (evt.mouseWheelScroll.wheel == Mouse::HorizontalWheel)
					mouse.hscroll = evt.mouseWheelScroll.delta;
				break;
			case Event::TextEntered:

				if (textEnteredCount < 32)
				{
					textEntered[textEnteredCount] = evt.text.unicode;
					textEnteredCount++;
				}
				break;
		}

	}


}

void handleNotePreview(int canPreview)
{
	static int oldOctave = -1;
	if (oldOctave != sidebar->octave.value)
	{
		previewNoteStopAll();
	}


	oldOctave = sidebar->octave.value;
	mouse.leftLeft = mouse.leftRight = mouse.leftTop = mouse.leftDown = 0;
	switch (evt.type)
	{

		case Event::KeyPressed:

			if (!Keyboard::isKeyPressed(Keyboard::LControl) && !Keyboard::isKeyPressed(Keyboard::RControl) && canPreview)
			{

				int note = keyboard2note(evt.key.code);

				if (evt.key.code >= 0 && note >= 0)
				{
					previewNote(instrList->value, sidebar->octave.value * 12 + note, sidebar->defNoteVol.value, 0);
					sidebar->notePreview.setString(noteName(sidebar->octave.value * 12 + note));
				}
			}
			break;

	}

}

