   _____ _ _ _      __
    /  '' ) ) )    /  )
 ,-/-,   / / /    /   __ ______  _   __ _   _  __ 
(_/     / ' (_   (__/(_)/ / / <_/_)_(_)/_)_</_/ (_
                               / 
                              '  v1.5 (2018-04-04)

     by Stéphane Damo -- http://fmcomposer.org


Thank you for downloading FM Composer !


Recommended setup :

	- Windows 10 / 8 / 7
	- A full HD screen
	- Core i3 CPU or equivalent


Lowest tested, fully working setup :

	- Windows XP SP3
	- 1366*768 screen
	- Pentium III 933 Mhz
	- 256 MB RAM


This software is free and contains no ads nor intrusive features.


Thanks to Masami Komuro, Klairzaki Fil-Xter and Isaac Zuniga for their contributions
(quality testing, ideas, help for designing instruments and much more)


*** Additional credits ***

	- Laurent Gomila & contributors (SFML lib)
	- Guillaume Vareille (tinyfiledialogs lib)
	- Brodie Thiesfield (SimpleIni lib)
 	- Ross Bencina/Phil Burk/Roger B. Dannenberg (PortMidi/Audio lib)
	- Yann Collet (LZ4 lib)
	- The LAME MP3 encoder team
 	- The Google team (Material Icons)
	- Josh Coalson & Xiph.org foundation (FLAC encoder)


*** Changelog ***

v1.6 (2018-04-10)
	- [Fix] Rendering is now anti-aliased for non-round DPI screens (eg. 120 DPI)
	- [Fix] Glitchy 24-bit FLAC export / Wrong duration on VBR MP3 export
	- [Fix] The textbox had some edge cases that could lead to a crash

v1.5 (2018-04-04)
	- [Feature] Export bit depth choice for WAVE and FLAC exports
	- [Feature] Actions done through the right click menu in patterns are now undoable
	- [Feature] DirectSound devices are now shown in the available sound devices
	- [Fix] Can't add instruments to a new song if the default one wasn't found
	- [Fix] Crash when 'Remove unused' was clicked if the song had 1 instrument
	- [Fix] Messing with sound devices could crash under some circumstances

v1.4 (2018-03-03)
	- [Feature] FLAC export
	- [Feature] Editing step : entering a note automatically skips 0...16 rows for fast beat making (see 'Editing step' slider on the right side of the Pattern view)
	- [Feature] Support for high-DPI monitors
	- [Optimization] Reduced CPU usage
	- [Fix] Export bugs : 'stop to pattern xxx' was ignored for MP3 exports / song kept playing after exporting
	- [Fix] LAME MP3 is now used as dynamic library (needed by its license)

v1.3 (2018-02-18)
	- [Fix] Selection cursor was misplaced/missized after a Paste action 
	- [Fix] Player engine : notes without volume used the previous note volume, but ignored volumes changes inbetween
	- [Fix] Minor graphical glitch : channel header on the right side sometimes needs the user to scroll a bit more to show up

v1.2 (2018-02-04)
	- [Fix] 'Remove rows' function did nothing
	- [Fix] Small click noise occured on the next note after a channel was unmuted
	- [Fix] Recent songs menu width not immediately updated
	- [Fix] Blurry pattern top (channel params)

v1.1 (2018-01-31)
	- [Feature] Added some new demo songs, mostly from imported MIDIs
	- [Fix] Crash on Undo action under certain circumstances 
	- [Fix] Pattern list was selected on mouse release even if it wasn't focused
	- [Fix] small 1-frame graphical glitch when scrolling in pattern list
