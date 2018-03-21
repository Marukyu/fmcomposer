#include "../fmengine/fmlib.h"
#include "streamedExport.h"
#include <stdio.h>
#include "../gui/popup/popup.hpp"
#include "../views/settings/configEditor.hpp"
#include "portaudio.h"
#include "../libs/tinyfiledialogs/tinyfiledialogs.h"
#include <lame.h>
#include <FLAC/stream_encoder.h>

extern fmsynth *phanoo; 
extern Popup *popup;
extern RenderWindow* window;
extern PaStream *stream;
extern int exporting;
string exportFileName;
extern Thread thread;
extern ConfigEditor *config;
int export_param;
int exportFromPattern;
int exportToPattern;
int exportNbLoops;
int exportBitDepth;

const int mp3_bitrates[16] = {8, 16, 24, 32, 40, 48, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

void stopExport(){
	exporting=0;
	song_stop();
}

void exportFinished(){
	config->selectSoundDevice(config->approvedDeviceId,config->approvedSampleRate, config->currentLatency, true);
	Pa_StartStream( stream );
	fm->looping=-1;
	song_stop();
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

	int bitDepths_bytes[5] = {1,2,3,4,4};

	unsigned int size=0;
	int out[16384];

	int format;

	if (exportBitDepth == FM_RENDER_FLOAT)
	{
		format =  3; // IEEE float
	}
	else
	{
		format =  1; // integer
	}

	int bits=16,channels=2,bytes_per_sample=bitDepths_bytes[exportBitDepth];
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
	fwrite("data    ",8,1,fp);
	exportStart();

	while(fm->playing && exporting && fm->order<=exportToPattern){
		
		fm_render(fm, &out[0],16384,exportBitDepth);
		fwrite(&out[0],bitDepths_bytes[exportBitDepth]*16384,1,fp);
		size+=bitDepths_bytes[exportBitDepth]*16384;// bits per sample * num samples
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}
	song_stop();
	fseek(fp,40,0);
	size-=4;
	fwrite(&size,sizeof(int),1,fp);
	fseek(fp,4,0);
	size+=36;
	fwrite(&size,sizeof(int),1,fp);
	fclose(fp);
	exportFinished();
	return 1;
}


static FLAC__StreamEncoderWriteStatus stream_encoder_write_callback_(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[], size_t bytes, uint32_t samples, uint32_t current_frame, void *client_data)
{
	FILE *f = (FILE*)client_data;
	(void)encoder, (void)samples, (void)current_frame;



	if(fwrite(buffer, 1, bytes, f) != bytes)
		return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
	else
		return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

static void stream_encoder_metadata_callback_(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	(void)encoder, (void)metadata, (void)client_data;
}

static FLAC__StreamMetadata streaminfo_, padding_, seektable_, application1_, application2_, vorbiscomment_, cuesheet_, picture_, unknown_;
static FLAC__StreamMetadata *metadata_sequence_[] = { &vorbiscomment_, &padding_, &seektable_, &application1_, &application2_, &cuesheet_, &picture_, &unknown_ };
static const uint32_t num_metadata_ = sizeof(metadata_sequence_) / sizeof(metadata_sequence_[0]);

static FLAC__StreamEncoderSeekStatus stream_encoder_seek_callback_(const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
	FILE *f = (FILE*)client_data;
	(void)encoder;
	if(fseek(f, (long)absolute_byte_offset, SEEK_SET) < 0)
		return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
	else
		return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
}

static FLAC__StreamEncoderTellStatus stream_encoder_tell_callback_(const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
	FILE *f = (FILE*)client_data;
	long pos;
	(void)encoder;
	if((pos = ftell(f)) < 0)
		return FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
	else {
		*absolute_byte_offset = (FLAC__uint64)pos;
		return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
	}
}

int flacExportFunc(){
	short out[16384];
	int out2[16384];
	FILE *fp = fopen(exportFileName.c_str(), "wb");
	if (!fp){
		return 0;
	}


	FLAC__StreamEncoder *encoder;
	encoder = FLAC__stream_encoder_new();

	if(!FLAC__stream_encoder_set_compression_level(encoder,export_param))
		printf("aya");


	FLAC__stream_encoder_init_stream(encoder, stream_encoder_write_callback_, stream_encoder_seek_callback_, stream_encoder_tell_callback_, stream_encoder_metadata_callback_, fp);
	FLAC__stream_encoder_set_sample_rate(encoder,fm->sampleRate);
	FLAC__stream_encoder_set_bits_per_sample(encoder,16);
	FLAC__stream_encoder_set_channels(encoder, 2);
	
	FLAC__stream_encoder_set_metadata(encoder, metadata_sequence_,num_metadata_);



	int writtenBytes;
	exportStart();
	while(fm->playing && exporting && fm->order<=exportToPattern){
		fm_render(fm, &out[0],16384,exportBitDepth);
		for (unsigned i = 0; i < 16384; i++)
		{
			out2[i] = out[i];
		}
		writtenBytes = FLAC__stream_encoder_process_interleaved(encoder, out2, 16384/2);
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}

	FLAC__stream_encoder_finish(encoder);
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


	if (export_param<100){
		lame_set_VBR(lame, vbr_rh);
		lame_set_VBR_q(lame, export_param);
	}
	else{
		lame_set_VBR(lame, vbr_off);
		lame_set_brate(lame, mp3_bitrates[export_param-100]);
	}

    lame_init_params(lame);
	int writtenBytes;

	exportStart();
	while(fm->playing && exporting && fm->order<=exportToPattern){
		fm_render(fm, &out[0],16384, FM_RENDER_16);
		writtenBytes = lame_encode_buffer_interleaved(lame, out, 16384/2, mp3_buffer, 16384);
		fwrite(&mp3_buffer[0],writtenBytes,1,fp);
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}

	lame_close(lame);
	fclose(fp);
	exportFinished();
	return 1;
}