#include "globalFunctions.hpp"
#include "input/noteInput.hpp"
#include "views/instrument/instrEditor.hpp"
#include "views/general/generalEditor.hpp"
#include "views/pattern/songEditor.hpp"
#include "views/pianoroll/pianoRoll.hpp"
#include "midi/midi.h"
#include "libs/tinyfiledialogs/tinyfiledialogs.h"
#include "libs/simpleini/SimpleIni.h"
#include "gui/mainmenu.hpp"
#include "portaudio.h"
#include "gui/sidebar.hpp"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif


extern CSimpleIniA ini_gmlist;
extern Menu *menu;
extern int keyboardScheme;

extern Popup *popup;

extern char songDefaultPath[256];
void* callbackFunc;

string windowTitle = "FM Composer - New song";
extern Text notePreview;
bool windowTooSmall;
string lastSongOpened;
int isSongModified = 0;
float frameTime60;
static string note2name[129];
string appdir, appconfigdir, instrDir, songDir;

PaStream *stream;
PaStreamParameters out;

int windowWidth, windowHeight, windowDpi=96;

RenderWindow *window;
fmsynth *fm;
sf::Clock mclock;
sf::WindowHandle windowHandle;
bool audioInitialized = false;
Event evt;

int copiedSliderValue, copiedSliderValueOk;

string kfx_globalParams[8] = { "Volume", "Transpose", "Tuning", "LFO Speed", "LFO Delay", "LFO Attack", "LFO Waveform", "LFO Offset" };
string kfx_operatorParams[32] = { "Volume", "Muted", "Waveform", "Multiplier", "Frequency", "Quartertones", "Fine tuning", "Env Delay", "Env Initial Level",
"Env Attack", "Env Hold", "Env Decay", "Env Sustain", "Env Release", "Env Loop", "LFO FM", "LFO AM" };


int mouseCursor = CURSOR_NORMAL, oldMouseCursor = CURSOR_NORMAL;

#ifdef _WIN32
string pathSeparator = "\\";
#else 
string pathSeparator = "/";
#endif

string intervals[25] = {
	"Perfect unison",
	"Minor second",
	"Major second",
	"Minor third",
	"Major third",
	"Perfect fourth",
	"Tritone",
	"Perfect fifth",
	"Minor sixth",
	"Major sixth",
	"Minor seventh",
	"Major seventh",
	"Perfect octave",
	"Minor ninth",
	"Major ninth",
	"Minor tenth",
	"Major tenth",
	"Perfect eleventh",
	"Augmented eleventh",
	"Perfect twelfth",
	"Minor thirteenth",
	"Major thirteenth",
	"Minor fourteenth",
	"Major fourteenth",
	"Double octave"
};

int keyboard2note(int keyCode)
{
	return noteMappings[keyCode + 1];
}

void computeNoteNames()
{
	const string noteNames[12] = { "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B " };
	for (int i = 0; i < 128; i++)
	{
		note2name[i] = noteNames[i % 12] + int2str[i / 12];
	}
	note2name[128] = "==";
}

string& noteName(int note)
{
	return note2name[clamp(note, 0, 128)];
}

void song_pause()
{
	fm_stop(fm, 0);
	menu->setVertexRect(6*4, 100, 32, 32);
}

void song_stop()
{
	fm_stop(fm, 1);
	menu->setVertexRect(6*4, 100, 32, 32);
	
}

void song_play()
{
	songEditor->updateMutedChannels();
	fm_play(fm);
	menu->setVertexRect(6*4, 36, 32, 32);
}

void song_playPause()
{
	if (fm->playing)
	{
		song_pause();
	}
	else
	{
		song_play();
	}

}

bool appInFocus(sf::RenderWindow* app)
{
	if (app == NULL)
		return false;
#ifdef _WIN32
	HWND handle = app->getSystemHandle();
	bool one = handle == GetFocus();
	bool two = handle == GetForegroundWindow();
	return one && two;
#else
	// TODO: proper focus implementation on non-win32 systems
	return true;
#endif
}

extern List *instrList;

int isWindowMaximized(){
	sf::VideoMode vm = VideoMode::getDesktopMode();

	return (int)window->getSize().x > (int)vm.width - 40 && (int)window->getSize().y > (int)vm.height - 80;
}


void updateViews(int width, int height)
{
	float dpiRatio = windowDpi / 96.0;


	int oldX = window->getPosition().x;
	int oldY = window->getPosition().y;



	/* Scaling for high DPI monitors */
	if (windowDpi != 96)
	{
		width = width / dpiRatio;
		height = height / dpiRatio;
	}


	if(isWindowMaximized()){
#ifdef _WIN32
		ShowWindow(GetActiveWindow(), SW_MAXIMIZE);
#endif
	}


	width = max(width, 1300);
	height = max(height, 700);

	windowWidth = width;
	windowHeight = height;



	globalView.reset(FloatRect(0.f, 0.f, width, height));
	
	pianoRoll->resetView(width, height);

	popup->updateWindow();


	borderView.reset(FloatRect(0.f, 0.f, width, height));
	borderView.setCenter((1280 - ((float)width / 2)), ((float)height / 2));




	instrEditor->resetView(width, height);

	songEditor->resetView(width, height);
}


