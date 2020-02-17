// created by Mark O. Brown
#include "stdafx.h"
#include "ATMCD32D.h"
#include "Andor/AndorCameraCore.h"
#include "PrimaryWindows/AndorWindow.h"
#include "Andor/AndorTriggerModes.h"
#include "Andor/AndorRunMode.h"

#include <chrono>
#include <process.h>
#include <algorithm>
#include <numeric>
#include <random>


std::string AndorCameraCore::getSystemInfo()
{
	std::string info;
	// can potentially get more info from this.
	//AndorCapabilities capabilities;
	//getCapabilities( capabilities );
	info += "Camera Model: " + flume.getHeadModel() + "\n\t";
	int num; 
	flume.getSerialNumber(num);
	info += "Camera Serial Number: " + str(num) + "\n";
	return info;
}


AndorCameraCore::AndorCameraCore( bool safemode_opt ) : safemode( safemode_opt ), flume( safemode_opt )
{
	runSettings.emGainModeIsOn = false;
	flume.initialize( );
	flume.setBaselineClamp( 1 );
	flume.setBaselineOffset( 0 );
	flume.setDMAParameters( 1, 0.0001f );
}

void AndorCameraCore::initializeClass(Communicator* comm, chronoTimes* imageTimes)
{
	threadInput.comm = comm;
	threadInput.imageTimes = imageTimes;
	threadInput.Andor = this;
	threadInput.expectingAcquisition = false;
	threadInput.safemode = safemode;
	threadInput.runMutex = &camThreadMutex;
	// begin the camera wait thread.
	_beginthreadex(NULL, 0, &AndorCameraCore::cameraThread, &threadInput, 0, &cameraThreadID);
}

void AndorCameraCore::updatePictureNumber( ULONGLONG newNumber )
{
	currentPictureNumber = newNumber;
}

/* 
 * pause the camera thread which watches the camera for pictures
 */
void AndorCameraCore::pauseThread()
{
	// andor should not be taking images anymore at this point.
	threadInput.expectingAcquisition = false;
}


/*
 * this should get called when the camera finishes running. right now this is very simple.
 */
void AndorCameraCore::onFinish()
{
	//threadInput.signaler.notify_all();
	cameraIsRunning = false;
}


void AndorCameraCore::setCalibrating( bool cal )
{
	calInProgress = cal;
}


bool AndorCameraCore::isCalibrating( )
{
	return calInProgress;
}

/*
 * this thread watches the camera for pictuers and when it sees a picture lets the main thread know via a message. 
 * it gets initialized at the start of the program and is basically always running.
 */
