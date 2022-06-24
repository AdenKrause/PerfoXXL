#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : IFBLUEFIN.C                                                  **
** version  : 1.01                                                         **
** date     : 23.06.2010                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** -Interface to Bluefin via CAN using the IMA library                     **
**  Needs Dataobject DO_IMA                                                **
** -TCPIP Server for data transfer to BluefinServer, takes messages        **
**  from Bluefin (in connected XMLBuffer array) and transfers this         **
**  to all connected clients (max 3) periodically                          **
**  if no connection to Bluefin is established the message "BLUEFIN OFF"   **
**  is sent periodically                                                   **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 15.01.2007
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.01
Date        : 23.06.2010
Revised by  : Herbert Aden
Description : Improved version from LSJet.

*/
#define _IFBLUEFIN_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include "glob_var.h"
#include "egmglob_var.h"
#include "in_out.h"
#include <AsIMA.h>
#include <Ethernet.h>
#include <string.h>
#include <ctype.h>
#include "asstring.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define IMA_CONN_OK          (0)

#define TCP_IP_PORT		2020				/* Port für Kommunikation */
#define SEND_BUFFER_LEN	3000				/* Buffer-Länge für das Senden */
#define OLD_SEND_BUFFER_LEN	2500				/* XML Buffer-Länge für Bluefin <=2.10*/
#define RECV_BUFFER_LEN	150					/* Empfangs-Buffer Länge */
#define TIMEOUT			200					/* Abbruch nach TIMEOUT * Zykluszeit Sek.*/
#define	RCVTIMEOUT		200
#define MAXCLIENTS		3
#define	LIVEBYTECNT		160
#define SEND_EVERY_N_SEC 3					/* send every 5 seconds an xml message*/

/* sonstige Konstanten */
#define TCP_IP_OK			0
#define TCP_IP_NO_RCV_DATA	27211
#define TCP_IP_BUSY		65535
#define TCPIPMGR_TIMEOUT 14893
#define BUF_ZERO 14896

#define OPEN_PORT			0
#define WAIT_OPEN_PORT		1
#define SEND_DATA			2
#define WAIT_SEND_DATA		3
#define RECV_DATA			4
#define WAIT_RECV_DATA		5
#define CLOSE_PORT			6
#define WAIT_CLOSE_PORT		7
#define WAIT				8
#define PREPARE_SEND_DATA   9

/****************************************************************************/
/**                                                                        **/
/**                      TYPEDEFS AND STRUCTURES                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      PROTOTYPES OF LOCAL FUNCTIONS                     **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                           GLOBAL VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    INT                 IMAStatusInit,IMAStatusComm;
_LOCAL    imaInfoStruct       errorInfo,ConnInfoPV,ConnInfoAUX;
_LOCAL    UDINT               IMAident;

_LOCAL    INT                 IMAStatusInitVCP,IMAStatusInitXSOld,IMAStatusCommVCP,IMAStatusCommXSOld;
_LOCAL    imaInfoStruct       errorInfoVCP,ConnInfoPVVCP,ConnInfoAUXVCP;
_LOCAL    imaInfoStruct       errorInfoXSOld,ConnInfoPVXSOld,ConnInfoAUXXSOld;
_LOCAL    UDINT               IMAidentVCP,IMAidentXSOld;

_LOCAL    UINT                byte_link_cnt;

/* structs for Ethernet-Library-Functions */
_LOCAL	TCPserv_typ			TCP_Server[MAXCLIENTS];
_LOCAL	TCPrecv_typ			TCP_Recv[MAXCLIENTS];
_LOCAL	TCPsend_typ			TCP_Send[MAXCLIENTS];
_LOCAL	TCPclose_typ		TCP_Close[MAXCLIENTS];
_LOCAL	SINT	Step[MAXCLIENTS];
_LOCAL	UINT	SendTimeout[MAXCLIENTS];
_LOCAL	UINT	RcvLength;

