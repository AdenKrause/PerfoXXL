#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/***************************************************************************/
/*                                                                         */
/* Datei:    ip_srv_c.c                                                    */
/*                                                                         */
/* Autor:    Stefan Hebel / B&R Technisches Büro Nord                      */
/*                                                                         */
/* Abstrakt: Diese Task wartet darauf, daß sich ein TCP-IP Client am ein-  */
/*           gestellten Port anzumelden. Wenn die Verbindung hergestellt   */
/*           worden ist, wartet der Servar auf Daten vom Client. Nachdem   */
/*           Daten erfolgreich empfangen wurden, werden die eigenen Daten  */
/*           gesendet.Nach einem Timeout wird der Port geschlossen und es  */
/*           wird versucht, die Verbindung wieder neu aufzubauen.          */
/*                                                                         */
/* Datum:    11.Aug.2002      letzte Änderungen: 11.Aug.2002 / Heb         */
/*                                                                         */

/*	1.00	xx.xx.xx	  	erstes release			HA		*/
/*	1.10	05.x2.x4	  	bei Befehl 14 wird jetzt auch der Status mitgeschickt		HA		*/
/*							abschaltbar via Kompatibilitätsflag */
/*	1.20	18.09.06	  			*/
/*		 - mehr Sicherheit beim Telnet Server (Längenprüfung der übertragenen strings*/
/*		   besonders beim Einzelzeichen Empfang via telnet konnte man vorher sehr leicht das PP*/
/*		   zum Absturz bringen*/
/*		 - Server ist jetzt nicht mehr case-sensitive und kennt bye, quit und exit um die Verbindung zu trennen */

/***************************************************************************/

/***** Include-Files: *****/
#include <bur/plc.h>
#include <bur/plctypes.h>
#include <Ethernet.h>
#include <string.h>
#include <ctype.h>
#include "AsBrStr.h"
#include "glob_var.h"
#include "in_out.h"
#include "egmglob_var.h"

/***** Variablen-/Konstantendeklaration: *****/
/* Anwendungsspezifische Konstanten: */
#define TCP_IP_PORT		2000				/* Port für Kommunikation */
#define SEND_BUFFER_LEN	150					/* Buffer-Länge für das Senden */
#define RECV_BUFFER_LEN	150					/* Empfangs-Buffer Länge */
#define TIMEOUT			200					/* Abbruch nach TIMEOUT * Zykluszeit Sek.*/
#define	RCVTIMEOUT		200
#define MAXCLIENTS		4
#define	LIVEBYTECNT		160

/* sonstige Konstanten */
#define TCP_IP_OK			0
#define TCP_IP_NO_RCV_DATA	27211
#define TCP_IP_BUSY		65535

#define OPEN_PORT			0
#define WAIT_OPEN_PORT		1
#define SEND_DATA			2
#define WAIT_SEND_DATA		3
#define RECV_DATA			4
#define WAIT_RECV_DATA		5
#define CLOSE_PORT			6
#define WAIT_CLOSE_PORT		7


typedef struct {
	REAL		XPosMm;
	DINT		MotorPos[16];
	REAL		MotorPosMm[16];
	USINT		MotorTemp[16];
	USINT		MotorStatus[16][5];
	USINT		AlarmList[64];
	USINT		Input[16];
	USINT		Output[16];
	USINT		SequenceSteps[16];
	USINT		Trolleys[2];
	USINT		PlateTypes[2];
	USINT		PlateCount[2];
	USINT		MachineStatusBusy;
	USINT		MachineStatusError;
	USINT		MachineStatusStandby;
	USINT		EGMStatus;
	USINT		VCPStatus;
	USINT		TrolleyOpen;
	USINT		PanoramaAdapter;
	USINT		ParameterChanged;
	USINT		PlateAtFeeder;
	USINT		PlateAtShuttle;
	USINT		PlateInDropPosition;
	USINT		PlateAdjusted;
	USINT		PlateAtDeloader;
	USINT		PlateOnConveyorbelt;
	UDINT		PlatesPerHour;
} TSendData;

TSendData	SendData;
_GLOBAL DINT LastStatus;
_GLOBAL	USINT	MotorTemp[MAXMOTORS];
_GLOBAL	STRING	MotorFaultStatus[MAXMOTORS][5];

/* Strukturen fuer Ethernet-Libraray-Funktionen */
_LOCAL	TCPserv_typ			TCP_Server[MAXCLIENTS];
_LOCAL	TCPrecv_typ			TCP_Recv[MAXCLIENTS];
_LOCAL	TCPsend_typ			TCP_Send[MAXCLIENTS];
_LOCAL	TCPclose_typ		TCP_Close[MAXCLIENTS];

/* Variablen: */
char	SendBuffer[MAXCLIENTS][SEND_BUFFER_LEN];
char	RecvBuffer[MAXCLIENTS][RECV_BUFFER_LEN];
char	RecvData[MAXCLIENTS][RECV_BUFFER_LEN];

_LOCAL	SINT	Step[MAXCLIENTS];
_LOCAL	UINT	SendTimeout;
_LOCAL	UINT	RcvLength;
_LOCAL	USINT	LiveByteCnt;
_LOCAL	UINT	ConnectionInvisible;


_LOCAL char	tmp[20];

int CurrentClient,Status,parse;
_LOCAL USINT Count[MAXCLIENTS];

/*HA 23.10.03 V1.67 send laserpower to TiffBlaster if a value is entered (even if it didn't change) */
/*therefore SendLP must be GLOBAL*/
_GLOBAL 	BOOL	SendYOffset,SendLP,SendOSCommand;

_LOCAL	USINT	SendYOffsetStep,SendLPStep,BeamStep,TBShutdownStep,
				SendOSCommandStep;
		UINT	SendYOffsetTimeout,SendLPTimeout,BeamTimeout,OSTimeout;
