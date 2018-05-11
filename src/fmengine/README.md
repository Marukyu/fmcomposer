# Adding FM Composer song playback to your software

- Copy the files from this folder to your project's folder
- Include "fmlib.h" to your C/C++ code
- Create the FMC engine with the desired playback frequency
```
fmsynth* fm = fm_create(44100);
```

- Load some song 
```
// from a file
fm_loadSong(fm,"mysong.fmcs");

// from memory
fm_loadSongFromMemory(fm, char* data, unsigned length)
```
- Play !
```
fm_play(fm);
```

To get the sound output, you need to request it using the fm_render function. Usually, the framework/sound library you use will give you some callback function for that.

```
void myAudioCallback(short *out, int nbFrames){

  // nbFrames*2 because the output is stereo
  // FM_RENDER_16 will output 16 bit signed short
  // There are also FM_RENDER_24, FM_RENDER_FLOAT and other other formats available
  
  fm_render(fm,out,nbFrames*2,FM_RENDER_16);

}
```

- Change the output volume
```
fm_setPlaybackVolume(fm,int volume); // 0 to 99
```

- Get the song length in seconds
```
float songLength = fm_getSongLength(fm);
```

- Set the playback position
```
// pattern is the index of the pattern to seek at
// row is the row number
// cutMode tells how the playing notes are affected : 0 = keep playing notes, 1 = force note off, 2 = hard cut
fm_setPosition(fm, int pattern, int row, int cutMode);
```

- Get the playback position
```
int currentPattern, currentRow;
fm_getPosition(fm, &currentPattern, &currentRow);
```

- Change the song tempo
```
// tempo is in BPM, from 1 to 255. Setting may be overrided by the song if some pattern use Tempo commands
fm_setTempo(fm, int tempo);
```

- Once you are tired of this
```
fm_destroy(fm); // free resources allocated with fm_create

```

There are a lot more functions in fmlib.h, take a look at it for more informations
