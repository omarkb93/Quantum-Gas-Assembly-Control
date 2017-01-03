#include "stdafx.h"
#include "MainWindow.h"
#include "initializeMainWindow.h"
#include "commonFunctions.h"
#include "myAgilent.h"
#include "NiawgController.h"
#include "CameraWindow.h"
#include "myErrorHandler.h"

IMPLEMENT_DYNAMIC(MainWindow, CDialog)

BEGIN_MESSAGE_MAP( MainWindow, CDialog )
	ON_WM_CTLCOLOR()
	ON_COMMAND_RANGE( ID_ACCELERATOR_ESC, ID_ACCELERATOR_ESC, &MainWindow::passCommonCommand )
	ON_COMMAND_RANGE( ID_ACCELERATOR_F5, ID_ACCELERATOR_F5, &MainWindow::passCommonCommand )
	ON_COMMAND_RANGE( MENU_ID_RANGE_BEGIN, MENU_ID_RANGE_END, &MainWindow::passCommonCommand )
	ON_COMMAND_RANGE( IDC_DEBUG_OPTIONS_RANGE_BEGIN, IDC_DEBUG_OPTIONS_RANGE_END, &MainWindow::passDebugPress )
	ON_COMMAND_RANGE( IDC_MAIN_OPTIONS_RANGE_BEGIN, IDC_MAIN_OPTIONS_RANGE_END, &MainWindow::passMainOptionsPress )
	// 

	ON_CBN_SELENDOK( IDC_EXPERIMENT_COMBO, &MainWindow::handleExperimentCombo )
	ON_CBN_SELENDOK( IDC_CATEGORY_COMBO, &MainWindow::handleCategoryCombo )
	ON_CBN_SELENDOK( IDC_CONFIGURATION_COMBO, &MainWindow::handleConfigurationCombo )
	ON_CBN_SELENDOK( IDC_SEQUENCE_COMBO, &MainWindow::handleSequenceCombo )
	ON_CBN_SELENDOK( IDC_ORIENTATION_COMBO, &MainWindow::handleOrientationCombo )
	// 
	ON_NOTIFY( NM_DBLCLK, IDC_VARIABLES_LISTVIEW, &MainWindow::listViewDblClick )
	ON_NOTIFY( NM_RCLICK, IDC_VARIABLES_LISTVIEW, &MainWindow::handleRClick )
	ON_REGISTERED_MESSAGE( eStatusTextMessageID, &MainWindow::onStatusTextMessage )
	ON_REGISTERED_MESSAGE( eErrorTextMessageID, &MainWindow::onErrorMessage )
	ON_REGISTERED_MESSAGE( eFatalErrorMessageID, &MainWindow::onFatalErrorMessage )
	ON_REGISTERED_MESSAGE( eNormalFinishMessageID, &MainWindow::onNormalFinishMessage )
	ON_REGISTERED_MESSAGE( eColoredEditMessageID, &MainWindow::onColoredEditMessage )
	ON_REGISTERED_MESSAGE( eDebugMessageID, &MainWindow::onDebugMessage )

	ON_COMMAND_RANGE( IDC_MAIN_STATUS_BUTTON, IDC_MAIN_STATUS_BUTTON, &MainWindow::passClear )
	ON_COMMAND_RANGE( IDC_ERROR_STATUS_BUTTON, IDC_ERROR_STATUS_BUTTON, &MainWindow::passClear )
	ON_COMMAND_RANGE( IDC_DEBUG_STATUS_BUTTON, IDC_DEBUG_STATUS_BUTTON, &MainWindow::passClear )
END_MESSAGE_MAP()

void MainWindow::setNiawgRunningState( bool newRunningState )
{
	this->niawg.setRunningState( newRunningState );
	return;
}

bool MainWindow::niawgIsRunning()
{
	return this->niawg.isRunning();
}

