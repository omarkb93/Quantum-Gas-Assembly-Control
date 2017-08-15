#pragma once

#include <unordered_map>
#include <string>
#include <array>
#include <vector>

// this fairly simplistic class could easily just be a namespace instead.
class Gpib
{
	public:
		Gpib::Gpib();
		void gpibWrite( int deviceID, std::string msg );
		std::string gpibRead( int deviceID );
		std::string gpibQuery( int deviceID, std::string query );
		std::string queryIdentity( int deviceAddress );

		int gpibIbdev( int pad );
		void gpibSend( int address, std::string message );
		std::string Gpib::gpibReceive( int address );

		std::string getErrMessage( long errCode );
};
