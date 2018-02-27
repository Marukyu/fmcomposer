#ifndef SONGFILEACTIONS_H
#define SONGFILEACTIONS_H

void song_load(const char* filename, bool fromAutoReload=false);

int song_save();

int song_saveas();

void song_open();

void song_clear();

#endif