unsigned __stdcall AndorCameraCore::cameraThread( void* voidPtr )
{
	cameraThreadInput* input = (cameraThreadInput*) voidPtr;
	//... I'm not sure what this lock is doing here... why not inside while loop?
	int safeModeCount = 0;
	long pictureNumber = 0;
	bool armed = false;		
	std::unique_lock<std::timed_mutex> lock (*input->runMutex, std::chrono::milliseconds(1000));
	if (!lock.owns_lock ())
	{
		errBox ("ERROR: ANDOR IMAGING THREAD FAILED TO LOCK THE RUN MUTEX! IMAGING THREAD CLOSING!");
		return -1;
	}

	while ( !input->Andor->cameraThreadExitIndicator )
	{
		/* 
		 * wait until unlocked. this happens when data is started.
		 * the first argument is the lock.  The when the lock is locked, this function just sits and doesn't use cpu, 
		 * unlike a while(gGlobalCheck){} loop that waits for gGlobalCheck to be set. The second argument here is a 
		 * lambda, more or less a quick inline function that doesn't in this case have a name. This handles something
		 * called spurious wakeups, which are weird and appear to relate to some optimization things from the quick
		 * search I did. Regardless, I don't fully understand why spurious wakeups occur, but this protects against
		 * them.
		 */
		// Also, anytime this gets locked, the count should be reset.
		//input->signaler.wait( lock, [input]() { return input->expectingAcquisition; } );
		while (!input->expectingAcquisition) 
		{
			input->signaler.wait (lock);
		}
		
		if ( !input->safemode )
		{
			try
			{
				int status = input->Andor->flume.queryStatus();
				if (status == DRV_IDLE && armed)
				{
					// get the last picture. acquisition is over so getAcquisitionProgress returns 0.
					if ( input->Andor->isCalibrating( ) )
					{
						input->comm->sendCameraCalProgress( -1 );
						// signal the end to the main thread.
						input->comm->sendCameraCalFin( );
						armed = false;
					}
					else
					{
						//input->comm->sendCameraProgress( -1 );
						// signal the end to the main thread.
						input->comm->sendCameraFin( );
						// make sure the thread waits when it hits the condition variable.
						input->expectingAcquisition = false;
						armed = false;
					}
				}
				else
				{
					input->Andor->flume.waitForAcquisition();
					if ( pictureNumber % 2 == 0 )
					{
						(*input->imageTimes).push_back( std::chrono::high_resolution_clock::now( ) );
					}
					armed = true;
					try
					{
						input->Andor->flume.getAcquisitionProgress(pictureNumber);
					}
					catch (Error& exception)
					{
						input->comm->sendError(exception.trace());
					}
					if ( input->Andor->isCalibrating( ) )
					{
						input->comm->sendCameraCalProgress( pictureNumber );
					}
					else
					{
						input->comm->sendCameraProgress( pictureNumber );
					}
				}
			}
			catch (Error&)
			{
				//...? When does this happen? not sure why this is here...
			}
		}
		else // safemode
		{
			// simulate an actual wait.
			Sleep( 200 );
			if ( pictureNumber % 2 == 0 )
			{
				(*input->imageTimes).push_back( std::chrono::high_resolution_clock::now( ) );
			}
			if ( input->Andor->cameraIsRunning && safeModeCount < input->Andor->runSettings.totalPicsInExperiment())
			{
				if ( input->Andor->runSettings.acquisitionMode == AndorRunModes::mode::Kinetic)
				{
					safeModeCount++;
					if ( input->Andor->isCalibrating( ) )
					{
						input->comm->sendCameraCalProgress( safeModeCount );
					}
					else
					{
						input->comm->sendCameraProgress( safeModeCount );
					}
				}
				else
				{
					if ( input->Andor->isCalibrating( ) )
					{
						input->comm->sendCameraCalProgress( 1 );
					}
					else
					{
						input->comm->sendCameraProgress( 1 );
					}
				}
			}
			else
			{
				input->Andor->cameraIsRunning = false;
				safeModeCount = 0;
				if ( input->Andor->isCalibrating( ) )
				{
					input->comm->sendCameraCalFin( );
				}
				else
				{
					input->comm->sendCameraFin( );
				}
				input->expectingAcquisition = false;
			}
		}
	}
	return 0;
}


/*
 * Get whatever settings the camera is currently using in it's operation, assuming it's operating.
 */
AndorRunSettings AndorCameraCore::getAndorRunSettings()
{
	return runSettings;
}

void AndorCameraCore::setSettings(AndorRunSettings settingsToSet)
{
	runSettings = settingsToSet;
}

void AndorCameraCore::setAcquisitionMode()
{
	flume.setAcquisitionMode(int(runSettings.acquisitionMode));
}

/* 
	* Large function which initializes a given camera image run.
	*/
void AndorCameraCore::armCamera( double& minKineticCycleTime )
{
	/// Set a bunch of parameters.
	// Set to 1 MHz readout rate in both cases
	flume.setADChannel(1);
	if (runSettings.emGainModeIsOn)
	{
		flume.setHSSpeed(0, 0);
	}
	else
	{
		flume.setHSSpeed(1, 0);
	}
	setAcquisitionMode();
	setReadMode();
	setExposures();
	setImageParametersToCamera();
	// Set Mode-Specific Parameters
	if (runSettings.acquisitionMode == AndorRunModes::mode::Video)
	{
		setFrameTransferMode();
		setKineticCycleTime ();
	}
	else if (runSettings.acquisitionMode == AndorRunModes::mode::Kinetic)
	{
		setKineticCycleTime();
		setScanNumber();
		// set this to 1.
		setNumberAccumulations(true);
		setFrameTransferMode ( );
	}	
	else if (runSettings.acquisitionMode == AndorRunModes::mode::Accumulate)
	{
		setAccumulationCycleTime();
		setNumberAccumulations(false);
	}
	setGainMode();
	setCameraTriggerMode();

	flume.queryStatus();

	/// Do some plotting stuffs
	// get the min time after setting everything else.
	minKineticCycleTime = getMinKineticCycleTime( );

	cameraIsRunning = true;
	std::unique_lock<std::timed_mutex> lock (camThreadMutex, std::chrono::milliseconds(1000));
	if (!lock.owns_lock ())
	{/* Then the thread couldn't get a lock on the spurious wakeup check, but that means that the thread is just
	 waiting for an acquisition... should fix this behavior, but should be okay to continue.*/	}

	 // remove the spurious wakeup check.
	threadInput.expectingAcquisition = true;
	// notify the thread that the experiment has started..
	threadInput.signaler.notify_all();
	flume.startAcquisition();
}


