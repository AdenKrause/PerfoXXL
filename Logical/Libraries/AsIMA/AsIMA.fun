(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: AsIMA
 * File: AsIMA.fun
 * Author: B+R
 ********************************************************************
 * Functions and function blocks of library AsIMA
 ********************************************************************)
                                                                      
FUNCTION IMAcomm : UINT					(*operates the INA2000 manager as a client (processes communication)*)
	VAR_INPUT
		ident	:UDINT;					(*identifier returned from IMAinit*)
		connInfoPV	:imaInfoStruct;		(*structure for outputting connection status and error information for variable services*)
		connInfoAUX	:imaInfoStruct;		(*structure for outputting connection status and error information for other services*)
	END_VAR
END_FUNCTION
FUNCTION IMAclose : UINT				(*closes the INA2000 manager (dismantles communication and frees up allocated memory)*)
	VAR_INPUT
		ident	:UDINT;					(*identifier returned from IMAinit*)
	END_VAR
END_FUNCTION
FUNCTION IMAinit : UINT					(*initializes the INA2000 Manager (evaluates the configuration data object and allocates memory)*)
	VAR_INPUT
		pDatObjName	:UDINT;				(*string containing the name of the configuration data object, given as a pointer*)
		errorInfo	:imaInfoStruct;		(*structure for outputting error information*)
		ident	:REFERENCE TO UDINT;	(*identifier of the client/server communication instance*)
	END_VAR
END_FUNCTION