/* Buffer for XML Message */
_LOCAL SINT	XMLBuffer[SEND_BUFFER_LEN];
_LOCAL SINT	OldXMLBuffer[OLD_SEND_BUFFER_LEN];
_LOCAL UINT EnablePVComm;
_LOCAL UINT	DataLock;
_LOCAL BOOL                CAN_EGM1;
_LOCAL BOOL                CAN_EGM2;
_LOCAL BOOL                CAN_VCP;
_LOCAL STRING              BFVERSION[9];
_LOCAL UINT                IMACommStep;
_LOCAL UINT                VisConnOK;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
/* for IMA Comm */
static    STRING              DOName[32]="do_ima";
static    STRING              DONameVCP[32]="do_vcp";
static    STRING              DONameXSOld[32]="do_xsold";
/* for Ethernet server */
static    SINT                SendBuffer[MAXCLIENTS][SEND_BUFFER_LEN];
static    SINT                RecvBuffer[MAXCLIENTS][RECV_BUFFER_LEN];
static    SINT                RecvData[MAXCLIENTS][RECV_BUFFER_LEN];

static    int                 CurrentClient,parse;
static    USINT               Count[MAXCLIENTS];
static    int                 i;
static    USINT               SendDataCnt[MAXCLIENTS];
static    BOOL                CloseConnection[MAXCLIENTS];
static    UINT                NumberOfFreePorts;
static    UINT                WaitCounter[MAXCLIENTS];
static    BOOL                ConnectedTo3000Byte;
/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/
_INIT void InitFunc(void)
{
#ifdef MODE_PERFORMANCE
	int i;
	ConnectedTo3000Byte = FALSE;
	IMACommStep = 0;
	IMAStatusInit = IMAinit((UDINT)DOName, &errorInfo, &IMAident);
	IMAStatusInitVCP = IMAinit((UDINT)DONameVCP, &errorInfoVCP, &IMAidentVCP);
	IMAStatusInitXSOld = IMAinit((UDINT)DONameXSOld, &errorInfoXSOld, &IMAidentXSOld);

	CurrentClient = 1;
	/* Initialise TCP-IP Data structs: */
	for(i = 0;i<MAXCLIENTS;i++)
	{
		SendDataCnt[i] = 0;
		/* TCP-IP Client: */
		TCP_Server[i].enable	= TRUE;
		TCP_Server[i].porta	= TCP_IP_PORT+i;

		/* TCP-IP Client Senden: */
		TCP_Send[i].enable		= TRUE;
		TCP_Send[i].buffer		= (UDINT)&SendBuffer[i][0];
		TCP_Send[i].buflng		= SEND_BUFFER_LEN;

		/* TCP-IP Client Empfangen: */
		TCP_Recv[i].enable		= TRUE;
		TCP_Recv[i].buffer		= (UDINT)&RecvBuffer[i][0];
		TCP_Recv[i].mxbuflng	= RECV_BUFFER_LEN;

		/* TCP-IP Client schließen */
		TCP_Close[i].enable	= TRUE;

		Step[i] = OPEN_PORT;
		Count[i] = 0;
		CloseConnection[i] = 0;
	}
	NumberOfFreePorts = MAXCLIENTS;
	AlarmBitField[35] = FALSE;
	VisConnOK = 0;
#endif
}