/* 
 * This function checks for new pictures, if they exist it gets them, and shapes them into the array which holds all of
 * the pictures for a given repetition.
 */
std::vector<std::vector<long>> AndorCameraCore::acquireImageData (Communicator* comm)
{
	try
	{
		try
		{
			flume.checkForNewImages ();
		}
		catch (Error & err)
		{
			comm->sendError("Error seen while checking for new images: " + err.trace ());
		}
		// each image processed from the call from a separate windows message
		// If there is no data the acquisition must have been aborted
		int experimentPictureNumber = runSettings.showPicsInRealTime ? 0
			: (((currentPictureNumber - 1) % runSettings.totalPicsInVariation ()) % runSettings.picsPerRepetition);
		if (experimentPictureNumber == 0)
		{
			imagesOfExperiment.clear ();
			imagesOfExperiment.resize (runSettings.showPicsInRealTime ? 1 : runSettings.picsPerRepetition);
		}
		std::vector<long> tempImage(runSettings.imageSettings.size (),0);
		imagesOfExperiment[experimentPictureNumber].resize (runSettings.imageSettings.size ());
		if (!safemode)
		{
			try
			{
				flume.getOldestImage(tempImage);
			}
			catch (Error &)
			{
				// let the blank image roll through to keep the image numbers going sensibly.
				//throwNested ("Error while calling getOldestImage.");
			}
			// immediately rotate
			for (UINT imageVecInc = 0; imageVecInc < imagesOfExperiment[experimentPictureNumber].size (); imageVecInc++)
			{
				imagesOfExperiment[experimentPictureNumber][imageVecInc] = tempImage[((imageVecInc
					% runSettings.imageSettings.width ()) + 1) * runSettings.imageSettings.height ()
					- imageVecInc / runSettings.imageSettings.width () - 1];
			}
		}
		else
		{
			for (auto imageVecInc : range (imagesOfExperiment[experimentPictureNumber].size ()))
			{
				std::random_device rd;
				std::mt19937 e2 (rd ());
				std::normal_distribution<> dist (180, 20);
				std::normal_distribution<> dist2 (350, 100);
				tempImage[imageVecInc] = dist (e2) + 10;
				if (((imageVecInc / runSettings.imageSettings.width ()) % 2 == 1)
					&& ((imageVecInc % runSettings.imageSettings.width ()) % 2 == 1))
				{
					// can have an atom here.
					if (UINT (rand ()) % 300 > imageVecInc + 50)
					{
						// use the exposure time and em gain level 
						tempImage[imageVecInc] += runSettings.exposureTimes[experimentPictureNumber] * 1e3 * dist2 (e2);
						if (runSettings.emGainModeIsOn)
						{
							tempImage[imageVecInc] *= runSettings.emGainLevel;
						}
					}
				}
			}
			for (auto imageVecInc : range (imagesOfExperiment[experimentPictureNumber].size ()))
			{
				auto& ims = runSettings.imageSettings;
				imagesOfExperiment[experimentPictureNumber][imageVecInc] = tempImage[((imageVecInc % ims.width ()) + 1) * ims.height ()
					- imageVecInc / ims.width () - 1];
			}
		}
		return imagesOfExperiment;
	}
	catch (Error &)
	{
		throwNested ("Error Seen in acquireImageData.");
	}
}


// sets this based on internal settings object.
void AndorCameraCore::setCameraTriggerMode()
{
	std::string errMsg;
	int trigType;
	if (runSettings.triggerMode == AndorTriggerMode::mode::Internal)
	{
		trigType = 0;
	}
	else if (runSettings.triggerMode == AndorTriggerMode::mode::External)
	{
		trigType = 1;
	}
	else if (runSettings.triggerMode == AndorTriggerMode::mode::StartOnTrigger)
	{
		trigType = 6;
	}
	flume.setTriggerMode(trigType);
}


void AndorCameraCore::setTemperature()
{
	// Get the current temperature
	if (runSettings.temperatureSetting < -60 || runSettings.temperatureSetting > 25)
	{
		int answer = promptBox( "Warning: The selected temperature is outside the \"normal\" temperature range of the "
								"camera (-60 through 25 C). Proceed anyways?", MB_OKCANCEL );
		if (answer == IDCANCEL)
		{
			return;
		}
	}
	// Proceedure to initiate cooling
	changeTemperatureSetting( false );
}


void AndorCameraCore::setReadMode()
{
	flume.setReadMode(runSettings.readMode);
}