int i,j,SendCounter,RcvTimeout;
REAL	OldYOffset;
UDINT	OldPower;
_LOCAL int	tmplength;

char pt_tmp[10];
_LOCAL	USINT SendDataCnt[MAXCLIENTS];
BOOL NoAnswer,CloseConnection[MAXCLIENTS];

BOOL SendBData[MAXCLIENTS];
BOOL OldTBVersion;

_GLOBAL USINT *TmpPtr;
_GLOBAL BOOL	PutTmpDataToSRAM;
_LOCAL	BOOL	LoadParamFile,ReadActive;
int LoadParamFileClient;

_GLOBAL	STRING FileIOName[MAXFILENAMELENGTH];
_GLOBAL	STRING	FileType[MAXFILETYPELENGTH];
_GLOBAL	UDINT *FileIOData;
_GLOBAL	UDINT	FileIOLength;
_GLOBAL	BOOL	WriteFileCmd,ReadFileCmd,DeleteFileCmd,WriteFileOK,ReadFileOK,DeleteFileOK,
				FileNotExisting,ReadDirCmd,ReadDirOK,NoDisk;

void StatusToSendBuffer(void)
{
	int i;
/*HA V1.50 send state to TB*/
				strcpy(SendBuffer[0],"ST");
				if (!BUSY && !ERROR && STANDBY)
					strcat(SendBuffer[0],"1");
				else
				if (BUSY && !ERROR && !STANDBY)
					strcat(SendBuffer[0],"2");
				else
				if (!BUSY && ERROR && !STANDBY)
					strcat(SendBuffer[0],"3");
				else
					strcat(SendBuffer[0],"0");

				if ( StatusEGM1 && StatusEGM2 )
					strcat(SendBuffer[0],"0");
				else
					strcat(SendBuffer[0],"1");

				if ( !VCP )
					strcat(SendBuffer[0],"0");
				else
					strcat(SendBuffer[0],"1");

/*erstmal nullen rein*/
					strcat(SendBuffer[0],"0000000000000000000");

/*left trolley*/

				if (GlobalParameter.TrolleyLeft != 0)
				{
					tmplength = brsitoa((UDINT)(GlobalParameter.TrolleyLeft),(UDINT) &tmp[0]);
					SendBuffer[0][5] = '0';
					SendBuffer[0][6] = '0';
					if(tmplength==1)
						SendBuffer[0][6] = tmp[0];
					else
					{
						SendBuffer[0][5] = tmp[0];
						SendBuffer[0][6] = tmp[1];
					}
/*left platetype*/
					tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyLeft].PlateType),(UDINT) &tmp[0]);
					SendBuffer[0][9] = '0';
					SendBuffer[0][10] = '0';
					if(tmplength==1)
						SendBuffer[0][10] = tmp[0];
					else
					{
						SendBuffer[0][9] = tmp[0];
						SendBuffer[0][10] = tmp[1];
					}
/*left platestack*/
					tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft),(UDINT) &tmp[0]);
					SendBuffer[0][13] = '0';
					SendBuffer[0][14] = '0';
					SendBuffer[0][15] = '0';
					if(tmplength <= 3 && tmplength > 0)
					{
						for(i=1;i<=tmplength;i++)
							SendBuffer[0][(15+1)-i] = tmp[tmplength-i];
					}
					else
					{
						SendBuffer[0][13] = '9';
						SendBuffer[0][14] = '9';
						SendBuffer[0][15] = '9';
					}

				}
				else	/*no left trolley*/
				{
					SendBuffer[0][5] = '9';
					SendBuffer[0][6] = '9';
					SendBuffer[0][9] = '9';
					SendBuffer[0][10] = '9';
					SendBuffer[0][13] = '9';
					SendBuffer[0][14] = '9';
					SendBuffer[0][15] = '9';
				}

				if (GlobalParameter.TrolleyRight != 0)
				{
					tmplength = brsitoa((UDINT)(GlobalParameter.TrolleyRight),(UDINT) &tmp[0]);
					SendBuffer[0][7] = '0';
					SendBuffer[0][8] = '0';
					if(tmplength==1)
						SendBuffer[0][8] = tmp[0];
					else
					{
						SendBuffer[0][7] = tmp[0];
						SendBuffer[0][8] = tmp[1];
					}
/*right platetype*/
					tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyRight].PlateType),(UDINT) &tmp[0]);
					SendBuffer[0][11] = '0';
					SendBuffer[0][12] = '0';
					if(tmplength==1)
						SendBuffer[0][12] = tmp[0];
					else
					{
						SendBuffer[0][11] = tmp[0];
						SendBuffer[0][12] = tmp[1];
					}
/*right platestack*/
					tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyRight].PlatesLeft),(UDINT) &tmp[0]);
					SendBuffer[0][16] = '0';
					SendBuffer[0][17] = '0';
					SendBuffer[0][18] = '0';
					if(tmplength <= 3 && tmplength > 0)
					{
						for(i=1;i<=tmplength;i++)
							SendBuffer[0][(18+1)-i] = tmp[tmplength-i];
					}
					else
					{
						SendBuffer[0][16] = '9';
						SendBuffer[0][17] = '9';
						SendBuffer[0][18] = '9';
					}
				}
				else
/* no right trolley: left trolley with 2 stacks?*/
				{
					SendBuffer[0][7] = '9';
					SendBuffer[0][8] = '9';

					if (GlobalParameter.TrolleyLeft != 0)
					{
						if (Trolleys[GlobalParameter.TrolleyLeft].Double && Trolleys[GlobalParameter.TrolleyLeft].RightStack)
						{
							tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyLeft].PlateType),(UDINT) &tmp[0]);
							SendBuffer[0][11] = '0';
							SendBuffer[0][12] = '0';
							if(tmplength==1)
								SendBuffer[0][12] = tmp[0];
							else
							{
								SendBuffer[0][11] = tmp[0];
								SendBuffer[0][12] = tmp[1];
							}
