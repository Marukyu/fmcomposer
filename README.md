# FM Composer
Music and sound creation tool, featuring a custom FM synthesizer engine and a tracker-like interface.
Released with 150+ FM instruments and drums, ranging from synth sounds to realistic acoustic sounds, covering the whole MIDI instrument set.

# Why
Modern world lacks modern trackers. And there is very few custom sound engines in the VST world, most of them are emulating old existing hardware.
This is a attempt to make a good yet minimalistic FM synthesizer, with a very graphical, intuitive GUI.
# Technical overview
GUI is written in C++ and using the SFML library to get low level access to the display, keyboard and mouse input. Each GUI element has its class. They are used by Views which are the actual program screens (Pattern screen, Instrument screen, Settings etc.). Views, for most of them, contain everything they need for providing the features to the user. This results in quite big classes, which are split in several files for readability. S

The audio engine is written in pure C and was optimized for lowest CPU usage possible (cache-friendly data organization, loops on small data sets, no function calls in critical parts). A part of the optimization is related to the use of FISTP instruction instead of the standard C ftol() for int-to-float conversions. This is done through the /QiFist commandline on Visual Studio.

# Multi platform compatibility
I chose only cross platform libraries for this project, and added #defines in my code when I called OS-specific functions. It should be *relatively* easy to make it work on Linux and MacOS. However I lack time for this stuff. If someone want to have fun with that, your help is welcome !
