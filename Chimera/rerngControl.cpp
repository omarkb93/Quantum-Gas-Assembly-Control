﻿#include "stdafx.h"
#include "rerngControl.h"
#include "ProfileSystem.h"
#include "Thrower.h"


rerngGuiOptions rerngControl::getParams( )
{
	rerngGuiOptions tempParams;
	tempParams.active = experimentIncludesRerng.GetCheck( );
	tempParams.outputInfo = outputRearrangeEvents.GetCheck( );
	tempParams.outputIndv = outputIndividualEvents.GetCheck( );
	tempParams.preprogram = preprogramMoves.GetCheck( );
	tempParams.useCalibration = useCalibration.GetCheck( );
	tempParams.useFast = fastMoveOption.GetCheck( );
	CString tempTxt;
	try
	{
		flashingRateEdit.GetWindowTextA( tempTxt );
		// convert to Hz from MHz
		tempParams.flashingRate = 1e6 * std::stod( str( tempTxt ) );
		moveSpeedEdit.GetWindowTextA( tempTxt );
		// convert to s from ms
		tempParams.moveSpeed = 1e-3 * std::stod( str( tempTxt ) );
		movingBiasEdit.GetWindowTextA( tempTxt );
		tempParams.moveBias = std::stod( str( tempTxt ) );
		// convert from ns to s
		deadTimeEdit.GetWindowTextA( tempTxt );
		tempParams.deadTime = std::stod( str(tempTxt) ) * 1e-9;
		staticMovingRatioEdit.GetWindowTextA( tempTxt );
		tempParams.staticMovingRatio = std::stod( str( tempTxt ) );
		finalMoveTimeEdit.GetWindowTextA( tempTxt );
		tempParams.finalMoveTime = 1e-3 * std::stod( str(tempTxt) );
		fastMoveTimeEdit.GetWindowTextA( tempTxt );
		tempParams.fastMoveTime = 1e-6 * std::stod( str( tempTxt ) );
	}
	catch ( std::invalid_argument&)
	{
		thrower( "ERROR: Failed to convert rearrangement parameters to correct format! check that the inputs are the "
				 "correct types please." );
	}
	return tempParams;
}


void rerngControl::rearrange( int width, int height, fontMap fonts )
{
	preprogramMoves.rearrange( width, height, fonts );
	useCalibration.rearrange( width, height, fonts );
	header.rearrange(width, height, fonts);
	experimentIncludesRerng.rearrange( width, height, fonts );
	flashingRateText.rearrange( width, height, fonts );
	flashingRateEdit.rearrange( width, height, fonts );
	moveSpeedText.rearrange( width, height, fonts );
	moveSpeedEdit.rearrange( width, height, fonts );
	movingBiasText.rearrange( width, height, fonts );
	movingBiasEdit.rearrange( width, height, fonts );
	deadTimeText.rearrange( width, height, fonts );
	deadTimeEdit.rearrange( width, height, fonts );
	staticMovingRatioEdit.rearrange( width, height, fonts );
	staticMovingRatioText.rearrange( width, height, fonts );
	outputRearrangeEvents.rearrange( width, height, fonts );
	outputIndividualEvents.rearrange( width, height, fonts );
	finalMoveTimeText.rearrange( width, height, fonts );
	finalMoveTimeEdit.rearrange( width, height, fonts );
	fastMoveOption.rearrange( width, height, fonts );
	fastMoveTime.rearrange( width, height, fonts );
	fastMoveTimeEdit.rearrange( width, height, fonts );
}


