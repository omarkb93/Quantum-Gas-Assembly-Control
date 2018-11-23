﻿#pragma once
#include "Windows.h"
#include "Control.h"
#include "resource.h"



class ErrDialog : public CDialog
{
	DECLARE_DYNAMIC ( ErrDialog );

	public:
		enum class type
		{
			error,
			info
		};

		ErrDialog ( std::string description, type dlgType_ ) : CDialog ( IDD_ERROR_DIALOG )
		{
			dlgType = dlgType_;
			descriptionText = description;
		}
		BOOL OnInitDialog ( ) override;
		HBRUSH OnCtlColor ( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
		void catchOk ( );
	private:
		type dlgType;
		DECLARE_MESSAGE_MAP ( );
		std::string descriptionText;
		Control<CEdit> description;
		Control<CEdit> header;
};