BOOL MainWindow::PreTranslateMessage(MSG* pMsg)
{
	for (int toolTipInc = 0; toolTipInc < this->tooltips.size(); toolTipInc++)
	{
		this->tooltips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void MainWindow::setNiawgDefaults(bool isFirstTime)
{
	this->niawg.setDefaultWaveforms(this, isFirstTime);
}

std::unordered_map<std::string, CFont*> MainWindow::getFonts()
{
	return mainFonts;
}

void MainWindow::passClear(UINT id)
{
	if (id == IDC_MAIN_STATUS_BUTTON)
	{
		this->mainStatus.clear();
	}
	else if (id == IDC_ERROR_STATUS_BUTTON)
	{
		this->errorStatus.clear();
	}
	else if (id == IDC_DEBUG_STATUS_BUTTON)
	{
		this->debugStatus.clear();
	}
	return;
}

void MainWindow::OnCancel()
{
	passCommonCommand(ID_FILE_MY_EXIT);
	return;
}
void MainWindow::OnClose()
{
	passCommonCommand(WM_CLOSE);
	return;
}

void MainWindow::stopNiawg()
{
	this->niawg.configureOutputEnabled(VI_FALSE);
	this->niawg.abortGeneration();
}

BOOL MainWindow::OnInitDialog()
{

	/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///
	///				Initialize NIAWG
	///
	// get time now
	time_t dateStart = time(0);
	struct tm datePointerStart;
	localtime_s(&datePointerStart, &dateStart);
	std::string logFolderNameStart = "Date " + std::to_string(datePointerStart.tm_year + 1900) + "-" + std::to_string(datePointerStart.tm_mon + 1) + "-"
		+ std::to_string(datePointerStart.tm_mday) + " Time " + std::to_string(datePointerStart.tm_hour) + "-" + std::to_string(datePointerStart.tm_min) + "-"
		+ std::to_string(datePointerStart.tm_sec);
	// initialize default file names and open the files.
	std::vector<std::fstream> default_hConfigVerticalScriptFile, default_hConfigHorizontalScriptFile, default_vConfigVerticalScriptFile,
		default_vConfigHorizontalScriptFile;
	default_hConfigVerticalScriptFile.push_back(std::fstream(DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_HCONFIG_VERTICAL_SCRIPT.nScript"));
	default_hConfigHorizontalScriptFile.push_back(std::fstream(DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_HCONFIG_HORIZONTAL_SCRIPT.nScript"));
	default_vConfigVerticalScriptFile.push_back(std::fstream(DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_VCONFIG_VERTICAL_SCRIPT.nScript"));
	default_vConfigHorizontalScriptFile.push_back(std::fstream((DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_VCONFIG_HORIZONTAL_SCRIPT.nScript")));

	// check errors
	if (!default_hConfigVerticalScriptFile[0].is_open())
	{
		errBox("FATAL ERROR: Couldn't open default file. Was looking for file " + DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_HCONFIG_VERTICAL_SCRIPT.nScript");
		return -1;
	}
	if (!default_hConfigHorizontalScriptFile[0].is_open())
	{
		errBox("FATAL ERROR: Couldn't open default file. Was looking for file " + DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_HCONFIG_HORIZONTAL_SCRIPT.nScript");
		return -1;
	}
	if (!default_vConfigVerticalScriptFile[0].is_open())
	{
		errBox("FATAL ERROR: Couldn't open default file. Was looking for file " + DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_VCONFIG_VERTICAL_SCRIPT.nScript");
		return -1;
	}
	if (!default_vConfigHorizontalScriptFile[0].is_open())
	{
		errBox("FATAL ERROR: Couldn't open default file. Was looking for file " + DEFAULT_SCRIPT_FOLDER_PATH + "DEFAULT_VCONFIG_HORIZONTAL_SCRIPT.nScript");
		return -1;
	}

	// parameters for variables used by the default file. (there shouldn't be any, these are essentially just placeholders so that I can use the same functions.
	std::vector<char> defXVarNames, defYVarNames;
	// parameters for variables used by the default file. (there shouldn't be any, these are essentially just placeholders so that I can use the same functions.
	std::vector<std::string> defXVarFileNames, defYVarFileNames;
	// parameters for variables used by the default file. (there shouldn't be any, these are essentially just placeholders so that I can use the same functions.
	std::vector<std::fstream> defXVarFiles;
	if (!TWEEZER_COMPUTER_SAFEMODE)
	{
		std::ofstream hConfigVerticalDefaultScriptLog(EXPERIMENT_LOGGING_FILES_PATH + logFolderNameStart + "\\Default hConfig Vertical Script.script");
		std::ofstream hConfigHorizontalDefaultScriptLog(EXPERIMENT_LOGGING_FILES_PATH + logFolderNameStart + "\\Default hConfig Horizontal Script.script");
		std::ofstream vConfigVerticalDefaultScriptLog(EXPERIMENT_LOGGING_FILES_PATH + logFolderNameStart + "\\Default vConfig Vertical Script.script");
		std::ofstream vConfigHorizontalDefaultScriptLog(EXPERIMENT_LOGGING_FILES_PATH + logFolderNameStart + "\\Default vConfig Horizontal Script.script");
		hConfigVerticalDefaultScriptLog << default_hConfigVerticalScriptFile[0].rdbuf();
		hConfigHorizontalDefaultScriptLog << default_hConfigHorizontalScriptFile[0].rdbuf();
		vConfigVerticalDefaultScriptLog << default_vConfigVerticalScriptFile[0].rdbuf();
		vConfigHorizontalDefaultScriptLog << default_vConfigHorizontalScriptFile[0].rdbuf();
	}
	try
	{
		niawg.initialize();
	}
	catch (my_exception& except)
	{
		errBox("ERROR: NIAWG Did not start smoothly: " + except.whatStr());
		return -1;
	}

	// Use this section of code to output some characteristics of the 5451. If you want.
	/*
	ViInt32 maximumNumberofWaveforms, waveformQuantum, minimumWaveformSize, maximumWaveformSize;

	if (myNIAWG::NIAWG_CheckWindowsError(niFgen_QueryArbWfmCapabilities(eSessionHandle, &maximumNumberofWaveforms, &waveformQuantum, &minimumWaveformSize, &maximumWaveformSize)))
	{
		return -1;
	}

	MessageBox(NULL, (LPCSTR)std::to_string(maximumNumberofWaveforms).c_str(), NULL, MB_OK);
	MessageBox(NULL, (LPCSTR)std::to_string(waveformQuantum).c_str(), NULL, MB_OK);
	MessageBox(NULL, (LPCSTR)std::to_string(minimumWaveformSize).c_str(), NULL, MB_OK);
	MessageBox(NULL, (LPCSTR)std::to_string(maximumWaveformSize).c_str(), NULL, MB_OK);
	*/

	this->niawg.setDefaultWaveforms(this, true);
	// but the default starts in the horizontal configuration, so switch back and start in this config.
	this->setOrientation(HORIZONTAL_ORIENTATION);
	try
	{
		this->restartNiawgDefaults();
	}
	catch (my_exception& exception)
	{
		errBox("ERROR: failed to start niawg default waveforms! Niawg gave the following error message: " + exception.whatStr());
	}
	// default value of this variable.
	eDontActuallyGenerate = false;
	eCurrentScript = "DefaultScript";
	// not done with the script, it will not stay on the NIAWG, so I need to keep track of it so thatI can reload it onto the NIAWG when necessary.	
	/// Initialize Windows
	this->TheScriptingWindow = new ScriptingWindow;
	this->TheCameraWindow = new CameraWindow(this, TheScriptingWindow);

	TheScriptingWindow->getFriends(this, TheCameraWindow);

	TheScriptingWindow->Create(IDD_LARGE_TEMPLATE, 0);
	TheScriptingWindow->ShowWindow(SW_SHOW);
	// initialize the camera window
	
	TheCameraWindow->Create(IDD_LARGE_TEMPLATE, 0);
	TheCameraWindow->ShowWindow(SW_SHOW);


	// initialize the COMM.
	this->comm.initialize( this, this->TheScriptingWindow, this->TheCameraWindow );

	int id = 1000;
	POINT statusPos = { 0,0 };
	this->mainStatus.initialize( statusPos, this, id, 975, "EXPERIMENT STATUS", RGB( 50, 50, 250 ), mainFonts, tooltips );
	statusPos = { 480, 0 };
	this->errorStatus.initialize( statusPos, this, id, 480, "ERROR STATUS", RGB( 200, 0, 0 ), mainFonts, tooltips );
	this->debugStatus.initialize( statusPos, this, id, 480, "DEBUG STATUS", RGB( 13, 152, 186 ), mainFonts, tooltips );
	POINT configStart = { 960, 0 };
	this->profile.initializeControls( configStart, this, id, mainFonts, tooltips );
	POINT notesStart = { 960, 235 };
	this->notes.initializeControls( notesStart, this, id, mainFonts, tooltips );
	POINT controlLocation = { 1440, 95 };
	this->variables.initializeControls( controlLocation, this, id, mainFonts, tooltips );
	this->settings.initialize( id, controlLocation, this, mainFonts, tooltips );
	this->debugger.initialize( id, controlLocation, this, mainFonts, tooltips );
	texter.initializeControls( controlLocation, this, false, id, mainFonts, tooltips );
	POINT statusLocations = { 960, 910 };
	this->boxes.initialize( statusLocations, id, this, 960, mainFonts, tooltips );
	this->shortStatus.initialize( statusLocations, this, id, mainFonts, tooltips );
	
	niawg.initialize();

	CMenu menu;
	menu.LoadMenu(IDR_MAIN_MENU);
	this->SetMenu(&menu);
	this->ShowWindow(SW_MAXIMIZE);
	// just initializes the rectangles.
	TheCameraWindow->redrawPictures();
	return TRUE;
}

void MainWindow::restartNiawgDefaults()
{
	niawg.restartDefault();
}

HBRUSH MainWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			int num = pWnd->GetDlgCtrlID();
			CBrush* ret = this->shortStatus.handleColor(pWnd, pDC, this->mainRGBs, this->mainBrushes);
			if (ret)
			{
				return *ret;
			}
			ret = this->boxes.handleColoring( num, pDC, mainBrushes, mainRGBs );
			if ( ret )
			{
				return *ret;
			}
			else
			{
				pDC->SetTextColor(mainRGBs["White"]);
				pDC->SetBkColor(mainRGBs["Medium Grey"]);
				return *mainBrushes["Medium Grey"];
			}
		}
		case CTLCOLOR_EDIT:
		{
			pDC->SetTextColor(mainRGBs["White"]);
			pDC->SetBkColor(mainRGBs["Dark Blue"]);
			return *mainBrushes["Dark Blue"];
		}
		case CTLCOLOR_LISTBOX:
		{
			pDC->SetTextColor(mainRGBs["White"]);
			pDC->SetBkColor(mainRGBs["Dark Grey"]);
			return *mainBrushes["Dark Grey"];
		}
		default:
		{
			return *mainBrushes["Light Grey"];
		}
	}
	return NULL;
}

void MainWindow::passCommonCommand(UINT id)
{
	// pass the command id to the common function, filling in the pointers to the windows which own objects needed.
	commonFunctions::handleCommonMessage(id, this, this, this->TheScriptingWindow, this->TheCameraWindow);
	return;
}

profileSettings MainWindow::getCurentProfileSettings()
{
	return this->profile.getCurrentProfileSettings();
}

bool MainWindow::checkProfileReady()
{
	return this->profile.allSettingsReadyCheck(this->TheScriptingWindow, this);
}

bool MainWindow::checkProfileSave()
{
	return this->profile.checkSaveEntireProfile(this->TheScriptingWindow, this);
}

bool MainWindow::setOrientation(std::string orientation)
{
	return this->profile.setOrientation(orientation);
}

void MainWindow::updateConfigurationSavedStatus(bool status)
{
	this->profile.updateConfigurationSavedStatus(status);
	return;
}

std::string MainWindow::getNotes(std::string whichLevel)
{
	std::transform(whichLevel.begin(), whichLevel.end(), whichLevel.begin(), ::tolower);
	if (whichLevel == "experiment")
	{
		return this->notes.getExperimentNotes();
	}
	else if (whichLevel == "category")
	{
		return this->notes.getCategoryNotes();
	}
	else if (whichLevel == "configuration")
	{
		return this->notes.getConfigurationNotes();
	}
	else
	{
		throw std::invalid_argument(("The Main window's getNotes function was called with a bad argument:" 
			+ whichLevel + ". Acceptable arguments are \"experiment\", \"category\", and \"configuration\". "
			"This throw can be continued successfully, the notes will just be recorded.").c_str());
	}
	return "";
}
void MainWindow::setNotes(std::string whichLevel, std::string notes)
{
	std::transform(whichLevel.begin(), whichLevel.end(), whichLevel.begin(), ::tolower);
	if (whichLevel == "experiment")
	{
		this->notes.setExperimentNotes(notes);
	}
	else if (whichLevel == "category")
	{
		this->notes.setCategoryNotes(notes);
	}
	else if (whichLevel == "configuration")
	{
		this->notes.setConfigurationNotes(notes);
	}
	else
	{
		throw std::invalid_argument(("The Main window's setNotes function was called with a bad argument:"
			+ whichLevel + ". Acceptable arguments are \"experiment\", \"category\", and \"configuration\". "
			"This throw can be continued successfully, the notes will just not load.").c_str());
	}
	return;
}

std::vector<variable> MainWindow::getAllVariables()
{
	return this->variables.getAllVariables();
}

void MainWindow::clearVariables()
{
	this->variables.clearVariables();
	return;
}

void MainWindow::addVariable(std::string name, bool timelike, bool singleton, double value, int item)
{
	this->variables.addVariable(name, timelike, singleton, value, item);
	return;
}

debugOptions MainWindow::getDebuggingOptions()
{
	return this->debugger.getOptions();
}

void MainWindow::setDebuggingOptions(debugOptions options)
{
	this->debugger.setOptions(options);
	return;
}

mainOptions MainWindow::getMainOptions()
{
	return this->settings.getOptions();
}

void MainWindow::updateStatusText(std::string whichStatus, std::string text)
{
	std::transform(whichStatus.begin(), whichStatus.end(), whichStatus.begin(), ::tolower);
	if (whichStatus == "error")
	{
		this->errorStatus.addStatusText(text);
	}
	else if (whichStatus == "debug")
	{
		this->debugStatus.addStatusText(text);
	}
	else if (whichStatus == "main")
	{
		this->mainStatus.addStatusText(text);
	}
	else
	{
		throw std::invalid_argument("Main Window's updateStatusText function recieved a bad argument for which status"
			" control to update. Options are \"error\", \"debug\", and \"main\", but recieved " + whichStatus + ". This"
			"exception can be safely ignored.");
	}
	return;
}

void MainWindow::addTimebar(std::string whichStatus)
{
	std::transform(whichStatus.begin(), whichStatus.end(), whichStatus.begin(), ::tolower);
	if (whichStatus == "error")
	{
		this->errorStatus.appendTimebar();
	}
	else if (whichStatus == "debug")
	{
		this->debugStatus.appendTimebar();
	}
	else if (whichStatus == "main")
	{
		this->mainStatus.appendTimebar();
	}
	else
	{
		throw std::invalid_argument("Main Window's addTimebar function recieved a bad argument for which status"
			" control to update. Options are \"error\", \"debug\", and \"main\", but recieved " + whichStatus + ". This"
			"exception can be safely ignored.");
	}
	return;
	return;
}

void MainWindow::setShortStatus(std::string text)
{
	this->shortStatus.setText(text);
	return;
}

void MainWindow::changeShortStatusColor(std::string color)
{
	this->shortStatus.setColor(color);
	return;
}

void MainWindow::passDebugPress(UINT id)
{
	this->debugger.handleEvent(id, this);
	return;
}

void MainWindow::passMainOptionsPress(UINT id)
{
	this->settings.handleEvent(id, this);
	return;
}

void MainWindow::listViewDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	variables.updateVariableInfo(this, this->TheScriptingWindow);
	this->profile.updateConfigurationSavedStatus(false);
	return;
}
void MainWindow::handleRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	variables.deleteVariable();
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MainWindow::handleExperimentCombo()
{
	this->profile.experimentChangeHandler(this->TheScriptingWindow, this);
	return;
}
void MainWindow::handleCategoryCombo()
{
	this->profile.categoryChangeHandler(this->TheScriptingWindow, this);
}
void MainWindow::handleConfigurationCombo()
{
	this->profile.configurationChangeHandler(this->TheScriptingWindow, this);
}
void MainWindow::handleSequenceCombo()
{
	this->profile.sequenceChangeHandler();
}
void MainWindow::handleOrientationCombo()
{
	try
	{
		this->profile.orientationChangeHandler(this);
	}
	catch (my_exception& except)
	{
		colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
		comm.sendColorBox( colors );
		comm.sendError("ERROR: failed to change orientation: " + except.whatStr());
	}
}

void MainWindow::changeBoxColor( colorBoxes<char> colors )
{
	this->boxes.changeColor( colors );
}

void MainWindow::setMainOptions(mainOptions options)
{
	this->settings.setOptions(options);
	return;
}

LRESULT MainWindow::onStatusTextMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	this->mainStatus.addStatusText(statusMessage);
	return 0;
}

LRESULT MainWindow::onErrorMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	this->errorStatus.addStatusText(statusMessage);
	return 0;
}

LRESULT MainWindow::onFatalErrorMessage(WPARAM wParam, LPARAM lParam)
{
	// normal msg stuff
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	this->errorStatus.addStatusText(statusMessage);
	//
	myAgilent::agilentDefault();
	std::string msgText = "Exited with Error!\r\nPassively Outputting Default Waveform.";
	this->changeShortStatusColor("R");
	colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
	comm.sendColorBox( colors );
	std::string orientation = this->getCurentProfileSettings().orientation;
	try
	{
		niawg.restartDefault();
		colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
		comm.sendError("EXITED WITH ERROR!");
		comm.sendColorBox( colors );
		comm.sendStatus("EXITED WITH ERROR!\r\nInitialized Default Waveform\r\n");
	}
	catch (my_exception& except)
	{
		colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
		comm.sendError("EXITED WITH ERROR! " + except.whatStr());
		comm.sendColorBox( colors );
		comm.sendStatus("EXITED WITH ERROR!\r\nNIAWG RESTART FAILED!\r\n");
	}
	this->setNiawgRunningState( false );
	return 0;
}

/*
LRESULT MainWindow::onVariableStatusMessage(WPARAM wParam, LPARAM lParam)
{
	int currentOutput = (int)lParam;
	std::string msgText = "Outpitting Series #" + std::to_string(currentOutput) + ". \r\nWriting Varying Waveforms for Set # "
		+ std::to_string(currentOutput + 1) + "...\r\n";
	this->setShortStatus(msgText);
	this->changeShortStatusColor("Y");
	this->theScriptingWindow->changeBoxColor("Y");
	return 0;
}
*/

LRESULT MainWindow::onNormalFinishMessage(WPARAM wParam, LPARAM lParam)
{
	myAgilent::agilentDefault();
	std::string msgText = "Passively Outputting Default Waveform";
	this->setShortStatus(msgText);
	this->changeShortStatusColor("B");
	colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
	comm.sendColorBox( colors );
	std::string orientation = this->getCurentProfileSettings().orientation;
	try
	{
		niawg.restartDefault();
	}
	catch (my_exception& except)
	{
		colorBoxes<char> colors = { /*niawg*/'R', /*camera*/'-', /*intensity*/'-' };
		comm.sendError("ERROR! The niawg finished normally, but upon restarting the default waveform, threw the "
			"following error: " + except.whatStr());
		comm.sendColorBox( colors );
		comm.sendStatus("ERROR!\r\n");
		return -3;
	}
	this->setNiawgRunningState( false );
	return 0;
}

Communicator* MainWindow::getComm()
{
	return &comm;
}

LRESULT MainWindow::onColoredEditMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	this->setShortStatus(statusMessage);
	return 0;
}

LRESULT MainWindow::onDebugMessage(WPARAM wParam, LPARAM lParam)
{
	char* pointerToMessage = (char*)lParam;
	std::string statusMessage(pointerToMessage);
	delete[] pointerToMessage;
	this->debugStatus.addStatusText(statusMessage);
	return 0;
}

std::unordered_map<std::string, CBrush*> MainWindow::getBrushes()
{
	return this->mainBrushes;
}
std::unordered_map<std::string, COLORREF> MainWindow::getRGB()
{
	return this->mainRGBs;
}
