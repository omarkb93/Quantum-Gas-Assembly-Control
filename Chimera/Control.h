#pragma once

#include "commonTypes.h"
#include "CameraPositions.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <unordered_map>
#include <string>
#include <type_traits>


static HFONT toolTipFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, 
									   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));

/*
Some standard IDs for controls. Can be combined with other options for specific controls. Use of these shortens and 
standardizes control initializations.
*/
const DWORD NORM_CWND_OPTIONS = WS_VISIBLE | WS_CHILD;
const DWORD NORM_PUSH_OPTIONS = NORM_CWND_OPTIONS | BS_DEFPUSHBUTTON | WS_TABSTOP;
const DWORD NORM_STATIC_OPTIONS = NORM_CWND_OPTIONS | ES_READONLY | ES_CENTER | SS_ENDELLIPSIS;
const DWORD NORM_HEADER_OPTIONS = NORM_STATIC_OPTIONS | WS_BORDER | ES_SUNKEN;
const DWORD NORM_EDIT_OPTIONS = NORM_CWND_OPTIONS | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN;
const DWORD NORM_COMBO_OPTIONS = NORM_CWND_OPTIONS | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_TABSTOP;
const DWORD NORM_RADIO_OPTIONS = NORM_CWND_OPTIONS | BS_AUTORADIOBUTTON | WS_TABSTOP;
const DWORD NORM_CHECK_OPTIONS = NORM_CWND_OPTIONS | BS_AUTOCHECKBOX | WS_TABSTOP | BS_RIGHT;
const DWORD NORM_LISTVIEW_OPTIONS = NORM_CWND_OPTIONS | WS_BORDER | LVS_REPORT | LVS_EDITLABELS;
/*
 * This is a complex class definition. The first line here declares that this is a template class with a class template
 * argument named ControlType.  I.e. my class is based on a class called ControlType.
 * 
 * The second line forces ControlType to be inherited from some CWnd class, i.e. ControlType should always be
 * some typical MFC control, the purpose of which is clear in the next line.
 *
 * the last line names the template class to be "Control" and says that it inherits it's template argument. This allows
 * me to quickly create class objects based on existing MFC class objects which all have a set of useful additional 
 * parameters that I make use of in my GUI. It ensures that the user correctly inherits from an MFC type both because
 * this is simply what this is designed for and also so that I can use CWnd functions inside the rearrange function 
 * here.
 */
template <class ControlType> class Control : public ControlType
{
	public:
		Control();
		// Standard Position: for objects that don't move around and have permanent positions in their windows.
		RECT sPos;
		// Kinetic Series Mode Positon: position for camera-mode-sensitive objects in kinetic series mode.
		RECT seriesPos;
		// Accumulate Mode Position
		RECT amPos;
		// Continuous Single Scan Mode Position
		RECT videoPos;
		fontTypes fontType;
		int triggerModeSensitive;
		//
		int colorState = 0;
		void rearrange(int width, int height, fontMap fonts);
		void rearrange(std::string cameraMode, std::string trigMode, int width, int height, fontMap fonts);
		void rearrange( std::string cameraMode, std::string trigMode, int width, int height );
		void rearrange( int width, int height);
		void setToolTip( std::string text, cToolTips& tooltips, CWnd* master );
		void setPositions( cameraPositions& pos, LONG xoff, LONG yoff, LONG width, LONG height,
						   bool yInc = false, bool xInc=false, bool videoToo=false );
	private:
		int toolTipID;
		CToolTipCtrl toolTip;
		bool toolTipIsSet;
};


template<class ControlType> 
Control<ControlType>::Control()
{
	// assert that the template class is derived from CWnd. This doesn't actually do anything in run-time. It's also
	// probably redundant because of all the functionality designed around CWnd in this class, like the below function.
	ControlType obj;
}


