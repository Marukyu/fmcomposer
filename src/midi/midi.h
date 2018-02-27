#ifndef MIDI_H
#define MIDI_H

#include "../fmengine/fmlib.h"
#include "../libs/portmidi/portmidi.h"
#include "../views/settings/configEditor.hpp"
#include <vector>
using namespace std;

#include "midiInstrNames.h"



extern fmsynth *phanoo;
extern ConfigEditor *configEditor;
extern vector<int> midiExportAssoc;
extern vector<int> midiExportAssocChannels;

void midi_getEvents();
void midi_selectDevice(int id);
vector<string>* midi_refreshDevices();

int midiImport(const char* filename);

void midiExport(const char* filename);

void midiReceiveEnable(int enabled);

#endif