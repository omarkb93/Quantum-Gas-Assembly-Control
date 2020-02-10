// created by Mark O. Brown
#include "stdafx.h"
#include "TextPromptDialog.h"

IMPLEMENT_DYNAMIC(TextPromptDialog, CDialog)

BEGIN_MESSAGE_MAP(TextPromptDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_COMMAND(IDOK, &TextPromptDialog::catchOk)
	ON_COMMAND(IDCANCEL, &TextPromptDialog::catchCancel)
	ON_COMMAND(ID_ACCELERATOR_F5 , &TextPromptDialog::catchf5)
END_MESSAGE_MAP()

void TextPromptDialog::catchf5( )
{
	errBox( "hi!" );
}


BOOL TextPromptDialog::OnInitDialog()
{
	DWORD options = WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (passwordOption)
	{
		options |= ES_PASSWORD;
	}
	description.Create(cstr(descriptionText), options | ES_READONLY, { 0,0,1000,75 }, this, 0);
	prompt.Create( options, { 0,75,1000,150 }, this, 0 );
	return TRUE;
}

void TextPromptDialog::catchCancel()
{
	*result = "";
	EndDialog(1);
}

void TextPromptDialog::catchOk()
{
	CString text;
	prompt.GetWindowText(text);
	std::string textStr(text);
	*result = cstr(textStr);
	EndDialog(0);
}