/*right platestack*/
							tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyLeft].PlatesRight),(UDINT) &tmp[0]);
							SendBuffer[0][16] = '0';
							SendBuffer[0][17] = '0';
							SendBuffer[0][18] = '0';
							if(tmplength <= 3 && tmplength > 0)
							{
								for(i=1;i<=tmplength;i++)
									SendBuffer[0][(18+1)-i] = tmp[tmplength-i];
							}
							else
							{
								SendBuffer[0][16] = '9';
								SendBuffer[0][17] = '9';
								SendBuffer[0][18] = '9';
							}
						}
						else  /* no right trolley and no right stack*/
						{
							SendBuffer[0][7] = '9';
							SendBuffer[0][8] = '9';
							SendBuffer[0][11] = '9';
							SendBuffer[0][12] = '9';
							SendBuffer[0][16] = '9';
							SendBuffer[0][17] = '9';
							SendBuffer[0][18] = '9';
						}
					}
				}

				if (/*PanoramaAdapter*/ ManualMode)
				{
					SendBuffer[0][19] = 'P';
/*Plate Type from Pano Input*/
					tmplength = brsitoa((UDINT)(PlateType),(UDINT) &tmp[0]);
					SendBuffer[0][9] = '0';
					SendBuffer[0][10] = '0';
					SendBuffer[0][11] = '0';
					SendBuffer[0][12] = '0';
					if(tmplength==1)
					{
						SendBuffer[0][10] = tmp[0];
						SendBuffer[0][12] = tmp[0];
					}
					else
					{
						SendBuffer[0][9] = tmp[0];
						SendBuffer[0][10] = tmp[1];
						SendBuffer[0][11] = tmp[0];
						SendBuffer[0][12] = tmp[1];
					}
				}
				else
					SendBuffer[0][19] = ' ';

				if(TrolleyOpen)
					SendBuffer[0][20] = 'O';
				else
					SendBuffer[0][20] = 'C';

/*Alarmbitfield übertragen*/
				for(i=0;i<MAXALARMS;i++)
				{
					SendBuffer[0][21+i] = AlarmBitField[i]+48;
				}
/* HA 28.07.05 zusätzliche Info an Netlink: Wieviele Platten im System und welcher Typ*/
				SendBuffer[0][21+MAXALARMS] =  '0';
				SendBuffer[0][21+MAXALARMS+1] =  '0';
				tmplength = brsitoa((UDINT)(UnexposedPlatesInSystem),(UDINT) &tmp[0]);
				if(tmplength==1)
				{
					SendBuffer[0][21+MAXALARMS] =  tmp[0];
					if (/*PanoramaAdapter*/ ManualMode)
						tmplength = brsitoa((UDINT)(PlateType),(UDINT) &tmp[0]);
					else
					/* q&d für Performance */
						tmplength = brsitoa((UDINT)(Trolleys[GlobalParameter.TrolleyLeft].PlateType),(UDINT) &tmp[0]);

					if(tmplength==1)
						SendBuffer[0][21+MAXALARMS+1] =  tmp[0];
				}

				TCP_Send[0].buflng = 21+MAXALARMS;
}

