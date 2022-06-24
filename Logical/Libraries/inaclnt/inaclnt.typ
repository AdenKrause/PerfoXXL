(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Library: inaclnt
 * File: inaclnt.typ
 * Author: B+R
 ********************************************************************
 * Data types of library inaclnt
 ********************************************************************)
                                                                      
TYPE
	inaObjLocalEventPV : STRUCT				(*internal use*)
		pEventObj	: UDINT ;			
		pPollObj	: UDINT ;			
	END_STRUCT;
	inaObjRemoteEventPV : STRUCT			(*internal use*)
		pEventObj	: UDINT ;
		hysterese	: UDINT ;
		idle_tmo	: UDINT ;
		max_tmo	: UDINT ;
	END_STRUCT;
	inaInfoActionStruct : STRUCT			(*internal use*)			
		l_identcnt	: USINT ;
		command	: UINT ;
		option	: UDINT ;
	END_STRUCT;
	inaInfoLinkStruct : STRUCT				(*internal use*)				
		link_type	: USINT ;
		no_copy	: USINT ;
		no_check	: USINT ;
		dummy	: USINT ;
	END_STRUCT;
	inaInfoConnectStruct : STRUCT			(*internal use*)			
		LinkCntMax	: UINT ;
		LinkCntAct	: UINT ;
		ConnAbortCnt	: UINT ;
	END_STRUCT;
	inaInfoIQDrvStruct : STRUCT				(*internal use*)
		ConnCntMax	: USINT ;
		ConnCntAct	: USINT ;
	END_STRUCT;
	inaInfoIQDrvGlobalStruct : STRUCT		(*internal use*)
		IQDrvCntMax	: USINT ;
		IQDrvCntAct	: USINT ;
	END_STRUCT;
END_TYPE