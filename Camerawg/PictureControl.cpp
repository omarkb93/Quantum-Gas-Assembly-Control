﻿#include "stdafx.h"
#include "PictureControl.h"


std::pair<int, int> PictureControl::checkClickLocation( CPoint clickLocation )
{
	CPoint test;
	for (int horizontalInc = 0; horizontalInc < grid.size(); horizontalInc++)
	{
		for (int verticalInc = 0; verticalInc < grid[horizontalInc].size(); verticalInc++)
		{
			RECT relevantRect = grid[horizontalInc][verticalInc];
			// check if inside box
			if (clickLocation.x < relevantRect.right && clickLocation.x > relevantRect.left
				 && clickLocation.y < relevantRect.bottom && clickLocation.y > relevantRect.top)
			{
				return { horizontalInc , verticalInc };
				// then click was inside a box so this should do something.
			}
		}
	}
	// null result. only first number is checked.
	return { -1, -1 };
}


void PictureControl::updatePalette( HPALETTE palette )
{
	imagePalette = palette;
}


void PictureControl::handleEditChange( int id )
{
	if (id == this->editMax.ID)
	{
		int max;
		CString str;
		editMax.GetWindowTextA( str );
		try
		{
			max = std::stoi( std::string(str) );
		}
		catch (std::invalid_argument&)
		{
			errBox( "Please enter an integer." ); 
			return;
		}
		sliderMax.SetPos( max );
		maxSliderPosition = max;
	}
	if (id == this->editMin.ID)
	{
		int min;
		CString str;
		editMin.GetWindowTextA( str );
		try
		{
			min = std::stoi( std::string( str ) );
		}
		catch (std::invalid_argument&)
		{
			errBox( "Please enter an integer." );
			return;
		}
		sliderMin.SetPos( min );
		minSliderPosition = min;
	}
}


void PictureControl::handleScroll(int id, UINT nPos)
{
	if (id == sliderMax.ID)
	{
		sliderMax.SetPos(nPos);
		editMax.SetWindowTextA(std::to_string(nPos).c_str());
		maxSliderPosition = nPos;
	}
	else if (id == sliderMin.ID)
	{
		sliderMin.SetPos(nPos);
		editMin.SetWindowTextA(std::to_string(nPos).c_str());
		minSliderPosition = nPos;
	}
}


void PictureControl::setPictureArea( POINT loc, int width, int height )
{
	// this is important for the control to know where it should draw controls.
	originalBackgroundArea = { loc.x, loc.y, loc.x + width, loc.y + height };
	// reserve some area for the scroll bars.
	originalBackgroundArea.right -= 100;
	currentBackgroundArea = originalBackgroundArea;
	// handle sliders
	loc.x += originalBackgroundArea.right - originalBackgroundArea.left;
	labelMin.sPos = { loc.x, loc.y, loc.x + 50, loc.y + 30 };
	editMin.sPos = { loc.x, loc.y + 30, loc.x + 50, loc.y + 60 };
	sliderMin.sPos = { loc.x, loc.y + 60, loc.x + 50, loc.y + originalBackgroundArea.bottom - originalBackgroundArea.top };
	labelMax.sPos = { loc.x + 50, loc.y, loc.x + 100, loc.y + 30 };
	editMax.sPos = { loc.x + 50, loc.y + 30, loc.x + 100, loc.y + 60 };
	sliderMax.sPos = { loc.x + 50, loc.y + 60, loc.x + 100, loc.y + originalBackgroundArea.bottom - originalBackgroundArea.top };
}


