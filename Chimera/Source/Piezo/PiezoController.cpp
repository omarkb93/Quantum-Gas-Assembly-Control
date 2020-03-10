﻿#include "stdafx.h"
#include "PiezoController.h"
#include <boost/lexical_cast.hpp>

PiezoController::PiezoController (piezoSetupInfo info) :
	core(info)
{

}

std::string PiezoController::getConfigDelim ( )
{
	return core.configDelim;
}

PiezoCore& PiezoController::getCore ( )
{
	return core;
}

void PiezoController::handleProgramNowPress ( )
{
	double xval, yval, zval;
	try
	{
		CString txt;
		edits.x.GetWindowTextA ( txt );
		xval = boost::lexical_cast<double>( txt );
		edits.y.GetWindowTextA ( txt );
		yval = boost::lexical_cast<double>( txt );
		edits.z.GetWindowTextA ( txt );
		zval = boost::lexical_cast<double>( txt );
	}
	catch( boost::bad_lexical_cast& )
	{
		thrower ( "ERROR: failed to convert one of the edit texts to a double!" );
	}
	core.programXNow ( xval );
	core.programYNow ( yval );
	core.programZNow ( zval );
	updateCurrentValues ( );
}

void PiezoController::updateCurrentValues ( )
{
	currentVals.x.SetWindowTextA ( cstr ( core.getCurrentXVolt ( ) ) );
	currentVals.y.SetWindowTextA ( cstr ( core.getCurrentYVolt ( ) ) );
	currentVals.z.SetWindowTextA ( cstr ( core.getCurrentZVolt ( ) ) );
}

void PiezoController::handleOpenConfig ( std::ifstream& configFile, Version ver )
{
	if ( ver > Version ( "4.5" ) )
	{
		auto configVals = core.getPiezoSettingsFromConfig ( configFile, ver );
		edits.x.SetWindowTextA ( configVals.first.x.c_str ( ) );
		edits.y.SetWindowTextA ( configVals.first.y.c_str ( ) );
		edits.z.SetWindowTextA ( configVals.first.z.c_str ( ) );
		ctrlButton.SetCheck ( configVals.second );
		updateCtrl ( );
	}
}

void PiezoController::handleSaveConfig ( std::ofstream& configFile )
{
	configFile << core.configDelim << "\n";
	CString txt;
	edits.x.GetWindowTextA ( txt );
	configFile << txt << "\n";
	edits.y.GetWindowTextA ( txt );
	configFile << txt << "\n";
	edits.z.GetWindowTextA ( txt );
	configFile << txt << "\n" << ctrlButton.GetCheck() << "\n";
	configFile << "END_" + core.configDelim << "\n";
}


std::string PiezoController::getDeviceInfo ( )
{
	return core.getDeviceInfo( );
}

std::string PiezoController::getPiezoDeviceList ( )
{
	return core.getDeviceList ( );
}

void PiezoController::updateCtrl ( )
{
	auto ctrl = ctrlButton.GetCheck ( );
	core.setCtrl ( ctrl );
	edits.x.EnableWindow ( ctrl );
	edits.y.EnableWindow ( ctrl );
	edits.z.EnableWindow ( ctrl );
}

void PiezoController::rearrange ( UINT width, UINT height, fontMap fonts )
{
	programNowButton.rearrange ( width, height, fonts );
	ctrlButton.rearrange ( width, height, fonts );
	edits.x.rearrange ( width, height, fonts );
	edits.y.rearrange ( width, height, fonts );
	edits.z.rearrange ( width, height, fonts );
	currentVals.x.rearrange ( width, height, fonts );
	currentVals.y.rearrange ( width, height, fonts );
	currentVals.z.rearrange ( width, height, fonts );
	labels.x.rearrange ( width, height, fonts );
	labels.y.rearrange ( width, height, fonts );
	labels.z.rearrange ( width, height, fonts );
}


void PiezoController::initialize ( POINT& pos, cToolTips& toolTips, CWnd* parent, int& id, LONG width, UINT programButtonID, 
								   piezoChan<std::string> names, UINT ctrlButtonID )
{
	core.initialize ( );
	programNowButton.sPos = {pos.x, pos.y, pos.x + 6*width/8, pos.y + 25};
	programNowButton.Create ( "Program Pzt Now", NORM_PUSH_OPTIONS, programNowButton.sPos, parent, 
							  programButtonID );
	ctrlButton.sPos = { pos.x + 6*width / 8, pos.y, pos.x + width, pos.y += 25 };
	ctrlButton.Create ( "Ctrl?", NORM_CHECK_OPTIONS, ctrlButton.sPos, parent, ctrlButtonID );
	ctrlButton.SetCheck ( true );

	labels.x.sPos = { pos.x, pos.y, pos.x + width / 3, pos.y + 20 };
	labels.x.Create ( names.x.c_str(), NORM_STATIC_OPTIONS | WS_BORDER, labels.x.sPos, parent, id++ );
	labels.y.sPos = { pos.x + width / 3, pos.y, pos.x + 2 * width / 3, pos.y + 20 };
	labels.y.Create ( names.y.c_str ( ), NORM_STATIC_OPTIONS | WS_BORDER, labels.y.sPos, parent, id++ );
	labels.z.sPos = { pos.x + 2 * width / 3, pos.y, pos.x + width, pos.y += 20 };
	labels.z.Create ( names.z.c_str ( ), NORM_STATIC_OPTIONS | WS_BORDER, labels.z.sPos, parent, id++ );

	edits.x.sPos = { pos.x, pos.y, pos.x + width/3, pos.y + 20 };
	edits.x.Create ( NORM_EDIT_OPTIONS | WS_BORDER, edits.x.sPos, parent, id++ );
	edits.y.sPos = { pos.x+ width/3, pos.y, pos.x + 2*width/3, pos.y + 20 };
	edits.y.Create ( NORM_EDIT_OPTIONS | WS_BORDER, edits.y.sPos, parent, id++ );
	edits.z.sPos = { pos.x+ 2*width/3, pos.y, pos.x + width, pos.y += 20 };
	edits.z.Create ( NORM_EDIT_OPTIONS | WS_BORDER, edits.z.sPos, parent, id++ );

	currentVals.x.sPos = { pos.x, pos.y, pos.x + width/3, pos.y + 20 };
	currentVals.x.Create ( "", NORM_STATIC_OPTIONS | WS_BORDER, currentVals.x.sPos, parent, id++ );
	currentVals.y.sPos = { pos.x + width/3, pos.y, pos.x + 2*width/3, pos.y + 20 };
	currentVals.y.Create ( "", NORM_STATIC_OPTIONS | WS_BORDER, currentVals.y.sPos, parent, id++ );
	currentVals.z.sPos = { pos.x + 2*width/3, pos.y, pos.x + width, pos.y += 20 };
	currentVals.z.Create ( "", NORM_STATIC_OPTIONS | WS_BORDER, currentVals.z.sPos, parent, id++ );
	updateCurrentValues ( );
}
