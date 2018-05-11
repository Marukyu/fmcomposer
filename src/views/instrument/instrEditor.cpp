#include "instrEditor.hpp"
#include "../../input/noteInput.hpp"
#include "algoPresets.hpp"
#include "../../libs/simpleini/SimpleIni.h"
#include "../pattern/songEditor.hpp"
#include "../settings/configEditor.hpp"
#include "../../gui/sidebar.hpp"
#include "../../gui/drawBatcher.hpp"

extern CSimpleIniA ini_gmlist;
extern void *focusedElement;
InstrEditor* instrEditor;


InstrEditor::InstrEditor(int y)
: algo(806, y + 380, 35, 0, "Algorithm", 35), feedback(321, y - 21, 99, 0, "Feedback", 0, 85), feedbackSource(406, y - 21, 6, 1, "src", 1, 36)
, save(814, y + 15, "Save", -1, 6), load(894, y + 15, "Load", -1, 6), instrName(806, y + 50, 23, ""), lfoDelay(811, y + 450, 99, 0, "Delay", 0, 150)
, lfoSpeed(811, y + 430, 99, 0, "Speed", 0, 150), lfoA(811, y + 470, 99, 0, "Attack", 25, 150), add(1077, 320, "Add", -1, 6), adsr(Vector2f(200, 100)), lfoOffsetBar(Vector2f(1, 26))
, lfoWaveform(811, y + 490, 19, 0, "Waveform", 0, 150), lfoOffset(811, y + 510, 31, 0, "Offset", 0, 150), waveform(*tileset), connector(*tileset), lfo("LFO", font, charSize),
volume(806, y + 80, 99, 0, "Volume", 80), tuning(806, y + 120, 100, -100, "Tuning", 0,100), lfoBG(Vector2f(198, 130)), envReset(806, y + 567, "Reset env."), phaseReset(806, y + 587, "Reset phase"),
transpose(806, y + 100, 12, -12, "Base note", 0,100), newNote("On new note :", font, charSize), lfoReset(893, y + 587, "Reset LFO"), instrCleanup(1070, 730, "Remove unused", -1, 6),
temperament(806 + 6, y + 657 + 6, "Temperament", -1, 6), k_fx1(806, y + 627, 7, 0, "", 0, 69, 0), k_fx2(806 + 70, y + 627, 16, 0, "", 0, 130, 0), kfx_text("K pattern effect control :", font, charSize)
, zoom(1), smoothTransition(879, y + 567, "Smooth transition"), copied(false), valueChanged(0), opCopied(-1), opSelected(0), transposable(910 + 6, y + 100+8,"Trans-\nposable")
{


	newNote.setColor(colors[BLOCKTEXT]);
	newNote.setPosition(806, y + 380 + 166);

	kfx_text.setColor(colors[BLOCKTEXT]);
	kfx_text.setPosition(806, y + 607);

	lfo.setColor(colors[BLOCKTEXT]);
	lfo.setPosition(806, y + 410);
	lfoBG.setPosition(806, y + 410);
	lfoBG.setFillColor(colors[BLOCKBG]);
	lfoOffsetBar.setFillColor(colors[WAVEFORMOFFSETBAR]);

	/* Try to load default piano sound*/
	if (fm_loadInstrument(fm, string(appdir + "/instruments/" + config->defaultPreloadedSound + ".fmci").c_str(), 0) < 0)
	{
		/* Fallback to the default melodic*/
		if (fm_loadInstrument(fm, string(string(appdir + "/instruments/") + ini_gmlist.GetValue("melodic", "default", "0") + string(".fmci")).c_str(), 0) < 0)
		{

			fm_resizeInstrumentList(fm, 1);
		}
	}
	customPreset = fm->instrument[instrList->value];

	op.push_back(OpGUI(10, y + 10, "Operator 1"));
	op.push_back(OpGUI(10, y + 250, "Operator 2"));
	op.push_back(OpGUI(10, y + 490, "Operator 3"));
	op.push_back(OpGUI(500, y + 10, "Operator 4"));
	op.push_back(OpGUI(500, y + 250, "Operator 5"));
	op.push_back(OpGUI(500, y + 490, "Operator 6"));
	op[0].slider[0].setValue(50);
	op[1].slider[0].setValue(85);
	op[2].slider[0].setValue(50);
	op[4].slider[0].setValue(50);

	waveform.setPosition(965, y + 455);
	connector.setTextureRect(IntRect(300, 330, 220, 28));

	operatorMenu.add("Copy operator");
	operatorMenu.add("Paste operator");
	operatorMenu.add("Copy envelope");
	operatorMenu.add("Paste envelope");

	instrumentMenu.add("Copy");
	instrumentMenu.add("Paste");
	instrumentMenu.add("Remove");

	editTools.add("Undo");
	editTools.add("Redo");


	connector.setPosition(798, 395);


	for (int i = 0; i < 6; i++)
	{
		outs[i].position.x = connector.getPosition().x + 6 + i * 32;
		outs[i].position.y = connector.getPosition().y - 12;
		outs[i].depth = 0;
		outs[i].isBottom = 1;
		for (int j = 0; j < 5; j++)
		{
			outs[i].top[j] = NULL;
			outs[i].bot[j] = NULL;
			fmop[i].top[j] = NULL;
			fmop[i].bot[j] = NULL;
			fmop[i].isBottom = 0;
		}
		fmop[i].connect(&outs[i]);
		fmop[i].number.setString(int2str[i + 1]);
		fmop[i].id = i;

	}

	updateFromFM();
	updateInstrListFromFM();
	songModified(0);
	
}