_CYCLIC void CyclicFunc(void)
{
/*****************************************************************************/
/* get data from Bluefin via INA comm */
/*****************************************************************************/

#ifdef MODE_BOTH
/* Performance und Bluefin: EGM Signale direkt aus der Software*/
	StatusEGM1 = NoError;
	StatusEGM2 = Ready;
	VCP = VCP_OK;
#else
/* Performance allein: EGM Signale via CAN Anbindung oder Eingänge*/

	if(IMAStatusInit == IMA_OK)  /* Init was OK (should always be the case) */
	{
		switch (IMACommStep)
		{
			case 0:
			{ /* Versuch, mit der neuesten Version zu connecten */
				IMAStatusCommVCP = IMAcomm(IMAidentVCP, &ConnInfoPVVCP, &ConnInfoAUXVCP);

				if(IMAStatusCommVCP == IMA_OK)
				{
/*
 * Bluefin XS Version bis 3.04 hat 2500 Byte Puffer, deswegen dann anderen
 * Verbindungs DB benutzen
 */
					if(  (BFVERSION[0] == '3')
					  && (BFVERSION[2] == '0')
					  && (BFVERSION[5] != 'L') /* LC Version erkennen, die hat nämlich
					                            * auch 3000 Byte und VCP Verbindung
					                            */
				  )
					{
						IMACommStep = 20;
					}
					else
					{
						ConnectedTo3000Byte = TRUE;
						StatusEGM1 = CAN_EGM1;
						StatusEGM2 = CAN_EGM2;
						VCP = CAN_VCP;
						AlarmBitField[35] = FALSE;
						VisConnOK = 1;

						if(NumberOfFreePorts < MAXCLIENTS)
							EnablePVComm = 1;
						else
							EnablePVComm = 0;
					}
				}
				else /* fehlgeschlagen: Verbindung beenden und alte Version probieren */
					IMACommStep = 20;
				break;
			}


			case 20:
			{ /* Versuch, mit Version XS mit 2500 Byte zu connecten */
				IMAStatusCommXSOld = IMAcomm(IMAidentXSOld, &ConnInfoPVXSOld, &ConnInfoAUXXSOld);

				if(IMAStatusCommXSOld == IMA_OK)
				{
					ConnectedTo3000Byte = FALSE;
					StatusEGM1 = CAN_EGM1;
					StatusEGM2 = CAN_EGM2;
					VCP = CAN_VCP;
					AlarmBitField[35] = FALSE;
					VisConnOK = 1;

					if(NumberOfFreePorts < MAXCLIENTS)
						EnablePVComm = 1;
					else
						EnablePVComm = 0;
				}
				else /* fehlgeschlagen: Verbindung beenden und alte Version probieren */
					IMACommStep = 30;
				break;
			}


			case 30:
			{ /* Versuch, mit der alten Version zu connecten */
				IMAStatusComm = IMAcomm(IMAident, &ConnInfoPV, &ConnInfoAUX);

				if(IMAStatusComm == IMA_OK)
				{
/*
 * Bluefin XS Version bis 3.04 hat auch 2500 Byte Puffer, wird nicht per Fehler erkannt,
 * weil hier (im hierzu gehörigen DB) nicht auf CAN_VCP zugegriffen wird
 * deshalb über Versionsnummer
 */
					if(  (BFVERSION[0] == '3')
					  )
					{
						IMACommStep = 20;
					}
					else
					{
						ConnectedTo3000Byte = FALSE;
						StatusEGM1 = CAN_EGM1;
						StatusEGM2 = CAN_EGM2;
						VCP = CAN_VCP;
						AlarmBitField[35] = FALSE;
						VisConnOK = 1;

						if(NumberOfFreePorts < MAXCLIENTS)
							EnablePVComm = 1;
						else
							EnablePVComm = 0;
					}
				}
				else /* fehlgeschlagen: Fehlermeldung und von vorn beginnen */
				{
					ConnInfoPVVCP.commandClearError = TRUE;
					ConnInfoPVXSOld.commandClearError = TRUE;
					ConnInfoPV.commandClearError = TRUE;
					VisConnOK = 0;
					IMACommStep = 0;
					StatusEGM1 = InEGM1;
					StatusEGM2 = InEGM2;
					VCP = In_VCPOK;
					break;
				}
				break;
			}
		}/*switch*/
	}
	else  /* comm not OK: EGM signals via Digital inputs*/
	{
		ConnectedTo3000Byte = FALSE;
		StatusEGM1 = InEGM1;
		StatusEGM2 = InEGM2;
		VCP = In_VCPOK;
	}


/*****************************************************************************/
/* TCP/IP Server */
/*****************************************************************************/
	NumberOfFreePorts = 0;

	for(CurrentClient = 0;CurrentClient<MAXCLIENTS;CurrentClient++)
	{
	/***** Öffnen des Ports ************************************************/
		if (Step[CurrentClient] == OPEN_PORT)
		{
 			CloseConnection[CurrentClient] = 0;
			/* 1. Aufruf der Funktion */
			TCPserv (&TCP_Server[CurrentClient]);
			/* Status prüfen: */
			if (TCP_Server[CurrentClient].status == TCP_IP_OK)
			{
				/* Port geöffnet */
				/* Identifier zuordnen ... */
				TCP_Send[CurrentClient].cident = TCP_Server[CurrentClient].cident;
				TCP_Recv[CurrentClient].cident = TCP_Server[CurrentClient].cident;
				TCP_Close[CurrentClient].cident = TCP_Server[CurrentClient].cident;
				/* ... und zum nächsten Schritt gehen */
				Step[CurrentClient] = RECV_DATA;
				SendTimeout[CurrentClient] = 0;
			}
			else
				NumberOfFreePorts++;
		} /* if (Step == WAIT_OPEN_PORT) */

	/***** Daten Empfangen *************************************************/
		if (Step[CurrentClient] == RECV_DATA)
		{
			TCPrecv (&TCP_Recv[CurrentClient]);
			/* Status prüfen: */
			if (TCP_Recv[CurrentClient].status == TCP_IP_OK)
			{
				if(TCP_Recv[CurrentClient].rxbuflng == 0) /*CLOSE von Gegenstelle*/
				{
					Step[CurrentClient] = CLOSE_PORT;
					Count[CurrentClient] = 0;
				}
				else	/* Daten empfangen -> auswerten*/
				{
					parse = 0;
					/*einzelne Zeichen (via telnet)*/
					if(TCP_Recv[CurrentClient].rxbuflng < 3)
					{
						RecvData[CurrentClient][Count[CurrentClient]] = RecvBuffer[CurrentClient][0];
						Count[CurrentClient]++;
/* safety !*/
						if (Count[CurrentClient] >= RECV_BUFFER_LEN )
						{
							strcpy(SendBuffer[CurrentClient],"HEY! \a\aSTOP SENDING!!");
							Step[CurrentClient] = SEND_DATA;
							TCP_Send[CurrentClient].buffer		= (UDINT)&SendBuffer[CurrentClient];
							TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
							Count[CurrentClient] = 0;
							parse = 0;
						}
						else
						if( RecvBuffer[CurrentClient][0] == 10 || RecvBuffer[CurrentClient][0] == 13 )
						{
							RecvData[CurrentClient][Count[CurrentClient]] = 0;
							parse = 1;
						}
					}
					else
					{
						memcpy(RecvData[CurrentClient],RecvBuffer[CurrentClient],TCP_Recv[CurrentClient].rxbuflng);
						Count[CurrentClient] = TCP_Recv[CurrentClient].rxbuflng;
						parse = 1;
					}

					if(parse)
					{
						i=0;
						while(  ( RecvData[CurrentClient][i] != 0 )
						     && ( i < RECV_BUFFER_LEN) )
						{
							RecvData[CurrentClient][i] = toupper( RecvData[CurrentClient][i] );
							i++;
						}
						RecvData[CurrentClient][Count[CurrentClient]-1] = 0;

						if (strncmp(RecvData[CurrentClient],"BYE",3) == 0
						||	strncmp(RecvData[CurrentClient],"QUIT",4) == 0
						||	strncmp(RecvData[CurrentClient],"EXIT",4) == 0)
						{
							strcpy(SendBuffer[CurrentClient],"Good bye, cu next time...\r\n");
							CloseConnection[CurrentClient] = 1;
						}
						Step[CurrentClient] = SEND_DATA;
						TCP_Send[CurrentClient].buffer		= (UDINT)&SendBuffer[CurrentClient];
						TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
						Count[CurrentClient] = 0;
						parse = 0;
					}
					SendDataCnt[CurrentClient]=0;
				}
			}
			else
			{
/*nothing received: ok, send from time to time*/
				SendDataCnt[CurrentClient]++;
				if( SendDataCnt[CurrentClient] >= 10*SEND_EVERY_N_SEC )
				{
					if(  (IMAStatusComm    == IMA_OK)
					 ||  (IMAStatusCommVCP == IMA_OK)
					 ||  (IMAStatusCommXSOld == IMA_OK) )
					{
						DataLock = 1;
						Step[CurrentClient] = WAIT;
						SendDataCnt[CurrentClient] = 0;
						WaitCounter[CurrentClient] = 0;
					}
					else
					{
						strcpy(SendBuffer[CurrentClient],"BLUEFIN OFF");
						TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
						Step[CurrentClient] = SEND_DATA;
						SendDataCnt[CurrentClient] = 0;
					}
				}
			}
		}

/* SENDEDATEN VORBEREITEN */
		if (Step[CurrentClient] == PREPARE_SEND_DATA)
		{
			if(ConnectedTo3000Byte)
			{
/* Bluefin XS Version mit 3000 Byte xml Puffer */
				if( strncmp(XMLBuffer,"<KrauseMessage>",15) == 0 )
				{
					memcpy(SendBuffer[CurrentClient],XMLBuffer,sizeof(XMLBuffer));
					TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
					Step[CurrentClient] = SEND_DATA;
					SendDataCnt[CurrentClient] = 0;
					DataLock = 0;
				}
			}
			else
			{
/* Bluefin standard Version mit 2500 Byte xml Puffer */
				if( strncmp(OldXMLBuffer,"<KrauseMessage>",15) == 0 )
				{
					memcpy(SendBuffer[CurrentClient],OldXMLBuffer,sizeof(OldXMLBuffer));
					SendBuffer[CurrentClient][sizeof(OldXMLBuffer)-1] = 0;
					TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
					Step[CurrentClient] = SEND_DATA;
					SendDataCnt[CurrentClient] = 0;
					DataLock = 0;
				}
			}
		}


/************** senden ****************************/

		if (Step[CurrentClient] == SEND_DATA)
		{

			TCPsend (&TCP_Send[CurrentClient]);
			/* Status prüfen: */
			if (TCP_Send[CurrentClient].status == TCP_IP_OK)
			{
				/* Senden war OK */
				if (CloseConnection[CurrentClient])
					Step[CurrentClient] = CLOSE_PORT;
				else
					Step[CurrentClient] = RECV_DATA;
				CloseConnection[CurrentClient] = 0;
				SendTimeout[CurrentClient] = 0;
			}
			else
			if (TCP_Send[CurrentClient].status == TCP_IP_BUSY)
			{
			/* VOID, just wait*/
			}
			else
			if (TCP_Send[CurrentClient].status == ERR_ETH_NET_SLOW)
			{
			/* Check for timeout, after 3 sec close the port and wait for reconnection */
				SendTimeout[CurrentClient]++;
				if(SendTimeout[CurrentClient] > 30) /* 3 sec*/
					Step[CurrentClient] = CLOSE_PORT;
			}
			else
			if (TCP_Send[CurrentClient].status == TCPIPMGR_TIMEOUT)
			{
			/* VOID, just wait*/
			}
			else	/* any other state, close after a certain time*/
			{
				SendTimeout[CurrentClient]++;
				if(SendTimeout[CurrentClient] > 30) /* 3 sec*/
					Step[CurrentClient] = CLOSE_PORT;
			}
		}



	/***** Port schließen **************************************************/
		if (Step[CurrentClient] == CLOSE_PORT)
		{
			SendTimeout[CurrentClient] = 0;
			TCPclose (&TCP_Close[CurrentClient]);
			/* Status prüfen: */
			if (TCP_Close[CurrentClient].status == TCP_IP_OK)
			{
				/* Port geschlossen, neu öffnen: */
				Step[CurrentClient] = OPEN_PORT;
			}
		} /* if (Step == WAIT_CLOSE_PORT) */


		if (Step[CurrentClient] == WAIT)
		{
			WaitCounter[CurrentClient]++;
			if( WaitCounter[CurrentClient] > 20 )
			{
				Step[CurrentClient] = PREPARE_SEND_DATA;
				WaitCounter[CurrentClient] = 0;
			}
		} /* if (Step == WAIT) */

	} /*for*/
#endif

}


