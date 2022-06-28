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
/*	1.11		15.10.15	SMTP Client removed						HA		*/
/*																			*/
/****************************************************************************/

/***** Include-Files: *****/
#include <bur/plc.h>
#include <bur/plctypes.h>
#include <string.h>
/* damit das switch unten funktioniert*/
#define _REPLACE_CONST
#include <AsICMP.h>
#include "glob_var.h"
#include <AsBrStr.h>

/***** Variablen-/Konstantendeklaration: *****/

#ifndef TRUE
#define TRUE				1
#endif
#ifndef FALSE
#define FALSE				0
#endif

USINT PingOK,PingError;
_LOCAL	IcmpPing_typ PingVar;
_LOCAL	UINT	PingOKInvisible,PingErrorInvisible;
_LOCAL USINT StartPing,IPAdr1,IPAdr2,IPAdr3,IPAdr4;

STRING IPAdress[16],tmp[10];
TOF_10ms_typ PingLEDOff;


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
		brsitoa(IPAdr1,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		brsitoa(IPAdr2,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		brsitoa(IPAdr3,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		strcat(IPAdress,".");
		brsitoa(IPAdr4,(UDINT) &tmp[0]);
		strcat(IPAdress,tmp);
		PingVar.pHost = (UDINT)IPAdress;	/* IP Address of the Reciever */
		PingVar.timeout = 100;			/* Time of waiting */
		PingOK = 0;
		PingError = 0;
		IcmpPing(&PingVar);
		switch(PingVar.status)
		{
			case ERR_OK:
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
			case ERR_FUB_BUSY:
				/* Fub is already working */
				/* call again */
				break;
			case icmpERR_NO_RESPONSE:
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
			case icmpERR_PARAMETER:
			case icmpERR_SOCKET_CREATE:
			case icmpERR_SYSTEM:
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
	IPAdr1 = 192;
	IPAdr2 = 168;
	IPAdr3 = 77;
	IPAdr4 = 99;
} /* Ende Funktion _INIT void init (void) */


/***** ZYKLISCHE FUNKTION **************************************************/
_CYCLIC void cyclic (void)
{
	Ping();

} /* Ende Funktion _CYCLIC void cyclic (void) */


/* ENDE */


