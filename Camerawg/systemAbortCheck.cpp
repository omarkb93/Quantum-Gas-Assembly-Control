#include "stdafx.h"
#include "systemAbortCheck.h"
#include "externals.h"
#include "postMyString.h"

/*
 * This function checks whether the system abort flag has been set, and if so, sends some messages and returns true. If not aborting, it returns false.
 */
void systemAbortCheck(Communicator* comm, bool& aborting)
{
	// check if aborting
	if (eAbortNiawgFlag == true)
	{
		comm->sendStatus("Aborted!\r\n");
		aborting = true;
		thrower( "Aborted!" );
	}
}