void rerngControl::initialize( int& id, POINT& loc, CWnd* parent, cToolTips& tooltips )
{
	header.sPos = { loc.x, loc.y, loc.x + 480, loc.y += 25 };
	header.Create( "REARRANGEMENT OPTIONS", NORM_HEADER_OPTIONS, header.sPos, parent, id++ );
	header.fontType = fontTypes::HeadingFont;
	experimentIncludesRerng.sPos = { loc.x, loc.y, loc.x + 240, loc.y += 25 };
	experimentIncludesRerng.Create( "Experiment has Rerng?", NORM_CHECK_OPTIONS, 
									experimentIncludesRerng.sPos, parent, IDC_RERNG_EXPERIMENT_BUTTON );
	flashingRateText.sPos = { loc.x, loc.y, loc.x + 200, loc.y + 25 };
	flashingRateText.Create( "Flashing Rate (MHz)", NORM_STATIC_OPTIONS, flashingRateText.sPos, parent, id++ );
	flashingRateEdit.sPos = { loc.x + 200, loc.y, loc.x + 240, loc.y += 25 };
	flashingRateEdit.Create( NORM_EDIT_OPTIONS, flashingRateEdit.sPos, parent, id++ );
	flashingRateEdit.SetWindowTextA( "1" );
	moveSpeedText.sPos = { loc.x, loc.y, loc.x + 200, loc.y + 25 };
	moveSpeedText.Create( "Move Speed (ms)", NORM_STATIC_OPTIONS, moveSpeedText.sPos, parent, id++ );
	moveSpeedEdit.sPos = { loc.x + 200, loc.y, loc.x + 240, loc.y += 25 };
	moveSpeedEdit.Create( NORM_EDIT_OPTIONS | ES_AUTOHSCROLL, moveSpeedEdit.sPos, parent, id++ );
	moveSpeedEdit.SetWindowTextA( "0.06" );

	loc.y -= 75;
	loc.x += 240;

	movingBiasText.sPos = { loc.x, loc.y, loc.x + 200, loc.y + 25 };
	movingBiasText.Create( "Moving Tweezer Bias (/1)", NORM_STATIC_OPTIONS, movingBiasText.sPos, parent, id++ );
	movingBiasEdit.sPos = { loc.x + 200, loc.y, loc.x + 240, loc.y += 25 };
	movingBiasEdit.Create( NORM_EDIT_OPTIONS, movingBiasEdit.sPos, parent, id++ );
	movingBiasEdit.SetWindowTextA( "0.3" );

	deadTimeText.sPos = { loc.x, loc.y, loc.x + 200, loc.y + 25 };
	deadTimeText.Create( "Dead Time (ns)", NORM_STATIC_OPTIONS, deadTimeText.sPos, parent, id++ );
	deadTimeEdit.sPos = { loc.x + 200, loc.y, loc.x + 240, loc.y += 25 };
	deadTimeEdit.Create( NORM_EDIT_OPTIONS, deadTimeEdit.sPos, parent, id++ );
	deadTimeEdit.SetWindowTextA( "0" );
	staticMovingRatioText.sPos = { loc.x, loc.y, loc.x + 200, loc.y + 25 };
	staticMovingRatioText.Create( "Static/Moving Ratio", NORM_STATIC_OPTIONS, staticMovingRatioText.sPos, parent, id++ );
	staticMovingRatioEdit.sPos = { loc.x + 200, loc.y, loc.x + 240, loc.y += 25 };
	staticMovingRatioEdit.Create( NORM_EDIT_OPTIONS, staticMovingRatioEdit.sPos, parent, id++ );
	staticMovingRatioEdit.SetWindowTextA( "1" );
	loc.x -= 240;
	outputRearrangeEvents.sPos = { loc.x, loc.y, loc.x + 240, loc.y += 25 };
	outputRearrangeEvents.Create( "Output Event Info", NORM_CHECK_OPTIONS, outputRearrangeEvents.sPos, parent, id++ );
	outputIndividualEvents.sPos = { loc.x, loc.y, loc.x + 240, loc.y += 25 };
	outputIndividualEvents.Create( "Output Individual Event Info?", NORM_CHECK_OPTIONS, outputIndividualEvents.sPos, 
								   parent, id++ );
	loc.y -= 50;
	preprogramMoves.sPos = { loc.x + 240, loc.y, loc.x + 480, loc.y += 25 };
	preprogramMoves.Create( "Preprogram Moves?", NORM_CHECK_OPTIONS, preprogramMoves.sPos, parent, id++ );
	useCalibration.sPos = { loc.x + 240, loc.y, loc.x + 480, loc.y += 25 };
	useCalibration.Create( "Use Calibration?", NORM_CHECK_OPTIONS, useCalibration.sPos, parent, id++ );

	finalMoveTimeText.sPos = { loc.x, loc.y, loc.x + 240, loc.y + 25 };
	finalMoveTimeText.Create( "Final-Move-Time (ms): ", NORM_STATIC_OPTIONS, finalMoveTimeText.sPos, parent, id++ );
	finalMoveTimeEdit.sPos = { loc.x + 240, loc.y, loc.x + 480, loc.y += 25 };
	finalMoveTimeEdit.Create( NORM_EDIT_OPTIONS, finalMoveTimeEdit.sPos, parent, id++ );
	finalMoveTimeEdit.SetWindowTextA( "1" );

	fastMoveOption.sPos = { loc.x, loc.y, loc.x + 480, loc.y += 25 };
	fastMoveOption.Create( "FAST-MOVE", NORM_CHECK_OPTIONS, fastMoveOption.sPos, parent, id++ );

	fastMoveTime.sPos = { loc.x+240, loc.y, loc.x + 440, loc.y + 25 };
	fastMoveTime.Create( "Fast-Move (us):", NORM_STATIC_OPTIONS, fastMoveTime.sPos, parent, id++ );
	fastMoveTimeEdit.sPos = { loc.x + 440, loc.y, loc.x + 480, loc.y += 25 };
	fastMoveTimeEdit.Create( NORM_EDIT_OPTIONS, fastMoveTimeEdit.sPos, parent, id++ );
	fastMoveTimeEdit.SetWindowTextA( "2" );
	handleCheck( );
}


void rerngControl::handleCheck( )
{
	auto active = experimentIncludesRerng.GetCheck( );
	flashingRateEdit.EnableWindow(active);
	moveSpeedEdit.EnableWindow( active );
	movingBiasEdit.EnableWindow( active );
	deadTimeEdit.EnableWindow( active );
	staticMovingRatioEdit.EnableWindow( active );
	preprogramMoves.EnableWindow( active );
	useCalibration.EnableWindow( active );
	outputRearrangeEvents.EnableWindow( active );
	outputIndividualEvents.EnableWindow( active );
	finalMoveTimeEdit.EnableWindow( active );
	fastMoveOption.EnableWindow( active );
	fastMoveTimeEdit.EnableWindow( active );
}


