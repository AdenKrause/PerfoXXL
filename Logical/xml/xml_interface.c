#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/***************************************************************************/
/*                                                                         */
/* Datei:    xml_interface.c                                               */
/*                                                                         */
/* Autor:    Herbert Aden, Krause-Biagosch GmbH                            */
/*                                                                         */
/* Abstrakt: Diese Task wartet darauf, daß sich ein TCP-IP Client am ein-  */
/*           gestellten Port anmeldet.   Wenn die Verbindung hergestellt   */
/*           worden ist, schickt der server zyklisch XML codierte          */
/*           wird versucht, die Verbindung wieder neu aufzubauen.          */
/*                                                                         */
/* Datum:    08.12.2006       letzte Änderungen: 25.01.2007  / HA          */
/*                                                                         */
/***************************************************************************/

/***** Include-Files: *****/
#include <Ethernet.h>
#include <string.h>
#include <ctype.h>
#include <visapi.h>
#include "asstring.h"
#include "glob_var.h"
#include "EGMglob_var.h"

/***** Variablen-/Konstantendeklaration: *****/
/* Anwendungsspezifische Konstanten: */
#define TCP_IP_PORT		2020				/* Port für Kommunikation */
#define SEND_BUFFER_LEN	3000				/* Buffer-Länge für das Senden */
#define RECV_BUFFER_LEN	150					/* Empfangs-Buffer Länge */
#define TIMEOUT			200					/* Abbruch nach TIMEOUT * Zykluszeit Sek.*/
#define	RCVTIMEOUT		200
#define MAXCLIENTS		5
#define	LIVEBYTECNT		160
#define SEND_EVERY_N_SEC 5					/* send every 5 seconds an xml message*/

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

#define MAXERRORS			51

/* Strukturen fuer Ethernet-Libraray-Funktionen */
_LOCAL    TCPserv_typ         TCP_Server[MAXCLIENTS];
_LOCAL    TCPrecv_typ         TCP_Recv[MAXCLIENTS];
_LOCAL    TCPsend_typ         TCP_Send[MAXCLIENTS];
_LOCAL    TCPclose_typ        TCP_Close[MAXCLIENTS];

_LOCAL    SINT                Step[MAXCLIENTS];
_LOCAL    USINT               Count[MAXCLIENTS];
_LOCAL    USINT               SendDataCnt[MAXCLIENTS];
/* Buffer for XML Message */
_LOCAL    SINT                XMLBuffer[SEND_BUFFER_LEN];
_LOCAL    UINT                SendTimeout[MAXCLIENTS];
_LOCAL    UINT                DataLock;

/* Variablen: */
static    SINT                SendBuffer[MAXCLIENTS][SEND_BUFFER_LEN];
static    SINT                RecvBuffer[MAXCLIENTS][RECV_BUFFER_LEN];
static    SINT                RecvData[MAXCLIENTS][RECV_BUFFER_LEN];
static    BOOL                CloseConnection[MAXCLIENTS];
static    int                 CurrentClient,parse;
static    int                 i;