void InstrEditor::opEnvPaste()
{
	if (opCopied < 0)
		return;

	fm->instrument[instrList->value].op[opSelected].h = copiedInstr.op[opCopied].h;
	fm->instrument[instrList->value].op[opSelected].delay = copiedInstr.op[opCopied].delay;
	fm->instrument[instrList->value].op[opSelected].i = copiedInstr.op[opCopied].i;
	fm->instrument[instrList->value].op[opSelected].a = copiedInstr.op[opCopied].a;
	fm->instrument[instrList->value].op[opSelected].d = copiedInstr.op[opCopied].d;
	fm->instrument[instrList->value].op[opSelected].s = copiedInstr.op[opCopied].s;
	fm->instrument[instrList->value].op[opSelected].r = copiedInstr.op[opCopied].r;

	updateFromFM();
	updateToFM();
}

void InstrEditor::opPaste()
{
	if (opCopied < 0)
		return;
	
	char oldConnect = fm->instrument[instrList->value].op[opSelected].connect;
	char oldConnect2 = fm->instrument[instrList->value].op[opSelected].connect2;
	char oldConnectOut = fm->instrument[instrList->value].op[opSelected].connectOut;

	memcpy(&fm->instrument[instrList->value].op[opSelected], &copiedInstr.op[opCopied], sizeof(fm_instrument_operator));
	fm->instrument[instrList->value].op[opSelected].connect = oldConnect;
	fm->instrument[instrList->value].op[opSelected].connect2 = oldConnect2;
	fm->instrument[instrList->value].op[opSelected].connectOut = oldConnectOut;

	updateFromFM();
	updateToFM();
	valueChanged = 1;
	addToUndoHistory();
}




