#include "instrEditor.hpp"

void InstrEditor::updateFromFM()
{

	currentHistoryPos.resize(fm->instrumentCount, 0);
	history.resize(fm->instrumentCount);

	for (unsigned i = 0; i < FM_op; ++i)
	{
		if (fm->instrument[instrList->value].op[i].fixedFreq == true)
		{
			op[i].slider[1].name.setString("Frequency");
			op[i].slider[1].setMinMax(1, 255);
			op[i].fixedFreq.setText("Fixed");
		}
		else
		{
			op[i].slider[1].name.setString("Mult");
			op[i].slider[1].setMinMax(0, 40);
			op[i].fixedFreq.setText("Ratio");
		}

		op[i].envLoop.selected = fm->instrument[instrList->value].op[i].envLoop == true;
		op[i].mute.selected = fm->instrument[instrList->value].op[i].muted;
		if (op[i].mute.selected)
		{
			op[i].mute.setText(ICON_MD_VOLUME_OFF);
		}
		else
		{
			op[i].mute.setText(ICON_MD_VOLUME_UP);
		}
		op[i].slider[0].setValue(fm->instrument[instrList->value].op[i].vol);
		op[i].slider[1].setValue(fm->instrument[instrList->value].op[i].mult);
		op[i].slider[2].setValue(fm->instrument[instrList->value].op[i].detune);
		op[i].slider[3].setValue(fm->instrument[instrList->value].op[i].a);
		op[i].slider[4].setValue(fm->instrument[instrList->value].op[i].d);
		op[i].slider[5].setValue(fm->instrument[instrList->value].op[i].s);
		op[i].slider[6].setValue(fm->instrument[instrList->value].op[i].r);
		op[i].slider[7].setValue(fm->instrument[instrList->value].op[i].kbdVolScaling);
		op[i].slider[8].setValue(fm->instrument[instrList->value].op[i].lfoFM);
		op[i].slider[9].setValue(fm->instrument[instrList->value].op[i].lfoAM);
		op[i].slider[10].setValue(fm->instrument[instrList->value].op[i].waveform);
		op[i].waveform.setTextureRect(IntRect(810, 32 * op[i].slider[10].value, 36, 32));
		op[i].fixedFreq.selected = fm->instrument[instrList->value].op[i].fixedFreq;
		op[i].slider[11].setValue(fm->instrument[instrList->value].op[i].kbdAScaling);
		op[i].slider[12].setValue(fm->instrument[instrList->value].op[i].kbdDScaling);
		op[i].slider[13].setValue(fm->instrument[instrList->value].op[i].velSensitivity);
		op[i].slider[14].setValue(fm->instrument[instrList->value].op[i].h);
		op[i].slider[15].setValue(fm->instrument[instrList->value].op[i].delay);
		op[i].slider[16].setValue(fm->instrument[instrList->value].op[i].i);
		op[i].slider[17].setValue(fm->instrument[instrList->value].op[i].finetune);
		op[i].slider[18].setValue(fm->instrument[instrList->value].op[i].kbdCenterNote);
		op[i].slider[19].setValue(fm->instrument[instrList->value].op[i].kbdPitchScaling);
		op[i].slider[20].setValue(fm->instrument[instrList->value].op[i].pitchInitialRatio);
		op[i].slider[21].setValue(fm->instrument[instrList->value].op[i].pitchDecay);
		op[i].slider[22].setValue(fm->instrument[instrList->value].op[i].pitchFinalRatio);
		op[i].slider[23].setValue(fm->instrument[instrList->value].op[i].pitchRelease);
		op[i].slider[24].setValue(fm->instrument[instrList->value].op[i].offset);

	}
	feedbackSource.setValue(fm->instrument[instrList->value].feedbackSource + 1);
	instrName.setText(fm->instrument[instrList->value].name);
	feedback.setValue(fm->instrument[instrList->value].feedback);
	lfoSpeed.setValue(fm->instrument[instrList->value].lfoSpeed);
	lfoDelay.setValue(fm->instrument[instrList->value].lfoDelay);
	lfoA.setValue(fm->instrument[instrList->value].lfoA);
	lfoWaveform.setValue(fm->instrument[instrList->value].lfoWaveform);
	waveform.setTextureRect(IntRect(810 + (lfoWaveform.value / 13) * 36, 32 * (lfoWaveform.value % 13), 36, 32));
	lfoOffset.setValue(fm->instrument[instrList->value].lfoOffset);
	volume.setValue(fm->instrument[instrList->value].volume);
	lfoOffsetBar.setPosition(waveform.getPosition().x + lfoOffset.value, waveform.getPosition().y + 6);
	tuning.setValue(fm->instrument[instrList->value].tuning);
	transpose.setValue(fm->instrument[instrList->value].transpose);
	envReset.selected = fm->instrument[instrList->value].envReset;
	phaseReset.selected = fm->instrument[instrList->value].phaseReset;
	lfoReset.selected = fm->instrument[instrList->value].flags & FM_INSTR_LFORESET;
	smoothTransition.selected = fm->instrument[instrList->value].flags & FM_INSTR_SMOOTH;
	transposable.checked = fm->instrument[instrList->value].flags & FM_INSTR_TRANSPOSABLE;

	k_fx1.setValue(fm->instrument[instrList->value].kfx / 32);
	if (k_fx1.value == 0)
		k_fx2.setMinMax(0, 7);
	else
		k_fx2.setMinMax(0, 16);
	k_fx2.setValue(fm->instrument[instrList->value].kfx % 32);


	updateAlgoFromFM();

	if (history[instrList->value].size() == 0)
	{
		valueChanged = 1;
		addToUndoHistory();
	}

}