static const char *ErrorTexts[] =
{
	"Not-Aus",
	"kein Antriebstakt",
	"Gummierung Füllstand zu niedrig",
	"Vorspülen Füllstand zu niedrig",
	"Nachspülen Füllstand zu niedrig",
	"Entwickler Füllstand zu niedrig",
	"Entwickler Temperatur zu niedrig",
	"Entwickler Temperatur zu hoch",
	"Nacherwärmung 1 Temperatur zu niedrig",
	"Nacherwärmung 1 Temperatur zu hoch",
	"Nacherwärmung 2 Temperatur zu niedrig",
	"Nacherwärmung 2 Temperatur zu hoch",
	"Plattenstau",
	"Entwickler zu alt",
	"E/A Modul nicht gefunden",
	"Nacherwärmung Temperatur-Sensor 1 defekt",
	"Nacherwärmung Temperatur-Sensor 2 defekt",
	"Nacherwärmung Umgebungs-Sensor defekt",
	"Entwickler Temperatur-Sensor defekt",
	"Nacherwärmung Heizung 1 defekt",
	"Nacherwärmung Heizung 2 defekt",
	"Vorspülstation: Befüllen fehlgeschlagen",
	"Nachspülstation: Befüllen fehlgeschlagen",
	"Entwickler Befüllen fehlgeschlagen",
	"Nacherwärmung 1 überhitzt",
	"Nacherwärmung 2 überhitzt",
	"Entwickler überhitzt",
	"Antrieb Geschwindigkeitsabweichung zu groß",
	"Temperatur Modul nicht gefunden",
	"Digitales Eingangsmodul nicht gefunden",
	"Digitales Ausgangsmodul nicht gefunden",
	"Analoges Ausgangsmodul nicht gefunden",
	"Versorgungsmodul nicht gefunden",
	"Gummierung: Kein Durchfluß",
	"Entwicklerzirkulation: Kein Durchfluß",
	"Vorspülen: Kein Durchfluß",
	"Nachspülen: Kein Durchfluß",
	"Regenerat Kanister leer",
	"Entwickler Kanister leer",
	"Nacherw.: Heizung 3 defekt",
	"Nacherw.: Heizung 4 defekt",
	"Nacherwärmung Kreis 3 Temp. zu niedrig",
	"Nacherwärmung Kreis 3 Temp. zu hoch",
	"Nacherwärmung Kreis 4 Temp. zu niedrig",
	"Nacherwärmung Kreis 4 Temp. zu hoch",
	"Nacherwärmung Kreis 3 überhitzt",
	"Nacherwärmung Kreis 4 überhitzt",
	"Nacherw.: Temperaturfühler 3 defekt",
	"Nacherw.: Temperaturfühler 4 defekt",
	"Abwassertank fast voll!",
	"Abwassertank voll, Maschine gestoppt!",
	"EOL",	/* End of Errorlist (for detection in loop) */
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" "
};

/***** INIT FUNKTION *******************************************************/
_INIT void init (void)
{
	int i;
	CurrentClient = 1;
	/* Initialisierung der TCP-IP Datenstrukturen: */

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

} /* Ende Funktion _INIT void init (void) */



