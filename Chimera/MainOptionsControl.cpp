
#include "stdafx.h"
#include "MainOptionsControl.h"

void MainOptionsControl::rearrange( int width, int height, fontMap fonts )
{
	header.rearrange( width, height, fonts );
	rearrangeButton.rearrange( width, height, fonts );
	randomizeRepetitionsButton.rearrange( width, height, fonts );
	randomizeVariationsButton.rearrange( width, height, fonts );
}


void MainOptionsControl::handleSaveConfig(std::ofstream& saveFile)
{
	saveFile << "MAIN_OPTIONS\n";
	saveFile << rearrangeButton.GetCheck() << "\n";
	saveFile << randomizeRepetitionsButton.GetCheck() << "\n";
	saveFile << randomizeVariationsButton.GetCheck() << "\n";
	saveFile << "END_MAIN_OPTIONS\n";
}


void MainOptionsControl::handleOpenConfig(std::ifstream& openFile, double version)
{
 	ProfileSystem::checkDelimiterLine(openFile, "MAIN_OPTIONS");
	openFile >> currentOptions.rearrange;
	rearrangeButton.SetCheck( currentOptions.rearrange );
	openFile >> currentOptions.randomizeRepetitions;
	randomizeRepetitionsButton.SetCheck( currentOptions.randomizeRepetitions );
	openFile >> currentOptions.randomizeVariations;
	randomizeVariationsButton.SetCheck( currentOptions.randomizeVariations );
 	ProfileSystem::checkDelimiterLine(openFile, "END_MAIN_OPTIONS");
}


void MainOptionsControl::initialize( int& id, POINT& loc, CWnd* parent, cToolTips& tooltips )
{
	header.sPos = { loc.x, loc.y, loc.x + 480, loc.y += 20 };
	header.Create( "MAIN OPTIONS", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_CENTER, header.sPos, parent, id++ );
	header.fontType = HeadingFont;
	//
	rearrangeButton.sPos = { loc.x, loc.y, loc.x + 480 , loc.y += 25 };
	rearrangeButton.Create( "Includes Rearrangement Procedure?", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHT,
							rearrangeButton.sPos, parent, id++ );
	randomizeRepetitionsButton.sPos = { loc.x, loc.y, loc.x + 480 , loc.y += 25 };
	randomizeRepetitionsButton.Create( "Randomize Repetitions?", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHT,
									   randomizeRepetitionsButton.sPos, parent, id++ );
	randomizeRepetitionsButton.EnableWindow( false );

	randomizeVariationsButton.sPos = { loc.x, loc.y, loc.x + 480 , loc.y += 25 };
	randomizeVariationsButton.Create( "Randomize Variations?", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_RIGHT,
									 randomizeVariationsButton.sPos, parent, id++ );
	//
	currentOptions.randomizeRepetitions = false;
	currentOptions.randomizeVariations = true;
	currentOptions.rearrange = false;	
}

bool MainOptionsControl::handleEvent(UINT id, MainWindow* comm)
{
	if (id == rearrangeButton.GetDlgCtrlID())
	{
		comm->updateConfigurationSavedStatus(false);
	}
	return TRUE;
}

mainOptions MainOptionsControl::getOptions()
{
	currentOptions.rearrange = rearrangeButton.GetCheck();
	currentOptions.randomizeRepetitions = randomizeRepetitionsButton.GetCheck();
	currentOptions.randomizeVariations = randomizeVariationsButton.GetCheck();
	return currentOptions;
}
