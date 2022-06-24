/* Automation Studio generated header file */
/* Do not edit ! */

#ifndef _ASIMA_
#define _ASIMA_
#ifdef __cplusplus
extern "C" 
{
#endif

#include <bur/plctypes.h>

#include <brsystem.h>
#include <DataObj.h>
#include <inaclnt.h>
#include <sys_lib.h>

#ifndef _IEC_CONST
#define _IEC_CONST _WEAK const
#endif

/* Constants */
#ifdef _REPLACE_CONST
 #define IMA_OK 0U
 #define IMA_IDLE 1U
 #define IMA_ERROR 4U
 #define ERR_IMA_NAME 28905U
 #define ERR_IMA_IDENT 28900U
 #define ERR_IMA_NR_PV 28909U
 #define ERR_IMA_VALUE 28906U
 #define ERR_IMA_INTERN 28917U
 #define ERR_IMA_PV_ADR 28912U
 #define ERR_IMA_PV_LEN 28913U
 #define ERR_IMA_SYNTAX 28903U
 #define IMA_CONNECTING 2U
 #define ERR_IMA_KEYWORD 28904U
 #define ERR_IMA_NR_BRMOD 28911U
 #define ERR_IMA_TMP_FREE 28916U
 #define ERR_IMA_TMP_ALLOC 28915U
 #define IMA_DISCONNECTING 3U
 #define ERR_IMA_INFOSTRUCT 28902U
 #define ERR_IMA_NR_PV_LIST 28910U
 #define ERR_IMA_TASK_CYCLE 28914U
 #define ERR_IMA_DATOBJ_NAME 28901U
 #define ERR_IMA_NR_CONNECTIONS 28908U
 #define ERR_IMA_DATA_INCOMPLETE 28907U
#else
 _IEC_CONST unsigned short IMA_OK = 0U;
 _IEC_CONST unsigned short IMA_IDLE = 1U;
 _IEC_CONST unsigned short IMA_ERROR = 4U;
 _IEC_CONST unsigned short ERR_IMA_NAME = 28905U;
 _IEC_CONST unsigned short ERR_IMA_IDENT = 28900U;
 _IEC_CONST unsigned short ERR_IMA_NR_PV = 28909U;
 _IEC_CONST unsigned short ERR_IMA_VALUE = 28906U;
 _IEC_CONST unsigned short ERR_IMA_INTERN = 28917U;
 _IEC_CONST unsigned short ERR_IMA_PV_ADR = 28912U;
 _IEC_CONST unsigned short ERR_IMA_PV_LEN = 28913U;
 _IEC_CONST unsigned short ERR_IMA_SYNTAX = 28903U;
 _IEC_CONST unsigned short IMA_CONNECTING = 2U;
 _IEC_CONST unsigned short ERR_IMA_KEYWORD = 28904U;
 _IEC_CONST unsigned short ERR_IMA_NR_BRMOD = 28911U;
 _IEC_CONST unsigned short ERR_IMA_TMP_FREE = 28916U;
 _IEC_CONST unsigned short ERR_IMA_TMP_ALLOC = 28915U;
 _IEC_CONST unsigned short IMA_DISCONNECTING = 3U;
 _IEC_CONST unsigned short ERR_IMA_INFOSTRUCT = 28902U;
 _IEC_CONST unsigned short ERR_IMA_NR_PV_LIST = 28910U;
 _IEC_CONST unsigned short ERR_IMA_TASK_CYCLE = 28914U;
 _IEC_CONST unsigned short ERR_IMA_DATOBJ_NAME = 28901U;
 _IEC_CONST unsigned short ERR_IMA_NR_CONNECTIONS = 28908U;
 _IEC_CONST unsigned short ERR_IMA_DATA_INCOMPLETE = 28907U;
#endif




/* Datatypes and datatypes of function blocks */
typedef struct imaInfoStruct
{	unsigned short connectionStatus;
	unsigned short commandClearError;
	unsigned short errorNr;
	unsigned short errorSectionRowNr;
	plcstring errorSectionName[81];
	plcstring errorString[81];
} imaInfoStruct;



/* Prototyping of functions and function blocks */
unsigned short IMAcomm(unsigned long ident, struct imaInfoStruct* connInfoPV, struct imaInfoStruct* connInfoAUX);
unsigned short IMAclose(unsigned long ident);
unsigned short IMAinit(unsigned long pDatObjName, struct imaInfoStruct* errorInfo, unsigned long* ident);


#ifdef __cplusplus
};
#endif
#endif /* _ASIMA_ */

                                                           