template <class ControlType>
void Control<ControlType>::setPositions( cameraPositions& pos, LONG xoff, LONG yoff, LONG width, LONG height,
										 bool yInc = false, bool xInc = false, bool videoToo = false )
{
	/*
	 * This function, while a little logicy itself, makes setting the positions on the camera window much easier.
	 * @param pos: the object tracking the position of the next control in all modes.
	 * @param xoff: the x offset of the position from pos.
	 * @param yoff: the y offset of the position from pos.
	 * @param width: the width of the control.
	 * @param height: the height of the control.
	 * @param xInc: whether to increment pos.x or not.
	 * @param yInc: whether to increment pos.y or not.
	 * @param videotoo: whether to define videoPos or not.
	 */
	seriesPos = { pos.seriesPos.x + xoff, pos.seriesPos.y + yoff, 
				  pos.seriesPos.x + xoff + width, pos.seriesPos.y + yoff + height };
	amPos     = { pos.amPos.x + xoff, pos.amPos.y + yoff, pos.amPos.x + xoff + width, pos.amPos.y + yoff + height };
	if ( videoToo )
	{
		videoPos = { pos.videoPos.x + xoff, pos.videoPos.y + yoff,
			pos.videoPos.x + xoff + width, pos.videoPos.y + yoff + height };
		if ( xInc )
		{
			pos.videoPos.x += width;
		}
		if ( yInc )
		{
			pos.videoPos.y += height;
		}
	}
	else
	{
		videoPos = { -1,-1,-1,-1 };
	}
	if ( xInc )
	{
		pos.seriesPos.x += width;
		pos.amPos.x += width;
	}
	else if ( yInc )
	{
		pos.seriesPos.y += height;
		pos.amPos.y += height;
	}
}


template <class ControlType> 
void Control<ControlType>::rearrange(int width, int height, fontMap fonts)
{
	rearrange("", "", width, height, fonts);
}

template <class ControlType>
void Control<ControlType>::rearrange( int width, int height )
{
	rearrange( "", "", width, height );
}

template <class ControlType>
void Control<ControlType>::rearrange( std::string cameraMode, std::string trigMode, int width, int height )
{
	// make sure the control has been initialized
	if ( !m_hWnd )
	{
		return;
	}
	/// Set Positions
	double widthScale = width / 1920.0;
	double heightScale = height / 997.0;
	// extra heigh added to certain controls based on random things like the trigger mode.
	double extraHeight = 0;
	// The last check here is that the mode is affected by the trigger and that the control in question gets drawn in this mode.
	if ( trigMode == "External" && triggerModeSensitive && ((cameraMode == "Kinetic Series Mode" && seriesPos.bottom != -1)
															 || cameraMode == "Accumulation Mode" && amPos.bottom != -1) )
	{
		extraHeight += -25;
	}
	// handle simple case.
	if ( sPos.bottom != 0 || sPos.top != 0 )
	{
		ShowWindow( SW_SHOW );
		RECT position = { long( widthScale * sPos.left ), long( heightScale * (sPos.top + extraHeight) ),
			long( widthScale * sPos.right ), long( heightScale * sPos.bottom ) };
		MoveWindow( &position, TRUE );
	}
	else if ( cameraMode == "Kinetic Series Mode" )
	{
		if ( seriesPos.left == -1 || (triggerModeSensitive == -1 && trigMode == "External Trigger") )
		{
			ShowWindow( SW_HIDE );
		}
		else
		{
			ShowWindow( SW_SHOW );
			RECT position = { long( widthScale * seriesPos.left ), long( heightScale * (seriesPos.top + extraHeight) ),
				long( widthScale * seriesPos.right ), long( heightScale * (seriesPos.bottom + extraHeight) ) };
			MoveWindow( &position, TRUE );
		}
	}
	else if ( cameraMode == "Video Mode" )
	{
		if ( videoPos.left == -1 || (triggerModeSensitive == -1 && trigMode == "External Trigger") )
		{
			ShowWindow( SW_HIDE );
		}
		else
		{
			ShowWindow( SW_SHOW );
			RECT position = { long( widthScale * videoPos.left ), long( heightScale * (videoPos.top + extraHeight) ),
				long( widthScale * videoPos.right ), long( heightScale * (videoPos.bottom + extraHeight) ) };
			MoveWindow( &position, TRUE );
		}
	}
	else if ( cameraMode == "Accumulation Mode" )
	{
		if ( amPos.left == -1 || (triggerModeSensitive == -1 && trigMode == "External Trigger") )
		{
			ShowWindow( SW_HIDE );
		}
		else
		{
			ShowWindow( SW_SHOW );
			RECT position = { long( widthScale * amPos.left ), long( heightScale * (amPos.top + extraHeight) ),
				long( widthScale * amPos.right ), long( heightScale * (amPos.bottom + extraHeight) ) };
			MoveWindow( &position, TRUE );
		}
	}
}


