﻿// created by Mark O. Brown
#pragma once
#include "CustomMfcControlWrappers/myButton.h"

#include "Plotting/dataPoint.h"
#include "GeneralImaging/memdc.h"
#include "gdiplus.h"
#include <QLineSeries>

#include "PrimaryWindows/IChimeraWindowWidget.h"
#include <qchart.h>
#include <qchartview.h>
#include <qlineseries.h>

#include <mutex>
#include <vector>
#include <memory>

enum class plotStyle
{
	// ttl and dac plot use steps.
	TtlPlot,
	DacPlot,
	// uses circs and error bars to represent data and errors
	ErrorPlot,
	// uses bars for histograms
	HistPlot,
	VertHist,
	// 
	OscilloscopePlot
};


typedef std::vector<dataPoint> plotDataVec; 
typedef std::shared_ptr<plotDataVec> pPlotDataVec;
typedef std::shared_ptr<QtCharts::QLineSeries> pQtPlotDataVec;

struct plotMinMax
{
	double min_x, min_y, max_x, max_y;
};
/*
* This is a custom object that I use for plotting. All of the drawing is done manually by standard win32 / MFC
* functionality. Plotting used to be done by gnuplot, an external program which my program would send data to in
* real-time, but this custom plotter, while it took some work (it was fun though) allows me to embed plots in the
* main windows and have a little more direct control over the data being plotted.
*/
class PlotCtrl
{
	public:
		PlotCtrl( std::vector<pPlotDataVec> dataHolder, plotStyle inStyle, std::vector<int> thresholds,
				  std::string titleIn = "Title!", bool narrowOpt=false, bool plotHistOption=false);
		~PlotCtrl( );
		void rearrange( int width, int height, fontMap fonts );
		void init( POINT& topLeftLoc, LONG width, LONG height, IChimeraWindowWidget* parent );
		dataPoint getMainAnalysisResult ( );
		std::vector<pPlotDataVec> getCurrentData ( );

		std::vector<std::mutex> dataMutexes;
		void setStyle (plotStyle newStyle);
		void setData (std::vector<pPlotDataVec> newData);
		void setTitle (std::string newTitle);
		void setThresholds (std::vector<int> newThresholds);
		void refreshData ();
	private:
		const bool narrow;
		std::vector<int> thresholds;
		plotStyle style;
		QtCharts::QChart* chart;
		QtCharts::QChartView* view;
		// first level deliminates different lines which get different colors. second level deliminates different 
		// points within the line.
		std::string title;
		std::vector<pPlotDataVec> data;
		std::vector<QtCharts::QLineSeries*> qtData;
};

