#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : Jet3                                                         **
** filename : UDP.C                                                        **
** version  : 1.00                                                         **
** date     : 24.09.2010                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** implements an udp client/server                                         **
**                                                                         **
** Copyright (c) 2010, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 24.09.2010
Revised by  : Herbert Aden
Description : Original version. Based on XJet Version


*/

#define _UDP_C

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include "udp.h"
#include "auxfunc.h"


/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/



/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/

/*
 * copies the last n chars from src to dest, if n>strlen(src) the complete
 * src string will be copied
 * returns the actual no of chars copied
 */
int strendcpy(char *dest,const char *src,int n)
{
	const char *p1;
	int srclen = strlen(src);
	int offs = srclen - n;
	int res = srclen;

	if(offs > 0)
	{
		p1 = &src[offs];
		strcpy(dest,p1);
		res = n;
	}
	else
		strcpy(dest,src);

	return res;
}

/****************************************************************************/
/* Parser */
/****************************************************************************/
void Parse(char *buf)
{
	char *pctmp;
	char token[MAXTOKENS][MAXTOKENLENGTH];
	int i = 0;
/* erstmal alle Tokens initialisieren*/
	for(i=0;i<MAXTOKENS;i++)
		strcpy(token[i],"-");

	strncpy(DebugRecStr,buf,DEBUGSTRSIZE-1);
	DebugRecStr[DEBUGSTRSIZE-1] = '\0';

	i = 0;
	pctmp = strtok(&buf[0],(char *)";");
	do
	{
		if(pctmp != NULL)
		{
			strncpy(token[i],pctmp,MAXTOKENLENGTH-1);
			token[i][MAXTOKENLENGTH-1] = '\0';
			i++;
		}
		pctmp = strtok(NULL,(char *)";");
	}
	while (pctmp != NULL && i<MAXTOKENS);


	if( STREQN(&buf[0],"la paloma",9)
	 || STREQN(&buf[0],"La Paloma",9)
	 || STREQN(&buf[0],"La paloma",9)
	 || STREQN(&buf[0],"la Paloma",9)
	  )
		SendTestAnswer = TRUE;
	else
	if( i > 2 )
	{
/* Netlink Kommandos */
		if(STREQN(token[0],"PERFOXXL",8))
		{
			if(i > 3)
			{
				if(STREQN(token[1],"NEXTJOB",7))
				{
/*
token2 -> job id
token3 -> plate type
token4 -> next plate type
token5 -> plateconfig
 */
					int tmpPlateType = brsatoi((UDINT)token[3]);
					strcpy(tmpID,token[2]);
					if(!START)
						SendJobAnswer = SEND_MACHINE_NOT_STARTED;
					else
					if(PlateAtAdjustedPosition.present && (STREQN(PlateAtAdjustedPosition.ID,"???",3)))
					{
						strcpy(PlateAtAdjustedPosition.ID,tmpID);
						SendJobAnswer = SEND_OK;
					}
					else
					if(PlateAtFeeder.present && (STREQN(PlateAtFeeder.ID,"???",3)))
					{
						strcpy(PlateAtFeeder.ID,tmpID);
						SendJobAnswer = SEND_OK;
						if( (tmpPlateType == brsatoi((UDINT)token[4]))
						 && !PlateToDo.present
						  )
						{
							PlateToDo.present = TRUE;
							PlateToDo.PlateType = tmpPlateType;
							PlateToDo.NextPlateType = 0;
							PlateToDo.PlateConfig = PlateAtFeeder.PlateConfig;
							strcpy(PlateToDo.ID,"???");
						}
					}
					else
					if(PlateToDo.present)
					{
						if(STREQN(PlateToDo.ID,"???",3))
						{
							strcpy(PlateToDo.ID,tmpID);
							if(!ManualMode)
								PlateToDo.NextPlateType = brsatoi((UDINT)token[4]);
							SendJobAnswer = SEND_OK;
						}
						else
							SendJobAnswer = SEND_BUFFER_FULL;
					}
					else
					if((tmpPlateType > 0) && (tmpPlateType < MAXPLATETYPES))
					{
						USINT PlateTypeToCheck;
						if((GlobalParameter.TrolleyLeft == 0) || (GlobalParameter.TrolleyLeft >= MAXTROLLEYS))
							GlobalParameter.TrolleyLeft = 1;

						if(ManualMode)
							PlateTypeToCheck = PlateType; /* manual mode platetype selected by user */
						else
							PlateTypeToCheck = Trolleys[GlobalParameter.TrolleyLeft].PlateType;

						if(tmpPlateType != PlateTypeToCheck)
							SendJobAnswer = SEND_PLATE_NOT_LOADED;
						else
						{
							PlateToDo.present = TRUE;
							PlateToDo.PlateType = tmpPlateType;
							strcpy(PlateToDo.ID,tmpID);
							if(STREQ(token[5],"10"))
								PlateToDo.PlateConfig = LEFT;
							if(STREQ(token[5],"02"))
								PlateToDo.PlateConfig = RIGHT;
							if(STREQ(token[5],"12"))
								PlateToDo.PlateConfig = BOTH;
							if(STREQ(token[5],"3") || STREQ(token[5],"30"))
								PlateToDo.PlateConfig = PANORAMA;
							if(STREQ(token[5],"4") || STREQ(token[5],"40"))
								PlateToDo.PlateConfig = BROADSHEET;

							if(!ManualMode)
								PlateToDo.NextPlateType = brsatoi((UDINT)token[4]);

							SendJobAnswer = SEND_OK;
						}
					}
					else
						SendJobAnswer = SEND_PLATE_NOT_LOADED;
				}
			}
		}
		else
/* LSJet Kommandos */
		if(STREQN(token[0],"JET",3))
		{
			if(i == 3)
			{
				if(STREQN(token[2],"\r\n",2))
				{
					if( STREQN(token[1],"START",5))
					{
						START = TRUE;
						SendJetStartOK = TRUE;
					}
					else
					if( STREQN(token[1],"STOP",4))
					{
						START = FALSE;
						SendJetStopOK = TRUE;
					}
					else
					if( STREQN(token[1],"REFERENCE",9))
					{
						ReferenceStart = TRUE;
						SendJetRefOK = TRUE;
					}
					else
					if( STREQN(token[1],"CONTROLVOLTAGE",14))
					{
						SendJetControlvoltageOK = TRUE;
					}
					else
					if( STREQN(token[1],"ANSWEROK",8))
					{
						gAbfrageOK = TRUE;
						AlarmBitField[15] = FALSE; /* Trolley leer */
						AlarmBitField[26] = FALSE; /* Ausrichtfehler */
						AlarmBitField[28] = FALSE; /* Vakuumfehler Aufnahme */
						AlarmBitField[29] = FALSE; /* Vakuumfehler Übergabe */
					}
					else
					if( STREQN(token[1],"ANSWERCANCEL",12))
					{
						gAbfrageCancel = TRUE;
					}
					else
					if( STREQN(token[1],"CLEARMACHINE",12))
					{
						gClearLinePressed = TRUE;
					}
				}
			}
		}
		else
		if( STREQN(token[0],"CLEARMACHINE",12))
		{
			gClearLinePressed = TRUE;
		}
	}
}


