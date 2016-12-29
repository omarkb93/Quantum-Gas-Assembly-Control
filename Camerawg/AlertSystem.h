#pragma once
#include "Control.h"
#include <Mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "Winmm.lib")

struct cameraPositions;

class AlertSystem
{
	public:
		AlertSystem() : alertMessageID{ 0 } 
		{
			mciSendString("open \"C:\\Users\\Regal Lab\\Documents\\Quantum Gas Assembly Control\\Camera\\Final Fantasy VII - Victory Fanfare [HQ].mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
		}
		~AlertSystem();
		bool initialize(cameraPositions& positions, CWnd* parent, bool isTriggerModeSensitive, int& id,
			std::unordered_map<std::string, CFont*> fonts, std::vector<CToolTipCtrl*>& tooltips);
		bool alertMainThread(int level);
		bool soundAlert();
		bool rearrange(std::string cameraMode, std::string triggerMode, int width, int height,
			std::unordered_map<std::string, CFont*> fonts);
		void handleCheckBoxPress();
		unsigned int getAlertThreshold();
		unsigned int getAlertMessageID();
		bool setAlertThreshold();
		bool alertsAreToBeUsed();
		bool soundIsToBePlayed();
		bool playSound();
		bool stopSound();
	private:
		Control<CStatic> title;
		Control<CButton> alertsActiveCheckBox;
		Control<CStatic> alertThresholdText;
		Control<CEdit> alertThresholdEdit;
		Control<CButton> soundAtFinshCheckBox;
		int alertThreshold;
		bool useAlerts;
		unsigned int alertMessageID = 0;
};