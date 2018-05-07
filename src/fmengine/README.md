# Adding FM Composer song playback to your software

- Copy the files from this folder to your project's folder
- Include "fmlib.h" to your C/C++ code
- Create the FMC engine with the desired playback frequency
`
fmsynth* fm = fm_create(44100);
`

- Load some song 
`
fm_loadSong(fm,"mysong.fmcs");
`
- Play !
`
fm_play(fm);
`

To get the sound output, you need to request it using the fm_render function. Usually, the framework/sound library you use will give you some callback function for that.

```
void myAudioCallback(short *out, int nbFrames){

  // nbFrames*2 because the output is stereo
  // FM_RENDER_16 will output 16 bit signed short..
  // There are also FM_RENDER_24, FM_RENDER_FLOAT and more for other formats
  fm_render(fm,out,nbFrames*2,FM_RENDER_16);

}
```