void InstrEditor::update()
{

	if (contextMenu == &editTools)
	{
		switch (contextMenu->clicked())
		{
			case 0:
				undo();
				break;
			case 1:
				redo();
				break;
		}
	}

	// events for border
	mouse.pos = input_getmouse(borderView);

	// add an instrument
	if (add.clicked())
	{

		if (fm_loadInstrument(fm, string(string("instruments/") + ini_gmlist.GetValue("melodic", "default", "0") + string(".fmci")).c_str(), fm->instrumentCount)<0)
		{
			fm_resizeInstrumentList(fm, fm->instrumentCount+1);
		}

		updateInstrListFromFM();
		instrList->select(instrList->text.size() - 1);
		updateFromFM();
		instrList->selected = 1;
		valueChanged = 1;
		addToUndoHistory();
	}


	// operator menu
	if (contextMenu == &operatorMenu)
	{
		switch (contextMenu->clicked())
		{
			// copy operator
			case 0:
				// copy envelope
			case 2:
				copiedInstr = fm->instrument[instrList->value];
				opCopied = opSelected;
				break;
				// paste operator
			case 1:
				opPaste();
				break;

				// paste envelope
			case 3:
				opEnvPaste();
				valueChanged = 1;
				addToUndoHistory();
				break;
		}
	}

	mouse.pos = input_getmouse(globalView);
	if (mouse.scroll != 0 && mouse.pos.y >= 168 && mouse.pos.x < window->getSize().x - 215)
	{ // mouse wheel scroll
		if (keyboard.ctrl)
		{
			setZoom(zoom + mouse.scroll*0.1);
		}
	}




	for (unsigned i = 0; i<FM_op; ++i)
	{
		fmop[fmop[i].id].muted = (op[i].mute.selected || op[i].slider[0].value == 0);
	}

	mouse.pos = input_getmouse(borderView);

	if (instrCleanup.clicked())
	{
		cleanupInstruments();
	}

	/*if (contextMenu || mouse.pos.x > (int)sidebar->borderRight.getPosition().x)
		return;*/

	mouse.pos = input_getmouse(instrView);


	int oneOpHovered = 0;
	for (unsigned i = 0; i < FM_op; ++i)
	{
		if (fmop[i].hovered)
			oneOpHovered = 1;
	}

	if (!oneOpHovered)
	{
		for (unsigned i = 0; i < FM_op; ++i)
		{
			fmop[i].highlighted = 0;
		}
	}

	for (unsigned i = 0; i < FM_op; ++i)
	{ // user changes a value of a parameter
		if (op[i].update())
		{
			updateToFM();
			valueChanged = 1;
			for (unsigned j = 0; j < FM_op; ++j)
			{
				if (i != j)
				{
					op[j].selected=false;
				}
			}
		}
		else
		{
			if ( mouse.clickd)
			{
				for (unsigned j = 0; j < FM_op; ++j)
				{
					if (i != j)
					{
						op[j].selected=false;
					}
				}
			}
			if (mouse.clickd && op[i].selected)
			{
				operatorMenu.show();
			}
		}

		if (op[i].selected)
			opSelected=i;


		if (op[i].hover())
		{
			for (unsigned j = 0; j < FM_op; ++j)
			{
				if (i == fmop[j].id)
					fmop[j].highlighted = 1;
			}
		}
	}
	mouse.pos = input_getmouse(op[0].view);
	if (feedback.update() || feedbackSource.update())
	{
		updateToFM();
		valueChanged = 1;
	}



	mouse.pos = input_getmouse(instrView);

	if (temperament.clicked())
	{
		popup->show(POPUP_TEMPERAMENT);
		for (int i = 0; i < 12; i++)
			popup->sliders[i].setValue(fm->instrument[instrList->value].temperament[i]);
	}

	if (transposable.clicked() || transpose.update() || tuning.update() || volume.update() || lfoSpeed.update() || lfoDelay.update() || lfoA.update() || lfoWaveform.update() || lfoOffset.update() ||
		k_fx1.update() || k_fx2.update())
	{
		waveform.setTextureRect(IntRect(810 + (lfoWaveform.value / 13) * 36, 32 * (lfoWaveform.value % 13), 36, 32));
		lfoOffsetBar.setPosition(waveform.getPosition().x + lfoOffset.value*36.0 / 32, waveform.getPosition().y + 6);
		updateToFM();
		valueChanged = 1;
	}
	if (algo.update())
	{
		if (algo.value == 35)
		{
			algo.setDisplayedValueOnly("Custom");
			fm->instrument[instrList->value] = customPreset;
			updateAlgoFromFM();
		}
		else
		{
			loadAlgoPreset(algo.value, &fmop[0], &outs[0]);
		}
		updateToFM();
		valueChanged = 1;
	}


	for (int i = 0; i < 6; i++)
	{
		if (fmop[i].update())
		{
			valueChanged = 1;
			break;
		}
	}







	if (instrName.modified())
	{
		//memset(fm->instrument[instrList->value].name,0,24);
		strncpy(fm->instrument[instrList->value].name, instrName.text.getString().toAnsiString().c_str(), 24);

		instrList->text[instrList->value].setString(int2str[instrList->value] + " " + instrName.text.getString());
	}

	if (save.clicked())
	{
		instrument_save();
	}

	else if (load.clicked())
	{
		instrument_open();
	}
	else if (envReset.clicked())
	{
		envReset.selected = !envReset.selected;
		updateToFM();
	}
	else if (phaseReset.clicked())
	{
		phaseReset.selected = !phaseReset.selected;
		updateToFM();
	}
	else if (lfoReset.clicked())
	{
		lfoReset.selected = !lfoReset.selected;
		updateToFM();
	}
	else if (smoothTransition.clicked())
	{
		smoothTransition.selected = !smoothTransition.selected;
		updateToFM();
	}

	

	if (valueChanged && mouse.clickgReleased)
		addToUndoHistory();
}