/***** INIT FUNKTION *******************************************************/
_INIT void init (void)
{
	int i;
	CurrentClient = 1;
	/* Initialisierung der TCP-IP Datenstrukturen: */

	for(i = 0;i<MAXCLIENTS;i++)
	{
		SendBData[i] = 0;
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

	OldYOffset =0.0;
	OldPower =0;
	SendCounter=0;
	OldTBVersion = 0;
	LoadParamFile = 0;

} /* Ende Funktion _INIT void init (void) */


/***** ZYKLISCHE FUNKTION **************************************************/
_CYCLIC void cyclic (void)
{
	/***** Öffnen des Ports ************************************************/
	if (Step[0] == OPEN_PORT)
	{
		/* 1. Aufruf der Funktion */
		TCPserv (&TCP_Server[0]);
		Step[0] = WAIT_OPEN_PORT;
	}
	if (Step[0] == WAIT_OPEN_PORT)
	{
		/* Status prüfen: */
		if (TCP_Server[0].status == TCP_IP_OK)
		{
			/* Port geöffnet */
			/* Identifier zuordnen ... */
			TCP_Send[0].cident = TCP_Server[0].cident;
			TCP_Recv[0].cident = TCP_Server[0].cident;
			TCP_Close[0].cident = TCP_Server[0].cident;
			/* ... und zum nächsten Schritt gehen */
			Step[0] = RECV_DATA;
			TCPConnected = 1;
			ConnectionInvisible = 0;
			SendYOffset = 1;
			SendLP = 1;
			AlarmBitField[24] = 0;
		}
		else
		{
			/* Funktion erneut aufrufen */
			TCPserv (&TCP_Server[0]);
			TCPConnected = 0;
			ConnectionInvisible = 1;
		}
	} /* if (Step == WAIT_OPEN_PORT) */



/**************************************************************/
/*  Y-OFFSET */
/**************************************************************/

/*detect changed Offset*/
	if(PlateParameter.YOffset != OldYOffset && TCPConnected )
	{
		OldYOffset = PlateParameter.YOffset;
		SendYOffset = 1;
	}

	switch (SendYOffsetStep)
	{
		case 0:
		{
			if(SendYOffset && TCPConnected )
			{
				if( PlateParameter.YOffset>=0.00 && PlateParameter.YOffset<=999.99)
					SendYOffsetStep = 5;
				else
					SendYOffset = 0;
			}
			break;
		}
		case 5:
		{
			if(TCPSendCmd == 0)
			{
				strcpy(TCPCmd,"YO");
				tmplength = brsitoa((UDINT)(PlateParameter.YOffset *100),(UDINT) &tmp[0]);
/*fill the string with zeros to get fixed 5 digits*/
				for(i=0;i< 5-tmplength;i++)
				{
					TCPCmd[2+i] = '0';
					TCPCmd[2+i+1] = 0;
				}
				tmp[5] = 0; /*safety!*/
				strcat(TCPCmd,tmp);
				TCPSendCmd = 1;
				SendYOffsetStep = 8;
			}
			SendYOffsetTimeout = 0;
			break;
		}
		case 8:
		{
			if( !TCPRcvFlag)
			{
				SendYOffsetTimeout++;
				if(SendYOffsetTimeout>150)
				{
					SendCounter++;
					if(SendCounter < 5)
						SendYOffsetStep = 5;
					else
					{
					/*ERROR*/
						SendYOffsetStep = 0;
						SendYOffset = 0;
						SendCounter = 0;
						AlarmBitField[24] = 1;
					}
				}
				break;
			}

			AlarmBitField[24] = 0;
/*check for YOOK*/
			if( !strcmp(TCPAnswer,"YOOK") )
			{
				TCPRcvFlag = 0;
				SendYOffsetStep = 0;
				SendYOffset = 0;
				SendYOffsetTimeout = 0;
				SendCounter = 0;
			}

			break;
		}
	} /*switch*/


/**************************************************************/
/*  BEAM ON/OFF*/
/**************************************************************/
	switch (BeamStep)
	{
		case 0:
		{
			if( (BeamON || BeamOFF) && TCPConnected )
			{
				if(BeamON && BeamOFF) BeamON = 0;
				BeamStep = 5;
			}
			break;
		}
		case 5:
		{
			if(TCPSendCmd == 0)
			{
				if(BeamON)
					strcpy(TCPCmd,"43=ON");
				if(BeamOFF)
					strcpy(TCPCmd,"43=OFF");
				TCPSendCmd = 1;
				BeamStep = 8;
			}
			BeamTimeout = 0;
			break;
		}
		case 8:
		{
			if( !TCPRcvFlag)
			{
				BeamTimeout++;
				if(BeamTimeout>150)
				{
					SendCounter++;
					if(SendCounter < 5)
						BeamStep = 5;
					else
					{
					/*ERROR*/
						BeamStep = 0;
						BeamON = 0;
						BeamOFF = 0;
						SendCounter = 0;
						AlarmBitField[24] = 1;
					}
				}
				break;
			}

			AlarmBitField[24] = 0;
/*check for answer*/
			if( ( !strcmp(TCPAnswer,"SSH1") && BeamON )
			||  ( !strcmp(TCPAnswer,"SSH0") && BeamOFF ) )
			{
				TCPRcvFlag = 0;
				BeamStep = 0;
				BeamON = 0;
				BeamOFF = 0;
				BeamTimeout = 0;
				SendCounter = 0;
			}

			break;
		}
	} /*switch*/


/**************************************************************/
/*  TIFF Blaster shutdown*/
/**************************************************************/
	switch (TBShutdownStep)
	{
		case 0:
		{
/*safety*/
			if( !TCPConnected )
				TBShutdown = 0;

			if( (TBShutdown) && TCPConnected )
			{
				TBShutdownStep = 5;
			}
			break;
		}
		case 5:
		{
			if(TCPSendCmd == 0)
			{
				strcpy(TCPCmd,"SD");
				TCPSendCmd = 1;
				TBShutdownStep = 0;
				TBShutdown = 0;
			}
			break;
		}
	} /*switch*/



/**************************************************************/
/*  TIFF Blaster: Open Beam shutter */
/* V1.36 */
/**************************************************************/
	switch (SendOSCommandStep)
	{
		case 0:
		{
/*safety*/
			if( !TCPConnected )
				SendOSCommand = 0;

			if( (SendOSCommand) && TCPConnected )
			{
				SendOSCommandStep = 5;
			}
			break;
		}
		case 5:
		{
			if(TCPSendCmd == 0)
			{
				strcpy(TCPCmd,"OS");
				TCPSendCmd = 1;
				OSTimeout = 0;
				SendOSCommandStep = 8;
			}
			break;
		}
		case 8:
		{
			if( !TCPRcvFlag)
			{
				OSTimeout++;
				if(OSTimeout>150)
				{
					SendCounter++;
					if(SendCounter < 5)
						SendOSCommandStep = 5;
					else
					{
					/*ERROR*/
						SendOSCommandStep = 0;
						SendOSCommand = 0;
						SendCounter = 0;
						OSTimeout = 0;
						AlarmBitField[24] = 1;
					}
				}
				break;
			}

			AlarmBitField[24] = 0;
/*check for answer*/
			if( !strcmp(TCPAnswer,"OSOK"))
			{
				TCPRcvFlag = 0;
				SendOSCommandStep = 0;
				SendOSCommand = 0;
				SendCounter = 0;
				OSTimeout = 0;
				SendCounter = 0;
			}

			break;
		}

	} /*switch*/


/**************************************************************/
/*  LASERPOWER*/
/**************************************************************/

/*detect changed Laserpower*/
	if(GlobalParameter.RealLaserPower != OldPower && TCPConnected )
	{
		OldPower = GlobalParameter.RealLaserPower;
		SendLP = 1;
	}

	switch (SendLPStep)
	{
		case 0:
		{
			if(SendLP && TCPConnected )
			{
				if( GlobalParameter.RealLaserPower > 0 && GlobalParameter.RealLaserPower <= 1000)
				{
					SendLPStep = 1;
					AlarmBitField[31] = 0;
					AlarmBitField[32] = 0;
				}
				else
					SendLP = 0;
			}
			break;
		}

/*HA 28.07.04 V1.81 Tiffblaster Version abfragen*/
		case 1:
		{
			if(TCPSendCmd == 0)
			{
				strcpy(TCPCmd,"VER?");
				TCPSendCmd = 1;
				SendLPStep = 2;
			}
			SendLPTimeout = 0;
			break;
		}

		case 2:
		{
			if( !TCPRcvFlag)
			{
				SendLPTimeout++;
				if(SendLPTimeout>150)
				{
					SendCounter++;
					if(SendCounter < 5)
						SendLPStep = 1;
					else
					{
					/*ERROR, no answer: maybe old version, go ahead...*/
						SendLPStep = 5;
						SendLPTimeout = 0;
						SendCounter = 0;
						OldTBVersion = 1;
						SendLP = 0;
					}
				}
				break;
			}

/*Antwort empfangen, Auswertung*/
			AlarmBitField[24] = 0;
			TCPRcvFlag = 0;
			SendLPStep = 5;
			SendLPTimeout = 0;
			SendCounter = 0;

/*check for "Failure"->old version */
			if( !strcmp(TCPAnswer,"Failure") )
				OldTBVersion = 1;
			else
				OldTBVersion = 0;

			break;
		}

		case 5:
		{
			if(TCPSendCmd == 0)
			{
				if (OldTBVersion)
				{
					strcpy(TCPCmd,"LP");
					tmplength = brsitoa((UDINT)(GlobalParameter.LaserPower),(UDINT) &tmp[0]);
/*fill the string with zeros to get fixed 3 digits*/
					for(i=0;i< 3-tmplength;i++)
					{
						TCPCmd[2+i] = '0';
						TCPCmd[2+i+1] = 0;
					}
					tmp[3] = 0; /*safety!*/
					strcat(TCPCmd,tmp);
				}
				else /*neue Version 4 stellig*/
				{
					strcpy(TCPCmd,"LX");
					tmplength = brsitoa((UDINT)(GlobalParameter.RealLaserPower),(UDINT) &tmp[0]);
/*fill the string with zeros to get fixed 4 digits*/
					for(i=0;i< 4-tmplength;i++)
					{
						TCPCmd[2+i] = '0';
						TCPCmd[2+i+1] = 0;
					}
					tmp[4] = 0; /*safety!*/
					strcat(TCPCmd,tmp);
				}
				TCPSendCmd = 1;
				SendLPStep = 8;
			}
			SendLPTimeout = 0;
			break;
		}
		case 8:
		{
			if( !TCPRcvFlag)
			{
				SendLPTimeout++;
				if(SendLPTimeout>150)
				{
					SendCounter++;
					if(SendCounter < 5)
						SendLPStep = 5;
					else
					{
					/*ERROR*/
						SendLPStep = 0;
						SendLP = 0;
						SendCounter = 0;
						AlarmBitField[24] = 1;
					}
				}
				break;
			}

			AlarmBitField[24] = 0;
/*check for LPOK*/
			if( !strcmp(TCPAnswer,"LPOK") )
			{
				TCPRcvFlag = 0;
				SendLPStep = 10;
				SendLPTimeout = 0;
				SendCounter = 0;
			}

			break;
		}

/* HA 22.01.04 V1.74 New step to check for answer (result)*/
		case 10:
		{
/*HA 09.02.04 V1.75 Kompatibilitätsflag prüfen*/
			if ( GlobalParameter.UseOldTiffBlaster )
			{
				SendLPStep = 0;
				SendLP = 0;
				SendCounter = 0;
				break;
			}


			if( !TCPRcvFlag)
			{
				SendLPTimeout++;
				if(SendLPTimeout>12000)
				{
					/*ERROR timeout*/
					SendLPStep = 0;
					SendLP = 0;
					SendCounter = 0;
				}
				break;
			}

			AlarmBitField[24] = 0;
/*check for LPREADY*/
			if( !strcmp(TCPAnswer,"LPREADY") )
			{
				TCPRcvFlag = 0;
				SendLPStep = 0;
				SendLP = 0;
				SendLPTimeout = 0;
				SendCounter = 0;
			}
			else
/*check for LPFAILED*/
			if( !strcmp(TCPAnswer,"LPFAILED") )
			{
				TCPRcvFlag = 0;
				SendLPStep = 0;
				SendLP = 0;
				SendLPTimeout = 0;
				SendCounter = 0;
				AlarmBitField[31] = 1;
/* Laser power setting failed: error message */
				AbfrageText1 = 0;
				AbfrageText2 = 54;
				AbfrageText3 = 0;

				if(wBildAktuell != MESSAGEPIC )
					OrgBild = wBildAktuell;
				wBildNeu = MESSAGEPIC;
				AbfrageOK = 0;
				AbfrageCancel = 0;
				IgnoreButtons = 1;
				OK_CancelButtonInv = 1;
				OK_ButtonInv = 0;
				AbfrageIcon = CAUTION;
			}

			break;
		}	/*case 10*/

	} /*switch*/


	/***** Daten Empfangen *************************************************/
	if (TCPConnected)
	{
		TCPrecv (&TCP_Recv[0]);
		/* Status prüfen: */
		if (TCP_Recv[0].status == TCP_IP_OK)
		{
			if(TCP_Recv[0].rxbuflng == 0) /*CLOSE von Gegenstelle*/
			{
				Step[0] = CLOSE_PORT;
			}
			else	/* Daten empfangen -> auswerten*/
			{
/*check for LPERROR*/
				if( !memcmp(RecvBuffer[0],"LPERROR",7) )
				{
					START = 0;
					AlarmBitField[32] = 1;
	/* Laser power error message */
					AbfrageText1 = 0;
					AbfrageText2 = 55;
					AbfrageText3 = 0;

					if(wBildAktuell != MESSAGEPIC )
						OrgBild = wBildAktuell;
					wBildNeu = MESSAGEPIC;
					AbfrageOK = 0;
					AbfrageCancel = 0;
					IgnoreButtons = 1;
					OK_CancelButtonInv = 1;
					OK_ButtonInv = 0;
					AbfrageIcon = CAUTION;
				}
				else
				{
					TCPRcvFlag = 1;
					strcpy(TCPAnswer,RecvBuffer[0]);
					RcvLength = TCP_Recv[0].rxbuflng;
					TCPAnswer[RcvLength-1] = 0;
				}
			}
		}
/*Wenn keiner das  receive flag löscht, nach Zeit selbst löschen*/
		if(TCPRcvFlag)
		{
			RcvTimeout++;
			if(RcvTimeout > RCVTIMEOUT)
			{
				RcvTimeout=0;
				TCPRcvFlag = 0;
			}
		}
	}


	/***** Daten Senden ****************************************************/
	/***** Lebenszeichen Senden *******************************************/
	if(TCPConnected)
	{
		if(TCPSendCmd != 0)
		{
			if(TCPSendCmd == 2) /*Livebyte*/
			{
				StatusToSendBuffer();
			}
			else
			{
				if ( !strcmp(TCPCmd,"14") && !GlobalParameter.UseOldTiffBlaster ) /*soll 14 geschickt werden?*/
				{
					StatusToSendBuffer();
					SendBuffer[0][0] = '1';
					SendBuffer[0][1] = '4';
				}
				else
				if ( !strcmp(TCPCmd,"15"))  /*soll 15 geschickt werden?*/
				{
					StatusToSendBuffer();
					SendBuffer[0][0] = '1';
					SendBuffer[0][1] = '5';
				}
				else
				{
					memcpy(SendBuffer[0],TCPCmd,strlen(TCPCmd));
					TCP_Send[0].buflng = strlen(TCPCmd);
				}
			}

			TCPsend (&TCP_Send[0]);
			/* Status prüfen: */
			if (TCP_Send[0].status == TCP_IP_OK)
			{
			/* Senden war OK */
				TCPSendCmd = 0;
				SendTimeout = 0;
			}
			SendTimeout++;
			LiveByteCnt = 0;
			if(SendTimeout > TIMEOUT)
			{
				Step[0] = CLOSE_PORT;
				TCPSendCmd = 0;
				SendTimeout = 0;
				TCPConnected = 0;
				ConnectionInvisible = 1;
			}
		}
/*no sending? send livebyte */
		else
		{
			LiveByteCnt++;
			if(LiveByteCnt > LIVEBYTECNT)
			{
				if(TCPSendCmd == 0)
				{
					TCPSendCmd = 2;
					LiveByteCnt = 0;
				}
			}
		}
	}
	else /*no connection: reset send Cmd*/
		TCPSendCmd = 0;


	/***** Port schließen **************************************************/
	if (Step[0] == CLOSE_PORT)
	{
		TCPConnected = 0;
		ConnectionInvisible = 1;
		TCPclose (&TCP_Close[0]);
		Step[0] = WAIT_CLOSE_PORT;
	}
	if (Step[0] == WAIT_CLOSE_PORT)
	{
		/* Status prüfen: */
		if (TCP_Close[0].status == TCP_IP_OK)
		{
			/* Port geschlossen, neu öffnen: */
			Step[0] = OPEN_PORT;
		}
		else
		{
			/* Funktion erneut aufrufen */
			TCPclose (&TCP_Close[0]);
		}
	} /* if (Step == WAIT_CLOSE_PORT) */


/***************************************************************************/
/***************************************************************************/
/*		weitere Clients bedienen                                           */
/*                                                                         */


			SendData.XPosMm = FU.aktuellePosition_mm;
			for(i=0;i<16;i++)
			{
				SendData.MotorPos[i] = Motors[i].Position;
				SendData.MotorPosMm[i] = Motors[i].Position_mm;
				SendData.MotorTemp[i] = MotorTemp[i];
				for(j=0;j<4;j++)
					SendData.MotorStatus[i][j] = MotorFaultStatus[i][j];
				SendData.MotorStatus[i][4] = 0;
			}

			for(i=0;i<MAXALARMS;i++)
				SendData.AlarmList[i] = AlarmBitField[i];

			SendData.Input[0] = BeltLoose;
			SendData.Input[1] = 0;
			SendData.Input[2] = TrolleyCodeSensor;
			SendData.Input[3] = In_ConveyorBeltSensor[0];
			SendData.Input[4] = 0;
			SendData.Input[5] = 0;
			SendData.Input[6] = 0;
			SendData.Input[7] = 0;
			SendData.Input[8] = InEGM1;
			SendData.Input[9] = InEGM2;
			SendData.Input[10] = VCP;
			SendData.Input[11] = CoverLockOK;
			SendData.Input[12] = DoorsOK;
			SendData.Input[13] = 0;
			SendData.Input[14] = 0;
			SendData.Input[15] = 0;
/*
			SendData.Output[0] = out0;
			SendData.Output[1] = out1;
			SendData.Output[2] = out2;
			SendData.Output[3] = out3;
			SendData.Output[4] = out4;
			SendData.Output[5] = out5;
			SendData.Output[6] = out6;
			SendData.Output[7] = out7;
			SendData.Output[8] = out8;
			SendData.Output[9] = out9;
			SendData.Output[10] = out10;
			SendData.Output[11] = out11;
			SendData.Output[12] = out12;
			SendData.Output[13] = out13;
			SendData.Output[14] = out14;
			SendData.Output[15] = out15;
*/
			for(i=0;i<10;i++)
				SendData.SequenceSteps[i] = SequenceSteps[i];
/*HA 11.05.04 V1.77*/
			SendData.SequenceSteps[15] = CurrentStack;
/*
	SeqenceSteps[0]	PaperRemove
	SeqenceSteps[1]	Feeder
	SeqenceSteps[2]	Shuttle
	SeqenceSteps[3]	Adjust
	SeqenceSteps[4]	Expose
	SeqenceSteps[5]	Deload
	SeqenceSteps[6]	TrolleyOpen
	SeqenceSteps[7]	TrolleyClose
	SeqenceSteps[8]	Reference
*/

/*Status data*/
			SendData.MachineStatusBusy = BUSY;
			SendData.MachineStatusError = ERROR;
			SendData.MachineStatusStandby = STANDBY;
			SendData.EGMStatus = !(StatusEGM1 && StatusEGM2);
			SendData.VCPStatus = !VCP;
			SendData.Trolleys[0] = GlobalParameter.TrolleyLeft;
			SendData.Trolleys[1] = GlobalParameter.TrolleyRight;

			if (GlobalParameter.TrolleyLeft != 0)
			{
				SendData.PlateTypes[0] = Trolleys[GlobalParameter.TrolleyLeft].PlateType;
				SendData.PlateCount[0] = Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft;
			}
			else
			{
				SendData.PlateTypes[0] = 0;
				SendData.PlateCount[0] = 0;
			}

			if (GlobalParameter.TrolleyRight != 0)
			{
				SendData.PlateTypes[1] = Trolleys[GlobalParameter.TrolleyRight].PlateType;
				SendData.PlateCount[1] = Trolleys[GlobalParameter.TrolleyRight].PlatesLeft;
			}
			else
			{
/*2 stacks in double size trolley ?*/
				if (GlobalParameter.TrolleyLeft != 0)
				{
					if (Trolleys[GlobalParameter.TrolleyLeft].Double && Trolleys[GlobalParameter.TrolleyLeft].RightStack)
					{
						SendData.PlateTypes[1] = Trolleys[GlobalParameter.TrolleyLeft].PlateType;
						SendData.PlateCount[1] = Trolleys[GlobalParameter.TrolleyLeft].PlatesRight;
					}
					else
					{
						SendData.PlateTypes[1] = 0;
						SendData.PlateCount[1] = 0;
					}
				}
			}

			if (PanoramaAdapter)
			{
				SendData.PanoramaAdapter = 1;
				SendData.PlateTypes[0] = SendData.PlateTypes[1] = PlateType;
			}
			else
				SendData.PanoramaAdapter = 0;

			if(TrolleyOpen)
				SendData.TrolleyOpen = 1;
			else
				SendData.TrolleyOpen = 0;

			if (ParameterChanged)
				SendData.ParameterChanged = 1;
			ParameterChanged = 0;

			SendData.PlateAtFeeder = PlateAtFeeder.present;
			SendData.PlateAtShuttle = 0;
			SendData.PlateInDropPosition = 0;
			SendData.PlateAdjusted = AdjustReady;
			SendData.PlateAtDeloader = PlateAtDeloader.present;
			SendData.PlateOnConveyorbelt = PlateOnConveyorBelt.present;
			SendData.PlatesPerHour = PlatesPerHour;



	for(CurrentClient = 1;CurrentClient<MAXCLIENTS;CurrentClient++)
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
			}
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
						while ( RecvData[CurrentClient][i] != 0 )
						{
							RecvData[CurrentClient][i] = toupper( RecvData[CurrentClient][i] );
							i++;
						}
						RecvData[CurrentClient][Count[CurrentClient]-1] = 0;
						if( strncmp(RecvData[CurrentClient],"STATUS",6) == 0 )
						{
							if(BUSY)
								strcpy(SendBuffer[CurrentClient],"ST=BUSY\r\n");
							else if(ERROR)
									strcpy(SendBuffer[CurrentClient],"ST=ERROR\r\n");
								else if(STANDBY)
										strcpy(SendBuffer[CurrentClient],"ST=READY\r\n");
						}
						else
						if( strncmp(RecvData[CurrentClient],"SPEED",5) == 0 )
						{
							strcpy(SendBuffer[CurrentClient],"SPEED=");
							brsitoa(PlatesPerHour,(UDINT) &tmp[0]);
							strcat(SendBuffer[CurrentClient],tmp);
							strcat(SendBuffer[CurrentClient],"\r\n");
						}
						else
						if( strncmp(RecvData[CurrentClient],"TROLLEY",7) == 0 )
						{
							if(GlobalParameter.TrolleyLeft != 0)
							{
								strcpy(SendBuffer[CurrentClient],"LEFT=");
								brsitoa(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft,(UDINT) &tmp[0]);
								strcat(SendBuffer[CurrentClient],tmp);
								if(Trolleys[GlobalParameter.TrolleyLeft].Double)
								{
									strcat(SendBuffer[CurrentClient]," RIGHT=");
									brsitoa(Trolleys[GlobalParameter.TrolleyLeft].PlatesRight,(UDINT) &tmp[0]);
									strcat(SendBuffer[CurrentClient],tmp);
								}
							}
							else
							{
								strcpy(SendBuffer[CurrentClient],"");
							}
							if(GlobalParameter.TrolleyRight != 0)
							{
								if(GlobalParameter.TrolleyLeft != 0)
									strcat(SendBuffer[CurrentClient]," RIGHT=");
								else
									strcat(SendBuffer[CurrentClient],"RIGHT=");
								brsitoa(Trolleys[GlobalParameter.TrolleyRight].PlatesLeft,(UDINT) &tmp[0]);
								strcat(SendBuffer[CurrentClient],tmp);
							}
							if(GlobalParameter.TrolleyLeft == 0 && GlobalParameter.TrolleyRight == 0)
								strcpy(SendBuffer[CurrentClient],"NO TROLLEY");
							strcat(SendBuffer[CurrentClient],"\r\n");
						}
						else
/*noch nicht wirklich gut, sobald ein client dieses command schickt, wird das flag für alle */
/* global zurückgesetzt, besser: binäre sendedaten auch als array*/
						if (strncmp(RecvData[CurrentClient],"RESETPARAMCHANGED",17) == 0)
						{
							SendData.ParameterChanged = 0;
							NoAnswer = 1;
						}
						else
/* Load a specific parameter file */
/* syntax: LOADPARAMFILE BLAH\r\n   ohne Endung! */
						if (strncmp(RecvData[CurrentClient],"LOADPARAMFILE",13) == 0)
						{
							LoadParamFile = 1;
							LoadParamFileClient = CurrentClient;
							strcpy(FileIOName,&RecvData[CurrentClient][14]);
							FileIOName[strlen(FileIOName)-1] = 0; /*CRLF entfernen*/
							strcpy(SendBuffer[CurrentClient],"LOADPARAMFILE OK\r\n");
						}
						else
						if (strncmp(RecvData[CurrentClient],"BYE",3) == 0
						||	strncmp(RecvData[CurrentClient],"QUIT",4) == 0
						||	strncmp(RecvData[CurrentClient],"EXIT",4) == 0)
						{
							strcpy(SendBuffer[CurrentClient],"Good bye, cu next time...\r\n");
							CloseConnection[CurrentClient] = 1;
						}
						else
						{
							memcpy(tmp,RecvData[CurrentClient],10);
							tmp[10] = 0;
							if( strcmp(tmp,"LASERPOWER") == 0 )
							{
								/*new power to set?*/
								if(Count[CurrentClient] > 11 && RecvData[CurrentClient][10]=='=')
								{
									i=11;
									while (isdigit(RecvData[CurrentClient][i]) && i<Count[CurrentClient]) i++;
									if (i>11)
									{
										memcpy(tmp,&RecvData[CurrentClient][11],i-11);
										GlobalParameter.RealLaserPower = brsatoi((UDINT) &tmp);
									}
								}
								strcpy(SendBuffer[CurrentClient],"LASERPOWER=");
								brsitoa(GlobalParameter.RealLaserPower,(UDINT) &tmp[0]);
								strcat(SendBuffer[CurrentClient],tmp);
								strcat(SendBuffer[CurrentClient],"\r\n");
							}
							else
								strcpy(SendBuffer[CurrentClient],"unknown command\r\n");
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
/*nothing received: ok, send binary data*/
				if (TCP_Recv[CurrentClient].status == TCP_IP_NO_RCV_DATA)
				{
					SendDataCnt[CurrentClient]++;
					if( SendDataCnt[CurrentClient] >= 10)
					{
						SendDataCnt[CurrentClient] = 0;
						SendBData[CurrentClient] = 1;
					}
				}
			}
		}


