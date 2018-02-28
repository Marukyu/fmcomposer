#ifdef __cplusplus
extern "C"{
#endif

#ifndef FMLIB_H
#define FMLIB_H

	/* Number of channels (polyphony) */
#define FM_ch 24
	/* Number of operators */
#define FM_op 6


	enum{ FM_NOTE, FM_INSTR, FM_VOL, FM_FXTYPE, FM_FXVALUE };
	enum { FM_ERR_FILEIO = -1, FM_ERR_FILECORRUPTED = -2, FM_ERR_FILEVERSION = -3 };
	enum fmInstrumentFlags{FM_INSTR_LFORESET=1, FM_INSTR_SMOOTH=2, FM_INSTR_TRANSPOSABLE=4};

	typedef struct fm_instrument_operator
	{
		unsigned char mult;
		unsigned char finetune;
		char detune;
		char vol;
		char connectOut;
		char connect;
		char connect2;

		char delay, i, a, h, d, s, r;

		char kbdVolScaling;

		char kbdAScaling;
		char kbdDScaling;
		char velSensitivity;
		char lfoFM;
		char lfoAM;
		char waveform;
		char fixedFreq;

		char kbdCenterNote;
		char kbdPitchScaling;

		char pitchInitialRatio;
		char pitchDecay;
		char pitchFinalRatio;
		char pitchRelease;

		char envLoop;
		char muted;

		unsigned char offset;

	}fm_instrument_operator;


	/* FMCI file format */
	typedef struct fm_instrument{
		char magic[4]; /* "FMCI" identifier */
		unsigned char dummy;
		unsigned char version;
		char name[24];
		char toMix[4];
		char feedback;
		char lfoDelay;
		char lfoSpeed;
		char lfoA;
		char lfoWaveform;
		char lfoOffset;
		char volume;
		char feedbackSource;
		char envReset;
		char tuning;
		char transpose;
		char phaseReset;
		char flags;
		char temperament[12];
		unsigned char kfx;
		fm_instrument_operator op[FM_op];
	}fm_instrument;



	typedef struct Cell{
		unsigned char note;
		unsigned char instr;
		unsigned char vol;
		unsigned char fx;
		unsigned char fxdata;
	}Cell;


	typedef struct ChannelState{
		float time;
		unsigned char tempo;
		unsigned char vol[FM_ch];
		unsigned char pan[FM_ch];
	}ChannelState;

	typedef struct fm_operator{
		// dynamic operator data
		float *connect, *connect2, *connectOut, *toMix;
		float out;
		float *waveform;
		unsigned int phase; // 10.10 bit phase accumulator (10 MSB used for sine lookup table)
		unsigned int pitch;
		float amp, ampDelta;



		float			env;
		unsigned		state;
		float				incr;
		float				pitchMod, pitchTime, pitchDestRatio;
		// static operator data
		float		kbdVolScaling;
		unsigned		envCount;
		float	vol;

		unsigned	delay, offset;
		float	i, a, h, d, s, r;
		float prevAmp, realAmp, ampTarget;
		float	lfoFM, lfoAM;
		float portaDestIncr;
		unsigned char id, mult, baseVol, kbdCenterNote;
		char baseA, baseD;
		char finetune, detune, fixedFreq, envLoop, pitchFinalRatio;
		float velSensitivity, volScaling;
	}fm_operator;


	typedef struct fm_channel{
		int newNote;

		// real time panning/volume
		float pan, vol;

		// used for smoothing panning/volume changes (avoid clicks)
		float destPan, destVol;

		// initial song channel pannings/volumes/reverb amounts
		unsigned char initial_pan, initial_vol, initial_reverb;


		float instrVol;
		float reverbSend;
		int muted;

		float *feedbackSource;
		float mixer;


		// DAHDSR envelope
		float currentEnvLevel;
		float	feedbackLevel;
		int op0;
		int realChannelNumber;
		float lfo;
		// channels
		unsigned int	lfoPhase, lfoMask, lfoIncr, lfoWaveform;
		unsigned algo;
		unsigned char note, baseArpeggioNote;
		float arpTimer;
		int arpIter;
		fm_instrument* instr;
		unsigned char instrNumber;
		float ramping;
		float rampingPicture;
		float lfoEnv, lfoA, lfoFMCurrentValue, lfoAMCurrentValue;
		unsigned lfoDelayCpt, lfoDelayCptMax, lfoOffset;
		unsigned char fxActive, fxData, noteVol, untransposedNote;
		char transpose;
		unsigned active;


		float fadeFrom, fadeFrom2, fadeIncr, fade, tuning;
		float lastRender, lastRender2, delta;
		float pitchBend;
		fm_instrument* cInstr;
		fm_operator op[FM_op];

	}fm_channel;


	typedef struct fmsynth{
		char songName[64], author[64], comments[256];
		float globalVolume;
		unsigned char _globalVolume;
		fm_instrument *instrument; // here are stored your instruments
		unsigned char instrumentCount;
		char transpose, looping;
		unsigned char loopCount;
		int channelStatesDone;

		// reverb
		unsigned reverbPhaseL, reverbPhaseL2, reverbPhaseR, reverbPhaseR2;
		float* revBuf;
		unsigned revBufSize;

		unsigned allpassPhaseL, allpassPhaseR, allpassPhaseL2, allpassPhaseR2;

		unsigned allpassMod, allpassMod2, reverbMod1, reverbMod2, reverbMod3, reverbMod4;
		unsigned revOffset2, revOffset3, revOffset4, revOffset5, revOffset6, revOffset7;

		float reverbRoomSize, initialReverbRoomSize;
		float reverbLength, initialReverbLength;

		unsigned char tempo, initial_tempo;
		unsigned row, order, playing, saturated;

		float noConnect;
		ChannelState **channelStates;
		Cell(**pattern)[FM_ch];
		unsigned patternCount;
		unsigned *patternSize;
		unsigned frameTimer;
		float frameTimerFx;

		float outL, outR;
		unsigned char diviseur;
		float sampleRateRatio;


		unsigned sampleRate;
		float noteIncr[128];


		fm_channel ch[FM_ch];

		float transitionSpeed;
		int tempRow, tempOrder;
		unsigned readSeek, totalFileSize;
	}fmsynth;


	/** Creates the synth
		@param samplerate : sample rate in Hz
		@return pointer to the allocated fm synth
		*/
	fmsynth* fm_create(int samplerate);

	/** Load a song
		@param filename
		@return 1 if ok, 0 if failed
		*/
	int fm_loadSong(fmsynth* f, const char* filename);



	/** Load a song
		@param filename
		@return 1 if ok, 0 if failed
		*/
	int fm_loadSongFromMemory(fmsynth* f, char* data, unsigned len);

	/** Get total song length
		@return length in seconds
		*/
	float fm_getSongLength(fmsynth* f);

	/** Play the song */
	void fm_play(fmsynth* f);

	/** Stop the song
		@param mode : 0 = force note off, 1 = hard cut
		*/
	void fm_stop(fmsynth* f, int mode);



	/** Set the global volume
		@param volume : volume, 0-99
		*/
	void fm_setVolume(fmsynth *f, int volume);

	/** Render the sound
		@param buffer : audio buffer, left and right channels are interleaved
		@param length : number of samples to render
		*/
	void fm_render(fmsynth* f, signed short* buffer, unsigned length);

	/** Play a note
		@param instrument : instrument number, 0-255
		@param note : midi note number, 0-127 (C0 - G10)
		@param channel : channel number, 0-23
		@param volume : volume, 0-99
		*/
	void fm_playNote(fmsynth* f, unsigned instrument, unsigned note, unsigned channel, unsigned volume);

	/** Stops a note on a channel
		@param channel : channel number, 0-23
		*/
	void fm_stopNote(fmsynth* f, unsigned channel);





	/** Set the playing position
		@param pattern : pattern number
		@param row : row number
		@param mode : 0 = keep playing notes, 1 = force note off, 2 = hard cut
		*/
	void fm_setPosition(fmsynth* f, int pattern, int row, int mode);

	/** Set the playing position in seconds
		@param time : position in seconds
		@param mode : 0 = keep playing notes, 1 = force note off, 2 = hard cut
		*/
	void fm_setTime(fmsynth* f, int time, int mode);

	/** Set the tempo
		@param tempo : tempo in BPM, 0-255
		*/
	void fm_setTempo(fmsynth* f, int tempo);


	/** Get the current playing position
		@param *order : current pattern number
		@param *row : current row number
		*/
	void fm_getPosition(fmsynth* f, int *pattern, int *row);

	/** Get the playing time in seconds
		@return current playing time in seconds
		*/
	float fm_getTime(fmsynth* f);

	/** Set the sample rate
		@param samplerate : sample rate in Hz
		@return 1 if ok, 0 if failed (keeps the previous sample rate)
		*/
	int fm_setSampleRate(fmsynth* f, int samplerate);

	/** Set channel volume
		@param channel : channel number, 0-23
		@param volume : volume, 0-99
		*/
	void fm_setChannelVolume(fmsynth *f, int channel, int volume);

	/** Set channel panning
		@param channel : channel number, 0-23
		@param panning : panning, 0-255
		*/
	void fm_setChannelPanning(fmsynth *f, int channel, int panning);

	/** Set channel reverb
		@param channel : channel number, 0-23
		@param panning : reverb amount, 0-99
		*/
	void fm_setChannelReverb(fmsynth *f, int channel, int reverb);

	/** Write data to a pattern
		@param pattern : pattern number
		@param row : row number
		@param channel : channel number
		@param type : the column to write into (FM_NOTE, FM_INSTR, FM_VOL, FM_FXTYPE, FM_FXVALUE)
		@param value : the value to write. 255 is considered empty
		@return 1 if success, 0 if failed (pattern/row/channel/type out of bounds)
		*/
	int fm_write(fmsynth *f, unsigned pattern, unsigned row, unsigned channel, Cell data);

	/** Create a new pattern at the desired position
		@param rows : number of rows
		@param position : the position where to insert the pattern
		@return 1 if success, 0 if failed
		*/
	int fm_insertPattern(fmsynth* f, unsigned rows, unsigned position);

	/** Remove a pattern
		@param pattern : the pattern number
		@return 1 if success, 0 if failed
		*/
	int fm_removePattern(fmsynth* f, unsigned pattern);

	/** Resize a pattern. Contents are not stretched/scaled.
		@param pattern : the pattern number
		@param size : the new size
		@return 1 if success, 0 if failed
		*/
	int fm_resizePattern(fmsynth* f, unsigned pattern, unsigned size, unsigned scaleContent);







	/* ########################################################### */

	/* Stops a note by its number */
	void fm_stopNoteID(fmsynth* f, unsigned note);



	void fm_clearSong(fmsynth* f);

	int fm_getPatternSize(fmsynth *f, int pattern);

	int fm_resizeInstrumentList(fmsynth* f, unsigned size);
	void fm_portamento(fmsynth* f, unsigned channel, float value);


	void fm_patternClear(fmsynth* f);
	int fm_resizePatterns(fmsynth* f, unsigned count);
	int fm_loadInstrument(fmsynth* f, const char *filename, unsigned slot);
	int fm_loadInstrumentFromMemory(fmsynth* f, char *data, unsigned slot);
	int fm_saveInstrument(fmsynth* f, const char* filename, unsigned slot);
	void fm_removeInstrument(fmsynth* f, unsigned slot, int removeOccurences);
	void fm_movePattern(fmsynth* f, int from, int to);
	/* Saves the song to file */
	int fm_saveSong(fmsynth* f, const char* filename);


	void fm_buildStateTable(fmsynth* f, unsigned orderStart, unsigned orderEnd, unsigned channelStart, unsigned channelEnd);
	int fm_initReverb(fmsynth *f, float roomSize);

	/* Forces all sound to stop. Cut notes and reverb. */
	void fm_stopSound(fmsynth* f);
	void fm_moveChannels(fmsynth* f, int from, int to);
	int fm_clearPattern(fmsynth* f, unsigned pattern, unsigned rowStart, unsigned count);
	int fm_insertRows(fmsynth *f, unsigned pattern, unsigned row, unsigned count);
	int fm_removeRows(fmsynth *f, unsigned pattern, unsigned row, unsigned count);


	float fm_volumeToExp(int volume);

	int fm_isInstrumentUsed(fmsynth *f, unsigned id);
	void fm_createDefaultInstrument(fmsynth* f, unsigned slot);
#endif

#ifdef __cplusplus
}
#endif