void InstrEditor::draw()
{

	window->setView(globalView);

	for (unsigned i = 0; i < FM_op; ++i)
	{
		op[i].draw();
	}

	window->setView(op[0].view);

	feedback.draw();
	feedbackSource.draw();

	window->setView(instrView);

	window->draw(connector);
	


	drawBatcher.initialize();


	drawBatcher.addItem(&save);
	drawBatcher.addItem(&load);
	drawBatcher.addItem(&algo);
	drawBatcher.addItem(&lfoBG);
	drawBatcher.addItem(&lfoDelay);
	drawBatcher.addItem(&lfoSpeed);
	drawBatcher.addItem(&lfoA);
	drawBatcher.addItem(&lfoWaveform);
	drawBatcher.addItem(&lfoOffset);
	drawBatcher.addItem(&temperament);
	
	drawBatcher.addItem(&newNote);
	drawBatcher.addItem(&kfx_text);

	instrName.draw();

	transposable.draw();

	if (k_fx1.value == 0)
	{
		k_fx2.setMinMax(0, 7);
		k_fx1.name.setString("Global");
		k_fx2.name.setString(kfx_globalParams[k_fx2.value]);
	}
	else
	{
		k_fx2.setMinMax(0, 16);
		k_fx2.name.setString(kfx_operatorParams[k_fx2.value]);
		k_fx1.name.setString("Op " + std::to_string(k_fx1.value));
	}

	drawBatcher.addItem(&k_fx1);
	drawBatcher.addItem(&k_fx2);
	drawBatcher.addItem(&lfo);

	drawBatcher.addItem(&lfoOffsetBar);
	drawBatcher.addItem(&volume);
	drawBatcher.addItem(&transpose);
	drawBatcher.addItem(&tuning);
	drawBatcher.addItem(&smoothTransition);
	drawBatcher.addItem(&envReset);
	drawBatcher.addItem(&phaseReset);
	drawBatcher.addItem(&lfoReset);

	for (unsigned i = 0; i < FM_op; ++i)
	{
		drawBatcher.addItem(&fmop[i]);
	}

	for (unsigned i = 0; i < FM_op; ++i)
	{
		fmop[i].drawLines();
	}

	drawBatcher.draw();

	
	
	window->draw(waveform);
}



void InstrEditor::reset()
{
	currentHistoryPos.resize(0);
	history.resize(0);
	updateInstrListFromFM();
	updateFromFM();

}

void InstrEditor::removeInstrument()
{
	history.erase(history.begin() + instrList->value);
	currentHistoryPos.erase(currentHistoryPos.begin() + instrList->value);
	fm_removeInstrument(fm, instrList->value, 1);
	updateInstrListFromFM();
	updateFromFM();
}

void InstrEditor::resetView(int width, int height)
{

	instrView.setViewport(FloatRect(0, 0, 1, 1));

	instrView.reset({ 0.f, 0.f, (float)width, (float)height });

	int decale = 0;
	if (width < 1600)
	{
		decale = 1600 - width;
		
	}

	int decaleH = 0;
	if (height < 800)
	{
		decaleH = 800 - height;
		
	}

	instrView.setCenter((float)width / 2 - 300 + (int)(decale/1.25), (float)height / 2 - height/13 + 56 + decaleH/6);


	for (int i = 0; i < 3; i++)
	{
		op[i].view.setViewport(FloatRect(0, 0, 1, 1));
		op[i].view.reset(FloatRect(0.f, 0.f, width, height));
		op[i].view.setCenter((float)width / 2 - floor((i / 3)*width / 3.3) - 20 - 12 + decale / 16, (float)height / 2 - floor((i % 3)*(min(800, height)) / 3.2) - height/13);
	}
	for (int i = 3; i < 6; i++)
	{
		op[i].view.setViewport(FloatRect(0, 0, 1, 1));
		op[i].view.reset(FloatRect(0.f, 0.f, width, height));
		op[i].view.setCenter((float)width / 2 - 500 - 42 + decale / 3, (float)height / 2 - floor((i % 3)*(min(800, height)) / 3.2) - height/13);
	}

	float zoom = clamp(1 - max(1600 - width, 0)*0.0005, 0.8, 1);

	for (int i = 0; i < 6; i++)
	{
		op[i].setZoom(zoom);
	}

	feedback.setPosition(321 * zoom, feedback.y);
	feedbackSource.setPosition(406 * zoom, feedbackSource.y);

	instrList->setMaxRows(max(3, height / 17 - 29));

	instrCleanup.setPosition(1077, instrList->bg.getPosition().y + instrList->maxrows * 17 + 20);
}