bool equalsIgnoreCase(const string & a, const string & b)
{
	size_t size = a.size();

	if (size != b.size())
	{
		return false;
	}

	for (size_t i = 0; i < size; ++i)
	{
		if (tolower(a[i]) != tolower(b[i]))
		{
			return false;
		}
	}
	return true;
}


int checkExtension(string filename, string extension)
{
	return equalsIgnoreCase(filename.substr(filename.length() - extension.length()).c_str(), extension.c_str());
}



string forceExtension(string filename, string extension)
{
	if (equalsIgnoreCase(filename.substr(filename.length() - extension.length() - 1).c_str(), string("." + extension).c_str()))
	{
		return filename;
	}
	return filename + "." + extension;
}


void wrapValue(unsigned char *value, int append, int wrapLimit)
{

	int calc = toNumber<int>(std::to_string((int)*value) + std::to_string(append - 48));
	if (calc > wrapLimit)
		calc = append - 48;
	if (calc > wrapLimit)
		calc = wrapLimit;
	*value = calc;
}


string float2string(float f, int nd)
{
	ostringstream ostr;
	int tens = stoi("1" + string(nd, '0'));
	ostr << round(f*tens) / tens;
	return ostr.str();
}






void songModified(int _modified)
{

	if (!isSongModified && _modified)
	{
		window->setTitle(windowTitle + "*");
	}
	else if (isSongModified && !_modified)
	{
		window->setTitle(windowTitle);
	}
	isSongModified = _modified;
}

void setWindowTitle(string title)
{
	windowTitle = string("FM Composer - ") + title;
	window->setTitle(windowTitle);

}

#include <fstream>

void error(const std::string &text)
{
	fprintf(stderr, "%s", text.c_str());
	/* std::ofstream log_file(
		 "error.log", std::ios_base::out | std::ios_base::app );
		 log_file << text << "\n";*/
}

/* get directory from a complete file path */
std::string dirnameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/") + 1;
	return (std::string::npos == pos)
		? ""
		: fname.substr(0, pos);
}

sf::Color string2color(const char* s)
{
	char* copy = strdup(s);
	char * pch = strtok(copy, ",");

	int comp[4] = { atoi(pch), 0, 0, 255 };

	int count = 1;
	while (pch = strtok(NULL, " ,.-"))
	{
		comp[count] = atoi(pch);
		count++;
	}

	free(copy);

	// adapted from https://stackoverflow.com/questions/18398468/c-stl-convert-string-with-rgb-color-to-3-int-values
	return Color(comp[0], comp[1], comp[2], comp[3]);

}

static int patestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void *userData)
{
	char *out = (char*)outputBuffer;
	fm_render((fmsynth*)userData, &out[0], framesPerBuffer * 2, FM_RENDER_16);

	if (sidebar && songEditor)
	{
		for (unsigned i = 0; i < framesPerBuffer * 2; i += 16)
		{
			sidebar->vuMeter->setValue(((short*)out)[i], ((short*)out)[i + 1]);

			for (int ch = 0; ch < FM_ch; ch++)
			{

				songEditor->channelHead[ch].vu.setValue(fm->ch[ch].lastRender * !fm->ch[ch].muted);
			}
		}
	}


	return 0;
}


void global_initialize()
{




	/* Set config (preferences) directory */
#ifdef _WIN32
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath)))
	{
		appconfigdir = string(szPath) + pathSeparator + "fmcomposer" + pathSeparator;
		_mkdir(appconfigdir.c_str());
	}
	else
	{
		error("Can't get the application directory.");
	}

#else 
	appconfigdir = "~/.local/share"+pathSeparator+"fmcomposer"+pathSeparator;
	mkdir(appconfigdir.c_str(),0733);
#endif


	/* Load config files (preferences, last songs, midi instrument list..) */
	iniparams_load();

	/* Initialize libraries and sound engine */
	if (!(fm = fm_create(44100)))
	{
		error("Can't initialize the FM synthesizer");
	}

	if (Pm_Initialize() != pmNoError)
	{
		error("Can't initialize PortMidi");
	}

	midiReceiveEnable(1);

	PaError err;

	if ((err = Pa_Initialize()) != paNoError)
	{
		error(Pa_GetErrorText(err));
		error("Can't initialize PortAudio");
	}
	else
	{
		audioInitialized = true;
	}

	callbackFunc = (void*)&patestCallback;

	computeNoteNames();

}

void global_exit()
{
	Pa_CloseStream(stream);
	Pa_Terminate();
	Pm_Terminate();

	config->save();
}

#include <locale>
#include <codecvt>

string utf8_to_string(const char *utf8str, const locale& loc)
{
    // UTF-8 to wstring
    wstring_convert<codecvt_utf8<wchar_t>> wconv;
    wstring wstr = wconv.from_bytes(utf8str);
    // wstring to string
    vector<char> buf(wstr.size());
    use_facet<ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
    return string(buf.data(), buf.size());
}
