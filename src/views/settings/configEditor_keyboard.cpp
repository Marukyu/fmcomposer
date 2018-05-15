#include "configEditor.hpp"

void SFKeyToString(unsigned int keycode, char *keyStr)
{
	switch (keycode)
	{
		case sf::Keyboard::Escape: sprintf(keyStr, "Escape"); break;
		case sf::Keyboard::LControl: sprintf(keyStr, "LControl"); break;
		case sf::Keyboard::LShift: sprintf(keyStr, "LShift"); break;
		case sf::Keyboard::LAlt: sprintf(keyStr, "LAlt"); break;
		case sf::Keyboard::LSystem: sprintf(keyStr, "LSystem"); break;
		case sf::Keyboard::RControl: sprintf(keyStr, "RControl"); break;
		case sf::Keyboard::RShift: sprintf(keyStr, "RShift"); break;
		case sf::Keyboard::RAlt: sprintf(keyStr, "RAlt"); break;
		case sf::Keyboard::RSystem: sprintf(keyStr, "RSystem"); break;
		case sf::Keyboard::Menu: sprintf(keyStr, "Menu"); break;
		case sf::Keyboard::LBracket: sprintf(keyStr, "LBracket"); break;
		case sf::Keyboard::RBracket: sprintf(keyStr, "RBracket"); break;
		case sf::Keyboard::SemiColon: sprintf(keyStr, ";"); break;
		case sf::Keyboard::Comma: sprintf(keyStr, ","); break;
		case sf::Keyboard::Period: sprintf(keyStr, "."); break;
		case sf::Keyboard::Quote: sprintf(keyStr, "\'"); break;
		case sf::Keyboard::Slash: sprintf(keyStr, "/"); break;
		case sf::Keyboard::BackSlash: sprintf(keyStr, "\\"); break;
		case sf::Keyboard::Tilde: sprintf(keyStr, "~"); break;
		case sf::Keyboard::Equal: sprintf(keyStr, "="); break;
		case sf::Keyboard::Dash: sprintf(keyStr, "-"); break;
		case sf::Keyboard::Space: sprintf(keyStr, "Space"); break;
		case sf::Keyboard::Return: sprintf(keyStr, "Return"); break;
		case sf::Keyboard::BackSpace: sprintf(keyStr, "Back"); break;
		case sf::Keyboard::Tab: sprintf(keyStr, "Tab"); break;
		case sf::Keyboard::PageUp: sprintf(keyStr, "Page Up"); break;
		case sf::Keyboard::PageDown: sprintf(keyStr, "Page Down"); break;
		case sf::Keyboard::End: sprintf(keyStr, "End"); break;
		case sf::Keyboard::Home: sprintf(keyStr, "Home"); break;
		case sf::Keyboard::Insert: sprintf(keyStr, "Insert"); break;
		case sf::Keyboard::Delete: sprintf(keyStr, "Delete"); break;
		case sf::Keyboard::Add: sprintf(keyStr, "+"); break;
		case sf::Keyboard::Subtract: sprintf(keyStr, "-"); break;
		case sf::Keyboard::Multiply: sprintf(keyStr, "*"); break;
		case sf::Keyboard::Divide: sprintf(keyStr, "/"); break;
		case sf::Keyboard::Left: sprintf(keyStr, "Left"); break;
		case sf::Keyboard::Right: sprintf(keyStr, "Right"); break;
		case sf::Keyboard::Up: sprintf(keyStr, "UP"); break;
		case sf::Keyboard::Down: sprintf(keyStr, "Down"); break;
		case sf::Keyboard::Numpad0: sprintf(keyStr, "NP 0"); break;
		case sf::Keyboard::Numpad1: sprintf(keyStr, "NP 1"); break;
		case sf::Keyboard::Numpad2: sprintf(keyStr, "NP 2"); break;
		case sf::Keyboard::Numpad3: sprintf(keyStr, "NP 3"); break;
		case sf::Keyboard::Numpad4: sprintf(keyStr, "NP 4"); break;
		case sf::Keyboard::Numpad5: sprintf(keyStr, "NP 5"); break;
		case sf::Keyboard::Numpad6: sprintf(keyStr, "NP 6"); break;
		case sf::Keyboard::Numpad7: sprintf(keyStr, "NP 7"); break;
		case sf::Keyboard::Numpad8: sprintf(keyStr, "NP 8"); break;
		case sf::Keyboard::Numpad9: sprintf(keyStr, "NP 9"); break;
		case sf::Keyboard::F1: sprintf(keyStr, "F1"); break;
		case sf::Keyboard::F2: sprintf(keyStr, "F2"); break;
		case sf::Keyboard::F3: sprintf(keyStr, "F3"); break;
		case sf::Keyboard::F4: sprintf(keyStr, "F4"); break;
		case sf::Keyboard::F5: sprintf(keyStr, "F5"); break;
		case sf::Keyboard::F6: sprintf(keyStr, "F6"); break;
		case sf::Keyboard::F7: sprintf(keyStr, "F7"); break;
		case sf::Keyboard::F8: sprintf(keyStr, "F8"); break;
		case sf::Keyboard::F9: sprintf(keyStr, "F9"); break;
		case sf::Keyboard::F10: sprintf(keyStr, "F10"); break;
		case sf::Keyboard::F11: sprintf(keyStr, "F11"); break;
		case sf::Keyboard::F12: sprintf(keyStr, "F12"); break;
		case sf::Keyboard::F13: sprintf(keyStr, "F13"); break;
		case sf::Keyboard::F14: sprintf(keyStr, "F14"); break;
		case sf::Keyboard::F15: sprintf(keyStr, "F15"); break;
		case sf::Keyboard::Pause: sprintf(keyStr, "Paues"); break;
		case sf::Keyboard::A: sprintf(keyStr, "A"); break;

		case sf::Keyboard::B: sprintf(keyStr, "B"); break;
		case sf::Keyboard::C: sprintf(keyStr, "C"); break;
		case sf::Keyboard::D: sprintf(keyStr, "D"); break;
		case sf::Keyboard::E: sprintf(keyStr, "E"); break;
		case sf::Keyboard::F: sprintf(keyStr, "F"); break;
		case sf::Keyboard::G: sprintf(keyStr, "G"); break;
		case sf::Keyboard::H: sprintf(keyStr, "H"); break;
		case sf::Keyboard::I: sprintf(keyStr, "I"); break;
		case sf::Keyboard::J: sprintf(keyStr, "J"); break;
		case sf::Keyboard::K: sprintf(keyStr, "K"); break;
		case sf::Keyboard::L: sprintf(keyStr, "L"); break;
		case sf::Keyboard::M: sprintf(keyStr, "M"); break;
		case sf::Keyboard::N: sprintf(keyStr, "N"); break;
		case sf::Keyboard::O: sprintf(keyStr, "O"); break;
		case sf::Keyboard::P: sprintf(keyStr, "P"); break;
		case sf::Keyboard::Q: sprintf(keyStr, "Q"); break;
		case sf::Keyboard::R: sprintf(keyStr, "R"); break;
		case sf::Keyboard::S: sprintf(keyStr, "S"); break;
		case sf::Keyboard::T: sprintf(keyStr, "T"); break;
		case sf::Keyboard::U: sprintf(keyStr, "U"); break;
		case sf::Keyboard::V: sprintf(keyStr, "V"); break;
		case sf::Keyboard::W: sprintf(keyStr, "W"); break;
		case sf::Keyboard::X: sprintf(keyStr, "X"); break;
		case sf::Keyboard::Y: sprintf(keyStr, "Y"); break;
		case sf::Keyboard::Z: sprintf(keyStr, "Z"); break;
		//case sf::Keyboard::Exclamation: sprintf(keyStr, "!"); break;
		default:
			sprintf(keyStr, "%c", keycode);
	}
}


