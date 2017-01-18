#include "stdafx.h"
#include "MasterWindow.h"
#include "Control.h"
//#include "newVariableDialogProcedure.h"
#include "constants.h"
#include "viewAndChangeTTL_NamesProcedure.h"
#include "viewAndChangeDAC_NamesProcedure.h"
#include "textPromptDialogProcedure.h"
#include "TTL_System.h"
#include "explorerOpen.h"

IMPLEMENT_DYNAMIC(MasterWindow, CDialog)

BEGIN_MESSAGE_MAP(MasterWindow, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(TTL_ID_BEGIN, TTL_ID_END, &MasterWindow::handleTTLPush)
	ON_COMMAND(TTL_HOLD, &MasterWindow::handlTTLHoldPush)
	ON_COMMAND(ID_TTLS_VIEW_OR_CHANGE_TTL_NAMES, &MasterWindow::ViewOrChangeTTLNames)
	ON_COMMAND(ID_DACS_VIEW_OR_CHANGE_DAC_NAMES, &MasterWindow::ViewOrChangeDACNames)
	ON_COMMAND(ID_MASTER_SAVEMASTERCONFIGURATION, MasterWindow::SaveMasterConfig)
	ON_COMMAND(IDM_EXIT, &MasterWindow::Exit)
	ON_COMMAND(ID_MASTERSCRIPT_SAVESCRIPT, &MasterWindow::SaveMasterScript)
	ON_COMMAND(ID_MASTERSCRIPT_SAVESCRIPTAS, &MasterWindow::SaveMasterScriptAs)
	ON_COMMAND(ID_MASTERSCRIPT_NEWSCRIPT, &MasterWindow::NewMasterScript)
	ON_COMMAND(ID_MASTERSCRIPT_OPENSCRIPT, &MasterWindow::OpenMasterScript)
	ON_COMMAND(ID_MASTERSCRIPT_SAVEFUNCTION, &MasterWindow::SaveMasterFunction)
	ON_COMMAND(ID_CONFIGURATION_DELETE_CURRENT_CONFIGURATION, &MasterWindow::DeleteConfiguration)
	ON_COMMAND(ID_CONFIGURATION_NEW_CONFIGURATION, &MasterWindow::NewConfiguration)
	ON_COMMAND(ID_CONFIGURATION_SAVECONFIGURATIONSETTINGS, &MasterWindow::SaveConfiguration)
	ON_COMMAND(ID_CONFIGURATION_SAVE_CONFIGURATION_AS, &MasterWindow::SaveConfigurationAs)
	ON_COMMAND(ID_CONFIGURATION_RENAME_CURRENT_CONFIGURATION, &MasterWindow::RenameConfiguration)
	ON_COMMAND(ID_CATEGORY_SAVECATEGORYSETTINGS, &MasterWindow::SaveCategory)
	ON_COMMAND(ID_CATEGORY_SAVECATEGORYSETTINGSAS, &MasterWindow::SaveCategoryAs)
	ON_COMMAND(ID_CATEGORY_RENAME_CURRENT_CATEGORY, &MasterWindow::RenameCategory)
	ON_COMMAND(ID_CATEGORY_DELETE_CURRENT_CATEGORY, &MasterWindow::DeleteCategory)
	ON_COMMAND(ID_CATEGORY_NEW_CATEGORY, &MasterWindow::NewCategory)
	ON_COMMAND(ID_EXPERIMENT_NEW_EXPERIMENT_TYPE, &MasterWindow::NewExperiment)
	ON_COMMAND(ID_EXPERIMENT_DELETE_CURRENT_EXPERIMENT, &MasterWindow::DeleteExperiment)
	ON_COMMAND(ID_EXPERIMENT_SAVEEXPERIMENTSETTINGS, &MasterWindow::SaveExperiment)
	ON_COMMAND(ID_EXPERIMENT_SAVEEXPERIMENTSETTINGSAS, &MasterWindow::SaveExperimentAs)
	ON_COMMAND(ID_SEQUENCE_ADD_TO_SEQUENCE, &MasterWindow::AddToSequence)
	ON_COMMAND(ID_SEQUENCE_DELETE_SEQUENCE, &MasterWindow::DeleteSequence)
	ON_COMMAND(ID_SEQUENCE_RESET_SEQUENCE, &MasterWindow::ResetSequence)
	ON_COMMAND(ID_SEQUENCE_RENAMESEQUENCE, &MasterWindow::RenameSequence)
	ON_COMMAND(ID_SEQUENCE_NEW_SEQUENCE, &MasterWindow::NewSequence)
	ON_COMMAND(SET_REPETITION_ID, &MasterWindow::SetRepetitionNumber)
	ON_COMMAND(ID_ERROR_CLEAR, &MasterWindow::ClearError)
	ON_COMMAND(ID_STATUS_CLEAR, &MasterWindow::ClearGeneral)
	ON_COMMAND(ID_DAC_SET_BUTTON, &MasterWindow::SetDacs)
	ON_COMMAND(ID_FILE_EXPORTSETTINGSLOG, &MasterWindow::LogSettings)
	ON_COMMAND(ID_FILE_RUN, &MasterWindow::StartExperiment)
	ON_COMMAND(ID_PROFILE_SAVE_PROFILE, &MasterWindow::SaveEntireProfile)

	ON_CBN_SELENDOK(EXPERIMENT_COMBO_ID, &MasterWindow::OnExperimentChanged)
	ON_CBN_SELENDOK(CATEGORY_COMBO_ID, &MasterWindow::OnCategoryChanged)
	ON_CBN_SELENDOK(CONFIGURATION_COMBO_ID, &MasterWindow::OnConfigurationChanged)
	ON_CBN_SELENDOK(SEQUENCE_COMBO_ID, &MasterWindow::OnSequenceChanged)
	ON_CBN_SELENDOK(ORIENTATION_COMBO_ID, &MasterWindow::OnOrientationChanged)
	ON_CBN_SELENDOK(FUNCTION_COMBO_ID, &MasterWindow::HandleFunctionChange)

	ON_CONTROL_RANGE(EN_CHANGE, ID_DAC_FIRST_EDIT, (ID_DAC_FIRST_EDIT + 24), &MasterWindow::DAC_EditChange)

	ON_EN_CHANGE(MASTER_RICH_EDIT, &MasterWindow::EditChange)
	ON_EN_CHANGE(CONFIGURATION_NOTES_ID, &MasterWindow::ConfigurationNotesChange)
	ON_EN_CHANGE(CATEGORY_NOTES_ID, &MasterWindow::CategoryNotesChange)
	ON_EN_CHANGE(EXPERIMENT_NOTES_ID, &MasterWindow::ExperimentNotesChange)
	
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(LVN_COLUMNCLICK, LISTVIEW_CONTROL, &MasterWindow::ListViewClick)
	ON_NOTIFY(NM_DBLCLK, LISTVIEW_CONTROL, &MasterWindow::ListViewDblClick)
	ON_NOTIFY(NM_RCLICK, LISTVIEW_CONTROL, &MasterWindow::ListViewRClick)

END_MESSAGE_MAP()

void MasterWindow::HandleAbort()
{
	errBox( "ABORT" );
}

void MasterWindow::OnCancel()
{
	int answer = MessageBox( "Are you sure you'd like to exit?", "Exit", MB_OKCANCEL );
	if (answer == IDOK)
	{
		this->CDialog::OnCancel();
	}
}

void MasterWindow::HandleFunctionChange()
{
	this->masterScript.functionChangeHandler(this);
	return;
}

void MasterWindow::StartExperiment()
{
	
	// check to make sure ready.
	try
	{
		this->generalStatus.appendText( "Checking Read status...\r\n", 0 );
		this->profile.allSettingsReadyCheck( this );
		this->masterScript.checkSave( this );
		this->generalStatus.appendText( "Starting Experiment Thread...\r\n", 0 );
		this->manager.startExperimentThread( this );
	}
	catch (myException& exception)
	{
		this->generalStatus.appendText( "ABORTED! " + exception.whatStr() + "\r\n", 0 );
	}

	// actually start now...?.?.?.
	return;
}


void MasterWindow::ConfigurationNotesChange()
{
	this->profile.updateConfigurationSavedStatus(false);
	return;
}


void MasterWindow::CategoryNotesChange()
{
	this->profile.updateCategorySavedStatus(false);
	return;
}


void MasterWindow::ExperimentNotesChange()
{
	this->profile.updateExperimentSavedStatus(false);
	return;
}


void MasterWindow::SaveEntireProfile()
{
	this->profile.saveEntireProfile(this);
	return;
}


void MasterWindow::LogSettings()
{
	this->logger.generateLog(this);
	this->logger.exportLog();
	return;
}


void MasterWindow::SetDacs()
{
	// have the dac values change
	try
	{
		this->generalStatus.appendText( "Starting Setting Dacs...\r\n", 0 );
		this->dacBoards.handleButtonPress( &this->ttlBoard );
		this->generalStatus.appendText( "Analyzing Dac Info...\r\n", 0 );
		dacBoards.analyzeDAC_Commands();
		this->generalStatus.appendText( "Finalizing Dacs Info...\r\n", 0 );
		dacBoards.makeFinalDataFormat();
		// start the boards which actually sets the dac values.
		this->generalStatus.appendText( "Configuring Clocks...\r\n", 0 );
		dacBoards.configureClocks();
		this->generalStatus.appendText( "Stopping Dacs (if running)...\r\n", 0 );
		dacBoards.stopDacs();
		this->generalStatus.appendText( "Writing New Dac Settings...\r\n", 0 );
		dacBoards.writeDacs();
		this->generalStatus.appendText( "Arming Dacs...\r\n", 0 );
		dacBoards.startDacs();
		this->generalStatus.appendText( "Analyzing DAC TTL Triggers...\r\n", 0 );
		ttlBoard.analyzeCommandList();
		this->generalStatus.appendText( "Finalizing DAC TTL Triggers...\r\n", 0 );
		ttlBoard.convertToFinalFormat();
		this->generalStatus.appendText( "Writing DAC TTL Triggers...\r\n", 0 );
		ttlBoard.writeData();
		this->generalStatus.appendText( "Starting DAC TTL Triggers...\r\n", 0 );
		ttlBoard.startBoard();
		this->generalStatus.appendText( "Waiting until TTLs are finished...\r\n", 0 );
		ttlBoard.waitTillFinished();
		this->generalStatus.appendText( "Finished Setting Dacs.\r\n", 0 );
	}
	catch (myException& exception)
	{
		errBox( exception.what() );
		this->generalStatus.appendText( "Failed!\r\n", 0 );
		this->errorStatus.appendText( exception.what(), 0 );
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}


void MasterWindow::DAC_EditChange(UINT id)
{
	this->dacBoards.handleEditChange(id - ID_DAC_FIRST_EDIT);
	return;
}


void MasterWindow::ClearError()
{
	this->errorStatus.clear();
	return;
}

void MasterWindow::ClearGeneral()
{
	this->generalStatus.clear();
	return;
}

void MasterWindow::SetRepetitionNumber()
{
	if (!this->repetitionControl.handleButtonPush())
	{
		this->errorStatus.appendText("Set Repetition Number Failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::ListViewClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	if (!this->variables.handleClick(pNotifyStruct, result))
	{
		this->errorStatus.appendText("Handling variable listview click failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::OnExperimentChanged()
{
	try
	{
		this->profile.experimentChangeHandler( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Handling Experiment Selection Change Failed!\r\n", 0 );
	}
}


void MasterWindow::OnCategoryChanged()
{
	try
	{
		this->profile.categoryChangeHandler( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Handling Category Selection Change Failed!\r\n", 0 );
	}
}


void MasterWindow::OnConfigurationChanged()
{
	try
	{
		this->profile.configurationChangeHandler( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Handling Configuration Selection Change Failed!\r\n", 0 );
	}
}


void MasterWindow::OnSequenceChanged()
{
	try
	{
		this->profile.sequenceChangeHandler( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Handling Sequence Selection Change Failed!\r\n", 0 );
	}
}


void MasterWindow::OnOrientationChanged()
{
	try
	{
		this->profile.orientationChangeHandler( this );
	}
	catch (myException* exception)
	{
		this->errorStatus.appendText( "Handling Orientation Selection Change Failed!\r\n", 0 );
	}
}


void MasterWindow::DeleteConfiguration()
{
	try
	{
		this->profile.deleteConfiguration();
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Deleting Configuration Failed!\r\n", 0 );
	}
}


void MasterWindow::NewConfiguration()
{
	try
	{
		this->profile.newConfiguration( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "New Configuration Failed!\r\n", 0 );
	}
}


void MasterWindow::SaveConfiguration()
{
	try
	{
		this->profile.saveConfigurationOnly( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Saving Configuration Failed!\r\n", 0 );
	}
}


void MasterWindow::SaveConfigurationAs()
{
	try
	{
		this->profile.saveConfigurationAs( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Saving Configuration As Failed!\r\n", 0 );
	}
}


void MasterWindow::RenameConfiguration()
{
	try
	{
		this->profile.renameConfiguration( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Renaming Configuration Failed!\r\n", 0 );
	}
	return;
}


void MasterWindow::SaveCategory()
{
	try
	{
		this->profile.saveCategoryOnly( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Saving Category Failed!\r\n", 0 );
	}
}


void MasterWindow::SaveCategoryAs()
{
	try
	{
		this->profile.saveCategoryAs( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Saving Category As Failed!\r\n", 0 );
	}
}


void MasterWindow::RenameCategory()
{
	try
	{
		this->profile.renameCategory();
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Renaming Category Failed!\r\n", 0 );
	}
}


void MasterWindow::DeleteCategory()
{
	try
	{
		this->profile.deleteCategory();
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Deleting Category Failed!\r\n", 0);
	}
}


void MasterWindow::NewCategory()
{
	try
	{
		this->profile.newCategory( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("New Category Failed!\r\n", 0);
	}
}


void MasterWindow::NewExperiment()
{
	try
	{
		this->profile.newExperiment( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("New Experiment Failed!\r\n", 0);
	}
}


void MasterWindow::DeleteExperiment()
{
	try
	{
		this->profile.deleteExperiment();
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Delete Experiment Failed!\r\n", 0);
	}
}


void MasterWindow::SaveExperiment()
{
	try
	{
		this->profile.saveExperimentOnly( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Save Experiment Failed!\r\n", 0);
	}
}


void MasterWindow::SaveExperimentAs()
{
	try
	{
		this->profile.saveExperimentAs( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "Save Experiment As Failed!\r\n", 0 );
	}
}


void MasterWindow::AddToSequence()
{
	try
	{
		this->profile.addToSequence( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Add to Sequence Failed!\r\n", 0);
	}
}


void MasterWindow::DeleteSequence()
{
	try
	{
		this->profile.deleteSequence( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Delete Sequence Failed!\r\n", 0);
	}
}


void MasterWindow::ResetSequence()
{
	//this->profile.???
}


void MasterWindow::RenameSequence()
{
	try
	{
		this->profile.renameSequence( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("Rename Sequence Failed!\r\n", 0);
	}
}
void MasterWindow::NewSequence()
{
	try
	{
		this->profile.newSequence( this );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText("New Sequence Failed!\r\n", 0);
	}
	return;
}

//

void MasterWindow::SaveMasterScript()
{
	if (!this->masterScript.saveScript(this))
	{
		this->errorStatus.appendText("Save Master Script Failed!\r\n", 0);
	}
	return;
}

void MasterWindow::SaveMasterScriptAs()
{
	if (this->profile.getCurrentPathIncludingCategory() == "")
	{
		MessageBox("Please select a category before trying to save a the Master Script!", 0, 0);
	}
	std::string scriptName = (const char*)DialogBoxParam(this->programInstance, MAKEINTRESOURCE(IDD_TEXT_PROMPT_DIALOG), 0, (DLGPROC)textPromptDialogProcedure, (LPARAM)"Please enter new master script name.");
	if (scriptName == "")
	{
		return;
	}
	if (!this->masterScript.saveScriptAs(this->profile.getCurrentPathIncludingCategory() + scriptName + MASTER_SCRIPT_EXTENSION, this))
	{
		this->errorStatus.appendText("Save Master Script As Failed!\r\n", 0);
	}
	return;
}
void MasterWindow::NewMasterScript()
{
	if (!this->masterScript.newScript(this))
	{
		this->errorStatus.appendText("New Master Script Failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}
void MasterWindow::OpenMasterScript()
{
	// ???
	std::string address = explorerOpen(this, "*.mScript\0All\0 * .*", this->profile.getCurrentPathIncludingCategory());
	if (address == "")
	{
		return;
	}
	if (!this->masterScript.openParentScript(address, this))
	{
		this->errorStatus.appendText("Open Master Script Failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::SaveMasterFunction()
{
	if (!this->masterScript.saveAsFunction())
	{
		this->errorStatus.appendText("Save Master Script Function Failed!\r\n", 0);
	}
	return;
}

void MasterWindow::ListViewDblClick(NMHDR * pNotifyStruct, LRESULT * result)
{	
	std::vector<Script*> scriptList;
	scriptList.push_back(&masterScript);
	if (!this->variables.updateVariableInfo(scriptList, this))
	{
		this->errorStatus.appendText("Variables Double Click Handler Failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::ListViewRClick(NMHDR * pNotifyStruct, LRESULT * result)
{
	if (!this->variables.deleteVariable())
	{
		this->errorStatus.appendText("Variables Right Click Handler Failed!\r\n", 0);
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::OnTimer(UINT TimerVal)
{
	if (!this->masterScript.handleTimerCall(this))
	{
		this->errorStatus.appendText("Timer Call Handler Failed!\r\n", 0);
	}
	return;
}

void MasterWindow::EditChange()
{
	if (!this->masterScript.handleEditChange(this))
	{
		this->errorStatus.appendText("Edit Change Handler Failed!\r\n", 0);
	}
	return;
}

void MasterWindow::handleTTLPush(UINT id)
{
	try
	{
		this->ttlBoard.handleTTLPress( id );
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "TTL Press Handler Failed: " + exception.whatStr() + "\r\n", 0 );
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::handlTTLHoldPush()
{
	try
	{
		this->ttlBoard.handleHoldPress();
	}
	catch (myException& exception)
	{
		this->errorStatus.appendText( "TTL Hold Handler Failed: " + exception.whatStr() + "\r\n", 0 );
	}
	this->profile.updateConfigurationSavedStatus(false);
	return;
}

void MasterWindow::ViewOrChangeTTLNames()
{
	ttlInputStruct input;
	input.ttls = &this->ttlBoard;
	input.toolTips = this->toolTips;
	DialogBoxParam(this->programInstance, MAKEINTRESOURCE(IDD_VIEW_AND_CHANGE_TTL_NAMES), 0, (DLGPROC)viewAndChangeTTL_NamesProcedure, (LPARAM)&input);
	return;
}

void MasterWindow::ViewOrChangeDACNames()
{
	dacInputStruct input;
	input.dacs = &this->dacBoards;
	input.toolTips = this->toolTips;
	DialogBoxParam(this->programInstance, MAKEINTRESOURCE(IDD_VIEW_AND_CHANGE_DAC_NAMES), 0, (DLGPROC)viewAndChangeDAC_NamesProcedure, (LPARAM)&input);
	return;
}

void MasterWindow::SaveMasterConfig()
{
	if (!this->masterConfig.save(&this->ttlBoard, &this->dacBoards))
	{
		this->errorStatus.appendText("Master Configuration Save Handler Failed!\r\n", 0);
	}
	return;
}

void MasterWindow::Exit()
{
	this->EndDialog(0);
	return;
}

MasterWindow::~MasterWindow()
{
	if (masterWindowHandle)
	{
		DestroyWindow();
	}
}

HBRUSH MasterWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH result = this->ttlBoard.handleColorMessage(pWnd, this->masterBrushes, this->masterRGBs, pDC);
	if (result != NULL)
	{
		return result;
	}
	result = this->dacBoards.handleColorMessage(pWnd, this->masterBrushes, this->masterRGBs, pDC);
	if (result != NULL)
	{
		return result;
	}
	switch (nCtlColor)
	{

		case CTLCOLOR_STATIC:
		{
			pDC->SetTextColor(masterRGBs["Gold"]);
			pDC->SetBkColor(masterRGBs["Medium Grey"]);
			return masterBrushes["Medium Grey"];
		}
		case CTLCOLOR_EDIT:
		{
			pDC->SetTextColor(masterRGBs["White"]);
			pDC->SetBkColor(masterRGBs["Dark Grey"]);
			return masterBrushes["Dark Grey"];
		}
		case CTLCOLOR_LISTBOX:
		{
			pDC->SetTextColor(masterRGBs["White"]);
			pDC->SetBkColor(masterRGBs["Dark Grey"]);
			return masterBrushes["Dark Grey"];
		}
		default:
			return masterBrushes["Light Grey"];
	}
}

BOOL MasterWindow::PreTranslateMessage(MSG* pMsg)
{
	for (int toolTipInc = 0; toolTipInc < this->toolTips.size(); toolTipInc++)
	{
		this->toolTips[toolTipInc]->RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL MasterWindow::OnInitDialog()
{
	this->InitializeWindowControls();
	menu.LoadMenu(IDC_MASTERCODE);
	this->SetMenu(&menu);
	this->ShowWindow(SW_MAXIMIZE);
	return TRUE;
}

LRESULT MasterWindow::MasterWindowMessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			InitializeWindowControls();
			break;
		}
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		{
			INT_PTR result = NULL;// = this->ttlBoard.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->notes.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->errorStatus.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->generalStatus.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->variables.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			/*
			result = this->dacBoards.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			*/
			result = this->repetitionControl.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->profile.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			result = this->masterScript.handleColorMessage(hwnd, message, wParam, lParam, this->masterBrushes);
			if (result != NULL)
			{
				return result;
			}
			break;
		}
		case WM_NOTIFY:
		{
			int notifyMessage = ((LPNMHDR)lParam)->code;
			switch (notifyMessage)
			{
				case TTN_GETDISPINFOW:
				case TTN_GETDISPINFOA:
				{
					static CString mystring;
					mystring.ReleaseBuffer();
					LPNMTTDISPINFO info = (LPNMTTDISPINFO)lParam;
					mystring = this->toolTipText[info->hdr.hwndFrom].c_str();
					char text[1000];// = new char[this->toolTipText[info->hdr.hwndFrom].size() + 100];
					//text[this->toolTipText[info->hdr.hwndFrom].size()] = 0;
					//std::copy(this->toolTipText[info->hdr.hwndFrom].begin(), this->toolTipText[info->hdr.hwndFrom].end(), text);
					//strcpy_s(text, "");
					//strcpy_s(text, 1000, this->toolTipText[info->hdr.hwndFrom].c_str());
					
					strcpy_s(text, 1000, "testing");
					//TCHAR* myArray(this->toolTipText[info->hdr.hwndFrom].c_str());
					info->lpszText = mystring.GetBuffer();// mystring.GetBuffer(mystring.GetLength());//(LPSTR)this->toolTipText[info->hdr.hwndFrom].c_str();
					//info->
					info->hinst = NULL;
					//SendMessage(pInfo->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 200);
					break;
				}
				case NM_DBLCLK:
				{
					break;
				}
				case NM_RCLICK:
				{
					break;
				}
			}
			break;
		}
		case WM_DESTROY:
		{
			MasterWindow * pMasterWindow = new MasterWindow(9000);
			pMasterWindow = (MasterWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (pMasterWindow)
			{
				pMasterWindow->masterWindowHandle = 0;
			}
			break;
		}
		case WM_TIMER:
		{
			if (this->masterScript.coloringIsNeeded())
			{
				this->masterScript.handleTimerCall(this);
			}
			break;
		}
		/// Handle Everything Else...///////////////////////
		case WM_COMMAND:
		{
			/*
			if (!this->masterScript.handleEditChange(wParam, lParam, hwnd))
			{
				break;
			}
			*/
			if (!this->masterScript.childComboChangeHandler(wParam, lParam, this))
			{
				break;
			}
			/*
			if (!ttlBoard.handleTTLPress(wParam, lParam))
			{
				break;
			}
		
			if (!ttlBoard.handleHoldPress(wParam, lParam))
			{
				break;
			}	*/
			switch (LOWORD(wParam))
			{
				case ID_MASTERSCRIPT_SAVESCRIPT:
				{	
					break;
				}
				case ID_MASTERSCRIPT_SAVEFUNCTION:
				{
					break;
				}
				case ID_MASTER_SAVEMASTERCONFIGURATION:
				{
					this->masterConfig.save(&this->ttlBoard, &this->dacBoards);
					break;
				}
			}
		}
		default:
		{
			return DefWindowProc(message, wParam, lParam);
		}
	}
	return DefWindowProc(message, wParam, lParam);
}

int MasterWindow::InitializeWindowControls()
{
	masterWindowHandle = this->GetSafeHwnd();
	
	POINT controlLocation{ 0, 0 };
	profile.initialize(controlLocation, *this, this->toolTips, this);
	variables.initialize(controlLocation, masterWindowHandle, this->toolTips, this);
	repetitionControl.initialize(controlLocation, masterWindowHandle, this->toolTips, this);
	ttlBoard.initialize(controlLocation, masterWindowHandle, this->programInstance, this->toolTipText, this->toolTips, this);
	dacBoards.initialize(controlLocation, masterWindowHandle, this->toolTips, this);
	masterConfig.load(&this->ttlBoard, this->dacBoards, this->toolTips, this);
	
	RECT controlArea = { 960, 0, 1320, 540 };
	//RECT editSize, HWND windowHandle, std::string titleText
	generalStatus.initialize(controlArea, masterWindowHandle, "Status", this->masterRGBs["Light Blue"]);
	
	controlArea = { 960, 540, 1320, 1080};
	errorStatus.initialize(controlArea, masterWindowHandle, "Errors", this->masterRGBs["Light Red"]);

	controlLocation = POINT{ 480, 90 };
	notes.initializeControls(controlLocation, masterWindowHandle);
	controlLocation = POINT{ 1320, 0 };
	masterScript.initialize(600, 1080, controlLocation, this->toolTips, this->masterWindowHandle, this);
	return 0;
}

std::unordered_map<std::string, COLORREF> MasterWindow::getRGBs()
{
	return this->masterRGBs;
}