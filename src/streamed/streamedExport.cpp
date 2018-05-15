#include "../fmengine/fmlib.h"
#include "streamedExport.h"
#include <stdio.h>
#include "../gui/popup/popup.hpp"
#include "../views/settings/configEditor.hpp"
#include "portaudio.h"
#include "../libs/tinyfiledialogs/tinyfiledialogs.h"
#include <FLAC/stream_encoder.h>


#ifdef FMC_ENABLE_MP3_EXPORT
#include <lame/lame.h>
#endif

extern fmsynth *phanoo; 
extern Popup *popup;
extern RenderWindow* window;
extern PaStream *stream;

extern Thread thread;
extern ConfigEditor *config;

struct StreamedExport streamedExport = {};

const int mp3_bitrates[16] = {8, 16, 24, 32, 40, 48, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};

sf::Thread waveExportThread(&waveExportFunc);
sf::Thread mp3ExportThread(&mp3ExportFunc);
sf::Thread flacExportThread(&flacExportFunc);

static const char *exportFormats[3] = { "flac","wav","mp3"  };

void promptStreamedExport()
{
	static const char *filterNames[3][1] = {{ "*.flac"},{"*.wav"},{"*.mp3"}  };
	const char *descNames[3] = { "Export as FLAC","Export as WAVE","Export as MP3"  };
	const char *typeNames[3] = { "FLAC file","WAVE file","MP3 file"  };
					
	const char *fileName = tinyfd_saveFileDialog(descNames[streamedExport.format], NULL, 1, filterNames[streamedExport.format], typeNames[streamedExport.format]);
	if (fileName)
	{
		
		string fileNameOk = forceExtension(fileName, exportFormats[streamedExport.format]);
		song_stop();
		Pa_StopStream(stream);
		Pa_CloseStream(stream);
		popup->show(POPUP_WORKING);
		streamedExport.fileName = streamedExport.originalFileName = fileNameOk;
		switch (streamedExport.format)
		{
			case 0:
				flacExportThread.launch();
				break;
			case 1:
				waveExportThread.launch();
				break;
			case 2:
				mp3ExportThread.launch();
				break;
		}
						
	}
}


void stopExport(){
	streamedExport.running=0;
	song_stop();
}

std::string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot); 
}

void exportFinished(){
	
	song_stop();
	

	/* multi-track export */
	if (streamedExport.multitrackAssoc.size() > 0)
	{
		streamedExport.multiTrackIter++;
		streamedExport.fileName = remove_extension(streamedExport.originalFileName)+"-"+std::to_string(streamedExport.multiTrackIter+1)+"."+string(exportFormats[streamedExport.format]);

		if (streamedExport.multiTrackIter < streamedExport.multitrackAssoc.size())
		{
			switch (streamedExport.format)
			{
				case 0:
					flacExportFunc();
					break;
				case 1:
					waveExportFunc();
					break;
				case 2:
					mp3ExportFunc();
					break;
			}
		}

		
	}

	if (streamedExport.multitrackAssoc.size() == 0 || streamedExport.multiTrackIter >= streamedExport.multitrackAssoc.size()) {
		popup->close();
		if (!windowFocus){
			tinyfd_notifyPopup("FM Composer", "Export finished !","info");
		}
		streamedExport.running=0;
		config->selectSoundDevice(config->approvedDeviceId,config->approvedSampleRate, config->currentLatency, true);
		Pa_StartStream( stream );
		fm->looping=-1;
		for (unsigned i = 0; i < FM_ch; i++)
		{
			fm->ch[i].muted = streamedExport.mutedChannels[i];
		}
	}
}

void exportStart(){
	streamedExport.running=1;
	fm_setPosition(fm, streamedExport.fromPattern,0,2);
	song_play();
	fm->looping=streamedExport.nbLoops; // disable loop points so we aren't stuck forever

	/* multi-track export */
	if (streamedExport.multitrackAssoc.size() > 0)
	{
		for (unsigned i = 0; i < FM_ch; i++)
		{
			fm->ch[i].muted = 1;
		}

		for (unsigned i = 0; i < streamedExport.multitrackAssoc[streamedExport.multiTrackIter].size(); i++)
		{
			if (streamedExport.mutedChannels[streamedExport.multitrackAssoc[streamedExport.multiTrackIter][i]] != 1)
				fm->ch[streamedExport.multitrackAssoc[streamedExport.multiTrackIter][i]].muted = 0;
		}
	}
}

// size in bytes for 1 sample, for each fmRenderType (8, 16, 24 bits, 32 bits, float)
int bitDepths_bytes[5] = {1,2,3,4,4};