void ConfigEditor::updateKeyboardMappingDisplay()
{
	char keyName[16];
	for (int i = 0; i < 36; i++)
	{
		keyList.text[i].setString("");
	}
	for (int i = 0; i < 110; i++)
	{
		int note = noteMappings[i];

		if (note>-1 && note < 36)
		{
			SFKeyToString(i - 1, keyName);
			keyList.text[note].setString(string(keyName));
		}
	}
}


void ConfigEditor::loadKeyboardMappings(string keyboard)
{

	string mappings = ini_keyboards.GetValue("defaultKeyboardMappings", keyboard.c_str(), "");

	for (int i = 0; i < 110; i++)
	{
		noteMappings[i] = -1;
	}
	char* copy = strdup(mappings.c_str());
	char * pch;
	pch = strtok(copy, ",");

	noteMappings[atoi(pch) + 1] = 0;
	int count = 1;

	while (pch = strtok(NULL, " ,."))
	{
		noteMappings[atoi(pch) + 1] = count;
		count++;
	}

	free(copy);

	updateKeyboardMappingDisplay();
}

void ConfigEditor::handleKeyNoteMapping()
{


	if (noteList.clicked())
	{
		keyList.select(noteList.value);
		if (mouse.clickd)
		{
			keyMappingReset.show();
		}
	}

	if (keyList.clicked())
	{
		noteList.select(keyList.value);
		if (mouse.clickd)
		{
			keyMappingReset.show();
		}
	}

	if (keyList.scroll != noteList.scroll)
	{
		// follow scrollbar changes
		if (keyList.scrollbar.upd_cursor)
		{
			noteList.setScroll(keyList.scroll);
		}
		else if (noteList.scrollbar.upd_cursor)
		{
			keyList.setScroll(noteList.scroll);
		}
		// follow scroll change with mouse wheel
		else if (keyList.hovered)
		{
			noteList.setScroll(keyList.scroll);
		}
		else if (noteList.hovered)
		{
			keyList.setScroll(noteList.scroll);
		}
	}




	if (defaultAzerty.clicked())
	{
		loadKeyboardMappings("AZERTY");
	}
	if (defaultQwerty.clicked())
	{
		loadKeyboardMappings("QWERTY");
	}
	if (defaultQwertz.clicked())
	{
		loadKeyboardMappings("QWERTZ");
	}
}
