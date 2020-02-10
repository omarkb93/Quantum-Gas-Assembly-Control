// created by Mark O. Brown
#pragma once
#include "GeneralImaging/imageParameters.h"
#include "Andor/AndorRunMode.h"
#include "Andor/AndorTriggerModes.h"
#include <string>
#include <vector>

// this structure contains all of the main options which are necessary to set when starting a camera acquisition. All
// of these settings should be possibly modified by the user of the UI.
struct AndorRunSettings
{
	imageParameters imageSettings;
	//
	bool emGainModeIsOn = false;
	int emGainLevel = 0;
	int readMode = 4;
	AndorRunModes::mode acquisitionMode = AndorRunModes::mode::Kinetic;
	// 1 means frame transfer mode on, 0 means non-frame transfer mode.
	int frameTransferMode = 1;
	AndorTriggerMode::mode triggerMode = AndorTriggerMode::mode::External;
	bool showPicsInRealTime = false;
	//
	float kineticCycleTime = 0.1f;
	float accumulationTime = 0;
	int accumulationNumber = 1;
	std::vector<float> exposureTimes = { 0.026f };
	//
	UINT picsPerRepetition=1;
	ULONGLONG repetitionsPerVariation=10;
	ULONGLONG totalVariations = 3;
	ULONGLONG totalPicsInVariation();
	// this is an int to reflect that the final number that's programmed to the camera is an int
	int totalPicsInExperiment();
	int temperatureSetting = 25;
};


/*
	- Includes AndorRunSettings, which are the settings that the camera itself cares about.
	- Also some auxiliary settings which are never directly programmed to the camera, but are key for the way the 
	camera is used in the code.
*/
struct AndorCameraSettings
{
	AndorRunSettings andor;
	// not directly programmed to camera
	std::array<std::vector<int>, 4> thresholds;
	std::array<int, 4> palleteNumbers;
	double mostRecentTemp;
};