void InstrEditor::cleanupInstruments()
{
	for (int i = 0; i < fm->instrumentCount; i++)
	{
		if (!fm_isInstrumentUsed(fm, i))
		{
			if (i < history.size())
			{
				history.erase(history.begin() + i);
				currentHistoryPos.erase(currentHistoryPos.begin() + i);
			}
			fm_removeInstrument(fm, i, 1);
			/* Because instruments after the removed one are re-numbered, we need to check again the same i (it's the next instrument) */
			if (fm->instrumentCount==1)
				break;
			i--;
		}
	}
	updateInstrListFromFM();
	updateFromFM();
}

void InstrEditor::setZoom(float _zoom)
{
	zoom = clamp(_zoom, 0.7, 1.5);

	for (int i = 0; i < 6; i++)
	{
		op[i].setZoom(zoom);
	}
}

void InstrEditor::handleEvents()
{
	handleNotePreview(!instrName.editing);
	if (!contextMenu || contextMenu && !contextMenu->hover())
	{
		// note preview
		if (evt.type == Event::KeyPressed && !instrName.editing)
		{
			if (keyboard.ctrl)
			{
				switch (evt.key.code)
				{

					case Keyboard::Z: // undo
						undo();
						break;
					case Keyboard::Y: // redo
						redo();
						break;

					case Keyboard::C: // copier

						if (instrList->selected)
						{
							copied=1;
						}
						copiedInstr = fm->instrument[instrList->value];
						opCopied = opSelected;

						break;
					case Keyboard::V: // coller
						
						if (instrList->selected && copied)
						{

							fm->instrument[instrList->value] = copiedInstr;
							updateFromFM();
							updateInstrListFromFM();
							valueChanged=1;
							addToUndoHistory();
						}
						else
							opPaste();
							
						
						break;
					case Keyboard::Delete: // supprimer instrument
						if (instrList->selected)
						{
							removeInstrument();
						}
						break;

				}
			}
		}
		if (evt.type == Event::MouseButtonPressed)
		{

		}
		if (evt.type == Event::MouseButtonReleased)
		{
			if (evt.mouseButton.button == Mouse::Right)
			{
				linkOps();
				updateAlgoToFM();
				rearrangeOps();
			}
			// rearrange operators...
			int connected = 0;
			if (evt.mouseButton.button == Mouse::Left)
			{

				for (int i = 0; i < 6; i++)
				{
					if (fmop[i].active)
					{


						for (int j = 0; j < 6; j++)
						{
							if (i != j && fmop[i].hover(&fmop[j]))
							{
								if (fmop[i].connect(&fmop[j]))
								{
									connected = 1;
									break;
								}

							}
						}
						if (connected == 0)
						{
							int outPos = max(0, min(5, (fmop[i].position.x - connector.getPosition().x + 12) / 32));
							Operator* topmost = outs[outPos].getTopmost(&fmop[i]);
							if (topmost == &outs[outPos])
							{
								outs[outPos].top[0] = NULL;

							}
							fmop[i].connect(topmost, 1);

						}
						algo.setValue(35);

						algo.setDisplayedValueOnly("Custom");

						updateAlgoToFM();
						customPreset = fm->instrument[instrList->value];
						rearrangeOps();
						fmop[i].active = 0;
						valueChanged = 1;
						break;

					}
				}
				addToUndoHistory();

			}


		}

	}
}