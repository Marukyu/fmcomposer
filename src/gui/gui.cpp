#include "gui.hpp"
#include "list/list.hpp"
#include "contextmenu/contextmenu.hpp"

#define clamp(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))



sf::Cursor cursors[5];


extern Image icon;
extern List* instrList;
Color colors[MAX_THEMECOLORS];

Font font;
Font font_condensed;
Font font_symbols;
int ROW_HEIGHT;
int CH_WIDTH;
int COL_WIDTH;
int charSize = 14;

List *instrList;


ListMenu *recentSongs;
ListMenu *copyPasta;


void loadColors(CSimpleIniA* reader)
{
	const char *elems[MAX_THEMECOLORS] = {
		"background", "sidebarbg", "menubg", "blockbg", "sliderbg", "slideroutline", "slidervalue", "title", "buttonbgA", "buttonbgB", "buttontext",
		"textinput", "textinputtext", "textinputoutline", "focusoutline", "buttontoggledbg", "buttontoggledtext", "textinputcursor",
		"textinputbghover", "textinputbgfocus", "textinputoutlinefocus", "textinputoutlinehover", "sliderbar", "sliderbarhover",
		"vumeterbarlow", "vumeterbarmedium", "vumeterbarhigh", "vumeterbg", "vumetertext", "vumetertextsaturated", "vumetertopbar", "patternnote", "patterninstrument", "patternvolume", "patterneffect", "vumeterlines",
		"operatorbg", "operatorbghover", "operatortext", "operatortexthover", "operatoroutline", "operatorlinks", "operatorbgmuted", "patternrownumbers", "patternselection",
		"moveselectionoutline", "slidertitle", "sliderbghover", "blocktext", "listitembgfocus", "listitemtext", "listitemtextfocus", "blockbghover",
		"scrollbarbg", "scrollbarcursor", "scrollbarcursorhover", "scrollbarcursorclicked", "patternbg", "patternbgodd", "patterncolumseparator", "buttonbghoverA", "buttonbghoverB",
		"contextmenubg", "contextmenubghover", "contextmenutext", "contextmenutexthover", "buttontoggledbghover", "menuitemhover", "menuitemselect", "buttonbgclickedA", "buttonbgclickedB"
		, "operatorvumeter", "listbg", "popupshadow", "topbarbg", "scrollbarbghover", "waveformoffsetbar", "listoutline", "subtitle" };


	for (int i = 0; i < MAX_THEMECOLORS; i++)
	{
		colors[i] = string2color(reader->GetValue("theme", elems[i], "0,0,0"));
	}
}

void showInstrumentLights()
{
	/* Update instrument playing indicators in instrument list */

	for (int i = 0; i < instrList->pings.size(); i++)
	{
		instrList->pings[i] = 0;
	}
	for (int i = 0; i < FM_ch; i++)
	{
		if (fm->ch[i].active && fm->ch[i].instrNumber < instrList->pings.size())
			instrList->pings[fm->ch[i].instrNumber] = 1;
	}
}

void updateMouseCursor()
{
	static int oldMouseCursor = -1;
	if (mouse.cursor != oldMouseCursor)
	{
		oldMouseCursor = mouse.cursor;
		window->setMouseCursor(cursors[mouse.cursor]);
	}

}

void gui_initialize()
{

	cursors[0].loadFromSystem(sf::Cursor::Arrow);
	cursors[1].loadFromSystem(sf::Cursor::Hand);
	cursors[2].loadFromSystem(sf::Cursor::Text);
	cursors[3].loadFromSystem(sf::Cursor::SizeAll);
	cursors[4].loadFromSystem(sf::Cursor::SizeHorizontal);

	sf::ContextSettings s;

	s.antialiasingLevel=16;

	window = new RenderWindow(VideoMode(WindowWidth, WindowHeight), "FM Composer - New song",7U,s);

	
#ifdef _WIN32
	/* Get display DPI for scaling appropriately. Thx Saga Musix for this code */
	HDC dc = ::GetDC(window->getSystemHandle());
	windowDpi = ::GetDeviceCaps(dc, LOGPIXELSY);
	ReleaseDC(window->getSystemHandle(), dc);	printf("dpi %d\n",windowDpi);
#endif

	window->setVerticalSyncEnabled(true);

	/* Load font */
	if (!font.loadFromFile(string(appdir + "themes" + pathSeparator + "roboto.ttf").c_str()))
	{
		error("Can't load font file");
	}
	if (!font_condensed.loadFromFile(string(appdir + "themes" + pathSeparator + "roboto_condensed.ttf").c_str()))
	{
		error("Can't load font file");
	}
	if (!font_symbols.loadFromFile(string(appdir + "themes" + pathSeparator + "materialicons.ttf").c_str()))
	{
		error("Can't load font file");
	}

	
	/* Load icons */
	extern CSimpleIniA ini_theme;
	Image _tileset;

	if (!_tileset.loadFromFile(string(appdir + "themes" + pathSeparator + ini_theme.GetValue("theme", "iconset", "ui.png")).c_str()))
	{
		error("Can't load GUI icons");
	}

	tileset = new Texture();
	tileset->setSmooth(false);
	tileset->loadFromImage(_tileset);

	/* Set window icon */
	Image icon;
	icon.create(32, 32);
	icon.copy(_tileset, 0, 0, { 228, 64, 32, 32 });
	window->setIcon(32, 32, icon.getPixelsPtr());

	ROW_HEIGHT = charSize * 1.2;
	CH_WIDTH = 100;
	COL_WIDTH = CH_WIDTH / 4;

	recentSongs = new ListMenu();
	copyPasta = new ListMenu();

	copyPasta->add("Copy");
	copyPasta->add("Paste");

}

GuiElement::GuiElement()
{
	visible = true;
}

void GuiElement::setVisible(bool _visible)
{
	visible = _visible;
}
