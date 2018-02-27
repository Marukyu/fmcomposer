#ifndef NOTEPREVIEW_H
#define NOTEPREVIEW_H
#include "../gui/gui.hpp"
#include "../libs/portmidi/portmidi.h"


void previewNote(int instrument, int id, int volume, int isFromMidi);

void previewNoteStop(int id, int isFromMidi);

void previewNoteStopAll();

void previewNoteBend(fmsynth *f, int value);

void configurePreviewChannel(int channel);

#endif