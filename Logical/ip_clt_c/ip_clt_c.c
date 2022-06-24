#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	TCP/IP Client Funktionalität 											*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			25.02.03	erste Implementation der				HA		*/
/*							Ping Funktion									*/
/*	1.10		26.02.03	SMTP Client								HA		*/
/*																			*/
/****************************************************************************/

/***** Include-Files: *****/
#include <bur/plc.h>
#include <bur/plctypes.h>
#include <string.h>
#include <Ethernet.h>
#include <EthSock.h>
#include "glob_var.h"
#include "asstring.h"

/***** Variablen-/Konstantendeklaration: *****/
/* Anwendungsspezifische Konstanten: */
#define SEND_BUFFER_LEN	128					/* Buffer-Länge für das Senden */
#define RECV_BUFFER_LEN	128					/* Empfangs-Buffer Länge */
#define SMTP_TIMEOUT	50

/* sonstige Konstanten */
#define TCP_IP_OK			0
#define TCP_IP_BUSY			65535

/*eigene Fehlermeldungen*/
#define	SERVER_CLOSED_CONN	1000
#define NO_CONNECTION		1001
#define SEND_TIMEOUT		1002

/*B+R Fehlermeldungen ethernet*/
#define ERR_ETH_INVAL 		27122
#define ERR_ETH_TIMEOUT		27160
#define ERR_ETH_CONNREFUSED	27161
#define ERR_ETH_NET_SLOW 	27210

/*B+R Fehlermeldungen ping*/
#define	PING_NO_RESPONSE	27220
#define	PING_TIMEOUT		27221


typedef struct
{
	STRING 	Server[16];
	STRING	Sender[60];
	STRING	Recipient[60];
	STRING	Subject[60];
	STRING	DataLine1[60];
	STRING	DataLine2[60];
	STRING	DataLine3[60];
	STRING	DataLine4[60];
	STRING	DataLine5[60];
	USINT	Step;
	USINT	CommStep;
	USINT	StartSending;
	UINT	TimeoutCounter;
	UINT	ErrorNr;
}SMTP_typ;

_LOCAL	SMTP_typ SMTP;

/* Strukturen fuer Ethernet-Libraray-Funktionen */
_LOCAL	TCPclient_typ		SMTP_Client;
_LOCAL	TCPrecv_typ			SMTP_Recv;
_LOCAL	TCPsend_typ			SMTP_Send;
_LOCAL	TCPclose_typ		SMTP_Close;

/* Variablen: */
_LOCAL	STRING	SendBuffer[SEND_BUFFER_LEN];
_LOCAL	STRING	RecvBuffer[RECV_BUFFER_LEN];


USINT PingOK,PingError;
_LOCAL	ICMPping_typ PingVar;
_LOCAL	UINT	PingOKInvisible,PingErrorInvisible;
_LOCAL USINT StartPing,IPAdr1,IPAdr2,IPAdr3,IPAdr4;
_LOCAL	UINT	MailAnimCounter;
UINT	AnimTimer,TryCounter;

STRING IPAdress[16],tmp[10];
TOF_10ms_typ PingLEDOff;
BOOL	Reconnect;

void Ping(void)
{
	PingOKInvisible = !PingOK;
	PingErrorInvisible = !PingError;
	PingLEDOff.IN = StartPing;
	PingLEDOff.PT = 300;
	TOF_10ms(&PingLEDOff);
	if(!PingLEDOff.Q)
	{
		PingOK = 0;
		PingError = 0;
	}
	if(StartPing)
	{
		PingVar.enable = TRUE;			/* FUB enable */
		strcpy(IPAdress,"");
		itoa(IPAdr1,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		itoa(IPAdr2,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		itoa(IPAdr3,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		itoa(IPAdr4,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		PingVar.ipaddr = inet_addr((UDINT)IPAdress);	/* IP Address of the Reciever */
		PingVar.timeout = 100;			/* Time of waiting */
		PingOK = 0;
		PingError = 0;
		ICMPping(&PingVar);
		switch(PingVar.status)
		{
			case 0:
			{
				/* Ping OK */
				StartPing = FALSE;
				PingOK = 1;
				PingError = 0;
				/***********************/
				/* TODO: Go on working */
				/***********************/
				break;
			}
			case TCP_IP_BUSY:
				/* Fub is already working */
				/* call again */
				break;
			case PING_NO_RESPONSE:
			{
				/* No Response */
				/*************************/
				/* TODO: Failure routine */
				/*************************/
				StartPing = FALSE;
				PingError = 1;
				PingOK = 0;
				break;
			}
			case PING_TIMEOUT:
			{
				/* Response timedout */
				/***************************/
				/* TODO: Failure routine   */
				/*		 Timeout to short? */
				/***************************/
				StartPing = FALSE;
				PingError = 1;
				PingOK = 0;
				break;
			}
			default:
			{
				/* Error occurs */
				StartPing = FALSE;
				/*************************/
				/* TODO: Failure routine */
				/*************************/
				PingError = 1;
				PingOK = 0;
				break;
			}
		}
	}
}





/***** INIT FUNKTION *******************************************************/
_INIT void init (void)
{
	strcpy(SMTP.Sender,"");
	strcpy(SMTP.Recipient,"");
	IPAdr1 = 192;
	IPAdr2 = 168;
	IPAdr3 = 77;
	IPAdr4 = 99;
} /* Ende Funktion _INIT void init (void) */


/***** ZYKLISCHE FUNKTION **************************************************/
_CYCLIC void cyclic (void)
{
	Ping();

/*V2.00 Mail-Funktion disabled (Bilder entfernt)*/
/*	Mail(); */


} /* Ende Funktion _CYCLIC void cyclic (void) */


/* ENDE */