void PictureControl::initialize(POINT& loc, CWnd* parent, int& id, int width, int height)
{
	if (width < 100)
	{
		thrower("Pictures must be greater than 100 in width because this is the size of the max/min controls.");
	}
	if (height < 100)
	{
		thrower( "Pictures must be greater than 100 in height because this is the minimum height of the max/min controls." );
	}
	// this is important for the control to know where it should draw controls.
	setPictureArea( loc, width, height );

	loc.x += originalBackgroundArea.right - originalBackgroundArea.left;
	// "min" text
	labelMin.sPos = { loc.x, loc.y, loc.x + 50, loc.y + 30 };
	labelMin.ID = id++;
	labelMin.Create("MIN", WS_CHILD | WS_VISIBLE | SS_CENTER, labelMin.sPos, parent, labelMin.ID);
	labelMin.fontType = "Normal";
	// minimum number text
	editMin.sPos = { loc.x, loc.y + 30, loc.x + 50, loc.y + 60 };
	editMin.ID = id++;
	if (editMin.ID != IDC_PICTURE_1_MIN_EDIT && editMin.ID != IDC_PICTURE_2_MIN_EDIT && editMin.ID != IDC_PICTURE_3_MIN_EDIT 
		 && editMin.ID != IDC_PICTURE_4_MIN_EDIT)
	{
		throw;
	}
	editMin.Create(WS_CHILD | WS_VISIBLE | SS_LEFT | ES_AUTOHSCROLL, editMin.sPos, parent, editMin.ID);
	editMin.fontType = "Normal";
	// minimum slider
	sliderMin.sPos = { loc.x, loc.y + 60, loc.x + 50, loc.y + originalBackgroundArea.bottom - originalBackgroundArea.top};
	sliderMin.ID = id++;
	sliderMin.Create(WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT, sliderMin.sPos, parent, sliderMin.ID);
	sliderMin.SetRange(0, 20000);
	sliderMin.SetPageSize((minSliderPosition - minSliderPosition)/10.0);
	// "max" text
	labelMax.sPos = { loc.x + 50, loc.y, loc.x + 100, loc.y + 30 };
	labelMax.ID = id++;
	labelMax.Create("MAX", WS_CHILD | WS_VISIBLE | SS_CENTER, labelMax.sPos, parent, labelMax.ID);
	labelMax.fontType = "Normal";
	// maximum number text
	editMax.sPos = { loc.x + 50, loc.y + 30, loc.x + 100, loc.y + 60 };
	editMax.ID = id++;
	if (editMax.ID != IDC_PICTURE_1_MAX_EDIT && editMax.ID != IDC_PICTURE_2_MAX_EDIT
		 && editMax.ID != IDC_PICTURE_3_MAX_EDIT && editMax.ID != IDC_PICTURE_4_MAX_EDIT)
	{
		throw;
	}
	editMax.Create(WS_CHILD | WS_VISIBLE | SS_LEFT | ES_AUTOHSCROLL, editMax.sPos, parent, editMax.ID);
	editMax.fontType = "Normal";
	// maximum slider
	sliderMax.sPos = { loc.x + 50, loc.y + 60, loc.x + 100, loc.y + originalBackgroundArea.bottom - originalBackgroundArea.top};
	sliderMax.ID = id++;
	sliderMax.Create(WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_VERT, sliderMax.sPos, parent, sliderMax.ID);
	sliderMax.SetRange(0, 20000);
	sliderMax.SetPageSize((minSliderPosition - minSliderPosition) / 10.0);
	// reset this.
	loc.x -= originalBackgroundArea.right - originalBackgroundArea.left;
	// manually scroll the objects to initial positions.
	handleScroll( sliderMin.ID, 95);
	handleScroll( sliderMax.ID, 395);
}

void PictureControl::updateGridSpecs( imageParameters newParameters )
{
	// not strictly necessary.
	grid.clear();
	RECT& area = currentBackgroundArea;
	//
	grid.resize( newParameters.width );
	for (int widthInc = 0; widthInc < grid.size(); widthInc++)
	{
		grid[widthInc].resize( newParameters.height );
		for (int heightInc = 0; heightInc < grid[widthInc].size(); heightInc++)
		{
			// for all 4 pictures...
			grid[widthInc][heightInc].left = (int)(area.left + (double)widthInc * (area.right - area.left) / (double)grid.size() + 2);
			grid[widthInc][heightInc].right = (int)(area.left + (double)(widthInc + 1) * (area.right - area.left) / (double)grid.size() + 2);
			grid[widthInc][heightInc].top = (int)(area.top + (double)(heightInc)* (area.bottom - area.top) / (double)grid[widthInc].size());
			grid[widthInc][heightInc].bottom = (int)(area.top + (double)(heightInc + 1) * (area.bottom - area.top) / (double)grid[widthInc].size());
		}
	}
}


void PictureControl::setActive( bool activeState )
{
	active = activeState;
}


void PictureControl::redrawImage( CWnd* parent )
{
	drawBackground(parent);
	if (active && mostRecentImage.size() != 0)
	{
		drawBitmap( parent->GetDC(), mostRecentImage );
	}
}

