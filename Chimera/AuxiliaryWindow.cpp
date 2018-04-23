#include "stdafx.h"
#include "AuxiliaryWindow.h"
#include "DioSettingsDialog.h"
#include "AoSettingsDialog.h"
#include "TextPromptDialog.h"
#include "DioSystem.h"
#include "CameraWindow.h"
#include "MainWindow.h"
#include "Control.h"
#include "commonFunctions.h"
#include "openWithExplorer.h"
#include "saveWithExplorer.h"
#include "agilentStructures.h"
#include "Thrower.h"
#include "range.h"

AuxiliaryWindow::AuxiliaryWindow() : CDialog(), 
									 topBottomTek(TOP_BOTTOM_TEK_SAFEMODE, TOP_BOTTOM_TEK_USB_ADDRESS), 
									 eoAxialTek(EO_AXIAL_TEK_SAFEMODE, EO_AXIAL_TEK_USB_ADDRESS),
									 agilents{ TOP_BOTTOM_AGILENT_SETTINGS, AXIAL_AGILENT_SETTINGS,
												FLASHING_AGILENT_SETTINGS, UWAVE_AGILENT_SETTINGS },
									ttlBoard( true, true ),
									aoSys( ANALOG_OUT_SAFEMODE )
{}

BOOL AuxiliaryWindow::handleAccelerators( HACCEL m_haccel, LPMSG lpMsg )
{
	return globalVariables.handleAccelerators( m_haccel, lpMsg );
}




IMPLEMENT_DYNAMIC( AuxiliaryWindow, CDialog )


BEGIN_MESSAGE_MAP( AuxiliaryWindow, CDialog )
	ON_WM_TIMER( )

	ON_WM_CTLCOLOR( )
	ON_WM_SIZE( )

	ON_COMMAND_RANGE( MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &AuxiliaryWindow::passCommonCommand )
	ON_COMMAND_RANGE( TTL_ID_BEGIN, TTL_ID_END, &AuxiliaryWindow::handleTtlPush )

	ON_COMMAND( TTL_HOLD, &handlTtlHoldPush )
	ON_COMMAND( ID_DAC_SET_BUTTON, &SetDacs )
	ON_COMMAND( IDC_ZERO_TTLS, &zeroTtls )
	ON_COMMAND( IDC_ZERO_DACS, &zeroDacs )
	ON_COMMAND( IDOK, &handleEnter )
	ON_COMMAND( TOP_BOTTOM_PROGRAM, &passTopBottomTekProgram )
	ON_COMMAND( EO_AXIAL_PROGRAM, &passEoAxialTekProgram )
	ON_COMMAND( ID_GET_ANALOG_IN_VALUES, &GetAnalogInSnapshot )

	ON_COMMAND_RANGE( IDC_TOP_BOTTOM_CHANNEL1_BUTTON, IDC_UWAVE_PROGRAM, &AuxiliaryWindow::handleAgilentOptions )
	ON_COMMAND_RANGE( TOP_ON_OFF, AXIAL_FSK, &AuxiliaryWindow::handleTektronicsButtons )
	
	ON_CONTROL_RANGE( CBN_SELENDOK, IDC_TOP_BOTTOM_AGILENT_COMBO, IDC_TOP_BOTTOM_AGILENT_COMBO, 
					  &AuxiliaryWindow::handleAgilentCombo )
	ON_CONTROL_RANGE( CBN_SELENDOK, IDC_AXIAL_AGILENT_COMBO, IDC_AXIAL_AGILENT_COMBO, 
					  &AuxiliaryWindow::handleAgilentCombo )
	ON_CONTROL_RANGE( CBN_SELENDOK, IDC_FLASHING_AGILENT_COMBO, IDC_FLASHING_AGILENT_COMBO, 
					  &AuxiliaryWindow::handleAgilentCombo )
	ON_CONTROL_RANGE( CBN_SELENDOK, IDC_UWAVE_AGILENT_COMBO, IDC_UWAVE_AGILENT_COMBO, 
					  &AuxiliaryWindow::handleAgilentCombo )
	ON_REGISTERED_MESSAGE( eLogVoltsMessageID, &AuxiliaryWindow::onLogVoltsMessage )

	ON_CONTROL_RANGE( EN_CHANGE, ID_DAC_FIRST_EDIT, (ID_DAC_FIRST_EDIT + 23), &AuxiliaryWindow::DacEditChange )
	ON_NOTIFY( LVN_COLUMNCLICK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsColumnClick )
	ON_NOTIFY( NM_DBLCLK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsDblClick )
	ON_NOTIFY( NM_RCLICK, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::ConfigVarsRClick )
	ON_NOTIFY( NM_DBLCLK, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::GlobalVarDblClick )
	ON_NOTIFY( NM_RCLICK, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::GlobalVarRClick )

	ON_NOTIFY_RANGE( NM_CUSTOMDRAW, IDC_GLOBAL_VARS_LISTVIEW, IDC_GLOBAL_VARS_LISTVIEW, &AuxiliaryWindow::drawVariables )
	ON_NOTIFY_RANGE( NM_CUSTOMDRAW, IDC_CONFIG_VARS_LISTVIEW, IDC_CONFIG_VARS_LISTVIEW, &AuxiliaryWindow::drawVariables )
	
	ON_CONTROL_RANGE( EN_CHANGE, IDC_TOP_BOTTOM_EDIT, IDC_TOP_BOTTOM_EDIT, &AuxiliaryWindow::handleAgilentEditChange )
	ON_CONTROL_RANGE( EN_CHANGE, IDC_FLASHING_EDIT, IDC_FLASHING_EDIT, &AuxiliaryWindow::handleAgilentEditChange )
	ON_CONTROL_RANGE( EN_CHANGE, IDC_AXIAL_EDIT, IDC_AXIAL_EDIT, &AuxiliaryWindow::handleAgilentEditChange )
	ON_CONTROL_RANGE( EN_CHANGE, IDC_UWAVE_EDIT, IDC_UWAVE_EDIT, &AuxiliaryWindow::handleAgilentEditChange )
	ON_WM_RBUTTONUP( )
	ON_WM_LBUTTONUP( )
	ON_WM_TIMER( )
	ON_WM_PAINT( )
