﻿// created by Mark O. Brown
#pragma once
#include "DioRows.h"


struct servoInfo
{
	// in this case the system doesn't try to feed back, it just checks the value. generally should coincide with a 
	// larger tolerance. 
	bool monitorOnly = false;
	std::string servoName="";
	bool active = false;
	unsigned int aiInputChannel=0;
	unsigned int aoControlChannel=0;
	double setPoint=0;
	double controlValue=0;
	double changeInCtrl=0.1;
	double tolerance = 0.02;
	double gain = 0.02;
	bool servoed=false;
	double mostRecentResult = 0;
	std::vector<std::pair<DioRows::which, UINT> > ttlConfig;
	std::vector<std::pair<UINT, double>> aoConfig;
	bool currentlyServoing = false;
	unsigned int avgNum=100;
};