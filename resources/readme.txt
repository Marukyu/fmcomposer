   _____ _ _ _      __
    /  '' ) ) )    /  )
 ,-/-,   / / /    /   __ ______  _   __ _   _  __ 
(_/     / ' (_   (__/(_)/ / / <_/_)_(_)/_)_</_/ (_
                               / 
                              '  v1.4 (2018-02-28)

     by Stéphane Damo -- http://fmcomposer.org


***  Thank you for downloading FM Composer ***


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


*** Credits ***

	- Masami Komuro (demo song and some FM sounds)
	- Klairzaki Fil-Xter (quality testing)

	- Laurent Gomila & contributors (SFML lib)
	- Guillaume Vareille (tinyfiledialogs lib)
	- Brodie Thiesfield (SimpleIni lib)
 	- Ross Bencina/Phil Burk/Roger B. Dannenberg (PortMidi/Audio lib)
	- Yann Collet (LZ4 lib)
	- The LAME MP3 encoder team
 	- The Google team (Material Icons)
	- Josh Coalson & Xiph.org foundation (FLAC encoder)

*** Changelog ***

v1.4 (2018-02-28)
	- [Feature] FLAC export
	- [Feature] Editing step : entering a note automatically skips 0...16 rows for fast beat making (see 'Editing step' slider on the right side of the Pattern view)
	- [Feature] Support for high-DPI monitors
	- [Optimization] Reduced CPU usage
	- [Fix] Export bugs : 'stop to pattern xxx' was ignored for MP3 exports / song kept playing after exporting

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
