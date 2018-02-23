#include "../fmengine/fmlib.h"
#include "streamedExport.h"
#include <stdio.h>
#include "../gui/popup/popup.hpp"
#include "../views/settings/configEditor.hpp"
#include "portaudio.h"
#include "../libs/tinyfiledialogs/tinyfiledialogs.h"
#include <lame.h>

extern fmsynth *phanoo; 
extern Popup *popup;
extern RenderWindow* window;
extern PaStream *stream;
extern int exporting;
string exportFileName;
extern Thread thread;
extern ConfigEditor *config;
int vbr_quality;
int exportFromPattern;
int exportToPattern;
int exportNbLoops;

const int mp3_bitrates[16] = {8, 16, 24, 32, 40, 48, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

void stopExport(){
	exporting=0;
	song_stop();
}

void exportFinished(){
	config->selectSoundDevice(config->approvedDeviceId,config->approvedSampleRate, config->currentLatency, true);
	Pa_StartStream( stream );
	fm->looping=-1;
	popup->close();
	if (!windowFocus){
		tinyfd_notifyPopup("FM Composer", "Export finished !","info");
	}
	exporting=0;
}

void exportStart(){
	exporting=1;
	fm_setPosition(fm, exportFromPattern,0,2);
	song_play();
	fm->looping=exportNbLoops; // disable loop points so we aren't stuck forever
}

int waveExportFunc(){
	unsigned int size=0;
	short out[16384];
	int bits=16,format=1,channels=2,bytes_per_sample=2;
	int block_align=channels*bytes_per_sample;
	int bitrate=fm->sampleRate*channels*bytes_per_sample;
	int bits_sample=8*bytes_per_sample;
	FILE *fp = fopen(exportFileName.c_str(), "wb");
	if (!fp){
		popup->show(POPUP_SAVEFAILED);
		return 0;
	}

	fwrite("RIFF    WAVEfmt ",16,1,fp);
	fwrite((char*)&bits,4,1,fp); // SubChunk1Size 
	fwrite((char*)&format,2,1,fp); // pcm format
	fwrite((char*)&channels,2,1,fp); // nb channels
	fwrite((char*)&fm->sampleRate,4,1,fp); // sample rate
	fwrite((char*)&bitrate,4,1,fp); // byte rate =sample_rate*num_channels*bytes_per_sample
	fwrite((char*)&block_align,2,1,fp); // block align
	fwrite((char*)&bits_sample,2,1,fp); // bits/sample
	fwrite("data",4,1,fp);
	exportStart();
	while(fm->playing && exporting && fm->order<=exportToPattern){
		fm_render(fm, &out[0],16384);
		fwrite(&out[0],sizeof(short)*16384,1,fp);
		size+=sizeof(short)*16384;// bits per sample * num samples
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}
	song_stop();
	fseek(fp,40,0);
	fwrite(&size,sizeof(int),1,fp);
	fseek(fp,4,0);
	size+=32;
	fwrite(&size,sizeof(int),1,fp);
	fclose(fp);
	exportFinished();
	return 1;
}


int mp3ExportFunc(){
	short out[16384];
	unsigned char mp3_buffer[16384];
	FILE *fp = fopen(exportFileName.c_str(), "wb");
	if (!fp){
		return 0;
	}
	lame_t lame = lame_init();
    lame_set_in_samplerate(lame, fm->sampleRate);


	if (vbr_quality<100){
		lame_set_VBR(lame, vbr_rh);
		lame_set_VBR_q(lame, vbr_quality);
	}
	else{
		lame_set_VBR(lame, vbr_off);
		lame_set_brate(lame, mp3_bitrates[vbr_quality-100]);
	}

    lame_init_params(lame);
	int writtenBytes;

	exportStart();
	while(fm->playing && exporting){
		fm_render(fm, &out[0],16384);
		writtenBytes = lame_encode_buffer_interleaved(lame, out, 16384/2, mp3_buffer, 16384);
		fwrite(&mp3_buffer[0],writtenBytes,1,fp);
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}

	lame_close(lame);
	fclose(fp);
	exportFinished();
	return 1;
}