// input is the 2D array which gets mapped to the image.
void PictureControl::drawBitmap(CDC* deviceContext, std::vector<long> picData)
{
	mostRecentImage = picData;
	float yscale;
	long modrange = maxSliderPosition - minSliderPosition;
	double dTemp = 1;
	int pixelsAreaWidth;
	int pixelsAreaHeight;
	int dataWidth, dataHeight;
	int iTemp;
	HANDLE hloc;
	PBITMAPINFO pbmi;
	WORD argbq[PICTURE_PALETTE_SIZE];
	BYTE *DataArray;
	// Rotated
	SelectPalette( deviceContext->GetSafeHdc(), (HPALETTE)this->imagePalette, true );
	RealizePalette( deviceContext->GetSafeHdc() );
	pixelsAreaWidth = currentBackgroundArea.right - currentBackgroundArea.left + 1;
	pixelsAreaHeight = currentBackgroundArea.bottom - currentBackgroundArea.top + 1;
	dataWidth = grid.size();
	// assumes non-zero size...
	dataHeight = grid[0].size();
	// imageBoxWidth must be a multiple of 4, otherwise StretchDIBits has problems apparently T.T
	if (pixelsAreaWidth % 4)
	{
		pixelsAreaWidth += (4 - pixelsAreaWidth % 4);
	}
	yscale = (256.0f) / (float)modrange;
	for (int paletteIndex = 0; paletteIndex < PICTURE_PALETTE_SIZE; paletteIndex++)
	{
		argbq[paletteIndex] = (WORD)paletteIndex;
	}
	pbmi = (PBITMAPINFO)LocalAlloc( LPTR, sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * (1 << 8) ); 
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = 8;
	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biClrUsed = PICTURE_PALETTE_SIZE;
	pbmi->bmiHeader.biSizeImage = 0;
	pbmi->bmiHeader.biHeight = dataHeight;
	memcpy(pbmi->bmiColors, argbq, sizeof(WORD) * PICTURE_PALETTE_SIZE);
	DataArray = (BYTE*)malloc( (dataWidth * dataHeight) * sizeof( BYTE ) );
	memset( DataArray, 255, (dataWidth * dataHeight) * sizeof( BYTE ) );
	for (int heightInc = 0; heightInc < dataHeight; heightInc++)
	{
		for (int widthInc = 0; widthInc < dataWidth; widthInc++)
		{
			if (false)
			{
				//dTemp = ceil( yscale * (eImagesOfExperiment[experimentImagesInc][widthInc + heightInc * tempParam.width] - minValue) );
			}
			else
			{
				dTemp = ceil( yscale * (picData[widthInc + heightInc * dataWidth] - this->minSliderPosition) );
			}
			if (dTemp < 0)
			{
				// raise value to zero which is the floor of values this parameter can take.
				iTemp = 0;
			}
			else if (dTemp > PICTURE_PALETTE_SIZE - 1)
			{
				// round to maximum value.
				iTemp = PICTURE_PALETTE_SIZE - 1;
			}
			else
			{
				// no rounding or flooring to min or max needed.
				iTemp = (int)dTemp;
			}
			// store the value.
			DataArray[widthInc + heightInc * dataWidth] = (BYTE)iTemp;
		}
	}
	SetStretchBltMode( deviceContext->GetSafeHdc(), COLORONCOLOR );
	BYTE *finalDataArray = NULL;
	switch (dataWidth)
	{
		case 0:
		{
			//pixelsAreaHeight -= 1;
			pbmi->bmiHeader.biWidth = dataWidth;
			pbmi->bmiHeader.biSizeImage = 1;
			StretchDIBits( deviceContext->GetSafeHdc(), currentBackgroundArea.left, currentBackgroundArea.top, pixelsAreaWidth, 
						   pixelsAreaHeight, 0, 0, dataWidth, dataHeight, DataArray, (BITMAPINFO FAR*)pbmi, DIB_PAL_COLORS, SRCCOPY );
			break;
		}
		case 2:
		{
			// make array that is twice as long.
			finalDataArray = (BYTE*)malloc(dataWidth * dataHeight * 2);
			memset(finalDataArray, 255, dataWidth * dataHeight * 2);

			for (int dataInc = 0; dataInc < dataWidth * dataHeight; dataInc++)
			{
				finalDataArray[2 * dataInc] = DataArray[dataInc];
				finalDataArray[2 * dataInc + 1] = DataArray[dataInc];
			}
			pbmi->bmiHeader.biWidth = dataWidth * 2;
			StretchDIBits( *deviceContext, currentBackgroundArea.left, currentBackgroundArea.top, pixelsAreaWidth, pixelsAreaHeight, 0, 0, 
						   dataWidth * 2, dataHeight, finalDataArray, (BITMAPINFO FAR*)pbmi, DIB_PAL_COLORS, SRCCOPY );
			free(finalDataArray);
			break;
		}
		default:
		{
			// make array that is 4X as long.
			finalDataArray = (BYTE*)malloc(dataWidth * dataHeight * 4);
			memset(finalDataArray, 255, dataWidth * dataHeight * 4);
			for (int dataInc = 0; dataInc < dataWidth * dataHeight; dataInc++)
			{
				int data = DataArray[dataInc];
				finalDataArray[4 * dataInc] = data;
				finalDataArray[4 * dataInc + 1] = data;
				finalDataArray[4 * dataInc + 2] = data;
				finalDataArray[4 * dataInc + 3] = data;
			}
			pbmi->bmiHeader.biWidth = dataWidth * 4;
			StretchDIBits( *deviceContext, currentBackgroundArea.left, currentBackgroundArea.top, pixelsAreaWidth, pixelsAreaHeight, 0, 0, dataWidth * 4, dataHeight,
						   finalDataArray, (BITMAPINFO FAR*)pbmi, DIB_PAL_COLORS, SRCCOPY );
			free(finalDataArray);
			break;
		}
	}
	delete[] DataArray;
}