END_MESSAGE_MAP()


LRESULT AuxiliaryWindow::onLogVoltsMessage( WPARAM wp, LPARAM lp )
{
	aiSys.refreshCurrentValues( );
	aiSys.refreshDisplays( );
	cameraWindowFriend->writeVolts( wp, aiSys.getCurrentValues() );

	return TRUE;
}


void AuxiliaryWindow::GetAnalogInSnapshot( )
{
	aiSys.refreshCurrentValues( );
	aiSys.refreshDisplays( );
}


void AuxiliaryWindow::OnPaint( )
{
	CDialog::OnPaint( );
	if ( !mainWindowFriend->masterIsRunning( ) )
	{
		CRect size;
		GetClientRect( &size );
		CDC* cdc = GetDC( );
		// for some reason I suddenly started needing to do this. I know that memDC redraws the background, but it used to 
		// work without this and I don't know what changed. I used to do:
		cdc->SetBkColor( mainWindowFriend->getRgbs( )["Solarized Base 04"] );
		long width = size.right - size.left, height = size.bottom - size.top;
		// each dc gets initialized with the rect for the corresponding plot. That way, each dc only overwrites the area 
		// for a single plot.
		for ( auto& ttlPlt : ttlPlots )
		{
			ttlPlt->setCurrentDims( width, height );
			memDC ttlDC( cdc, &ttlPlt->GetPlotRect(  ) );
			ttlPlt->drawBackground( ttlDC, mainWindowFriend->getBrushes( )["Solarized Base04"],
									mainWindowFriend->getBrushes( )["Black"] );
			ttlPlt->drawTitle( ttlDC );
			ttlPlt->drawBorder( ttlDC );
			ttlPlt->plotPoints( &ttlDC );
		}
		for ( auto& dacPlt : aoPlots )
		{
			dacPlt->setCurrentDims( width, height );
			memDC dacDC( cdc, &dacPlt->GetPlotRect( ) );
			dacPlt->drawBackground( dacDC, mainWindowFriend->getBrushes( )["Solarized Base04"],
									mainWindowFriend->getBrushes( )["Black"] );
			dacPlt->drawTitle( dacDC );
			dacPlt->drawBorder( dacDC );
			dacPlt->plotPoints( &dacDC );
		}
		ReleaseDC( cdc );
	}
}


void AuxiliaryWindow::OnRButtonUp( UINT stuff, CPoint clickLocation )
{
	cameraWindowFriend->stopSound( );
}

void AuxiliaryWindow::OnLButtonUp( UINT stuff, CPoint clickLocation )
{
	cameraWindowFriend->stopSound( );
}


void AuxiliaryWindow::newAgilentScript( whichAg::agilentNames name)
{
	try
	{
		if ( !agilents[name].scriptingModeIsSelected( ) )
		{
			thrower( "Error: please set current agilent channel to scripting mode before attempting to create a script!" );
		}
		mainWindowFriend->updateConfigurationSavedStatus( false );
		agilents[name].checkSave( mainWindowFriend->getProfileSettings( ).categoryPath, mainWindowFriend->getRunInfo( ) );
		agilents[name].agilentScript.newScript( );
		agilents[name].agilentScript.updateScriptNameText( mainWindowFriend->getProfileSettings( ).categoryPath );
		agilents[name].agilentScript.colorEntireScript( getAllVariables( ), mainWindowFriend->getRgbs( ),
														  getTtlNames( ), getDacNames( ) );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}

}


void AuxiliaryWindow::openAgilentScript( whichAg::agilentNames name, CWnd* parent)
{
	try
	{
		if ( !agilents[name].scriptingModeIsSelected( ) )
		{
			thrower( "Error: please set current agilent channel to scripting mode before attempting to open a script!" );
		}
		mainWindowFriend->updateConfigurationSavedStatus( false );		
		agilents[name].agilentScript.checkSave( mainWindowFriend->getProfileSettings( ).categoryPath, 
												mainWindowFriend->getRunInfo( ) );
		std::string openFileName = openWithExplorer( parent, AGILENT_SCRIPT_EXTENSION );
		agilents[name].agilentScript.openParentScript( openFileName, 
													   mainWindowFriend->getProfileSettings( ).categoryPath,
													   mainWindowFriend->getRunInfo( ) );
		agilents[name].agilentScript.updateScriptNameText( mainWindowFriend->getProfileSettings( ).categoryPath );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}
}


void AuxiliaryWindow::updateAgilent( whichAg::agilentNames name )
{
	try
	{
		agilents[name].handleInput( mainWindowFriend->getProfileSettings( ).categoryPath,
									mainWindowFriend->getRunInfo( ) );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}
}


void AuxiliaryWindow::saveAgilentScript( whichAg::agilentNames name )
{
	try
	{
		if ( !agilents[name].scriptingModeIsSelected( ) )
		{
			thrower( "Error: please set current agilent channel to scripting mode before attempting to save script!" );
		}
		mainWindowFriend->updateConfigurationSavedStatus( false );
		agilents[name].agilentScript.saveScript( mainWindowFriend->getProfileSettings( ).categoryPath,
												   mainWindowFriend->getRunInfo( ) );
		agilents[name].agilentScript.updateScriptNameText( mainWindowFriend->getProfileSettings( ).categoryPath );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}
}


void AuxiliaryWindow::saveAgilentScriptAs( whichAg::agilentNames name, CWnd* parent )
{
	try
	{
		if ( !agilents[name].scriptingModeIsSelected( ) )
		{
			thrower( "Error: please set current agilent channel to scripting mode before attempting to save script!" );
		}
		mainWindowFriend->updateConfigurationSavedStatus( false );
		std::string extensionNoPeriod = agilents[name].agilentScript.getExtension( );
		if ( extensionNoPeriod.size( ) == 0 )
		{
			return;
		}
		extensionNoPeriod = extensionNoPeriod.substr( 1, extensionNoPeriod.size( ) );
		std::string newScriptAddress = saveWithExplorer( parent, extensionNoPeriod,
														 mainWindowFriend->getProfileSettings( ) );
		agilents[name].agilentScript.saveScriptAs( newScriptAddress, mainWindowFriend->getRunInfo( ) );
		agilents[name].agilentScript.updateScriptNameText( mainWindowFriend->getProfileSettings( ).categoryPath );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}
}


