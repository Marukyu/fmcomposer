#ifndef STREAMEDEXPORT_H
#define STREAMEDEXPORT_H

#include <vector>

extern int export_param;
extern int exportBitDepth;
extern const int mp3_bitrates[16];
extern std::vector< std::vector< int> > multitrackAssoc;

int waveExportFunc();
int mp3ExportFunc();
int flacExportFunc();
void stopExport();

#endif