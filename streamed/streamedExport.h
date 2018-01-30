#ifndef STREAMEDEXPORT_H
#define STREAMEDEXPORT_H

extern int vbr_quality;
extern const int mp3_bitrates[16];

int waveExportFunc();
int mp3ExportFunc();
void stopExport();

#endif