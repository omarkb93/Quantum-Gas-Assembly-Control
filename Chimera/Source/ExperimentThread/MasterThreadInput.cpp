﻿#include "stdafx.h"
#include "ExperimentThread/MasterThreadInput.h"
#include "PrimaryWindows/AuxiliaryWindow.h"
#include "PrimaryWindows/MainWindow.h"
#include "PrimaryWindows/AndorWindow.h"

ExperimentThreadInput::ExperimentThreadInput ( AuxiliaryWindow* auxWin, MainWindow* mainWin, AndorWindow* andorWin ) :
	ttls ( auxWin->getTtlBoard ( ) ), aoSys ( auxWin->getAoSys ( ) ), aiSys ( auxWin->getAiSys ( ) ),
	python ( mainWin->getPython ( ) ), niawg ( mainWin->getNiawg ( ) ), comm ( mainWin->getCommRef ( ) ),
	rsg ( auxWin->getRsg ( ) ), eoAxialTek ( auxWin->getEoAxialTek ( ) ), topBottomTek ( auxWin->getTopBottomTek ( ) ),
	globalControl( auxWin->getGlobals() ), andorCamera( andorWin->getCamera() ), dds( auxWin->getDds() ), 
	logger(andorWin->getLogger() ), piezoControllers( auxWin->getPiezoControllers() )
{ };