/************** senden ****************************/

		if ( SendBData[CurrentClient] )
		{
			SendBData[CurrentClient] = 0;

			TCP_Send[CurrentClient].buffer		= (UDINT)&SendData;
			TCP_Send[CurrentClient].buflng		= sizeof (SendData);
			Step[CurrentClient] = SEND_DATA;
		}

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
			}
			else
			if ( (TCP_Send[CurrentClient].status != TCP_IP_BUSY)  &&
				(TCP_Send[CurrentClient].status != ERR_ETH_NET_SLOW)
					)
			{
				LastStatus = TCP_Send[CurrentClient].status;
				if (TCP_Send[CurrentClient].status==14896)
					Step[CurrentClient] = RECV_DATA;
				else
					Step[CurrentClient] = CLOSE_PORT;
			}
		}



	/***** Port schließen **************************************************/
		if (Step[CurrentClient] == CLOSE_PORT)
		{
			TCPclose (&TCP_Close[CurrentClient]);
			/* Status prüfen: */
			if (TCP_Close[CurrentClient].status == TCP_IP_OK)
			{
				/* Port geschlossen, neu öffnen: */
				Step[CurrentClient] = OPEN_PORT;
			}
		} /* if (Step == WAIT_CLOSE_PORT) */


	} /*for*/

	if (LoadParamFile)
	{
		/*try to load sram from file*/
		FileIOData = (UDINT *) TmpPtr;
#ifdef MODE_BOTH
		FileIOLength = OVERALLSRAMSIZEEBYTES;
		strcpy(FileType,"SA1");
#else
	#ifdef MODE_PERFORMANCE
		FileIOLength = PERFSRAMSIZEBYTES;
		strcpy(FileType,"SA1");
	#else
		#ifdef MODE_BLUEFIN
		FileIOLength = OVERALLSRAMSIZEEBYTES;
		strcpy(FileType,"SA1");
		#endif
	#endif
#endif

		if(!ReadActive)
		{
			ReadActive = 1;
			ReadFileCmd = 1;
			ReadFileOK = 0;
			FileNotExisting = 0;
		}
		else
		{
/*wait for reading ready*/
			if(ReadFileOK)
			{
				LoadParamFile = 0;
				ReadActive = 0;
				ReadFileOK = 0;
				/*save to SRAM*/
				PutTmpDataToSRAM = 1;
				strcpy(SendBuffer[LoadParamFileClient],"PARAM FILE LOADED SUCCESSFULLY!");
				Step[LoadParamFileClient] = SEND_DATA;
				TCP_Send[LoadParamFileClient].buffer		= (UDINT)&SendBuffer[LoadParamFileClient][0];
				TCP_Send[LoadParamFileClient].buflng = strlen(SendBuffer[LoadParamFileClient]);

			}
/*file not existing*/
			if(FileNotExisting)
			{
				LoadParamFile = 0;
				ReadActive = 0;
				ReadFileOK = 0;
				FileNotExisting = 0;
				strcpy(SendBuffer[LoadParamFileClient],"PARAM FILE NOT FOUND!\r\n");
				Step[LoadParamFileClient] = SEND_DATA;
				TCP_Send[LoadParamFileClient].buffer		= (UDINT)&SendBuffer[LoadParamFileClient][0];
				TCP_Send[LoadParamFileClient].buflng = strlen(SendBuffer[LoadParamFileClient]);
			}
		}
	} /*if (loadparamfile)*/

} /* Ende Funktion _CYCLIC void cyclic (void) */


/* ENDE */