/*
 * recolor the box, clearing last run
 */
void PictureControl::drawBackground(CWnd* parent)
{	
	CDC* colorObj = parent->GetDC();
	colorObj->SelectObject(GetStockObject(DC_BRUSH));
	colorObj->SelectObject(GetStockObject(DC_PEN));
	// dark green brush
	colorObj->SetDCBrushColor(RGB(0, 10, 0));
	// Set the Pen to White
	colorObj->SetDCPenColor(RGB(255, 255, 255));
	// Drawing a rectangle with the current Device Context
	// (slightly larger than the image zone).
	RECT rectArea = { currentBackgroundArea.left, currentBackgroundArea.top, currentBackgroundArea.right, currentBackgroundArea.bottom};
	colorObj->Rectangle(&rectArea);
	parent->ReleaseDC(colorObj);
}

bool PictureControl::isActive()
{
	return active;
}

void PictureControl::drawGrid(CWnd* parent, CBrush* brush)
{
	if (!active)
	{
		return;
	}
	CDC* easel = parent->GetDC();
	easel->SelectObject(GetStockObject(DC_BRUSH));
	easel->SetDCBrushColor(RGB(255, 255, 255));
	// draw rectangles indicating where the pixels are.
	for (int widthInc = 0; widthInc < grid.size(); widthInc++)
	{
		for (int heightInc = 0; heightInc < grid[widthInc].size(); heightInc++)
		{
			easel->FrameRect(&grid[widthInc][heightInc], brush);
		}
	}
}

void PictureControl::drawRectangles( CWnd* parent, CBrush* brush )
{
	// locations for atom analysis...
}

void PictureControl::drawCircle(CWnd* parent, std::pair<int, int> selectedLocation)
{
	if (grid.size() == 0 || !active)
	{
		return;
	}
	if (mostRecentImage.size() != 0)
	{
		//this->drawBackground();
		//this->drawGrid();
	}
	RECT smallRect;
	RECT relevantRect = grid[selectedLocation.first][selectedLocation.second];
	smallRect.left = relevantRect.left + 7.0 * (relevantRect.right - relevantRect.left) / 16.0;
	smallRect.right = relevantRect.left + 9.0 * (relevantRect.right - relevantRect.left) / 16.0;
	smallRect.top = relevantRect.top + 7.0 * (relevantRect.bottom - relevantRect.top) / 16.0;
	smallRect.bottom = relevantRect.top + 9.0 * (relevantRect.bottom - relevantRect.top) / 16.0;
	// get appropriate brush and pen
	CDC* dc = parent->GetDC();
	dc->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
	dc->SelectObject( GetStockObject( DC_PEN ) );
	
	if (colorIndicator == 0 || colorIndicator == 2)
	{
		dc->SetDCPenColor( RGB( 255, 0, 0 ) );
		dc->Ellipse( relevantRect.left, relevantRect.top, relevantRect.right, relevantRect.bottom );
		dc->SelectObject( GetStockObject( DC_BRUSH ) );
		dc->SetDCBrushColor( RGB( 255, 0, 0 ) );
	}
	else
	{
		dc->SetDCPenColor( RGB( 0, 255, 0 ) );
		dc->Ellipse( relevantRect.left, relevantRect.top, relevantRect.right, relevantRect.bottom );
		dc->SelectObject( GetStockObject( DC_BRUSH ) );
		dc->SetDCBrushColor( RGB( 0, 255, 0 ) );
	}
	dc->Ellipse( smallRect.left, smallRect.top, smallRect.right, smallRect.bottom );
	parent->ReleaseDC( dc );
}


void PictureControl::rearrange(std::string cameraMode, std::string triggerMode, int width, int height, 
								std::unordered_map<std::string, CFont*> fonts)
{
	editMax.rearrange(cameraMode, triggerMode, width, height, fonts);
	editMin.rearrange(cameraMode, triggerMode, width, height, fonts);
	labelMax.rearrange(cameraMode, triggerMode, width, height, fonts);
	labelMin.rearrange(cameraMode, triggerMode, width, height, fonts);
	sliderMax.rearrange(cameraMode, triggerMode, width, height, fonts);
	sliderMin.rearrange(cameraMode, triggerMode, width, height, fonts);
	currentBackgroundArea.bottom =  originalBackgroundArea.bottom * height / 997.0;
	currentBackgroundArea.top = originalBackgroundArea.top * height / 997.0;
	currentBackgroundArea.left = originalBackgroundArea.left * width / 1920.0;
	currentBackgroundArea.right = originalBackgroundArea.right * width / 1920.0;
}