void rerngControl::handleOpenConfig( std::ifstream& openFile, Version ver )
{
	if (ver < Version("2.1" ) )
	{
		return;
	}
	ProfileSystem::checkDelimiterLine( openFile, "REARRANGEMENT_INFORMATION" );
	rerngGuiOptions info;
	openFile >> info.active;
	openFile >> info.flashingRate;
	openFile >> info.moveBias;
	openFile >> info.moveSpeed;
	if (ver < Version("2.3"))
	{
		std::string garbage;
 		openFile >> garbage;
		openFile >> garbage;
	}
	if (ver > Version("2.2"))
	{
		openFile >> info.deadTime;
		openFile >> info.staticMovingRatio;		
	}
	else
	{
		info.deadTime = 0;
		info.staticMovingRatio = 1;
	}
	if (ver > Version("2.5") )
	{
		openFile >> info.outputInfo;
	}
	else
	{
		info.outputInfo = false;
	}
	if (ver > Version("2.10" ) )
	{
		openFile >> info.outputIndv;
	}
	else
	{
		info.outputIndv = false;
	}
	if (ver > Version("2.11" ) )
	{
		openFile >> info.preprogram;
		openFile >> info.useCalibration;
	}
	else
	{
		info.preprogram = false;
		info.useCalibration = false;
	}
	if (ver > Version("2.12") )
	{
		openFile >> info.finalMoveTime;
	}
	else
	{
		info.finalMoveTime = 1e-3;
	}
	if (ver > Version("3.1"))
	{
		openFile >> info.useFast;
	}
	else
	{
		info.useFast = false;
	}
	if (ver > Version("3.1" ) )
	{
		openFile >> info.fastMoveTime;
	}
	else
	{
		info.fastMoveTime = 1e-6;
	}

	setParams( info );
	ProfileSystem::checkDelimiterLine( openFile, "END_REARRANGEMENT_INFORMATION" );
}


void rerngControl::handleNewConfig( std::ofstream& newFile )
{
	newFile << "REARRANGEMENT_INFORMATION\n";
	newFile << 0 << "\n";
	newFile << 1 << "\n";
	// move bias
	newFile << 1e-3*0.3 << "\n";
	// move speed
	newFile << 1e-6 * 60 << "\n";
	// dead time
	newFile << 75e-9 << "\n";
	newFile << "1\n";
	newFile << "0\n";
	newFile << "0\n";
	// preprogram
	newFile << "0\n";
	// use calibration
	newFile << "0\n";
	newFile << "0.001\n";
	// usefast, fastmovetime, timebetweenfastmoves
	newFile << "0\n0\n";
	newFile << "END_REARRANGEMENT_INFORMATION\n";
}


void rerngControl::handleSaveConfig( std::ofstream& saveFile )
{
 	saveFile << "REARRANGEMENT_INFORMATION\n";
	// conversions happen in getParams.
	rerngGuiOptions info = getParams( );
 	saveFile << info.active << "\n";
 	saveFile << info.flashingRate << "\n";
 	saveFile << info.moveBias << "\n";
	saveFile << info.moveSpeed << "\n";
	saveFile << info.deadTime << "\n";
	saveFile << info.staticMovingRatio << "\n";
	saveFile << info.outputInfo << "\n";
	saveFile << info.outputIndv << "\n";
	saveFile << info.preprogram << "\n";
	saveFile << info.useCalibration << "\n" << info.finalMoveTime << "\n" << info.useFast << "\n";
	saveFile << info.fastMoveTime << "\n";
	saveFile << "END_REARRANGEMENT_INFORMATION\n";
}


void rerngControl::setParams( rerngGuiOptions params )
{
	experimentIncludesRerng.SetCheck( params.active );
	// convert back to MHz from Hz
	flashingRateEdit.SetWindowTextA( cstr(1e-6*params.flashingRate) );
	movingBiasEdit.SetWindowTextA( cstr( params.moveBias ) );
	// convert back to ms from s
	moveSpeedEdit.SetWindowTextA( cstr( 1e3*params.moveSpeed ) );
	outputRearrangeEvents.SetCheck( params.outputInfo );
	// convert back to ns
	deadTimeEdit.SetWindowTextA( cstr( params.deadTime * 1e9) );
	staticMovingRatioEdit.SetWindowTextA( cstr( params.staticMovingRatio ) );
	
	outputRearrangeEvents.SetCheck( params.outputInfo );
	outputIndividualEvents.SetCheck( params.outputIndv );

	useCalibration.SetCheck( params.useCalibration );
	preprogramMoves.SetCheck( params.preprogram );
	// convert back to ms
	finalMoveTimeEdit.SetWindowTextA( cstr( params.finalMoveTime * 1e3 ) );

	fastMoveOption.SetCheck( params.useFast );
	// these are displayed in us.
	fastMoveTimeEdit.SetWindowTextA( cstr( params.fastMoveTime*1e6 ) );
}