void AuxiliaryWindow::OnTimer( UINT_PTR eventID )
{
	if ( eventID == 2 )
	{
		// don't query while experiment is running and getting querying between variations, this may cause a 
		// race condition.
		if ( aiSys.wantsContinuousQuery( ) && (!mainWindowFriend->masterIsRunning( ) 
												|| !aiSys.wantsQueryBetweenVariations() ) )
		{
			GetAnalogInSnapshot( );
		}
	}
	if ( eventID == 1 )
	{
		OnPaint( );
	}
	else
	{
		for ( auto& agilent : agilents )
		{
			agilent.agilentScript.handleTimerCall( getAllVariables( ), mainWindowFriend->getRgbs( ),
												   getTtlNames( ), getDacNames( ) );
		}
	}
}


Agilent& AuxiliaryWindow::whichAgilent( UINT id )
{
	if ( id >= IDC_TOP_BOTTOM_CHANNEL1_BUTTON && id <= IDC_TOP_BOTTOM_PROGRAM 
		 || id == IDC_TOP_BOTTOM_CALIBRATION_BUTTON )
	{
		return agilents[whichAg::TopBottom];
	}
	else if ( id >= IDC_AXIAL_CHANNEL1_BUTTON && id <= IDC_AXIAL_PROGRAM
			  || id == IDC_AXIAL_CALIBRATION_BUTTON )
	{
		return agilents[whichAg::Axial];
	}
	else if ( id >= IDC_FLASHING_CHANNEL1_BUTTON && id <= IDC_FLASHING_PROGRAM
			  || id == IDC_FLASHING_CALIBRATION_BUTTON )
	{
		return agilents[whichAg::Flashing];
	}
	else if ( id >= IDC_UWAVE_CHANNEL1_BUTTON && id <= IDC_UWAVE_PROGRAM
			  || id == IDC_UWAVE_CALIBRATION_BUTTON )
	{
		return agilents[whichAg::Microwave];
	}
	thrower( "ERROR: id seen in \"whichAgilent\" handler does not belong to any agilent!" );
}


void AuxiliaryWindow::handleAgilentEditChange( UINT id )
{
	Agilent& agilent = whichAgilent( id );
	try
	{
		agilent.agilentScript.handleEditChange( );
		SetTimer( SYNTAX_TIMER_ID, SYNTAX_TIMER_LENGTH, NULL );
	}
	catch ( Error& err )
	{
		sendErr( err.what( ) );
	}
}


void AuxiliaryWindow::passTopBottomTekProgram()
{
	try
	{
		topBottomTek.handleProgram();
		sendStatus( "Programmed Top/Bottom Tektronics Generator.\r\n" );
	}
	catch (Error& exception)
	{
		sendErr( "Error while programing top/Bottom Tektronics generator: " + exception.whatStr() + "\r\n" );
	}
}


void AuxiliaryWindow::passEoAxialTekProgram()
{
	try
	{
		eoAxialTek.handleProgram();
		sendStatus( "Programmed E.O.M / Axial Tektronics Generator.\r\n" );
	}
	catch (Error& exception)
	{
		sendErr( "Error while programing E.O.M. / Axial Tektronics generator: " + exception.whatStr() + "\r\n" );
	}
}


std::pair<UINT, UINT> AuxiliaryWindow::getTtlBoardSize()
{
	return ttlBoard.getTtlBoardSize();
}


void AuxiliaryWindow::handleNewConfig( std::ofstream& newFile )
{
	// order matters.
	configVariables.handleNewConfig( newFile );
	//functionVariables.handleNewConfig( newFile );
	ttlBoard.handleNewConfig( newFile );
	aoSys.handleNewConfig( newFile );
	for ( auto& agilent : agilents )
	{
		agilent.handleNewConfig( newFile );
	}
	topBottomTek.handleNewConfig( newFile );
	eoAxialTek.handleNewConfig( newFile );
}


void AuxiliaryWindow::handleSaveConfig( std::ofstream& saveFile )
{
	// order matters.
	configVariables.handleSaveConfig( saveFile );
	//functionVariables.handleSaveConfig( saveFile );
	ttlBoard.handleSaveConfig( saveFile );
	aoSys.handleSaveConfig( saveFile );
	for ( auto& agilent : agilents )
	{
		agilent.handleSavingConfig( saveFile, mainWindowFriend->getProfileSettings( ).categoryPath,
									mainWindowFriend->getRunInfo( ) );
	}
	topBottomTek.handleSaveConfig( saveFile );
	eoAxialTek.handleSaveConfig( saveFile );
}

void AuxiliaryWindow::handleOpeningConfig(std::ifstream& configFile, int versionMajor, int versionMinor )
{
	ttlBoard.prepareForce( );
	aoSys.prepareForce( );

	configVariables.normHandleOpenConfig(configFile, versionMajor, versionMinor );
	if ( (versionMajor == 3 && versionMinor > 1) || versionMajor > 3 )
	{
		//functionVariables.funcHandleOpenConfig( configFile, versionMajor, versionMinor );
	}
	ttlBoard.handleOpenConfig(configFile, versionMajor, versionMinor );
	aoSys.handleOpenConfig(configFile, versionMajor, versionMinor, &ttlBoard);
	aoSys.updateEdits( );
	agilents[whichAg::TopBottom].readConfigurationFile(configFile, versionMajor, versionMinor );
	agilents[whichAg::TopBottom].updateSettingsDisplay( 1, mainWindowFriend->getProfileSettings().categoryPath,
											   mainWindowFriend->getRunInfo() );
	agilents[whichAg::Axial].readConfigurationFile(configFile, versionMajor, versionMinor );
	agilents[whichAg::Axial].updateSettingsDisplay( 1, mainWindowFriend->getProfileSettings().categoryPath,
										   mainWindowFriend->getRunInfo() );
	agilents[whichAg::Flashing].readConfigurationFile(configFile, versionMajor, versionMinor );
	agilents[whichAg::Flashing].updateSettingsDisplay( 1, mainWindowFriend->getProfileSettings( ).categoryPath,
											  mainWindowFriend->getRunInfo( ) );
	if ( (versionMajor == 2 && versionMinor > 6) || versionMajor > 2)
	{
		agilents[whichAg::Microwave].readConfigurationFile( configFile, versionMajor, versionMinor );
		agilents[whichAg::Microwave].updateSettingsDisplay( 1, mainWindowFriend->getProfileSettings( ).categoryPath,
												   mainWindowFriend->getRunInfo( ) );
	}
	topBottomTek.handleOpeningConfig(configFile, versionMajor, versionMinor );
	eoAxialTek.handleOpeningConfig(configFile, versionMajor, versionMinor );
}


