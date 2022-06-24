(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: AsIMA
 * File: AsIMA.typ
 * Author: B+R
 ********************************************************************
 * Data types of library AsIMA
 ********************************************************************)
                                                                      
TYPE
	imaInfoStruct : STRUCT					(*information structure*)
		connectionStatus	: UINT ;		(*status of client/server communication, e.g., IMA_IDLE, IMA_OK , ..*)
		commandClearError	: UINT ;		(*if the value 1 is written to this element in the application, then the error is reset*)
		errorNr	: UINT ;					(*error number, e.g., ERR_IMA_IDENT, ERR_IMA_DATOBJ_NAME, ..*)
		errorSectionRowNr	: UINT ;		(*line number in a section in the configuration data object where the error occurred*)
		errorSectionName	: STRING[80] ;	(*name of the section in the configuration data object where the error occurred*)
		errorString	: STRING[80] ;			(*name of the configuration data object where the error occurred*)
	END_STRUCT;
END_TYPE