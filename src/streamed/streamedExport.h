#ifndef STREAMEDEXPORT_H
#define STREAMEDEXPORT_H

extern int export_param;
extern int exportBitDepth;
extern const int mp3_bitrates[16];

int waveExportFunc();
int mp3ExportFunc();
int flacExportFunc();
void stopExport();

#endif