UINT AuxiliaryWindow::getNumberOfDacs()
{
	return aoSys.getNumberOfDacs();
}


std::array<std::array<std::string, 16>, 4> AuxiliaryWindow::getTtlNames()
{
	return ttlBoard.getAllNames();
}


std::array<std::string, 24> AuxiliaryWindow::getDacNames()
{
	return aoSys.getAllNames();
}


void AuxiliaryWindow::drawVariables(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	if (id == IDC_GLOBAL_VARS_LISTVIEW)
	{
		globalVariables.handleDraw(pNMHDR, pResult, mainWindowFriend->getRgbs());
	}
	else
	{
		configVariables.handleDraw(pNMHDR, pResult, mainWindowFriend->getRgbs());
	}
}



void AuxiliaryWindow::ConfigVarsDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	std::vector<Script*> scriptList;
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		configVariables.updateParameterInfo(scriptList, mainWindowFriend, this, &ttlBoard, &aoSys);
	}
	catch (Error& exception)
	{
		sendErr("Variables Double Click Handler : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::ConfigVarsRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		configVariables.deleteVariable();
	}
	catch (Error& exception)
	{
		sendErr("Variables Right Click Handler : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


std::vector<parameterType> AuxiliaryWindow::getAllVariables()
{
	std::vector<parameterType> vars = configVariables.getEverything();
	std::vector<parameterType> vars2 = globalVariables.getEverything();
	vars.insert(vars.end(), vars2.begin(), vars2.end());
	return vars;
}


void AuxiliaryWindow::GlobalVarDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	std::vector<Script*> scriptList;
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		globalVariables.updateParameterInfo(scriptList, mainWindowFriend, this, &ttlBoard, &aoSys);
	}
	catch (Error& exception)
	{
		sendErr("Global Variables Double Click Handler : " + exception.whatStr() + "\r\n");
	}
}


void AuxiliaryWindow::GlobalVarRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		globalVariables.deleteVariable();
	}
	catch (Error& exception)
	{
		sendErr("Global Variables Right Click Handler : " + exception.whatStr() + "\r\n");
	}
}


