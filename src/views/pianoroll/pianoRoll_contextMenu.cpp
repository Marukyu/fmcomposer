#include "pianoRoll.hpp"


void Pianoroll::handleContextMenu()
{
	switch (contextMenu->clicked())
	{

		case 0:
			focusInstrument(noteInstr);
			break;


	}
}