int waveExportFunc(){

	unsigned int size=0;
	int out[16384];

	int format;

	if (streamedExport.bitDepth == FM_RENDER_FLOAT)
	{
		format =  3; // IEEE float
	}
	else
	{
		format =  1; // integer
	}

	int bits=16,channels=2,bytes_per_sample=bitDepths_bytes[streamedExport.bitDepth];
	int block_align=channels*bytes_per_sample;
	int bitrate=fm->sampleRate*channels*bytes_per_sample;
	int bits_sample=8*bytes_per_sample;
	FILE *fp = fopen(streamedExport.fileName.c_str(), "wb");
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

	while(fm->playing && streamedExport.running && fm->order<=streamedExport.toPattern){
		
		fm_render(fm, &out[0],16384,streamedExport.bitDepth);
		fwrite(&out[0],bitDepths_bytes[streamedExport.bitDepth]*16384,1,fp);
		size+=bitDepths_bytes[streamedExport.bitDepth]*16384;// bits per sample * num samples
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
	int out[16384*2];
	FILE *fp = fopen(streamedExport.fileName.c_str(), "wb");
	if (!fp){
		return 0;
	}


	FLAC__StreamEncoder *encoder;
	encoder = FLAC__stream_encoder_new();
	FLAC__stream_encoder_set_bits_per_sample(encoder,8*bitDepths_bytes[streamedExport.bitDepth]);
	FLAC__stream_encoder_set_compression_level(encoder,streamedExport.param);
	FLAC__stream_encoder_set_sample_rate(encoder,fm->sampleRate);
	FLAC__stream_encoder_set_channels(encoder, 2);

	FLAC__stream_encoder_init_stream(encoder, stream_encoder_write_callback_, stream_encoder_seek_callback_, stream_encoder_tell_callback_, stream_encoder_metadata_callback_, fp);

	FLAC__stream_encoder_set_metadata(encoder, metadata_sequence_,num_metadata_);



	int writtenBytes;
	exportStart();
	while(fm->playing && streamedExport.running && fm->order<=streamedExport.toPattern){
		fm_render(fm, &out[0],16384,streamedExport.bitDepth | FM_RENDER_PAD32);
		writtenBytes = FLAC__stream_encoder_process_interleaved(encoder, out, 16384/2);
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}

	FLAC__stream_encoder_finish(encoder);
	fclose(fp);
	exportFinished();
	return 1;
}

int mp3ExportFunc(){
#ifdef FMC_ENABLE_MP3_EXPORT
	short out[16384];
	unsigned char mp3_buffer[16384];
	FILE *fp = fopen(streamedExport.fileName.c_str(), "wb+");
	if (!fp){
		return 0;
	}
	lame_t lame = lame_init();
    lame_set_in_samplerate(lame, fm->sampleRate);

	// because we write it ourself, as someone suggested
	lame_set_write_id3tag_automatic(lame, 0);

	if (streamedExport.param<100){
		lame_set_VBR(lame, vbr_rh);
		lame_set_VBR_q(lame, streamedExport.param);
		lame_set_bWriteVbrTag(lame,1);
	}
	else{
		lame_set_VBR(lame, vbr_off);
		lame_set_brate(lame, mp3_bitrates[streamedExport.param-100]);
	}

    lame_init_params(lame);
	int writtenBytes;

	exportStart();

	// from https://sourceforge.net/p/lame/mailman/message/18557283/
	int imp3=lame_get_id3v2_tag(lame, mp3_buffer, sizeof(mp3_buffer));
	fwrite(&mp3_buffer[0], 1, imp3, fp);
	int audio_pos=ftell(fp);
	//

	while(fm->playing && streamedExport.running && fm->order<=streamedExport.toPattern){
		fm_render(fm, &out[0],16384, FM_RENDER_16);
		writtenBytes = lame_encode_buffer_interleaved(lame, out, 16384/2, mp3_buffer, 16384);
		fwrite(&mp3_buffer[0],writtenBytes,1,fp);
		popup->sliders[0].setValue(((float)fm->order/fm->patternCount)*100);
	}

	writtenBytes = lame_encode_flush(lame,mp3_buffer, 16384);

	if (writtenBytes > 0)
	{

		fwrite(&mp3_buffer[0],writtenBytes,1,fp);
	}

	// write the xing tag so vbr files display correct length
	imp3=lame_get_id3v1_tag(lame, mp3_buffer, sizeof(mp3_buffer));
	fwrite(&mp3_buffer[0], 1, imp3, fp);

	imp3=lame_get_lametag_frame(lame, mp3_buffer, sizeof(mp3_buffer));
	fseek(fp,audio_pos,SEEK_SET); // remember beginning of audio data
	fwrite(&mp3_buffer[0], 1, imp3, fp);

	lame_close(lame);
	fclose(fp);
	exportFinished();
	return 1;
#else
	return 0;
#endif
}
