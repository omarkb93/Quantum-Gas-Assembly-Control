﻿#pragma once

#include "afxwin.h"

// a standardized wrapper for experiment data which usually needs a copy for each variation of each sequence. 
template <class type>
class ExpWrap
{
	public:
		type& operator() ( UINT sequenceNumber, UINT variationNumber )
		{ 
			return data[ sequenceNumber ][ variationNumber ];
		};

		type operator() ( UINT sequenceNumber, UINT variationNumber ) const
		{ 
			return data[ sequenceNumber ][ variationNumber ];
		};

		UINT getNumSequences ( )
		{
			return data.size ( );
		}
		UINT getNumVariations( UINT seqNum )
		{
			return data[seqNum].size();
		}
		void resizeSeq ( UINT numSeqs )
		{ 
			data.resize ( numSeqs );
		}
		void resizeVariations ( UINT whichSeq, UINT numVariations )
		{
			if ( whichSeq >= data.size ( ) )
			{
				thrower ( "Tried to access experiment sequence index of ExpWrap which does not exist!" );
			}
			data[ whichSeq ].resize ( numVariations );
		}


	private:
		std::vector<std::vector<type>> data;
};
