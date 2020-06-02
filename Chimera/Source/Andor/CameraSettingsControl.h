// created by Mark O. Brown
#pragma once

#include "CustomMfcControlWrappers/Control.h"
#include "CustomMfcControlWrappers/myButton.h"
#include "ConfigurationSystems/Version.h"
#include "PictureSettingsControl.h"
#include "CameraImageDimensions.h"
#include "CameraCalibration.h"
#include "Andor/AndorCameraCore.h"
#include "ConfigurationSystems/Version.h"
#include "GeneralImaging/softwareAccumulationOption.h"
#include "PrimaryWindows/IChimeraWindowWidget.h"
#include <qlabel.h>
#include <qcheckbox>
#include <qcombobox.h>
#include <qlineedit.h>

struct cameraPositions;

/*
 * This large class maintains all of the settings & user interactions for said settings of the Andor camera. It more or
 * less contains the PictureSettingsControl Class, as this is meant to be the parent of such an object. It is distinct
 * but highly related to the Andor class, where the Andor class is the class that actually manages communications with
 * the camera and some base settings that the user does not change. Because of the close contact between this and the
 * andor class, this object is initialized with a pointer to the andor object.
 ***********************************************************************************************************************/
class AndorCameraSettingsControl
{
	public:
		AndorCameraSettingsControl();
		void setVariationNumber(UINT varNumber);
		void setRepsPerVariation(UINT repsPerVar);
		void updateRunSettingsFromPicSettings( );
		CBrush* handleColor(int idNumber, CDC* colorer );
		void initialize(POINT& pos, IChimeraWindowWidget* parent );
		void updateSettings( );
		void updateMinKineticCycleTime( double time );
		void setEmGain( bool currentlyOn, int currentEmGainLevel );
		void rearrange(int width, int height, fontMap fonts);
		void updateWindowEnabledStatus ();
		void handlePictureSettings();
		void updateTriggerMode( );
		void handleSetTemperaturePress();
		void changeTemperatureDisplay( AndorTemperatureStatus stat );
		void checkIfReady();
		void cameraIsOn( bool state );
		void handleModeChange( );
		void updateCameraMode( );
		AndorCameraSettings getSettings();
		AndorCameraSettings getCalibrationSettings( );
		bool getAutoCal( );
		bool getUseCal( );
		void setImageParameters(imageParameters newSettings);
		void setRunSettings(AndorRunSettings inputSettings);
		void updateImageDimSettings ( imageParameters settings );
		void updatePicSettings ( andorPicSettingsGroup settings );

		static andorPicSettingsGroup getPictureSettingsFromConfig (ConfigStream& configFile);

		void handleSaveConfig(ConfigStream& configFile);

		void handelSaveMasterConfig(std::stringstream& configFile);
		void handleOpenMasterConfig(ConfigStream& configFile, QtAndorWindow* camWin);

		std::vector<Matrix<long>> getImagesToDraw( const std::vector<Matrix<long>>& rawData  );

		const imageParameters fullResolution = { 1,512,1,512,1,1 };
		std::array<softwareAccumulationOption, 4> getSoftwareAccumulationOptions ( );
	private:
		double getKineticCycleTime( );
		double getAccumulationCycleTime( );
		UINT getAccumulationNumber( );
		imageParameters getImageParameters( );
		QLabel* header;
		// Hardware Accumulation Parameters
		QLabel* accumulationCycleTimeLabel;
		CQLineEdit* accumulationCycleTimeEdit;
		QLabel* accumulationNumberLabel;
		CQLineEdit* accumulationNumberEdit;

		QComboBox* cameraModeCombo;
		QLabel* emGainLabel;
		CQLineEdit* emGainEdit;
		QPushButton* emGainBtn;
		QLabel* emGainDisplay;
		QComboBox* triggerCombo;
		// Temperature
		QPushButton* setTemperatureButton;
		QPushButton* temperatureOffButton;
		CQLineEdit* temperatureEdit;
		QLabel* temperatureDisplay;
		QLabel* temperatureMsg;

		// Kinetic Cycle Time
		CQLineEdit* kineticCycleTimeEdit;
		QLabel* kineticCycleTimeLabel;
		QLabel* minKineticCycleTimeDisp;
		QLabel* minKineticCycleTimeLabel;
		// two subclassed groups.
		ImageDimsControl imageDimensionsObj;
		PictureSettingsControl picSettingsObj;

		CameraCalibration calControl;
		// the currently selected settings, not necessarily those being used to run the current
		// experiment.
		AndorCameraSettings settings;
};

