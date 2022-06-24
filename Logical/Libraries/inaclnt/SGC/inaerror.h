/****************************************************************************
*                                                                           *
*  Name     : inaerror.h                                                    *
*  Titel    : IQ-Driver Error-Codes                                         *
*  Version  : V 1.00                                                        *
*  Revision : 1.0                                                           *
*  Date     : 4 Nov 1999                                                    *
*                                                                           *
*                                                                           *
*  BESCHREIBUNG:                                                            *
*  -------------                                                            *
*  Header File  for   INA2000 Client FUBs: Error Code Defines               *
*                                                                           *
******************************COPYRIGHT (C)**********************************
*     THIS SOFTWARE IS THE PROPERTY OF B&R AUSTRIA: ALL RIGHTS RESERVED.    *
*     NO PART OF THIS SOFTWARE MAY BE USED OR COPIED IN ANY WAY WITHOUT     *
*              THE PRIOR WRITTEN PERMISSION OF B&R AUSTRIA.                 *
*****************************************************************************
*                                                                           *
*                                                                           *
*  Projekt  : IQ Treiber                                                    *
*  Datum    :  4.11.99                                                      *
*  Autor    : B.Eisenberger                                                 *
*                                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
*  AENDERUNGEN:                                                             *
*  ------------                                                             *
*                                                                           *
****************************************************************************/

#ifndef INAERROR_DEF
#define INAERROR_DEF 1

/*----------------------------------------------------*/
/* General Error Codes for IQ-Management: 10000-10019 */
/*----------------------------------------------------*/
#define ERR_IQVW_NO_SUCH_ID				10000	/* no such c_ident in use */
#define ERR_IQVW_MEM					10001	/* memory allocate error */
#define ERR_IQVW_DRIVER_MAX				10002	/* max IQ driver installed */
#define ERR_IQVW_CONN_MAX				10003	/* max connections used */
#define ERR_IQVW_NO_SUCH_DRV			10004	/* driver not found during INAconnect */
#define ERR_IQVW_SYNTAX					10006	/* basic syntax error in INAconnect "pPath" */
#define ERR_IQVW_ID_ALREADY				10007	/* Driver with same key already installed */
#define ERR_IQVW_VERSION				10008	/* INACLNT.BR does not fit to IQ driver */

/*-----------------------------------------------------*/
/* General Error Codes for all IQ drivers: 10020-10099 */
/*-----------------------------------------------------*/
#define ERR_INA_OFFSET					9984	/*0x2700*//* high byte offset to general INA error codes */
#define ERR_INA_INSTALL					10020	/* fatal error during installation of IQ driver */
#define ERR_INA_MEMORY					10021	/* memory allocate error */
#define ERR_INA_INPUT					10022	/* wrong input for FUB */
#define ERR_INA_PV_LIST					10023	/* wrong input in PV-List */
#define ERR_INA_ACCESS_TYPE				10024	/* access is not allowed */
#define ERR_INA_CONN_STATE				10025	/* wrong connection state for this command */
#define ERR_INA_OBJ_STATE				10026	/* wrong link state for this command */
#define ERR_INA_ACT_BUSY				10027	/* wrong action state for this command (working) */
#define ERR_INA_TYPE_CHECK				10028	/* Types between rem and loc do not match */
#define ERR_INA_REM_DATA				10029	/* internal action error with remote object */
#define ERR_INA_LOC_DATA 				10030	/* internal action error with local object */
#define	ERR_INA_REM_OBJ_TYPE			10031	/* remote: PV/DO has invalid type */
#define ERR_INA_LOC_OBJ_TYPE			10032	/* local: PV has invalid type */
#define ERR_INA_REM_DYN_PV_ZERO			10033	/* remote: dyn. PV is zero (uninitialized) */
#define ERR_INA_LOC_DYN_PV_ZERO			10034	/* local: dyn. PV is zero (uninitialized) */
#define ERR_INA_REM_OBJ_NOT_EXIST		10035	/* remote: PV or dyn. data object not exists */
#define ERR_INA_LOC_OBJ_NOT_EXIST		10036	/* local: PV or dyn. data object not exists */
#define ERR_INA_REM_OBJ_TYPE_CHANGED	10037	/* remote: Type of PV or dyn. data object changed */
#define ERR_INA_LOC_OBJ_TYPE_CHANGED	10038	/* local: Type of PV or dyn. data object changed */
#define ERR_INA_INPUTSIZE				10039	/* input structure for INAinfo is too small */
/* General Error Codes for IQ driver Object Management */
#define ERR_INA_NO_SUCH_ID				10040	/* Client: Conn/Link-Ident not valid */
#define ERR_INA_CONN_MAX				10041	/* Client: max connections reached */
#define ERR_INA_OBJ_MAX					10042	/* Client: max links for this connection reached */
#define ERR_INA_NO_ITEM_FOUND			10043	/* Client: object management could not find an(y more) item */
/* Errors Code for internal command queues */
#define ERR_INA_COMMAND_FIFO			10049	/* FUB can't be executed, internal fifo is full */
/* Error Code for Text Management */
#define ERR_INA_TEXT_MAN				10050	/* error with text management */

