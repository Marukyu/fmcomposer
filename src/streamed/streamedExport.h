#ifndef STREAMEDEXPORT_H
#define STREAMEDEXPORT_H

#include <string>
#include <vector>

typedef struct StreamedExport{
	int param;
	int fromPattern;
	int format;
	int toPattern;
	int nbLoops;
	int bitDepth;
	bool mutedChannels[FM_ch];
	std::vector< std::vector< int> > multitrackAssoc;
	int multiTrackIter;
	int running;
	std::string fileName;
	std::string originalFileName;
}StreamedExport;

extern struct StreamedExport streamedExport;

extern const int mp3_bitrates[16];

void promptStreamedExport();

int waveExportFunc();
int mp3ExportFunc();
int flacExportFunc();
void stopExport();

#endif
