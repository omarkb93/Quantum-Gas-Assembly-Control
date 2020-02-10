// created by Mark O. Brown
#include "stdafx.h"
#include "Repetitions.h"
#include <unordered_map>
#include "LowLevel/constants.h"
#include "PrimaryWindows/AuxiliaryWindow.h"
#include <boost/lexical_cast.hpp>

void Repetitions::rearrange(UINT width, UINT height, fontMap fonts)
{
	repetitionEdit.rearrange(width, height, fonts);
	repetitionDisp.rearrange(width, height, fonts);
	repetitionText.rearrange(width, height, fonts);
}


UINT Repetitions::getRepsFromConfig ( std::ifstream& openFile, Version ver )
{
	UINT repNum;
	openFile >> repNum;
	return repNum;
}


void Repetitions::handleNewConfig( std::ofstream& newFile )
{
	newFile << "REPETITIONS\n";
	newFile << 100 << "\n";
	newFile << "END_REPETITIONS\n";
}

void Repetitions::handleSaveConfig(std::ofstream& saveFile)
{
	saveFile << "REPETITIONS\n";
	saveFile << getRepetitionNumber() << "\n";
	saveFile << "END_REPETITIONS\n";
}


HBRUSH Repetitions::handleColorMessage(CWnd* window, brushMap brushes, rgbMap rGBs, CDC* cDC)
{
	DWORD controlID = window->GetDlgCtrlID();
	if (controlID == repetitionDisp.GetDlgCtrlID() || controlID == repetitionEdit.GetDlgCtrlID())
	{
		cDC->SetBkColor(rGBs["Medium Grey"]);
		cDC->SetTextColor(rGBs["White"]);
		return *brushes["Medium Grey"];
	}
	else
	{
		return NULL;
	}

}

void Repetitions::updateNumber(long repNumber)
{
	repetitionDisp.SetWindowText(cstr(repNumber));
}


void Repetitions::initialize(POINT& pos, cToolTips& toolTips, CWnd* parent, int& id)
{
	repetitionNumber = 100;

	// title
	repetitionText.sPos = { pos.x, pos.y, pos.x + 180, pos.y + 20 };
	repetitionText.Create("Repetition #", NORM_STATIC_OPTIONS, repetitionText.sPos, parent, id++);
	
	repetitionEdit.sPos = { pos.x + 180, pos.y, pos.x + 330, pos.y + 20 };
	repetitionEdit.Create( NORM_EDIT_OPTIONS, repetitionEdit.sPos, parent, IDC_REPETITION_EDIT );
	repetitionEdit.SetWindowText(cstr(repetitionNumber));

	repetitionDisp.sPos = { pos.x + 330, pos.y, pos.x + 480, pos.y += 20 };
	repetitionDisp.Create( NORM_STATIC_OPTIONS | SS_SUNKEN , repetitionDisp.sPos,
						  parent, id++);
	repetitionDisp.SetWindowText("-");
}


void Repetitions::setRepetitions(UINT number)
{
	repetitionNumber = number;
	repetitionEdit.SetWindowTextA(cstr(number));
	repetitionDisp.SetWindowTextA("---");
}


UINT Repetitions::getRepetitionNumber()
{
	CString text;
	repetitionEdit.GetWindowText(text);
	try
	{
		repetitionNumber = boost::lexical_cast<int>(str(text));
	}
	catch ( boost::bad_lexical_cast&)
	{
		throwNested ("Failed to convert repetition number text to an integer!");
	}
	return repetitionNumber;
}