/*---------------------------------------------*/
/* Error Codes for Ina2000 Server: 10100-10149 */
/*---------------------------------------------*/
/* Error Codes for Server Object Management */
#define ERR_INAS_MEMORY					10100	/* Server: allocate memory error in oject management */
#define ERR_INAS_NO_SUCH_ID				10101	/* Server: Conn/Link-Ident not valid */
#define ERR_INAS_CONN_MAX				10102	/* Server: max connections reached */
#define ERR_INAS_OBJ_MAX				10103	/* Server: max links for this connection reached */
#define ERR_INAS_NO_ITEM_FOUND			10104	/* Server: obj. man. could not find an(y more) item */
/* Error Codes for Server Statemachine */
#define ERR_INAS_STM_WRONG_STATE		10110	/* default error in server statemachine */
#define ERR_INAS_REQ_FRAME				10111	/* default error in request frame */
/* Error Codes for dynamic Data Object Management */
#define ERR_INAS_DO_VERSION				10120	/* old Commserv version in use */	
#define ERR_INAS_DO_INSTALL				10121	/* DO management not installed */
#define ERR_INAS_DO_MEMORY				10122	/* memory allocate error */
#define ERR_INAS_DO_NO_SUCH_ID			10123	/* no DO wit this ident */
#define ERR_INAS_DO_MAX					10124	/* max count of DOs reached */
#define ERR_INAS_DO_DUPLICATE			10125	/* DO with the same name exists already */
#define ERR_INAS_DO_DATA_MISMATCH		10126	/* Input/Output Data do not match with DO-size */
#define ERR_INAS_DO_INPUT				10127	/* Input not valid (size, cnt, name) */
#define	ERR_INAS_DO_FULL				10128	/* DO is fill, no write possible */
#define	ERR_INAS_DO_EMPTY				10129	/* DO is empty, no read possible */
#define	ERR_INAS_DO_BLOCKED				10130	/* DO ist blocked for r/w, no r/e possible */
#define ERR_INAS_DO_BUFFER				10132	/* buffer not valid or fetch/get state not valid */
#define ERR_INAS_DO_NOT_EXIST			ERR_INA_REM_OBJ_NOT_EXIST		/* DO does not exist */
#define ERR_INAS_DO_TYPE_CHANGED		ERR_INA_REM_OBJ_TYPE_CHANGED	/* Type of DO has changed */

/*---------------------------------------------------------*/
/* Error Codes free (IQ-Management,IQ-Mapper): 10150-10199 */
/*---------------------------------------------------------*/

/*-----------------------------------------------*/
/* Special Error Codes for InaIQDrv: 10200-10249 */
/*-----------------------------------------------*/
#define ERR_INAIQ_BASE				    10200	/* Error Code Base for INAIQDrv */				
/* Error in Statemachine */
#define ERR_INAIQ_SRV_NOT_IMPLEMENTED   10201	/* Service not implemented */
#define ERR_INAIQ_STM_WRONG_STATE		10202	/* Statmachine in wrong State */
#define ERR_INAIQ_UNEXPECTED_TEL		10203	/* unexpected Telegram */
/* Connection Error */
#define ERR_INAIQ_CONN_ABORT			10210	/* Connection is aborted */
/* Error Codes from Download STM */
#define ERR_INAIQ_DLM_ENCODE_TEL		10220	/* Error from Coder Function */	
#define ERR_INAIQ_DLM_DECODE_TEL		10221	/* Error from Decoder Function */
#define ERR_INAIQ_DLM_DEFAULT           10222	/* Default Error Code */
#define ERR_INAIQ_DLM_CANCEL		    10223	/* DL aborted from User */
#define ERR_INAIQ_DLM_NO_ACCESS         10224	/* Download locked for User */
#define ERR_INAIQ_DLM_BRM_INVALID       10225	/* no BR-Modul (2b97) */        
#define ERR_INAIQ_DLM_BRM_CS            10226	/* defect BR-Modul-Checksum */ 
#define ERR_INAIQ_DLM_BRM_INSTALL       10227	/* BR-Modul install Error */ 
#define ERR_INAIQ_DLM_BRM_LENGTH        10228	/* BR-Modul wrong Length */ 
#define ERR_INAIQ_DLM_TARGET_SIZE       10229	/* too less free memory on target */ 
#define ERR_INAIQ_DLM_BURN              10230	/* Error during burning BR-Modul */ 
#define ERR_INAIQ_DLM_NO_BRNC           10231	/* no NC-Manager installed */ 
#define ERR_INAIQ_DLM_BRNC_ERROR        10232	/* Error from NC-Manager-DL-Function */ 
/* Error Codes from Upload STM */
#define ERR_INAIQ_ULM_ENCODE_TEL		10240	/* Error from Coder Function */				
#define ERR_INAIQ_ULM_DECODE_TEL		10241	/* Error from Decoder Function */
#define ERR_INAIQ_ULM_DEFAULT           10242	/* Default Error Code */
#define ERR_INAIQ_ULM_CANCEL		    10243	/* UL aborted from User */
#define ERR_INAIQ_ULM_OBJ_NOT_EXISTS	10244	/* Modul doesn't exist (wrong name) */
#define ERR_INAIQ_ULM_OBJ_STATE_CONFLICT 10245	/* Modul state != inaOBJ_READY */
#define ERR_INAIQ_ULM_OBJ_NO_ACCESS		10246	/* Upload from Module is locked */
#define ERR_INAIQ_ULM_NO_ACCESS 		10247	/* Upload is locked for User */

/*-------------------------------*/
/* Error Codes free: 10250-10499 */
/*-------------------------------*/

#endif