void AuxiliaryWindow::ConfigVarsColumnClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		configVariables.handleColumnClick(pNotifyStruct, result);
	}
	catch (Error& exception)
	{
		sendErr("Handling config variable listview click : " + exception.whatStr() + "\r\n");
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::clearVariables()
{
	configVariables.clearVariables();
}


void AuxiliaryWindow::addVariable(std::string name, bool constant, double value, int item)
{
	parameterType var;
	var.name = name;
	var.constant = constant;
	var.ranges.push_back({ value, 0, 1, false, true });
	configVariables.addConfigParameter(var, item);
}


void AuxiliaryWindow::passCommonCommand(UINT id)
{
	try
	{
		commonFunctions::handleCommonMessage(id, this, mainWindowFriend, scriptingWindowFriend, cameraWindowFriend, this);
	}
	catch (Error& err)
	{
		// catch any extra errors that handleCommonMessage doesn't explicitly handle.
		errBox(err.what());
	}
}

void AuxiliaryWindow::loadFriends(MainWindow* mainWin, ScriptingWindow* scriptWin, CameraWindow* camWin)
{
	mainWindowFriend = mainWin;
	scriptingWindowFriend = scriptWin;
	cameraWindowFriend = camWin;
}


void AuxiliaryWindow::passRoundToDac()
{
	aoSys.handleRoundToDac(menu);
}


void AuxiliaryWindow::handleTektronicsButtons(UINT id)
{
	if (id >= TOP_ON_OFF && id <= BOTTOM_FSK)
	{
		topBottomTek.handleButtons(id - TOP_ON_OFF);
	}
	if (id >= EO_ON_OFF && id <= AXIAL_FSK)
	{
		eoAxialTek.handleButtons(id - EO_ON_OFF);
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::handleEnter()
{
	errBox("Hello, there!");
}


void AuxiliaryWindow::setVariablesActiveState(bool activeState)
{
	configVariables.setParameterControlActive(activeState);
}


UINT AuxiliaryWindow::getTotalVariationNumber()
{
	return configVariables.getTotalVariationNumber();
}


void AuxiliaryWindow::OnSize(UINT nType, int cx, int cy)
{
	SetRedraw( false );
	for ( auto& ttlPlt : ttlPlots )
	{
		ttlPlt->rearrange( cx, cy, getFonts( ) );
	}
	for ( auto& dacPlt : aoPlots )
	{
		dacPlt->rearrange( cx, cy, getFonts( ) );
	}
	topBottomTek.rearrange(cx, cy, getFonts());
	eoAxialTek.rearrange(cx, cy, getFonts());

	for ( auto& ag : agilents )
	{
		ag.rearrange( cx, cy, getFonts( ) );
	}

	RhodeSchwarzGenerator.rearrange(cx, cy, getFonts());

	ttlBoard.rearrange(cx, cy, getFonts());
	aoSys.rearrange(cx, cy, getFonts());
	aiSys.rearrange( cx, cy, getFonts( ) );

	configVariables.rearrange( cx, cy, getFonts( ) );
	globalVariables.rearrange( cx, cy, getFonts( ) );

	statusBox.rearrange( cx, cy, getFonts());
	SetRedraw();
	RedrawWindow();
}


fontMap AuxiliaryWindow::getFonts()
{
	return mainWindowFriend->getFonts();
}


void AuxiliaryWindow::handleAgilentOptions( UINT id )
{
	Agilent& agilent = whichAgilent( id );
	// zero the id.
	id -= IDC_TOP_BOTTOM_CHANNEL1_BUTTON;
	int agilentNum = id / 7;
	// figure out which box it was.	
	// call the correct function.
	if (id % 7 == 0)
	{
		// channel 1
		agilent.handleChannelPress( 1, mainWindowFriend->getProfileSettings().categoryPath, 
									 mainWindowFriend->getRunInfo() );
	}
	else if (id % 7 == 1)
	{
		// channel 2
		agilent.handleChannelPress( 2, mainWindowFriend->getProfileSettings().categoryPath, 
									 mainWindowFriend->getRunInfo() );
	}
	// sync is just a check, no handling needed.
	else if (id % 7 == 6)
	{
		try
		{
			agilent.handleInput( mainWindowFriend->getProfileSettings().categoryPath, mainWindowFriend->getRunInfo() );
			agilent.setAgilent();
			sendStatus( "Programmed Agilent " + agilent.getName() + ".\r\n" );
		}
		catch (Error& err)
		{
			sendErr( "Error while programming agilent " + agilent.getName() + ": " + err.what() + "\r\n" );
		}
	}
	// else it's a combo or edit that must be handled separately, not in an ON_COMMAND handling.
	mainWindowFriend->updateConfigurationSavedStatus( false );
}


void AuxiliaryWindow::handleAgilentCombo(UINT id)
{
	Agilent& ag = whichAgilent( id );
	try
	{
		ag.handleInput( mainWindowFriend->getProfileSettings( ).categoryPath, mainWindowFriend->getRunInfo( ) );
		ag.handleCombo( );
		ag.updateSettingsDisplay( mainWindowFriend->getProfileSettings( ).categoryPath, mainWindowFriend->getRunInfo( ) );
	}
	catch ( Error& err )
	{
		sendErr( "ERROR: error while handling agilent combo change: " + err.whatStr( ) );
	}
}


void AuxiliaryWindow::sendErr(std::string msg)
{
	mainWindowFriend->getComm()->sendError(msg);
}


void AuxiliaryWindow::sendStatus(std::string msg)
{
	mainWindowFriend->getComm()->sendStatus(msg);
}


void AuxiliaryWindow::zeroDacs( )
{
	try
	{
		aoSys.resetDacEvents( );
		ttlBoard.resetTtlEvents( );
		aoSys.prepareForce( );
		ttlBoard.prepareForce( );
		for ( int dacInc : range( 24 ) )
		{
			aoSys.prepareDacForceChange( dacInc, 0, &ttlBoard );
		}
		aoSys.updateEdits( );
		aoSys.organizeDacCommands( 0, 0 );
		aoSys.makeFinalDataFormat( 0, 0 );
		aoSys.stopDacs( );
		aoSys.configureClocks( 0, false, 0 );
		aoSys.writeDacs( 0, false, 0 );
		aoSys.startDacs( );
		ttlBoard.organizeTtlCommands( 0, 0 );
		ttlBoard.convertToFinalViewpointFormat( 0, 0 );
		ttlBoard.writeTtlData( 0, false, 0 );
		ttlBoard.startBoard( );
		ttlBoard.waitTillFinished( 0, false, 0 );
		sendStatus( "Zero'd DACs.\r\n" );
	}
	catch ( Error& exception )
	{
		sendStatus( "Failed to Zero DACs!!!\r\n" );
		sendErr( exception.what( ) );
	}
}


void AuxiliaryWindow::zeroTtls()
{
	try
	{
		ttlBoard.zeroBoard();
		sendStatus( "Zero'd TTLs.\r\n" );
	}
	catch (Error& exception)
	{
		sendStatus( "Failed to Zero TTLs!!!\r\n" );
		sendErr( exception.what() );
	}
}


void AuxiliaryWindow::loadMotSettings(MasterThreadInput* input)
{
	try
	{
		sendStatus("Loading MOT Configuration...\r\n" );
		input->auxWin = this;
		input->quiet = true;
		input->ttls = &ttlBoard;
		input->aoSys = &aoSys;
		input->aiSys = &aiSys;
		input->globalControl = &globalVariables;
		input->comm = mainWindowFriend->getComm();
		input->settings = { 0,0,0 };
		input->debugOptions = { 0, 0, 0, 0, 0, 0, 0, "", 0, 0, 0 };
		// don't get configuration variables. The MOT shouldn't depend on config variables.
		input->variables.clear( );
		input->variables.push_back(globalVariables.getEverything());
		// Only set it once, clearly.
		input->repetitionNumber = 1;
		input->rsg = &RhodeSchwarzGenerator;
		input->intensityAgilentNumber = -1;
		input->topBottomTek = &topBottomTek;
		input->eoAxialTek = &eoAxialTek;
		input->runMaster = true;
		input->runNiawg = false;
		input->dacData = dacData;
		input->ttlData = ttlData;
	}
	catch (Error& exception)
	{
		sendStatus(": " + exception.whatStr() + " " + exception.whatStr() + "\r\n" );
	}
}


// Gets called after alt-f4 or X button is pressed.
void AuxiliaryWindow::OnCancel()
{
	passCommonCommand(ID_FILE_MY_EXIT);
}


void AuxiliaryWindow::fillMasterThreadInput( MasterThreadInput* input )
{
	input->auxWin = this;
	input->ttls = &ttlBoard;
	input->aoSys = &aoSys;
	input->aiSys = &aiSys;
	input->globalControl = &globalVariables;
	input->dacData = dacData;
	input->ttlData = ttlData;
	/// variables.
	std::vector<std::vector<parameterType>> experimentVars;
	for ( auto seqFile : input->seq.sequence )
	{
		// load the variables. This little loop is for letting configuration variables overwrite the globals.
		// the config variables are loaded directly from the file.
		std::vector<parameterType> configVars = ParameterSystem::getConfigVariablesFromFile( seqFile.configFilePath() );
		std::vector<parameterType> globals = globalVariables.getEverything( );
		experimentVars.push_back( ParameterSystem::combineParametersForExperimentThread( configVars, globals) );
		globalVariables.setUsages( { globals } );
	}
	input->variables = experimentVars;
	input->constants.resize( input->variables.size( ) );
	// it's important to do this after the key is generated so that the constants have their values.
	for ( auto seqInc : range( input->variables.size( ) ) )
	{
		for ( auto& variable : input->variables[seqInc] )
		{
			if ( variable.constant )
			{
				input->constants[seqInc].push_back( variable );
			}
		}
	}
	input->rsg = &RhodeSchwarzGenerator;
	for ( auto& agilent : agilents )
	{
		input->agilents.push_back( &agilent );
	}
	topBottomTek.getTekSettings();
	eoAxialTek.getTekSettings();
	input->topBottomTek = &topBottomTek;
	input->eoAxialTek = &eoAxialTek;
}


void AuxiliaryWindow::changeBoxColor(systemInfo<char> colors)
{
	statusBox.changeColor(colors);
}


void AuxiliaryWindow::handleAbort()
{
	ttlBoard.unshadeTtls();
	aoSys.unshadeDacs();
}


void AuxiliaryWindow::handleMasterConfigSave(std::stringstream& configStream)
{
	// save info
	/// ttls
	for (UINT ttlRowInc = 0; ttlRowInc < ttlBoard.getTtlBoardSize().first; ttlRowInc++)
	{
		for (UINT ttlNumberInc = 0; ttlNumberInc < ttlBoard.getTtlBoardSize().second; ttlNumberInc++)
		{
			std::string name = ttlBoard.getName(ttlRowInc, ttlNumberInc);
			if (name == "")
			{
				// then no name has been set, so create the default name.
				switch (ttlRowInc)
				{
					case 0:
						name = "A";
						break;
					case 1:
						name = "B";
						break;
					case 2:
						name = "C";
						break;
					case 3:
						name = "D";
						break;
				}
				name += str(ttlNumberInc);
			}
			configStream << name << "\n";

			configStream << ttlBoard.getDefaultTtl(ttlRowInc, ttlNumberInc) << "\n";
		}
	}
	// DAC Names
	for (UINT dacInc = 0; dacInc < aoSys.getNumberOfDacs(); dacInc++)
	{
		std::string name = aoSys.getName(dacInc);
		std::pair<double, double> minMax = aoSys.getDacRange(dacInc);
		if (name == "")
		{
			// then the name hasn't been set, so create the default name
			name = "Dac" + str(dacInc);
		}
		configStream << name << "\n";
		configStream << minMax.first << " - " << minMax.second << "\n";
		configStream << aoSys.getDefaultValue(dacInc) << "\n";
	}

	// Number of Variables
	configStream << globalVariables.getCurrentNumberOfVariables() << "\n";
	/// Variables
	for (UINT varInc : range( globalVariables.getCurrentNumberOfVariables() ) )
	{
		parameterType info = globalVariables.getVariableInfo(varInc);
		configStream << info.name << " ";
		configStream << info.constantValue << "\n";
		// all globals are constants, no need to output anything else.
	}
}


void AuxiliaryWindow::handleMasterConfigOpen(std::stringstream& configStream, double version)
{
	ttlBoard.resetTtlEvents();
	ttlBoard.prepareForce();
	aoSys.resetDacEvents();
	aoSys.prepareForce();
	// save info
	for (UINT ttlRowInc : range( ttlBoard.getTtlBoardSize().first))
	{
		for (UINT ttlNumberInc : range( ttlBoard.getTtlBoardSize().second ) )
		{
			std::string name;
			std::string statusString;
			bool status;
			configStream >> name >> statusString;
			try
			{
				// should actually be zero or one, but just just convert to bool
				status = std::stoi(statusString);
			}
			catch (std::invalid_argument&)
			{
				thrower("ERROR: Failed to load one of the default ttl values!");
			}

			ttlBoard.setName(ttlRowInc, ttlNumberInc, name, toolTips, this);
			ttlBoard.forceTtl(ttlRowInc, ttlNumberInc, status);
			ttlBoard.updateDefaultTtl(ttlRowInc, ttlNumberInc, status);
		}
	}
	// getting aoSys.
	for (UINT dacInc : range( aoSys.getNumberOfDacs()) )
	{
		std::string name, defaultValueString, minString, maxString;
		double defaultValue, min, max;
		configStream >> name;
		if (version >= 1.2)
		{
			std::string trash;
			configStream >> minString >> trash;
			if (trash != "-")
			{
				thrower("ERROR: Expected \"-\" in config file between min and max values!");
			}
			configStream >> maxString;
		}
		configStream >> defaultValueString;
		try
		{
			defaultValue = std::stod(defaultValueString);
			if (version >= 1.2)
			{
				min = std::stod(minString);
				max = std::stod(maxString);
			}
			else
			{
				min = -10;
				max = 10;
			}
		}
		catch (std::invalid_argument&)
		{
			thrower("ERROR: Failed to load one of the default DAC values!");
		}
		aoSys.setName(dacInc, name, toolTips, this);
		aoSys.setMinMax(dacInc, min, max);
		aoSys.prepareDacForceChange(dacInc, defaultValue, &ttlBoard);
		aoSys.updateEdits( );
		aoSys.setDefaultValue(dacInc, defaultValue);
	}
	// variables.
	if (version >= 1.1)
	{
		int varNum;
		configStream >> varNum;
		if (varNum < 0 || varNum > 1000)
		{
			int answer = promptBox("ERROR: variable number retrieved from file appears suspicious. The number is "
									+ str(varNum) + ". Is this accurate?", MB_YESNO);
			if (answer == IDNO)
			{
				// don't try to load anything.
				varNum = 0;
				return;
			}
		}
		// Number of Variables
		globalVariables.clearVariables();
		for (int varInc = 0; varInc < varNum; varInc++)
		{
			parameterType tempVar;
			tempVar.constant = true;
			tempVar.overwritten = false;
			tempVar.active = false;
			double value;
			configStream >> tempVar.name >> value;
			tempVar.constantValue = value;
			tempVar.ranges.push_back({ value, value, 0, false, true });
			globalVariables.addGlobalParameter(tempVar, varInc);
		}
	}
	parameterType tempVar;
	tempVar.name = "";
	globalVariables.addGlobalParameter(tempVar, -1);
}


void AuxiliaryWindow::SetDacs()
{
	// have the dac values change
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		sendStatus("----------------------\r\n");
		aoSys.resetDacEvents();
		ttlBoard.resetTtlEvents();
		sendStatus( "Setting Dacs...\r\n" );
		aoSys.handleSetDacsButtonPress( &ttlBoard );
		aoSys.updateEdits( );
		aoSys.organizeDacCommands(0, 0);
		aoSys.makeFinalDataFormat(0, 0 );
		// start the boards which actually sets the dac values.
		aoSys.stopDacs();
		aoSys.configureClocks(0, false, 0 );
		sendStatus( "Writing New Dac Settings...\r\n" );
		aoSys.writeDacs(0, false, 0 );
		aoSys.startDacs();
		ttlBoard.organizeTtlCommands(0, 0 );
		ttlBoard.convertToFinalViewpointFormat(0, 0 );
		ttlBoard.writeTtlData(0, false, 0 );
		ttlBoard.startBoard();
		ttlBoard.waitTillFinished(0, false, 0 );
		sendStatus( "Finished Setting Dacs.\r\n" );
	}
	catch (Error& exception)
	{
		errBox( exception.what() );
		sendStatus( ": " + exception.whatStr() + "\r\n" );
		sendErr( exception.what() );
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::DacEditChange(UINT id)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		aoSys.handleEditChange(id - ID_DAC_FIRST_EDIT);
	}
	catch (Error& err)
	{
		sendErr(err.what());
	}
}


void AuxiliaryWindow::handleTtlPush(UINT id)
{
	try
	{
		mainWindowFriend->updateConfigurationSavedStatus( false );
		ttlBoard.handleTTLPress( id );
	}
	catch (Error& exception)
	{
		sendErr( "TTL Press Handler Failed: " + exception.whatStr() + "\r\n" );
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::handlTtlHoldPush()
{
	try
	{
		ttlBoard.handleHoldPress();
	}
	catch (Error& exception)
	{
		sendErr( "TTL Hold Handler Failed: " + exception.whatStr() + "\r\n" );
	}
	mainWindowFriend->updateConfigurationSavedStatus(false);
}


void AuxiliaryWindow::ViewOrChangeTTLNames()
{
	mainWindowFriend->updateConfigurationSavedStatus( false );
	ttlInputStruct input;
	input.ttls = &ttlBoard;
	input.toolTips = toolTips;
	TtlSettingsDialog dialog(&input, IDD_VIEW_AND_CHANGE_TTL_NAMES);
	dialog.DoModal();
}


void AuxiliaryWindow::ViewOrChangeDACNames()
{
	mainWindowFriend->updateConfigurationSavedStatus( false );
	aoInputStruct input;
	input.aoSys = &aoSys;
	input.toolTips = toolTips;
	AoSettingsDialog dialog(&input, IDD_VIEW_AND_CHANGE_DAC_NAMES);
	dialog.DoModal();
}


void AuxiliaryWindow::Exit()
{
	EndDialog(0);
}


HBRUSH AuxiliaryWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	brushMap brushes = mainWindowFriend->getBrushes();
	rgbMap rgbs = mainWindowFriend->getRgbs();
	HBRUSH result;
	for ( auto& ag : agilents )
	{
		result = ag.handleColorMessage( pWnd, brushes, rgbs, pDC );
		if ( result != NULL )
		{
			return result;
		}
	}
	result = ttlBoard.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = aoSys.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = topBottomTek.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = eoAxialTek.handleColorMessage(pWnd, brushes, rgbs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = *statusBox.handleColoring(pWnd->GetDlgCtrlID(), pDC, brushes, rgbs);
	if (result != NULL)
	{
		return result;
	}

	// default colors
	switch (nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			pDC->SetTextColor(rgbs["Solarized Base0"]);
			pDC->SetBkColor(rgbs["Medium Grey"]);
			return *brushes["Medium Grey"];
		}
		case CTLCOLOR_EDIT:
		{
			pDC->SetTextColor(rgbs["Solarized Yellow"]);
			pDC->SetBkColor(rgbs["Dark Grey"]);
			return *brushes["Dark Grey"];
		}
		case CTLCOLOR_LISTBOX:
		{
			pDC->SetTextColor(rgbs["Solarized Base0"]);
			pDC->SetBkColor(rgbs["Dark Grey"]);
			return *brushes["Dark Grey"];
		}
		default:
			return *brushes["Solarized Base04"];
	}
}


BOOL AuxiliaryWindow::PreTranslateMessage(MSG* pMsg)
{
	for (UINT toolTipInc = 0; toolTipInc < toolTips.size(); toolTipInc++)
	{
		toolTips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL AuxiliaryWindow::OnInitDialog()
{
	// don't redraw until the first OnSize.
	SetRedraw( false );

	int id = 4000;
	POINT controlLocation{ 0, 0 };
	try
	{
		auto rgbs = mainWindowFriend->getRgbs( );
		statusBox.initialize( controlLocation, id, this, 480, toolTips );
		ttlBoard.initialize( controlLocation, toolTips, this, id, rgbs );
		aoSys.initialize( controlLocation, toolTips, this, id, mainWindowFriend->getRgbs() );
		aiSys.initialize( controlLocation, this, id );
		topBottomTek.initialize( controlLocation, this, id, "Top-Bottom-Tek", "Top", "Bottom", 480,
		{ TOP_BOTTOM_PROGRAM, TOP_ON_OFF, TOP_FSK, BOTTOM_ON_OFF, BOTTOM_FSK }, rgbs );
		eoAxialTek.initialize( controlLocation, this, id, "EO / Axial", "EO", "Axial", 480, { EO_AXIAL_PROGRAM,
							   EO_ON_OFF, EO_FSK, AXIAL_ON_OFF, AXIAL_FSK }, rgbs );
		RhodeSchwarzGenerator.initialize( controlLocation, toolTips, this, id );
		controlLocation = POINT{ 480, 0 };

		agilents[whichAg::TopBottom].initialize( controlLocation, toolTips, this, id, "Top-Bottom-Agilent", 100,
										rgbs["Solarized Base03"], rgbs );
		agilents[whichAg::Axial].initialize( controlLocation, toolTips, this, id, "Microwave-Axial-Agilent", 100,
											 rgbs["Solarized Base03"], rgbs );
		agilents[whichAg::Flashing].initialize( controlLocation, toolTips, this, id, "Flashing-Agilent", 100, 
												rgbs["Solarized Base03"], rgbs );
		agilents[whichAg::Microwave].initialize( controlLocation, toolTips, this, id, "Microwave-Agilent", 100,
												 rgbs["Solarized Base03"], rgbs );
		controlLocation = POINT{ 1440, 0 };
		globalVariables.initialize( controlLocation, toolTips, this, id, "GLOBAL VARIABLES",
									mainWindowFriend->getRgbs(), IDC_GLOBAL_VARS_LISTVIEW, ParameterSysType::global );
		configVariables.initialize( controlLocation, toolTips, this, id, "CONFIGURATION VARIABLES",
									mainWindowFriend->getRgbs(), IDC_CONFIG_VARS_LISTVIEW, ParameterSysType::config );
		configVariables.setParameterControlActive( false );

		controlLocation = POINT{ 960, 0 };
		aoPlots.resize( NUM_DAC_PLTS );
		dacData.resize( NUM_DAC_PLTS );
		UINT linesPerDacPlot = 24 / dacData.size( );
		// initialize data structures.
		for ( auto& dacPlotData : dacData )
		{
			dacPlotData = std::vector<pPlotDataVec>( linesPerDacPlot );
			for ( auto& d : dacPlotData )
			{
				d = pPlotDataVec( new plotDataVec( 100, { 0,0,0 } ) );
			}
		}
		// initialize plot controls.
		UINT dacPlotSize = 500 / NUM_DAC_PLTS;
		for ( auto& dacPltCount : range(aoPlots.size()))
		{
			std::string titleTxt;
			switch ( dacPltCount )
			{
			case 0:
				titleTxt = "DACs: 0-7";
				break;
			case 1:
				titleTxt = "DACs: 8-15";
				break;
			case 2:
				titleTxt = "DACs: 16-23";
				break;
			}
			aoPlots[dacPltCount] = new PlotCtrl( dacData[dacPltCount], plotStyle::DacPlot, mainWindowFriend->getBrightPlotPens( ),
												  mainWindowFriend->getPlotFont( ), 
												 mainWindowFriend->getBrightPlotBrushes( ), titleTxt );
			aoPlots[dacPltCount]->init( controlLocation, 480, dacPlotSize, this );
			controlLocation.y += dacPlotSize;
		}
		// ttl plots are similar to aoSys.
		ttlPlots.resize( NUM_TTL_PLTS );
		ttlData.resize( NUM_TTL_PLTS );
		UINT linesPerTtlPlot =  64 / ttlData.size( );
		for ( auto& ttlPlotData : ttlData )
		{
			ttlPlotData = std::vector<pPlotDataVec>( linesPerTtlPlot );
			for ( auto& d : ttlPlotData )
			{
				d = pPlotDataVec( new plotDataVec( 100, { 0,0,0 } ) );
			}
		}
		UINT ttlPlotSize = 500 / NUM_TTL_PLTS;
		for ( auto& ttlPltCount : range( ttlPlots.size( ) ) )
		{
			// currently assuming 4 ttl plots...
			std::string titleTxt;
			switch ( ttlPltCount )
			{
			case 0:
				titleTxt = "Ttls: Row A";
				break;
			case 1:
				titleTxt = "Ttls: Row B";
				break;
			case 2:
				titleTxt = "Ttls: Row C";
				break;
			case 3:
				titleTxt = "Ttls: Row D";
				break;
			}
			ttlPlots[ttlPltCount] = new PlotCtrl( ttlData[ttlPltCount], plotStyle::TtlPlot, mainWindowFriend->getBrightPlotPens( ),
												  mainWindowFriend->getPlotFont( ), 
												  mainWindowFriend->getBrightPlotBrushes( ),
												  titleTxt );
			ttlPlots[ttlPltCount]->init( controlLocation, 480, ttlPlotSize, this );
			controlLocation.y += ttlPlotSize;
		}
	}
	catch (Error& exeption)
	{
		errBox( exeption.what() );
	}
	SetTimer( 1, 10000, NULL );
	SetTimer( 2, 1000, NULL );

	menu.LoadMenu( IDR_MAIN_MENU );
	SetMenu( &menu );
	return TRUE;
}


std::string AuxiliaryWindow::getOtherSystemStatusMsg( )
{
	// controls are done. Report the initialization status...
	std::string msg;
	msg += "DIO System:\n";
	if ( !DIO_SAFEMODE )
	{
		msg += "\tCode System is active!\n";
		msg += "\t" + ttlBoard.getSystemInfo( ) + "\n";
	}
	else
	{
		msg += "\tCode System is disabled! Enable in \"constants.h\"\n";
	}
	msg += "Analog Out System:\n";
	if ( !ANALOG_OUT_SAFEMODE )
	{
		msg += "\tCode System is Active!\n";
		msg += "\t" + aoSys.getSystemInfo( ) + "\n";
	}
	else
	{
		msg += "\tCode System is disabled! Enable in \"constants.h\"\n";
	}
	msg += "Analog In System:\n";
	if ( !ANALOG_IN_SAFEMODE )
	{
		msg += "\tCode System is Active!\n";
		msg += "\t" + aiSys.getSystemStatus( ) + "\n";
	}
	else
	{
		msg += "\tCode System is disabled! Enable in \"constants.h\"\n";
	}
	return msg;
}


std::string AuxiliaryWindow::getVisaDeviceStatus( )
{
	std::string msg;
	msg += "----------------------------------------------------------------------------------- VISA Devices\n";
	msg += "Tektronics 1:\n\t" + topBottomTek.queryIdentity( );
	msg += "Tektronics 2:\n\t" + eoAxialTek.queryIdentity( );
	for ( auto& agilent : agilents )
	{
		msg += agilent.getName( ) + ":\n\t" + agilent.getDeviceIdentity( );
	}
	return msg;
}


std::string AuxiliaryWindow::getGpibDeviceStatus( )
{
	std::string msg;
	msg += "----------------------------------------------------------------------------------- GPIB Devices:\n";
	msg += "RSG:\n";
	if ( RSG_SAFEMODE )
	{
		msg += "\tCode System is Active!\n";
		msg += "\t" + RhodeSchwarzGenerator.getIdentity( );
	}
	else
	{
		msg += "\tCode System is disabled! Enable in \"constants.h\"";
	}
	return msg;
}


