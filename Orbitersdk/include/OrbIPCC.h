//OrbIPCC.h

//OrbIPCC - Orbiter Inter-process Custom Communication
//Alpha 0.2
//
//Note: check contents of OrbIPC.dat (in windows directory) for debug output.

// warn if forgetting to add OrbiterSoundSDK.lib file
#pragma comment(lib,"OrbIPCC.lib")

/* OutputValue

	Send a value to the vMC Server

	Parameters:

		Vessel : Name of vessel (should be unique to this vessel instance)
		Item   : Name for data element (should be unique within this vessel)
		Value  : Value to send to Server
*/
VOID OutputValue (char *Vessel, char *Item, double Value);


/* GetValue

	Get a value from the vMC Server

	Parameters:

		Vessel : Name of vessel
		Item   : Name for data element

	Returns:

		Currently stored value in Server (or 0 if not stored).
*/
DOUBLE GetValue (char *Vessel, char *Item);