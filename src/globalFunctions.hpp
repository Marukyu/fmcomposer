#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H
#include <string>

#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include "fmengine/fmlib.h"
#include <portaudio.h>

#define WindowWidth 1366
#define WindowHeight 768
#define VERSION "1.7"
#define VERSION_DATE "2018-05-11"


template <typename T, typename T2>
constexpr T min(T x, T2 y)
{
	return x < y ? x : y;
}

template <typename T, typename T2>
constexpr T max(T x, T2 y)
{
	return x > y ? x : y;
}

template <typename T, typename T2, typename T3>
constexpr T clamp(T x, T2 low, T3 high)
{
	return x > high ? high : (x < low ? low : x);
}


using namespace std;
using namespace sf;

enum cursors{ CURSOR_NORMAL, CURSOR_HAND, CURSOR_TEXT, CURSOR_SWAP, CURSOR_HRESIZE };


// fast way to convert int (0-255 range) to strings
const string int2str[257] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100", "101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120", "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142", "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156", "157", "158", "159", "160", "161", "162", "163", "164", "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179", "180", "181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192", "193", "194", "195", "196", "197", "198", "199", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236", "237", "238", "239", "240", "241", "242", "243", "244", "245", "246", "247", "248", "249", "250", "251", "252", "253", "254", "255", "256" };
extern int windowFocus;
extern Event evt;
extern Uint32 textEntered[32];
extern void* callbackFunc;

extern View patternView, globalView, borderView, patternTopView, patNumView, instrView, pianorollView;
extern int isSongModified;
extern string lastSongOpened;
extern string songLoadedRequest;
extern float frameTime60;
extern string appdir, appconfigdir, instrDir, songDir;
extern string intervals[25];
extern RenderWindow *window;
extern fmsynth *fm;
extern sf::Clock mclock;
extern sf::WindowHandle windowHandle;
extern PaStream *stream;
extern PaStreamParameters out;
extern string pathSeparator;
extern bool audioInitialized;
extern string kfx_globalParams[8];
extern string kfx_operatorParams[32];
extern int windowWidth, windowHeight, windowDpi;
extern bool windowTooSmall;

extern int copiedSliderValue, copiedSliderValueOk;

void wrapValue(unsigned char *value, int append, int wrapLimit);

int keyboard2note(int keyCode); // convert a key code into note value

template<typename T>
T toNumber(const string& numberAsString)
{
	T valor;
	stringstream stream(numberAsString);
	stream >> valor;
	return valor;
}



void updateViews(int width, int height);

string float2string(float f, int nd);

bool equalsIgnoreCase(const string & a, const string & b);

int checkExtension(string filename, string extension);

void songModified(int modified);

void setWindowTitle(string title);

string forceExtension(string filename, string extension);

void song_playPause();

void song_pause();
void song_stop();
void song_play();

void error(const std::string &text);

/* Pre-compute note names (C-5,..) for every midi note number */
void computeNoteNames();

std::string dirnameOf(const std::string& fname);

sf::Color string2color(const char* s);

void global_initialize();

void global_exit();

string& noteName(int note);

int isWindowMaximized();

string utf8_to_string(const char *utf8str, const locale& loc);

#endif