void AndorCameraCore::setExposures()
{
	if (runSettings.exposureTimes.size() > 0 && runSettings.exposureTimes.size() <= 16)
	{
		flume.setRingExposureTimes(runSettings.exposureTimes.size(), runSettings.exposureTimes.data());
	}
	else
	{
		thrower ("ERROR: Invalid size for vector of exposure times, value of " + str(runSettings.exposureTimes.size()) + ".");
	}
}


void AndorCameraCore::setImageParametersToCamera()
{
	flume.setImage( runSettings.imageSettings.verticalBinning, runSettings.imageSettings.horizontalBinning,
				    runSettings.imageSettings.bottom, runSettings.imageSettings.top, 
				    runSettings.imageSettings.left, runSettings.imageSettings.right );
}


void AndorCameraCore::setKineticCycleTime()
{
	flume.setKineticCycleTime(runSettings.kineticCycleTime);
}


void AndorCameraCore::setScanNumber()
{
	if (runSettings.totalPicsInExperiment() == 0 && runSettings.totalPicsInVariation() != 0)
	{
		// all is good. The first variable has not been set yet.
	}
	else if (runSettings.totalPicsInVariation() == 0)
	{
		thrower ("ERROR: Scan Number Was Zero.\r\n");
	}
	else
	{
		//flume.setNumberKinetics(int(runSettings.totalPicsInExperiment()));
		flume.setNumberKinetics ( int ( runSettings.totalPicsInVariation ( ) ) );
	}
}


void AndorCameraCore::setFrameTransferMode()
{
	flume.setFrameTransferMode(runSettings.frameTransferMode);
}


/*
 * exposures should be initialized to be the correct size. Nothing else matters for the inputs, they get 
 * over-written.
 * throws exception if fails
 */
void AndorCameraCore::checkAcquisitionTimings(float& kinetic, float& accumulation, std::vector<float>& exposures)
{
	if ( exposures.size ( ) == 0 )
	{
		return;
	}
	float tempExposure, tempAccumTime, tempKineticTime;
	float * timesArray = NULL;
	std::string errMsg;
	if (safemode)
	{
		// if in safemode initialize this stuff to the values to be outputted.
		if (exposures.size() > 0)
		{
			tempExposure = exposures[0];
		}
		else
		{
			tempExposure = 0;
		}
		tempAccumTime = accumulation;
		tempKineticTime = kinetic;
	}
	else
	{
		tempExposure = 0;
		tempAccumTime = 0;
		tempKineticTime = 0;
	}
	// It is necessary to get the actual times as the system will calculate the
	// nearest possible time. eg if you set exposure time to be 0, the system
	// will use the closest value (around 0.01s)
	timesArray = new float[exposures.size()];
	if (!safemode)
	{
		flume.getAcquisitionTimes(tempExposure, tempAccumTime, tempKineticTime);
		flume.getAdjustedRingExposureTimes(exposures.size(), timesArray);
	}
	else 
	{
		for (UINT exposureInc = 0; exposureInc < exposures.size(); exposureInc++)
		{
			timesArray[exposureInc] = exposures[exposureInc];
		}
	}
	// Set times
	if (exposures.size() > 0)
	{
		for (UINT exposureInc = 0; exposureInc < exposures.size(); exposureInc++)
		{
			exposures[exposureInc] = timesArray[exposureInc];
		}
		delete[] timesArray;
	}
	accumulation = tempAccumTime;
	kinetic = tempKineticTime;
}

/*
 (
 */
void AndorCameraCore::setAccumulationCycleTime()
{
	flume.setAccumulationCycleTime(runSettings.accumulationTime);
}


void AndorCameraCore::setNumberAccumulations(bool isKinetic)
{
	std::string errMsg;
	if (isKinetic)
	{
		// right now, kinetic series mode always has one accumulation. could add this feature later if desired to do 
		// both kinetic and accumulation. Not sure there's actually much of a reason to use accumulations. 
		//setNumberAccumulations(true); // ???
		flume.setAccumulationNumber(1);
	}
	else
	{
		// ???
		// setNumberAccumulations(false); // ???
		flume.setAccumulationNumber(runSettings.accumulationNumber);
	}
}


void AndorCameraCore::setGainMode()
{
	if (!runSettings.emGainModeIsOn)
	{
		// Set Gain
		int numGain;
		flume.getNumberOfPreAmpGains(numGain);
		flume.setPreAmpGain(2);
		float myGain;
		flume.getPreAmpGain(2, myGain);
		// 1 is for conventional gain mode.
		flume.setOutputAmplifier(1);
	}
	else
	{
		// 0 is for em gain mode.
		flume.setOutputAmplifier(0);
		flume.setPreAmpGain(2);
		if (runSettings.emGainLevel > 300)
		{
			flume.setEmGainSettingsAdvanced(1);
		}
		else
		{
			flume.setEmGainSettingsAdvanced(0);
		}
		flume.setEmCcdGain(runSettings.emGainLevel);
	}
}


