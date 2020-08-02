// created by Mark O. Brown
#pragma once
#include "Segment.h"
#include "SegmentEnd.h"
#include <string>
#include <vector>
class ScriptStream;
/**
* The class ScriptedAgilentWaveform contains all of the information and handling relevant for the entire intensity waveform that gets programmed to the Andor.
* This includes a vector of segments which contain segment-specific information. The functions and variabels relevant for this class are:
*/

class ScriptedAgilentWaveform
{
	public:
		ScriptedAgilentWaveform();
		bool analyzeAgilentScriptCommand( int segNum, ScriptStream& script, std::vector<parameterType>& params,
			std::string& warnings);
		void writeData( int SegNum, unsigned long sampleRate );
		std::string compileAndReturnDataSendString( int segNum, int varNum, int totalSegNum, unsigned chan );
		void compileSequenceString( int totalSegNum, int sequenceNum, unsigned channel );
		std::string returnSequenceString();
		bool isVaried();
		void replaceVarValues();
		void replaceVarValues( unsigned variation, std::vector<parameterType>& variables);
		void convertPowersToVoltages( bool useCal, std::vector<double> calibCoeff );
		void normalizeVoltages();
		void calcMinMax();
		double getMaxVolt();
		double getMinVolt();
		unsigned long getSegmentNumber();
		std::vector<std::pair<double, double>> minsAndMaxes;
		unsigned long getNumTrigs( );
		void resetNumberOfTriggers( );
	private:
		unsigned numberOfTriggers;
		std::vector<Segment> waveformSegments;
		double maxVolt;
		double minVolt;
		int segmentNum;
		std::string totalSequence;
};
