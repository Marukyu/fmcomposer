#include "configEditor.hpp"
#include "pa_win_wasapi.h"


extern PaStream *stream;
extern PaStreamParameters out;

int ConfigEditor::selectSoundDevice(int soundDeviceId, int _samplerate, int _latency, bool force)
{
	currentSoundDeviceId = soundDeviceId;
	for (int i = 0; i < soundDevicesList.text.size(); i++)
	{
		if (soundDeviceId == soundDeviceIds[i])
		{
			soundDevicesList.select(i);
		}
	}
	int playing = fm->playing;


	const PaDeviceInfo* p = Pa_GetDeviceInfo(soundDeviceId);
	out.device = soundDeviceId;
	out.channelCount = 2;
	out.sampleFormat = paInt16;
	out.suggestedLatency = 0.001*_latency;
	out.hostApiSpecificStreamInfo = NULL;




	PaError err = Pa_IsFormatSupported(NULL, &out, _samplerate);

	if (err != paFormatIsSupported)
	{
		sampleRateError.setString("Unsupported !");
		if (approvedSampleRate == -1)
		{
			_samplerate = p->defaultSampleRate;
		}
		else
		{
			_samplerate = approvedSampleRate;
		}

		return -1;
	}
	else
	{

		if (force || soundDeviceId != approvedDeviceId || _samplerate != approvedSampleRate || _latency != currentLatency || sampleRateError.getString() != "")
		{
			sampleRateError.setString("");
			song_stop();
			Pa_StopStream(stream);

			PaError err;

			if ((err = Pa_OpenStream(&stream, NULL, &out, _samplerate, 0, 0, (PaStreamCallback*)callbackFunc, fm)) != paNoError)
			{
				sampleRateError.setString("Unable to open");
				error(Pa_GetErrorText(err));
				return -1;
			}


			if (_samplerate != approvedSampleRate)
			{
				fm_setSampleRate(fm, _samplerate);
			}


			Pa_StartStream(stream);
			if (playing)
				song_play();
			approvedDeviceId = soundDeviceId;
			approvedSampleRate = _samplerate;
			currentLatency = _latency;
		}
	}


	//approvedDeviceId = soundDeviceId;




	return _samplerate;
}


void ConfigEditor::selectBestSoundDevice()
{
	if (!audioInitialized)
		return;

	int soundDeviceId = atoi(ini_config.GetValue("config", "soundDeviceId", "-1"));
	int sampleRate = atoi(ini_config.GetValue("config", "sampleRate", "-1"));
	int latency = atoi(ini_config.GetValue("config", "desiredLatency", "-1"));



	if (soundDeviceId == -1)
	{
		soundDeviceId = Pa_GetDefaultOutputDevice();
	}
	const PaDeviceInfo* p = Pa_GetDeviceInfo(soundDeviceId);

	if (sampleRate == -1)
	{
		sampleRate = p->defaultSampleRate;
	}

	if (latency == -1)
	{
		latency = 1000 * p->defaultHighOutputLatency;
	}


	selectSoundDevice(soundDeviceId, sampleRate, latency);
}
