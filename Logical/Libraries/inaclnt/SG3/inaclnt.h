/* Automation Studio generated header file */
/* Do not edit ! */

#ifndef _INACLNT_
#define _INACLNT_
#ifdef __cplusplus
extern "C" 
{
#endif

#include <bur/plctypes.h>

#include <runtime.h>

#ifndef _IEC_CONST
#define _IEC_CONST _WEAK const
#endif

/* Constants */
#ifdef _REPLACE_CONST
 #define inaDL 4U
 #define inaPV 1U
 #define inaUL 3U
 #define inaCPU 3U
 #define inaHALT 7U
 #define inaREAD 1U
 #define inaWRITE 2U
 #define inaCSTART 5U
 #define inaENABLE 8U
 #define inaWSTART 6U
 #define inaBRMODUL 2U
 #define inaDATAOBJ 5U
 #define inaDISABLE 9U
 #define inaEVENTPV 8U
 #define inaDATETIME 4U
 #define inaIO_INPUT 6U
 #define ERR_IQVW_MEM 10001U
 #define inaACT_ERROR 3U
 #define inaACT_READY 0U
 #define inaIO_OUTPUT 7U
 #define inaMODE_LINK 2U
 #define inaMODE_RING 1U
 #define inaOBJ_ERROR 3U
 #define ERR_INA_INPUT 10022U
 #define inaCONN_ERROR 3U
 #define inaMODE_IQDRV 32U
 #define inaMODE_QUEUE 0U
 #define inaOBJ_ACTIVE 2U
 #define ERR_INA_MEMORY 10021U
 #define inaACT_WORKING 1U
 #define inaCONN_ACTIVE 2U
 #define inaMODE_ACTION 3U
 #define inaOBJ_LINKING 1U
 #define inaOPTION_DRAM 6U
 #define ERR_INAS_DO_MAX 10124U
 #define ERR_INAS_MEMORY 10100U
 #define ERR_INA_INSTALL 10020U
 #define ERR_INA_OBJ_MAX 10042U
 #define ERR_INA_PV_LIST 10023U
 #define ERR_IQVW_SYNTAX 10006U
 #define inaMODE_CONNECT 1U
 #define ERR_INAS_DO_FULL 10128U
 #define ERR_INAS_OBJ_MAX 10103U
 #define ERR_INA_ACT_BUSY 10027U
 #define ERR_INA_CONN_MAX 10041U
 #define ERR_INA_LOC_DATA 10030U
 #define ERR_INA_REM_DATA 10029U
 #define ERR_INA_TEXT_MAN 10050U
 #define ERR_IQVW_VERSION 10008U
 #define inaOPTION_FIXRAM 2U
 #define inaOPTION_SYSROM 4U
 #define inaOPTION_USRRAM 5U
 #define inaOPTION_USRROM 1U
 #define ERR_INAS_CONN_MAX 10102U
 #define ERR_INAS_DO_EMPTY 10129U
 #define ERR_INAS_DO_INPUT 10127U
 #define ERR_INA_INPUTSIZE 10039U
 #define ERR_INA_OBJ_STATE 10026U
 #define ERR_IQVW_CONN_MAX 10003U
 #define inaOPTION_MEMCARD 3U
 #define ERR_INAIQ_DLM_BURN 10230U
 #define ERR_INAS_DO_BUFFER 10132U
 #define ERR_INAS_DO_MEMORY 10122U
 #define ERR_INAS_REQ_FRAME 10111U
 #define ERR_INA_CONN_STATE 10025U
 #define ERR_INA_NO_SUCH_ID 10040U
 #define ERR_INA_TYPE_CHECK 10028U
 #define inaCONN_CONNECTING 1U
 #define inaOBJ_NOTEXISTING 0U
 #define ERR_INAS_DO_BLOCKED 10130U
 #define ERR_INAS_DO_INSTALL 10121U
 #define ERR_INAS_DO_VERSION 10120U
 #define ERR_INAS_NO_SUCH_ID 10101U
 #define ERR_INA_ACCESS_TYPE 10024U
 #define ERR_INA_LOC_DYN_PV_ZERO 10034U
 #define ERR_IQVW_DRIVER_MAX 10002U
 #define ERR_IQVW_ID_ALREADY 10007U
 #define ERR_IQVW_NO_SUCH_ID 10000U
 #define inaCONN_NOTEXISTING 0U
 #define inaMODE_IQDRVGLOBAL 16U
 #define inaOPTION_IMMEDIATE 16U
 #define inaOPTION_NO_OPTION 0U
 #define ERR_INAIQ_CONN_ABORT 10210U
 #define ERR_INAIQ_DLM_BRM_CS 10226U
 #define ERR_INAIQ_DLM_CANCEL 10223U
 #define ERR_INAIQ_ULM_CANCEL 10243U
 #define ERR_INA_COMMAND_FIFO 10049U
 #define ERR_INA_LOC_OBJ_TYPE 10032U
 #define ERR_INA_REM_OBJ_TYPE 10031U
 #define ERR_IQVW_NO_SUCH_DRV 10004U
 #define inaMODE_CONN_ACT_ERR 7U
 #define ERR_INAIQ_DLM_DEFAULT 10222U
 #define ERR_INAIQ_DLM_NO_BRNC 10231U
 #define ERR_INAIQ_ULM_DEFAULT 10242U
 #define ERR_INAS_DO_DUPLICATE 10125U
 #define ERR_INA_NO_ITEM_FOUND 10043U
 #define inaMODE_CONNECTSTRING 4U
 #define ERR_INAS_DO_NO_SUCH_ID 10123U
 #define ERR_INAS_NO_ITEM_FOUND 10104U
 #define ERR_INAIQ_ACCESS_DENIED 10212U
 #define ERR_INAIQ_DLM_NO_ACCESS 10224U
 #define ERR_INAIQ_ULM_NO_ACCESS 10247U
 #define ERR_INA_REM_DYN_PV_ZERO 10033U
 #define inaMODE_LINKSTRINGLOCAL 6U
 #define ERR_INAIQ_DLM_BRM_LENGTH 10228U
 #define ERR_INAIQ_DLM_BRNC_ERROR 10232U
 #define ERR_INAIQ_DLM_DECODE_TEL 10221U
 #define ERR_INAIQ_DLM_ENCODE_TEL 10220U
 #define ERR_INAIQ_NO_COMMSERV_BR 10211U
 #define ERR_INAIQ_ULM_DECODE_TEL 10241U
 #define ERR_INAIQ_ULM_ENCODE_TEL 10240U
 #define ERR_INAS_STM_WRONG_STATE 10110U
 #define inaMODE_LINKSTRINGREMOTE 5U
 #define ERR_INAIQ_DLM_BRM_INSTALL 10227U
 #define ERR_INAIQ_DLM_BRM_INVALID 10225U
 #define ERR_INAIQ_DLM_TARGET_SIZE 10229U
 #define ERR_INAS_DO_DATA_MISMATCH 10126U
 #define ERR_INA_LOC_OBJ_NOT_EXIST 10036U
 #define ERR_INA_REM_OBJ_NOT_EXIST 10035U
 #define ERR_INAIQ_ULM_OBJ_NO_ACCESS 10246U
 #define ERR_INAIQ_ULM_OBJ_NOT_EXISTS 10244U
 #define ERR_INA_LOC_OBJ_TYPE_CHANGED 10038U
 #define ERR_INA_REM_OBJ_TYPE_CHANGED 10037U
 #define ERR_INAIQ_ULM_OBJ_STATE_CONFLICT 10245U
 #define ERR_INAIQ_UNEXPECTED_TEL 10203U
 #define ERR_INAIQ_STM_WRONG_STATE 10202U
 #define ERR_INAIQ_SRV_NOT_IMPLEMENTED 10201U
#else
 _IEC_CONST unsigned short inaDL = 4U;
 _IEC_CONST unsigned char inaPV = 1U;
 _IEC_CONST unsigned short inaUL = 3U;
 _IEC_CONST unsigned char inaCPU = 3U;
 _IEC_CONST unsigned short inaHALT = 7U;
 _IEC_CONST unsigned short inaREAD = 1U;
 _IEC_CONST unsigned short inaWRITE = 2U;
 _IEC_CONST unsigned short inaCSTART = 5U;
 _IEC_CONST unsigned short inaENABLE = 8U;
 _IEC_CONST unsigned short inaWSTART = 6U;
 _IEC_CONST unsigned char inaBRMODUL = 2U;
 _IEC_CONST unsigned char inaDATAOBJ = 5U;
 _IEC_CONST unsigned short inaDISABLE = 9U;
 _IEC_CONST unsigned char inaEVENTPV = 8U;
 _IEC_CONST unsigned char inaDATETIME = 4U;
 _IEC_CONST unsigned char inaIO_INPUT = 6U;
 _IEC_CONST unsigned short ERR_IQVW_MEM = 10001U;
 _IEC_CONST unsigned short inaACT_ERROR = 3U;
 _IEC_CONST unsigned short inaACT_READY = 0U;
 _IEC_CONST unsigned char inaIO_OUTPUT = 7U;
 _IEC_CONST unsigned char inaMODE_LINK = 2U;
 _IEC_CONST unsigned char inaMODE_RING = 1U;
 _IEC_CONST unsigned short inaOBJ_ERROR = 3U;
 _IEC_CONST unsigned short ERR_INA_INPUT = 10022U;
 _IEC_CONST unsigned short inaCONN_ERROR = 3U;
 _IEC_CONST unsigned char inaMODE_IQDRV = 32U;
 _IEC_CONST unsigned char inaMODE_QUEUE = 0U;
 _IEC_CONST unsigned short inaOBJ_ACTIVE = 2U;
 _IEC_CONST unsigned short ERR_INA_MEMORY = 10021U;
 _IEC_CONST unsigned short inaACT_WORKING = 1U;
 _IEC_CONST unsigned short inaCONN_ACTIVE = 2U;
 _IEC_CONST unsigned char inaMODE_ACTION = 3U;
 _IEC_CONST unsigned short inaOBJ_LINKING = 1U;
 _IEC_CONST unsigned long inaOPTION_DRAM = 6U;
 _IEC_CONST unsigned short ERR_INAS_DO_MAX = 10124U;
 _IEC_CONST unsigned short ERR_INAS_MEMORY = 10100U;
 _IEC_CONST unsigned short ERR_INA_INSTALL = 10020U;
 _IEC_CONST unsigned short ERR_INA_OBJ_MAX = 10042U;
 _IEC_CONST unsigned short ERR_INA_PV_LIST = 10023U;
 _IEC_CONST unsigned short ERR_IQVW_SYNTAX = 10006U;
 _IEC_CONST unsigned char inaMODE_CONNECT = 1U;
 _IEC_CONST unsigned short ERR_INAS_DO_FULL = 10128U;
 _IEC_CONST unsigned short ERR_INAS_OBJ_MAX = 10103U;
 _IEC_CONST unsigned short ERR_INA_ACT_BUSY = 10027U;
 _IEC_CONST unsigned short ERR_INA_CONN_MAX = 10041U;
 _IEC_CONST unsigned short ERR_INA_LOC_DATA = 10030U;
 _IEC_CONST unsigned short ERR_INA_REM_DATA = 10029U;
 _IEC_CONST unsigned short ERR_INA_TEXT_MAN = 10050U;
 _IEC_CONST unsigned short ERR_IQVW_VERSION = 10008U;
 _IEC_CONST unsigned long inaOPTION_FIXRAM = 2U;
 _IEC_CONST unsigned long inaOPTION_SYSROM = 4U;
 _IEC_CONST unsigned long inaOPTION_USRRAM = 5U;
 _IEC_CONST unsigned long inaOPTION_USRROM = 1U;
 _IEC_CONST unsigned short ERR_INAS_CONN_MAX = 10102U;
 _IEC_CONST unsigned short ERR_INAS_DO_EMPTY = 10129U;
 _IEC_CONST unsigned short ERR_INAS_DO_INPUT = 10127U;
 _IEC_CONST unsigned short ERR_INA_INPUTSIZE = 10039U;
 _IEC_CONST unsigned short ERR_INA_OBJ_STATE = 10026U;
 _IEC_CONST unsigned short ERR_IQVW_CONN_MAX = 10003U;
 _IEC_CONST unsigned long inaOPTION_MEMCARD = 3U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BURN = 10230U;
 _IEC_CONST unsigned short ERR_INAS_DO_BUFFER = 10132U;
 _IEC_CONST unsigned short ERR_INAS_DO_MEMORY = 10122U;
 _IEC_CONST unsigned short ERR_INAS_REQ_FRAME = 10111U;
 _IEC_CONST unsigned short ERR_INA_CONN_STATE = 10025U;
 _IEC_CONST unsigned short ERR_INA_NO_SUCH_ID = 10040U;
 _IEC_CONST unsigned short ERR_INA_TYPE_CHECK = 10028U;
 _IEC_CONST unsigned short inaCONN_CONNECTING = 1U;
 _IEC_CONST unsigned short inaOBJ_NOTEXISTING = 0U;
 _IEC_CONST unsigned short ERR_INAS_DO_BLOCKED = 10130U;
 _IEC_CONST unsigned short ERR_INAS_DO_INSTALL = 10121U;
 _IEC_CONST unsigned short ERR_INAS_DO_VERSION = 10120U;
 _IEC_CONST unsigned short ERR_INAS_NO_SUCH_ID = 10101U;
 _IEC_CONST unsigned short ERR_INA_ACCESS_TYPE = 10024U;
 _IEC_CONST unsigned short ERR_INA_LOC_DYN_PV_ZERO = 10034U;
 _IEC_CONST unsigned short ERR_IQVW_DRIVER_MAX = 10002U;
 _IEC_CONST unsigned short ERR_IQVW_ID_ALREADY = 10007U;
 _IEC_CONST unsigned short ERR_IQVW_NO_SUCH_ID = 10000U;
 _IEC_CONST unsigned short inaCONN_NOTEXISTING = 0U;
 _IEC_CONST unsigned char inaMODE_IQDRVGLOBAL = 16U;
 _IEC_CONST unsigned long inaOPTION_IMMEDIATE = 16U;
 _IEC_CONST unsigned long inaOPTION_NO_OPTION = 0U;
 _IEC_CONST unsigned short ERR_INAIQ_CONN_ABORT = 10210U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BRM_CS = 10226U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_CANCEL = 10223U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_CANCEL = 10243U;
 _IEC_CONST unsigned short ERR_INA_COMMAND_FIFO = 10049U;
 _IEC_CONST unsigned short ERR_INA_LOC_OBJ_TYPE = 10032U;
 _IEC_CONST unsigned short ERR_INA_REM_OBJ_TYPE = 10031U;
 _IEC_CONST unsigned short ERR_IQVW_NO_SUCH_DRV = 10004U;
 _IEC_CONST unsigned char inaMODE_CONN_ACT_ERR = 7U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_DEFAULT = 10222U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_NO_BRNC = 10231U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_DEFAULT = 10242U;
 _IEC_CONST unsigned short ERR_INAS_DO_DUPLICATE = 10125U;
 _IEC_CONST unsigned short ERR_INA_NO_ITEM_FOUND = 10043U;
 _IEC_CONST unsigned char inaMODE_CONNECTSTRING = 4U;
 _IEC_CONST unsigned short ERR_INAS_DO_NO_SUCH_ID = 10123U;
 _IEC_CONST unsigned short ERR_INAS_NO_ITEM_FOUND = 10104U;
 _IEC_CONST unsigned short ERR_INAIQ_ACCESS_DENIED = 10212U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_NO_ACCESS = 10224U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_NO_ACCESS = 10247U;
 _IEC_CONST unsigned short ERR_INA_REM_DYN_PV_ZERO = 10033U;
 _IEC_CONST unsigned char inaMODE_LINKSTRINGLOCAL = 6U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BRM_LENGTH = 10228U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BRNC_ERROR = 10232U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_DECODE_TEL = 10221U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_ENCODE_TEL = 10220U;
 _IEC_CONST unsigned short ERR_INAIQ_NO_COMMSERV_BR = 10211U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_DECODE_TEL = 10241U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_ENCODE_TEL = 10240U;
 _IEC_CONST unsigned short ERR_INAS_STM_WRONG_STATE = 10110U;
 _IEC_CONST unsigned char inaMODE_LINKSTRINGREMOTE = 5U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BRM_INSTALL = 10227U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_BRM_INVALID = 10225U;
 _IEC_CONST unsigned short ERR_INAIQ_DLM_TARGET_SIZE = 10229U;
 _IEC_CONST unsigned short ERR_INAS_DO_DATA_MISMATCH = 10126U;
 _IEC_CONST unsigned short ERR_INA_LOC_OBJ_NOT_EXIST = 10036U;
 _IEC_CONST unsigned short ERR_INA_REM_OBJ_NOT_EXIST = 10035U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_OBJ_NO_ACCESS = 10246U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_OBJ_NOT_EXISTS = 10244U;
 _IEC_CONST unsigned short ERR_INA_LOC_OBJ_TYPE_CHANGED = 10038U;
 _IEC_CONST unsigned short ERR_INA_REM_OBJ_TYPE_CHANGED = 10037U;
 _IEC_CONST unsigned short ERR_INAIQ_ULM_OBJ_STATE_CONFLICT = 10245U;
 _IEC_CONST unsigned short ERR_INAIQ_UNEXPECTED_TEL = 10203U;
 _IEC_CONST unsigned short ERR_INAIQ_STM_WRONG_STATE = 10202U;
 _IEC_CONST unsigned short ERR_INAIQ_SRV_NOT_IMPLEMENTED = 10201U;
#endif




/* Datatypes and datatypes of function blocks */
typedef struct inaObjLocalEventPV
{	unsigned long pEventObj;
	unsigned long pPollObj;
} inaObjLocalEventPV;

typedef struct inaObjRemoteEventPV
{	unsigned long pEventObj;
	unsigned long hysterese;
	unsigned long idle_tmo;
	unsigned long max_tmo;
} inaObjRemoteEventPV;

typedef struct inaInfoActionStruct
{	unsigned char l_identcnt;
	unsigned short command;
	unsigned long option;
} inaInfoActionStruct;

typedef struct inaInfoLinkStruct
{	unsigned char link_type;
	unsigned char no_copy;
	unsigned char no_check;
	unsigned char dummy;
} inaInfoLinkStruct;

typedef struct inaInfoConnectStruct
{	unsigned short LinkCntMax;
	unsigned short LinkCntAct;
	unsigned short ConnAbortCnt;
} inaInfoConnectStruct;

typedef struct inaInfoIQDrvStruct
{	unsigned char ConnCntMax;
	unsigned char ConnCntAct;
} inaInfoIQDrvStruct;

typedef struct inaInfoIQDrvGlobalStruct
{	unsigned char IQDrvCntMax;
	unsigned char IQDrvCntAct;
} inaInfoIQDrvGlobalStruct;

typedef struct INAinfo
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	unsigned long l_ident;
	unsigned char mode;
	unsigned long pInfo;
	unsigned short size;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAinfo_typ;

typedef struct INAstate
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	unsigned long l_ident;
	unsigned char mode;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned short INAstate;
	unsigned short INAerror;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAstate_typ;

typedef struct INAaction
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	unsigned char l_identcnt;
	unsigned long pl_ident;
	unsigned short command;
	unsigned long option;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAaction_typ;

typedef struct INAunlink
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	unsigned long l_ident;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAunlink_typ;

typedef struct INAlink
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	unsigned char type;
	unsigned long pObjRemote;
	unsigned long pObjLocal;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned long l_ident;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit no_copy;
	plcbit no_check;
} INAlink_typ;

typedef struct INAdisconn
{
	/* VAR_INPUT (analog) */
	unsigned long c_ident;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAdisconn_typ;

typedef struct INAconnect
{
	/* VAR_INPUT (analog) */
	unsigned long pPath;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned long c_ident;
	/* VAR_INPUT (digital) */
	plcbit enable;
} INAconnect_typ;



/* Prototyping of functions and function blocks */
void INAinfo(struct INAinfo* inst);
void INAstate(struct INAstate* inst);
void INAaction(struct INAaction* inst);
void INAunlink(struct INAunlink* inst);
void INAlink(struct INAlink* inst);
void INAdisconn(struct INAdisconn* inst);
void INAconnect(struct INAconnect* inst);


#ifdef __cplusplus
};
#endif
#endif /* _INACLNT_ */