void InstrEditor::updateInstrListFromFM()
{
	instrList->text.clear();
	instrList->pings.clear();
	for (unsigned i = 0; i < fm->instrumentCount; i++)
		instrList->add(int2str[i] + " " + string(fm->instrument[i].name));
	instrList->select(instrList->value);
}


void InstrEditor::updateToFM()
{
	for (unsigned i = 0; i < FM_op; ++i)
	{

		fm->instrument[instrList->value].op[i].muted = op[i].mute.selected;
		fm->instrument[instrList->value].op[i].vol = op[i].slider[0].value;

		if (op[i].envLoop.selected)
			fm->instrument[instrList->value].op[i].envLoop = 1;
		else
			fm->instrument[instrList->value].op[i].envLoop = 0;

		fm->instrument[instrList->value].op[i].mult = op[i].slider[1].value;
		fm->instrument[instrList->value].op[i].detune = op[i].slider[2].value;
		fm->instrument[instrList->value].op[i].a = op[i].slider[3].value;
		fm->instrument[instrList->value].op[i].d = op[i].slider[4].value;
		fm->instrument[instrList->value].op[i].s = op[i].slider[5].value;
		fm->instrument[instrList->value].op[i].r = op[i].slider[6].value;
		fm->instrument[instrList->value].op[i].kbdVolScaling = op[i].slider[7].value;
		fm->instrument[instrList->value].op[i].lfoFM = op[i].slider[8].value;
		fm->instrument[instrList->value].op[i].lfoAM = op[i].slider[9].value;
		fm->instrument[instrList->value].op[i].waveform = op[i].slider[10].value;
		fm->instrument[instrList->value].op[i].fixedFreq = op[i].fixedFreq.selected;
		fm->instrument[instrList->value].op[i].kbdAScaling = op[i].slider[11].value;
		fm->instrument[instrList->value].op[i].kbdDScaling = op[i].slider[12].value;
		fm->instrument[instrList->value].op[i].velSensitivity = op[i].slider[13].value;
		fm->instrument[instrList->value].op[i].h = op[i].slider[14].value;
		fm->instrument[instrList->value].op[i].delay = op[i].slider[15].value;
		fm->instrument[instrList->value].op[i].i = op[i].slider[16].value;
		fm->instrument[instrList->value].op[i].finetune = op[i].slider[17].value;
		fm->instrument[instrList->value].op[i].kbdCenterNote = op[i].slider[18].value;
		fm->instrument[instrList->value].op[i].kbdPitchScaling = op[i].slider[19].value;
		fm->instrument[instrList->value].op[i].pitchInitialRatio = op[i].slider[20].value;
		fm->instrument[instrList->value].op[i].pitchDecay = op[i].slider[21].value;
		fm->instrument[instrList->value].op[i].pitchFinalRatio = op[i].slider[22].value;
		fm->instrument[instrList->value].op[i].pitchRelease = op[i].slider[23].value;
		fm->instrument[instrList->value].op[i].offset = op[i].slider[24].value;
	}
	fm->instrument[instrList->value].envReset = envReset.selected;
	fm->instrument[instrList->value].phaseReset = phaseReset.selected;

	if (lfoReset.selected)
		fm->instrument[instrList->value].flags |= FM_INSTR_LFORESET;
	else
		fm->instrument[instrList->value].flags &= ~FM_INSTR_LFORESET;

	if (smoothTransition.selected)
		fm->instrument[instrList->value].flags |= FM_INSTR_SMOOTH;
	else
		fm->instrument[instrList->value].flags &= ~FM_INSTR_SMOOTH;

	if (transposable.checked)
		fm->instrument[instrList->value].flags |= FM_INSTR_TRANSPOSABLE;
	else
		fm->instrument[instrList->value].flags &= ~FM_INSTR_TRANSPOSABLE;

	fm->instrument[instrList->value].feedback = feedback.value;
	fm->instrument[instrList->value].feedbackSource = feedbackSource.value - 1;
	fm->instrument[instrList->value].lfoSpeed = lfoSpeed.value;
	fm->instrument[instrList->value].lfoDelay = lfoDelay.value;
	fm->instrument[instrList->value].lfoA = lfoA.value;
	fm->instrument[instrList->value].lfoWaveform = lfoWaveform.value;
	fm->instrument[instrList->value].lfoOffset = lfoOffset.value;
	fm->instrument[instrList->value].tuning = tuning.value;
	fm->instrument[instrList->value].transpose = transpose.value;
	fm->instrument[instrList->value].kfx = k_fx1.value * 32 + k_fx2.value;


	
	fm->instrument[instrList->value].volume = volume.value;
	updateAlgoToFM();

	songModified(1);
}