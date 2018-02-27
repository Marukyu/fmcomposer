#include "instrEditor.hpp"
#include "../../libs/tinyfiledialogs/tinyfiledialogs.h"


void InstrEditor::instrument_load(const char *filename)
{
	if (loadInstrument(filename, instrList->value))
	{
		updateFromFM();
		updateInstrListFromFM();
		updateToFM();
		valueChanged = 1;
		addToUndoHistory();
	}
}
void InstrEditor::instrument_open()
{
	mouse.clickLock2 = 5;
	static const char * filters[4] = { "*.fmci" };
	const char *fileName = tinyfd_openFileDialog("Load an instrument", instrDir.c_str(), 1, filters, "FMComposer instrument", false);
	if (fileName)
	{
		instrDir = dirnameOf(fileName);
		instrument_load(fileName);
	}
}

void InstrEditor::instrument_save()
{
	mouse.clickLock2 = 5;
	static const char * filters[4] = { "*.fmci" };
	const char *fileName = tinyfd_saveFileDialog("Save an instrument", instrDir.c_str(), 1, filters, "FMComposer instrument");
	if (fileName)
	{
		instrDir = dirnameOf(fileName);
		string fileNameOk = forceExtension(fileName, "fmci");
		fm_saveInstrument(fm, fileNameOk.c_str(), instrList->value);
	}
}

int InstrEditor::loadInstrument(string filename, int slot)
{

	int result = fm_loadInstrument(fm, filename.c_str(), slot);

	if (result == 0)
		return 1;

	if (result == FM_ERR_FILEIO)
	{
		popup->show(POPUP_OPENFAILED);
	}
	else if (result == FM_ERR_FILEVERSION)
	{
		popup->show(POPUP_WRONGVERSION);
	}
	else if (result == FM_ERR_FILECORRUPTED)
	{
		popup->show(POPUP_FILECORRUPTED);
	}
	return 0;
}