#include "midi.h"
#include "../input/noteInput.hpp"
#include "../views/instrument/instrEditor.hpp"

PmTimestamp midithru_time_proc(void *info);


int selectedMidiDevice;
PortMidiStream* midiStream;
PmEvent midiBuffer[3];
PmTimestamp current_timestamp;
vector<int> midiDeviceIds;
vector<string> midiDeviceNames;
int midiReady, midiReceive, midiPedal;

void midi_getEvents()
{

	if (midiReady && Pm_Poll(midiStream))
	{

		int count = Pm_Read(midiStream, midiBuffer, 3);
		if (midiReceive)
		{
			for (int i = 0; i < count; i++)
			{
				switch (Pm_MessageStatus(midiBuffer[i].message) / 16)
				{
					case 8: // note off
						if (!midiPedal)
							previewNoteStop(Pm_MessageData1(midiBuffer[i].message), 1);
						break;
					case 9: // note on
						previewNote(instrList->value, Pm_MessageData1(midiBuffer[i].message), Pm_MessageData2(midiBuffer[i].message) / 1.282828, 1);
						break;
					case 0xB: // cc
						switch (Pm_MessageData1(midiBuffer[i].message))
						{
							case 64: // sustain pedal
								midiPedal = (Pm_MessageData2(midiBuffer[i].message) > 63);
								if (!midiPedal)
								{
									previewNoteStopAll();
								}
								break;
							case 0x78: // (120) all sound off
								fm_stopSound(fm);
								break;
							case 0x7B: // all notes off
								for (unsigned i = 0; i < FM_ch; i++)
								{
									fm_stopNote(fm, i);
								}
								break;
						}
						break;
					case 0xC: // Program Change
						instrList->select(Pm_MessageData1(midiBuffer[i].message));

						break;
					case 14: // Pitch Bend
						previewNoteBend(fm, 2 * Pm_MessageData2(midiBuffer[i].message) + (Pm_MessageData1(midiBuffer[i].message) > 63));
						break;

				}

			}
		}
	}
}

void midi_selectDevice(int id)
{

	if (midiStream || id < 0)
	{
		Pm_Close(midiStream);
		midiReady = 0;
	}
	if (id >= 0)
	{
		Pm_OpenInput(&midiStream, id, 0, 32, 0, 0);
		Pm_SetFilter(midiStream, PM_FILT_ACTIVE | PM_FILT_SYSEX | PM_FILT_CLOCK);
		midiReady = 1;
	}
}

vector<string>* midi_refreshDevices()
{
	Pm_Terminate();
	Pm_Initialize();
	int nbdevices = Pm_CountDevices();

	midiDeviceIds.clear();
	midiDeviceNames.clear();
	int nbInputDevices = 0;
	for (int i = 0; i < nbdevices; i++)
	{
		const PmDeviceInfo *device = Pm_GetDeviceInfo(i);

		//midi input devices
		if (device->input)
		{
			midiDeviceNames.push_back(device->name);
			midiDeviceIds.push_back(i);
			nbInputDevices++;
		}
	}
	if (nbInputDevices == 1)
	{
		midi_selectDevice(midiDeviceIds[0]);
	}

	return &midiDeviceNames;
}

void midiReceiveEnable(int enabled)
{
	midiReceive = enabled;
}