void AndorCameraCore::changeTemperatureSetting(bool turnTemperatureControlOff)
{
	char aBuffer[256];
	int minimumAllowedTemp, maximumAllowedTemp;
	// the default, in case the program is in safemode.
	minimumAllowedTemp = -60;
	maximumAllowedTemp = 25;
	// clear buffer
	wsprintf(aBuffer, "");
	// check if temp is in valid range
	flume.getTemperatureRange(minimumAllowedTemp, maximumAllowedTemp);
	if (runSettings.temperatureSetting < minimumAllowedTemp || runSettings.temperatureSetting > maximumAllowedTemp)
	{
		thrower ("ERROR: Temperature is out of range\r\n");
	}
	else
	{
		// if it is in range, switch on cooler and set temp
		if (turnTemperatureControlOff == false)
		{
			flume.temperatureControlOn();
		}
		else
		{
			flume.temperatureControlOff();
		}
	}

	if (turnTemperatureControlOff == false)
	{
		flume.setTemperature(runSettings.temperatureSetting);
	}
	else
	{
		thrower ("Temperature Control has been turned off.\r\n");
	}
}

AndorTemperatureStatus AndorCameraCore::getTemperature ( )
{
	AndorTemperatureStatus stat;
	stat.temperatureSetting = getAndorRunSettings().temperatureSetting;
	try
	{
		flume.getTemperature ( stat.temperature );
		// in this case you expect it to throw.
		//setTemperature = andorFriend->getAndorRunSettings().temperatureSetting;
		if ( ANDOR_SAFEMODE ) { thrower ( "SAFEMODE" ); }
	}
	catch ( Error& exception )
	{
		// if not stable this won't get changed.
		if ( exception.whatBare ( ) == "DRV_TEMPERATURE_STABILIZED" )
		{
			stat.msg = "Temperature has stabilized at " + str ( stat.temperature ) + " (C)\r\n";
		}
		else if ( exception.whatBare ( ) == "DRV_TEMPERATURE_NOT_REACHED" )
		{
			stat.msg = "Set temperature not yet reached. Current temperature is " + str ( stat.temperature ) + " (C)\r\n";
		}
		else if ( exception.whatBare ( ) == "DRV_TEMPERATURE_NOT_STABILIZED" )
		{
			stat.msg = "Temperature of " + str ( stat.temperature ) + " (C) reached but not stable.";
		}
		else if ( exception.whatBare ( ) == "DRV_TEMPERATURE_DRIFT" )
		{
			stat.msg = "Temperature had stabilized but has since drifted. Temperature: " + str ( stat.temperature );
		}
		else if ( exception.whatBare ( ) == "DRV_TEMPERATURE_OFF" )
		{
			stat.msg = "Temperature control is off. Temperature: " + str ( stat.temperature );
		}
		else if ( exception.whatBare ( ) == "DRV_ACQUIRING" )
		{
			// doesn't change color of temperature control. This way the color of the control represents the state of
			// the temperature right before the acquisition started, so that you can tell if you remembered to let it
			// completely stabilize or not.
			stat.msg = "Camera is Acquiring data. No updates are available. \r\nMost recent temperature: "
				+ str ( stat.temperature );
		}
		else if ( exception.whatBare ( ) == "SAFEMODE" )
		{
			stat.msg = "Application is running in Safemode... No Real Temperature Data is available.";
		}
		else
		{
			stat.msg = "Unexpected Temperature Code: " + exception.whatBare ( ) + ". Temperature: "
												   + str ( stat.temperature );
		}
	}
	return stat;
}

int AndorCameraCore::queryStatus ( )
{
	return flume.queryStatus ( );
}

bool AndorCameraCore::isRunning ( )
{
	return cameraIsRunning;
}

double AndorCameraCore::getMinKineticCycleTime ( )
{
	// get the currently set kinetic cycle time.
	float minKineticCycleTime, dummy1, dummy2;
	flume.setKineticCycleTime ( 0 );
	flume.getAcquisitionTimes ( dummy1, dummy2, minKineticCycleTime );

	// re-set whatever's currently in the settings.
	setKineticCycleTime ( );
	return minKineticCycleTime;
}

void AndorCameraCore::setIsRunningState ( bool state )
{
	cameraIsRunning = state;
}

void AndorCameraCore::abortAcquisition ( )
{
	flume.abortAcquisition ( );
}