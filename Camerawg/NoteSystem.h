#pragma once
#include "Control.h"
#include <string>
#include "commonTypes.h"

class NoteSystem
{
	public:
		bool setExperimentNotes(std::string notes);
		bool setCategoryNotes(std::string notes);
		bool setConfigurationNotes(std::string notes);
		bool initializeControls(POINT& topLeftPosition, CWnd* parentWindow, int& id, fontMap fonts, 
								std::vector<CToolTipCtrl*>& tooltips);
		std::string getExperimentNotes();
		std::string getCategoryNotes();
		std::string getConfigurationNotes();
		void rearrange(int width, int height, fontMap fonts);
	private:
		Control<CEdit> experimentNotes;
		Control<CEdit> categoryNotes;
		Control<CEdit> configurationNotes;
		Control<CStatic> experimentNotesHeader;
		Control<CStatic> categoryNotesHeader;
		Control<CStatic> configurationNotesHeader;
};