template <class ControlType>
void Control<ControlType>::rearrange( std::string cameraMode, std::string trigMode, int width, int height, fontMap fonts)
{
	if ( !m_hWnd )
	{
		return;
	}
	rearrange( cameraMode, trigMode, width, height );
	double widthScale = width / 1920.0;
	double heightScale = height / 997.0;
	/// Set Fonts
  	if (fontType == fontTypes::NormalFont)
	{
		if (widthScale * heightScale > 2)
		{
			SetFont(fonts["Normal Font Max"]);
		}
		else if (widthScale * heightScale > 0.8)
		{
			SetFont(fonts["Normal Font Large"]);
		}
		else if (widthScale * heightScale > 0.6)
		{
			SetFont(fonts["Normal Font Med"]);
		}
		else
		{
			SetFont(fonts["Normal Font Small"]);
		}
	}
	else if (fontType == fontTypes::CodeFont)
	{
		if (widthScale * heightScale > 2)
		{
			SetFont(fonts["Code Font Max"]);
		}
		else if (widthScale * heightScale > 0.8)
		{
			SetFont(fonts["Code Font Large"]);
		}
		else if (widthScale * heightScale > 0.6)
		{
			SetFont(fonts["Code Font Med"]);
		}
		else
		{
			SetFont(fonts["Code Font Small"]);
		}
	}
	else if (fontType == fontTypes::HeadingFont)
	{
		if (widthScale * heightScale > 2)
		{
			SetFont(fonts["Heading Font Max"]);
		}
		else if (widthScale * heightScale > 0.8)
		{
			SetFont(fonts["Heading Font Large"]);
		}
		else if (widthScale * heightScale > 0.6)
		{
			SetFont(fonts["Heading Font Med"]);
		}
		else
		{
			SetFont(fonts["Heading Font Small"]);
		}
	}
	else if (fontType == fontTypes::LargeFont)
	{
		if (widthScale * heightScale > 2)
		{
			SetFont(fonts["Larger Font Max"]);
		}
		else if (widthScale * heightScale > 0.8)
		{
			SetFont(fonts["Larger Font Large"]);
		}
		else if (widthScale * heightScale > 0.6)
		{
			SetFont(fonts["Larger Font Med"]);
		}
		else
		{
			SetFont(fonts["Larger Font Small"]);
		}
	}
	else if ( fontType == fontTypes::VeryLargeFont )
	{
		if ( widthScale * heightScale > 2 )
		{
			SetFont( fonts["Very Larger Font Max"] );
		}
		else if ( widthScale * heightScale > 0.8 )
		{
			SetFont( fonts["Very Larger Font Large"] );
		}
		else if ( widthScale * heightScale > 0.6 )
		{
			SetFont( fonts["Very Larger Font Med"] );
		}
		else
		{
			SetFont( fonts["Very Larger Font Small"] );
		}
	}
	else if (fontType == fontTypes::SmallFont)
	{
		if (widthScale * heightScale > 2)
		{
			SetFont(fonts["Smaller Font Max"]);
		}
		else if (widthScale * heightScale > 0.8)
		{
			SetFont(fonts["Smaller Font Large"]);
		}
		else if (widthScale * heightScale > 0.6)
		{
			SetFont(fonts["Smaller Font Med"]);
		}
		else
		{
			SetFont(fonts["Smaller Font Small"]);
		}
	}
}




/// template function for the class control system
// This sets a tooltip text to the inputted text. note that parent is the parent window, e.g. the MainWindow, not the 
// control handle which is accessed incide this function via this.
template <class ControlType> void Control<ControlType>::setToolTip(std::string text, cToolTips& tooltips, CWnd* parent)
{
	if (!toolTipIsSet)
	{
		toolTipID = tooltips.size();
		tooltips.push_back(new CToolTipCtrl);
		tooltips.back()->Create(parent, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON);
		tooltips.back()->SetMaxTipWidth(1200);
		tooltips.back()->SetTipBkColor(0x000000);
		tooltips.back()->SetTipTextColor(0xe0e0d0);
		tooltips.back()->SetDlgCtrlID(parent->GetDlgCtrlID());
		tooltips.back()->SetFont(CFont::FromHandle(toolTipFont));
		tooltips.back()->SetDelayTime(TTDT_AUTOPOP, 30000);
		tooltips.back()->AddTool(this, text.c_str());
		tooltips.back()->Activate(TRUE);
		toolTipIsSet = true;
	}
	else
	{
		tooltips[toolTipID]->DelTool(this);
		tooltips[toolTipID]->AddTool(this, text.c_str());
	}
}