/****************************************************************************/
/* INIT Teil*/
/****************************************************************************/
_INIT void init_UDP(void)
{
	strcpy(&buffer[0], "nothing sent");             /* Initialize the buffer */
	strcpy(&receivebuffer[0], "nothing received");             /* Initialize the buffer */
	counter = 0;
	UDPStep = 0;
	UDPRcvStep = 0;
	CounterMax = 5;
	StatusRequestTimeout.IN = FALSE;
	StatusRequestTimeout.PT = 1200;
	StatusRequestTimer.IN = FALSE;
	SendStatusInterval = 30;
	StatusRequestTimer.PT = SendStatusInterval;
	PlateAtFeeder.present = FALSE;
	PlateAtFeeder.PlateType = 0;
	strcpy(PlateAtFeeder.ID,"");
	PlateAtFeeder.ErrorCode = 0;
	PlateAtFeeder.Status = 0;

	PlateToDo = PlateAtFeeder;
	PlateToDo.PlateConfig = BOTH;
	PlateAtAdjustedPosition = PlateAtFeeder;
}

/****************************************************************************/
/* CYCLIC Teil*/
/****************************************************************************/
_CYCLIC void cyclic_UDP(void)
{
	TON_10ms(&StatusRequestTimeout);


	if( StatusRequestTimeout.Q )
	{
		StatusRequestTimeout.IN = FALSE;
	}

/* Statusrequest alle 1 sec triggern*/
	MachineOnlineEdgeR.CLK = START;
	R_TRIG(&MachineOnlineEdgeR);
	MachineOfflineEdgeR.CLK = !START;
	R_TRIG(&MachineOfflineEdgeR);
	if(MachineOnlineEdgeR.Q || MachineOfflineEdgeR.Q)
		SendStatusRequest = TRUE;

	StatusRequestTimer.IN = !SendStatusRequest;
	StatusRequestTimer.PT = SendStatusInterval;
	TON_10ms(&StatusRequestTimer);

	switch (UDPStep)
	{
		case 0:
		{

			if(
			    SendStatusRequest
			 || SendJobStates
			 || (SendJobAnswer != 0)
			 )
			{
				UDPStep = 10;
			}
			else
			if(StatusRequestTimer.Q)
			{
				StatusRequestTimer.IN = FALSE;
				{
					SendStatusRequest = TRUE;
					SendJobStates = TRUE;
				}
			}

			break;
		}

		case 10:
		{
			UDPopen_1.enable = TRUE;
			UDPopen_1.port = 60000;
			UDPopen_1.pIfAddr = NULL;
			UdpOpen(&UDPopen_1);
			if(UDPopen_1.status == 0)
				UDPStep = 20;
			if(UDPopen_1.status == udpERR_ALREADY_EXIST)
			{
				UDPclose_1.enable = TRUE;             /* Enables the FBK */
				UDPclose_1.ident = UDPopen_1.ident;
				UdpClose(&UDPclose_1);        /* Close the connection */
			}
			break;
		}
		case 20:
		{
			int length = 0;

			if(SendJobAnswer != 0)
			{
				switch (SendJobAnswer)
				{
					case SEND_OK:
					{
						strcpy(buffer,"PERFOXXL;NEXTJOB;");
						strcat(buffer,tmpID);
						strcat(buffer,";ACCEPTED");
						break;
					}
					case SEND_BUFFER_FULL:
					{
						strcpy(buffer,"PERFOXXL;NEXTJOB;");
						strcat(buffer,tmpID);
						strcat(buffer,";BUSY");
						break;
					}
					case SEND_MACHINE_NOT_STARTED:
					{
						strcpy(buffer,"PERFOXXL;NEXTJOB;");
						strcat(buffer,tmpID);
						strcat(buffer,";NOTSTARTED");
						break;
					}
					case SEND_PLATE_NOT_LOADED:
					{
						strcpy(buffer,"PERFOXXL;NEXTJOB;");
						strcat(buffer,tmpID);
						strcat(buffer,";WRONGPLATETYPE");
						break;
					}
					default:
					{
						strcpy(buffer,"PERFOXXL;NEXTJOB;");
						strcat(buffer,tmpID);
						strcat(buffer,";ERROR");
						break;
					}
				}
				strcat(buffer,"\r\n");
				length = strlen(buffer);
				SendJobAnswer = 0;
			}
			else
			if(SendStatusRequest)
			{
				SendStatusRequest = FALSE;
/*
				strcpy(buffer,"alive");
				strcat(buffer,"\r\n");
				length = strlen(buffer);
*/
			}
			else
			if(SendTestAnswer)
			{
				SendTestAnswer = FALSE;
				strcpy(buffer,"einmal muss es vohorbei sein...");
				strcat(buffer,"\r\n");
				length = strlen(buffer);
			}
			else
			if(SendJobStates)
			{
				SendJobStates = FALSE;
			}
/*
			else
			if(SendJobStates)
			{
				int i=0;
				strcpy(buffer,"PLATESTATE;");
				for(i=0;i<=1;i++)
				{
					if(PlateToDo[i].present)
					{
						strcat(buffer,PlateToDo[i].ID);
						strcat(buffer,STR_PLATESTATE_RESERVED);
					}
				}
				for(i=0;i<=1;i++)
				{
					if(PlateAtFeeder[i].present)
					{
						strcat(buffer,PlateAtFeeder[i].ID);
						strcat(buffer,STR_PLATESTATE_FEEDER);
					}
				}
				for(i=0;i<=1;i++)
				{
					if(PlateAtShuttle[i].present)
					{
						strcat(buffer,PlateAtShuttle[i].ID);
						strcat(buffer,STR_PLATESTATE_SHUTTLE);
					}
				}
				if(PlateAtDropPosition.present)
				{
					strcat(buffer,PlateAtDropPosition.ID);
					strcat(buffer,STR_PLATESTATE_TABLE);
				}
				if(PlateAtAdjustedPosition.present)
				{
					strcat(buffer,PlateAtAdjustedPosition.ID);
					switch (PlateAtAdjustedPosition.Status)
					{
						case PLATESTATE_ALIGNED:
						{
							strcat(buffer,STR_PLATESTATE_ALIGNED);
							break;
						}
						case PLATESTATE_IMAGING:
						{
							strcat(buffer,STR_PLATESTATE_IMAGING);
							break;
						}
						case PLATESTATE_DELOADING:
						{
							strcat(buffer,STR_PLATESTATE_DELOADING);
							break;
						}
						default:
						{
							strcat(buffer,STR_PLATESTATE_ALIGNED);
							break;
						}
					}
				}
				if(PlateOnConveyorBelt.present)
				{
					strcat(buffer,PlateOnConveyorBelt.ID);
					strcat(buffer,STR_PLATESTATE_CONVEYORBELT);
				}

				for(i=0;i<FAILEDPLATESARRAYSIZE;i++)
				{
					if(FailedPlate[i].present)
					{
						strcat(buffer,FailedPlate[i].ID);
						switch (FailedPlate[i].Status)
						{
							case PLATESTATE_FEEDERVAC_FAIL:
							{
								strcat(buffer,STR_PLATESTATE_FEEDERVAC_FAIL);
								break;
							}
							case PLATESTATE_SHUTTLEVAC_FAIL:
							{
								strcat(buffer,STR_PLATESTATE_SHUTTLEVAC_FAIL);
								break;
							}
							case PLATESTATE_ALIGN_FAIL:
							{
								strcat(buffer,STR_PLATESTATE_ALIGN_FAIL);
								break;
							}
							default:
							{
								strcat(buffer,STR_GENERAL_ERROR);
								break;
							}
						}

						ClearStation(&FailedPlate[i]);
					}
				}

				if(strlen(buffer) <= strlen("PLATESTATE;"))
					strcat(buffer,"NO PLATES;");

				strcat(buffer,"\r\n");
				length = strlen(buffer);
				SendJobStates = FALSE;
			}
*/
/* Add more send commands here*/


			if( (length > 0) && (length < SENDBUFSIZE) )
			{
				UDPsend_1.enable = TRUE;             /* Enables the FBK */
				UDPsend_1.ident = UDPopen_1.ident; /* Copy the Ident number */
				UDPsend_1.pData = (UDINT)&buffer[0];/* Address of the buffer */
				UDPsend_1.datalen = length;
				UDPsend_1.pHost = (UDINT) &RemoteIP[0];
				UDPsend_1.port = 60000;     /* Port number of the receiver */
				UDPStep = 35;
				strncpy(DebugSendStr,buffer,DEBUGSTRSIZE-1);
				DebugSendStr[DEBUGSTRSIZE-1] = '\0';
			}
			else
			{
				UDPStep = 40;
			}
			break;
		}

		case 35:
		{
			UdpSend(&UDPsend_1);        /* Sends the data to the member */
			if(UDPsend_1.status == 0)
			{
				UDPStep = 40;
				strcpy(buffer,"");
			}
			break;
		}

		case 40:
		{
			UDPclose_1.enable = 1;             /* Enables the FBK */
			UDPclose_1.ident = UDPopen_1.ident;
			UdpClose(&UDPclose_1);        /* Close the connection */
			if(UDPclose_1.status == 0)
				UDPStep = 0;
			counter = 0;
			break;
		}
	}

	switch (UDPRcvStep)
	{

		case 0:
		{
			UDPopen_Rcv.enable = TRUE;
			UDPopen_Rcv.port = 60001;
			UDPopen_Rcv.pIfAddr = NULL;
			UdpOpen(&UDPopen_Rcv);
			if(UDPopen_Rcv.status == 0)
				UDPRcvStep = 20;
			if(UDPopen_Rcv.status == udpERR_ALREADY_EXIST)
			{
				UDPclose_1.enable = TRUE;             /* Enables the FBK */
				UDPclose_1.ident = UDPopen_Rcv.ident;
				UdpClose(&UDPclose_1);        /* Close the connection */
			}

			break;
		}
		case 20:
		{
			STRING oldRemoteIP[20];
			strcpy(oldRemoteIP,RemoteIP);

			UDPreceive_1.enable = TRUE;             /* Enables the FBK */
			UDPreceive_1.ident = UDPopen_Rcv.ident; /* Copy the Ident number */
			UDPreceive_1.pData = (UDINT)&receivebuffer[0];/* Address of the buffer */
			UDPreceive_1.datamax = sizeof(receivebuffer)-1;   /* Size of the buffer */
			UDPreceive_1.pIpAddr = (UDINT) &RemoteIP[0];

			UdpRecv(&UDPreceive_1);        /* receives data from the member */

			remotePort = UDPreceive_1.port;

			if( UDPreceive_1.status == 0 )
			{
				if( !STREQ(oldRemoteIP,RemoteIP)
				 && (RemoteIP[0] == '1')
				 && (RemoteIP[1] == '9')
				 && (RemoteIP[2] == '2')
				 && (RemoteIP[3] == '.')
				 )
					saveRemoteIP = TRUE;
				receivebuffer[UDPreceive_1.recvlen] = 0;
				Parse(receivebuffer);

			} /* if status==0 */
			break;
		}
	}

}





/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


