(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: inaclnt
 * File: inaclnt.fun
 * Author: B+R
 ********************************************************************
 * Functions and function blocks of library inaclnt
 ********************************************************************)
                                                                      
FUNCTION_BLOCK INAinfo			(*requests a variety of information about the status of the client*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*connection identifier*)
		l_ident	:UDINT;			(*object identifier specification (optional)*)
		mode	:USINT;			(*specifies which information should be determined*)
		pInfo	:UDINT;			(*pointer to the info structure or the string where the desired information is written*)
		size	:UINT;			(*size of the created structure or length of the string including zero termination*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAstate			(*determines the state of an established connection, an object link instruction or a manipulation instruction*)
	VAR_INPUT		
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*connection identifier*)
		l_ident	:UDINT;			(*object identifier specification (optional)*)
		mode	:USINT;			(*specifies whether the state/error code of a connection, object, or action instruction should be queried*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
		INAstate	:UINT;		(*state of the selected instruction, e.g. inaCONN_NOTEXISTING, inaOBJ_NOTEXISTING, ..*)
		INAerror	:UINT;		(*error code of the chosen instruction*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAaction		(*carries out all desired manipulations on the linked object (READ, WRITE, UL, DL CSTART, WSTART, HALT)*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*connection identifier*)
		l_identcnt	:USINT;		(*number of linked objects on which the action will be carried out*)
		pl_ident	:UDINT;		(*pointer to a LONG array with contains the link identifiers*)
		command	:UINT;			(*action to be carried out, e.g. inaREAD, inaWRITE, ..*)
		option	:UDINT;			(*additional option input, e.g. inaOPTION_USRRAM, inaOPTION_USRRAM, ..*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAunlink		(*frees the link object with the specified link identifier up again*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*connection identifier*)
		l_ident	:UDINT;			(*identifier of the object to be cleared*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAlink			(*accesses so-called objects (can be a variable, a B&R module, a date/time object, a data object, or a CPU object)*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*connection identifier to which the object should be linked*)
		type	:USINT;			(*type, e.g. inaPV, inaDATAOBJ, inaBRMODUL, ..*)
		no_copy	:BOOL;			(*0 object description strings are copied internally, 1 object description strings are not copied*)
		no_check	:BOOL;		(*0 type control is active (standard case)*)
		pObjRemote	:UDINT;		(*pointer to the object description string for the remote object to be linked*)	
		pObjLocal	:UDINT;		(*Pointer to the object description string for the local object to be linked *)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
		l_ident	:UDINT;			(*identifier with which the link can be referenced and followed-up*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAdisconn		(*ends the existing connection which uses the recognized connection identifier*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		c_ident	:UDINT;			(*link identifier obtained by INAconnect*)
	END_VAR	
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK INAconnect		(*establishes a connection with the communication partner via the specified routing path*)
	VAR_INPUT
		enable	:BOOL;			(*enables execution*)
		pPath	:UDINT;			(*pointer to the connection path string*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;			(*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
		c_ident	:UDINT;			(*identifier with which the link can be referenced and followed-up*)
	END_VAR
END_FUNCTION_BLOCK