void CreateXMLMessage( USINT *buf)
{
	char tmp[20];
	char TempError[64];
	int i,ErrorNumber;


	void AddVal2(USINT *buf, char *before,char *Value, char *after)
	{
		strcat(buf,before);
		strcat(buf,Value);
		strcat(buf,after);
	}

	strcpy(buf,"<KrauseMessage>\r\n\t<Bluefin>\r\n\t\t<MachineName>Bluefin</MachineName>\r\n");

			strcat(buf,"\t\t<MachineNumber>");
			strncat(buf,EGMGlobalParam.MachineNumber,9);
			strcat(buf,"</MachineNumber>\r\n");

			strcat(buf,"\t\t<MachineState>\r\n\t\t\t<State>");
/* Status Message*/
				strcpy(TempError,"No error");
				ErrorNumber = 0;
				switch (EGMState.mode)
				{
					case (M_OFF):
					case (M_CLEANING):
					case (M_INITCLEANING):
					{
					    if( !(ControlVoltageOk &&  ActivateControlVoltage) )
							strcat(buf,"EMERGENCY</State>\r\n\t\t\t<ErrorNumber>");
						else
							strcat(buf,"STANDBY</State>\r\n\t\t\t<ErrorNumber>");
						break;
					}
					case (M_AUTO):
					{
						switch (EGMState.state)
						{
							case (S_READY):
							{
								strcat(buf,"READY</State>\r\n\t\t\t<ErrorNumber>");
								break;
							}
							case (S_OFF):
							case (S_HEATING):
							{
								strcat(buf,"STANDBY</State>\r\n\t\t\t<ErrorNumber>");
								break;
							}
							case (S_STARTPROCESSING):
							case (S_PROCESSING):
							case (S_WAITFORSTANDBY):
							{
								strcat(buf,"BUSY</State>\r\n\t\t\t<ErrorNumber>");
								break;
							}
							case (S_ERROR):
							{
								BOOL Err = FALSE;
								strcat(buf,"ERROR</State>\r\n\t\t\t<ErrorNumber>");
/* find 1st error in list */
								for(i=0;i<MAXERRORS;i++)
								{
									if (EGM_AlarmBitField[i] == TRUE)
										break;
/* safety: detect End of Errorlist */
									if( strcmp(ErrorTexts[i],"EOL") == 0 )
									{
										Err = TRUE;
										break;
									}

								}
/* safety: End of Errorlist was detected */
								if(!Err && i<MAXERRORS)
								{
									strncpy(TempError,ErrorTexts[i],sizeof(TempError)-1);
									ErrorNumber = i;
								}
								else
								{
									ErrorNumber = 0;
								}
								break;
							}
						}
						break;
					}

				}
				itoa(ErrorNumber,(UDINT )&tmp[0]);
				strcat(buf,tmp);
				strcat(buf,"</ErrorNumber>\r\n\t\t\t<ErrorDescription>");
				strcat(buf,TempError);
				strcat(buf,"</ErrorDescription>\r\n\t\t\t<Errorlist>");
				for(i=0;i<EGM_MAXALARMS;i++)
				{
					if(EGM_AlarmBitField[i])
						strcat(buf,"1");
					else
						strcat(buf,"0");
				}

				strcat(buf,"</Errorlist>\r\n\t\t\t<StateCode>");
/* Emergency stop soll auch zu Error führen:*/
			    if( !(ControlVoltageOk &&  ActivateControlVoltage) )
					itoa(S_ERROR,(UDINT )&tmp[0]);
			    else
					itoa(EGMState.state,(UDINT )&tmp[0]);
				strcat(buf,tmp);
				strcat(buf,"</StateCode>\r\n\t\t</MachineState>\r\n\t\t<MachineParam>\r\n\t\t\t<PreHeat>\r\n");
/* Preheat values*/
					ftoa((REAL)EGMPreheat.RealTemp[0]/10.0,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<Temperature1>\r\n\t\t\t\t\t<Description>Heating 1</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>deg. C</Unit>\r\n\t\t\t\t</Temperature1>\r\n");
					ftoa((REAL)EGMPreheat.RealTemp[1]/10.0,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<Temperature2>\r\n\t\t\t\t\t<Description>Heating 2</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>deg. C</Unit>\r\n\t\t\t\t</Temperature2>\r\n");
					ftoa((REAL)EGMPreheat.RealTemp[2]/10.0,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<Temperature3>\r\n\t\t\t\t\t<Description>Ambient</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>deg. C</Unit>\r\n\t\t\t\t</Temperature3>\r\n\t\t\t</PreHeat>\r\n\t\t\t<Developer>\r\n");
/* Developer values */
					ftoa((REAL)EGMDeveloperTank.RealTemp/10.0,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<Temperature1>\r\n\t\t\t\t\t<Description>Temperature of fluid</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>deg. C</Unit>\r\n\t\t\t\t</Temperature1>\r\n");
					if(EGMDeveloperTank.LevelInRange)
						strcat(buf,"\t\t\t\t<LevelOK>\r\n\t\t\t\t\t<Description>Level of developer</Description>\r\n\t\t\t\t\t<Type>boolean</Type>\r\n\t\t\t\t\t<Value>true</Value>\r\n\t\t\t\t</LevelOK>\r\n\t\t\t</Developer>\r\n\t\t\t<Speeds>\r\n");
					else
						strcat(buf,"\t\t\t\t<LevelOK>\r\n\t\t\t\t\t<Description>Level of developer</Description>\r\n\t\t\t\t\t<Type>boolean</Type>\r\n\t\t\t\t\t<Value>false</Value>\r\n\t\t\t\t</LevelOK>\r\n\t\t\t</Developer>\r\n\t\t\t<Speeds>\r\n");
					ftoa(EGMGlobalParam.CurrentPlateSpeed,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<PlateSpeedAct>\r\n\t\t\t\t\t<Description>Actual speed (measured)</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>m per min</Unit>\r\n\t\t\t\t</PlateSpeedAct>\r\n");
					ftoa((EGMGlobalParam.MainMotorFactor*EGMMainMotor.RatedRpm),(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<PlateSpeedRef>\r\n\t\t\t\t\t<Description>Referenced speed</Description>\r\n\t\t\t\t\t<Type>double</Type>\r\n\t\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t\t<Unit>m per min</Unit>\r\n\t\t\t\t</PlateSpeedRef>\r\n");
					itoa(EGMBrushMotor.RatedRpm*EGMGlobalParam.BrushFactor,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t\t<BrushSpeed>\r\n\t\t\t\t\t<Description>Speed of brushes</Description>\r\n\t\t\t\t\t<Type>int</Type>\r\n\t\t\t\t\t<Value>", tmp,
/*					"</Value>\r\n\t\t\t\t\t<Unit>rpm</Unit>\r\n\t\t\t\t</BrushSpeed>\r\n\t\t\t</Speeds>\r\n\t\t</MachineParam>\r\n\t</Bluefin>\r\n</KrauseMessage>\r\n");*/
/* *** */
					"</Value>\r\n\t\t\t\t\t<Unit>rpm</Unit>\r\n\t\t\t\t</BrushSpeed>\r\n\t\t\t</Speeds>\r\n\t\t</MachineParam>\r\n");
/* Verbrauchsdaten */
				strcat(buf,"\t\t<PlateCountValues>\r\n");
					itoa(OverallPlateCounter,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t<OverallPlates>\r\n\t\t\t\t<Description>Plates processed (overall)</Description>\r\n\t\t\t\t<Type>int</Type>\r\n\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t<Unit>pcs.</Unit>\r\n\t\t\t</OverallPlates>\r\n");
					itoa(SessionPlateCounter,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t<SessionPlates>\r\n\t\t\t\t<Description>Plates processed since power on</Description>\r\n\t\t\t\t<Type>int</Type>\r\n\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t<Unit>pcs.</Unit>\r\n\t\t\t</SessionPlates>\r\n");
					ftoa(OverallSqmCounter,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t<OverallSqm>\r\n\t\t\t\t<Description>Squaremeters processed (overall)</Description>\r\n\t\t\t\t<Type>double</Type>\r\n\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t<Unit>sqm</Unit>\r\n\t\t\t</OverallSqm>\r\n");
					ftoa(SessionSqmCounter,(UDINT )&tmp[0]);
					AddVal2(buf, "\t\t\t<SessionSqm>\r\n\t\t\t\t<Description>Squaremeters processed since power on</Description>\r\n\t\t\t\t<Type>double</Type>\r\n\t\t\t\t<Value>", tmp,
					"</Value>\r\n\t\t\t\t<Unit>sqm</Unit>\r\n\t\t\t</SessionSqm>\r\n\t\t</PlateCountValues>\r\n");
/* Version */
				strcat(buf,"\t\t<Version>");
				strcat(buf,gVERSION);
				strcat(buf,"</Version>\r\n\t</Bluefin>\r\n</KrauseMessage>\r\n");



/* safety */
	if(strlen(buf) > SEND_BUFFER_LEN)
		buf[SEND_BUFFER_LEN-1] = 0;
	return;
}



/***** ZYKLISCHE FUNKTION **************************************************/
_CYCLIC void cyclic (void)
{

	if ( DataLock == 0 )
		CreateXMLMessage( (USINT *)&XMLBuffer[0] );



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
				if( SendDataCnt[CurrentClient] >= 10*SEND_EVERY_N_SEC)
				{
					memcpy(SendBuffer[CurrentClient],XMLBuffer,sizeof(XMLBuffer));
					TCP_Send[CurrentClient].buflng = strlen(SendBuffer[CurrentClient]);
					Step[CurrentClient] = SEND_DATA;
					SendDataCnt[CurrentClient] = 0;
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


	} /*for*/


} /* Ende Funktion _CYCLIC void cyclic (void) */


/* ENDE */


