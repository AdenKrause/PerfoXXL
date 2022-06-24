#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Treiberprogramm für LUST FU CDD3000 über CAN		 					*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.10		13.08.02	zyklische Übertragung des Steuerwortes	HA		*/
/*	1.00		06.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include <string.h>					/*	Definitionen für Stringfunktionen	*/

#include "standard.h"
#include "sys_lib.h"

#include "bit_func.h"				/*	Macros für die Bitbearbeitung		*/
#include "glob_var.h"


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
#define STANDARD_PARAM_ENQ 5
#define STANDARD_PARAM_SEL 2
#define FELD_PARAM_ENQ 13
#define FELD_PARAM_SEL 12

#define WAITCYCLES		15
#define	TIMEOUT			50

/****************************************************************************/
/*		Variablendeklaration												*/
/****************************************************************************/
_GLOBAL	plcbit			CAN_ok;

/* Hilfsvariable für Watch */
_GLOBAL BOOL FU_INIT_RDY;

/* Struktur als Schnittstelle zur Applikation */
_GLOBAL	FU_Typ 			FU;

/* Struktur für zu schreibende/lesende Parameter */
_GLOBAL	FU_Para_Typ		FU_Param;

/* Struktur für die eigentlichen Telegrammdaten								*/
_GLOBAL	CAN_OBJEKT_FUtyp	Can_FU;

_LOCAL unsigned short i;
DINT* tmpPtr;
_GLOBAL BOOL MirroredMachine;
_LOCAL REAL ParkSpeed_Old;
extern REAL 	speed_old,StartPosition_old,PEGStartPosition_old,EndPosition_old,
		PEGEndPosition_old;

/****************************************************************************/
/*		Prototypen															*/
/****************************************************************************/
extern void ReferenzFahrt(void);
extern void resetFUData(void);
extern void ParkPosition(void);
extern void ZielPosition(void);
extern void Belichtung(void);

/****************************************************************************/
/*		Initialisierungs-UP													*/
/*   																		*/
/****************************************************************************/
_INIT void lust_cdd_init(void)
{
	FU_INIT_RDY = FALSE;
	FU.Status = 0;
	FU.ParaStep = 0;
	FU_Param.Error = 0;
	FU_Param.Ready = 0;
	strcpy(&FU.DrvError[0],"Init...");
	resetFUData();
	FU.CANError = TRUE;
	speed_old = 0.0;
	StartPosition_old = 0.0;
	PEGStartPosition_old = 0.0;
	EndPosition_old = 0.0;
	PEGEndPosition_old = 0.0;
	ParkSpeed_Old = 0.0;
}


/****************************************************************************/
/*		zyklischer Teil      												*/
/****************************************************************************/
_CYCLIC void lust_cdd_cyclic(void)
{
	int j;
/* Funktionen aufrufen*/
	ReferenzFahrt();
	ParkPosition();
	ZielPosition();
	Belichtung();

/* CAN nicht ok? dann ende */
	if (CAN_ok != 1)				/* Fehler aus 'can_comm' Task */
	{
		strcpy(&FU.DrvError[0],"CANERROR");
		resetFUData();
		FU.CANError = TRUE;
		return;
	}
/* CAN ist ok, also weiter*/

/* FU meldet sich nach dem Einschalten auf BAsis ID $607 */
	if (Can_FU.InitResEv == 1)
	{
			FU_INIT_RDY = 1;

	/* FU init quittieren, Antwort mit $dd */
			Can_FU.InitResEv = 0;
			Can_FU.InitReq[0] = 1;
			Can_FU.InitReqEv = 1;
/* Variablen initialisieren */
			FU.SetPara = 0;
			FU.GetPara = 0;
			FU.IndexPara = 0;
			FU.IdxIntern = 0;
			FU.timeoutCounter = 0;
			resetFUData();
			strcpy(&FU.DrvError[0],"INIT OK");
			FU.CANError = FALSE;
	}


/********************************************************************/
/* 																	*/
/* FU meldet seinen Status nach dem Initialisieren auf Basis $371   */
/* dieses Telegramm kommt zyklisch vom Antrieb, deshalb hier		*/
/* über Timeout Erkennung, ob Antrieb noch am Bus					*/
/* 																	*/
/********************************************************************/

	if (Can_FU.StatResEv == 1)
	{
			FU.CANError = FALSE;
			Can_FU.StatResEv = 0;
			FU.Status = Can_FU.StatRes[0];
/* Statusinformationen in Einzelbits eintragen */
		/* Byte 0 */
			FU.Error 			= BIT_TST(Can_FU.StatRes[0],0);
			FU.CANStatus		= BIT_TST(Can_FU.StatRes[0],1);
			FU.SollwertErreicht	= BIT_TST(Can_FU.StatRes[0],2);
			FU.Activ 			= BIT_TST(Can_FU.StatRes[0],4);
			FU.Rot0 			= BIT_TST(Can_FU.StatRes[0],5);
			FU.CReady 			= BIT_TST(Can_FU.StatRes[0],7);
		/* Byte 1*/
			FU.ENPO 			= BIT_TST(Can_FU.StatRes[1],0);
			FU.OSD00 			= BIT_TST(Can_FU.StatRes[1],1);
			FU.OSD01 			= BIT_TST(Can_FU.StatRes[1],2);
		/* Byte 2 */
			FU.RefOk 			= BIT_TST(Can_FU.StatRes[2],0);
			FU.Auto 			= BIT_TST(Can_FU.StatRes[2],1);
			FU.Ablauf 			= BIT_TST(Can_FU.StatRes[2],2);
			for (i=0;i<8;i++)
			{
				FU.ZustandsMerker[i] =  BIT_TST(Can_FU.StatRes[3],i);
			}
		/* aktuelle Pos aus bytes 4..7 */
			tmpPtr =(UDINT*) &Can_FU.StatRes[4];
			FU.aktuellePosition = *tmpPtr;
			FU.aktuellePosition_mm = ((float) FU.aktuellePosition)/X_Param.InkrementeProMm;

			FU.timeoutCounter = 0;
			strcpy(&FU.DrvError[0],"STAT OK");

	}
/* timeout überwachung */
	else
	{
			FU.timeoutCounter ++;
			if (FU.timeoutCounter >= TIMEOUT)
			{
				strcpy(&FU.DrvError[0],"Timeout");
				FU.timeoutCounter = 0;
				CAN_ok = 0;
				resetFUData();
				FU.CANError = TRUE;
			}

	}


/********************************************************************/
/* 																	*/
/* Steuerwort senden zyklisch										*/
/* 																	*/
/********************************************************************/

/* kein ENPO -> steuerdaten alle 0*/
	if (!FU.ENPO)
	{
		FU.IdxIntern = 0;
		for (i=0;i<8;i++)
			Can_FU.SteuReq[i] = 0;
/*HA 18.05.2005 V2.11 reset start commands if no ENPO*/
		FU.cmd_Start = 0;
		FU.cmd_Auto = 0;
	}

/********************************************************************/
/* falls Fehler: quittieren  */
	if (FU.Error)
	{
/* erst Fehler Quittierung AUS */
		if(FU.IdxIntern <= WAITCYCLES)
		{
			FU.IdxIntern++;
			if(FU.IdxIntern == WAITCYCLES)
			{
				Can_FU.SteuReq[0] = 0 ; /*  */
				FU.IdxIntern = 300;
				FU.cmd_Start = 0;
			}
		}
		else
/* dann Fehler Quittierung EIN (Flanke erzeugen)*/
		{
			if(FU.IdxIntern <= 300+WAITCYCLES)
			{
				FU.IdxIntern++;
				if(FU.IdxIntern == 300+WAITCYCLES)
				{
					Can_FU.SteuReq[0] = 128; /*  */
					FU.IdxIntern = 0;
				}
			}
		} /* else */
	} /*if (FU.Error)*/

/********************************************************************/
/* Befehl aus App: Regelung freigeben*/
	if(FU.cmd_Start)
	{
	/* Regelung nicht aktiv */
		if (!FU.Activ)
		{
	/* erst Regelung gezielt AUS schalten*/
			if(FU.IdxIntern <= WAITCYCLES)
			{
				FU.IdxIntern++;
				if(FU.IdxIntern == WAITCYCLES)
				{
					Can_FU.SteuReq[0] = BIT_CLR(Can_FU.SteuReq[0],0); /* Regelung AUS */
					Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],0); /* Auto AUS*/
					Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],1); /* Ablauf AUS*/
					FU.IdxIntern = 300;
				}
			} /*if(FU.IdxIntern <= 8)*/
	/* dann Regelung gezielt wieder EIN schalten*/
			else
			{
				FU.IdxIntern++;
				if(FU.IdxIntern == 300+WAITCYCLES)
				{
					Can_FU.SteuReq[0] = BIT_SET(Can_FU.SteuReq[0],0); /* Regelung EIN */
					Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],0); /* Auto AUS*/
					Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],1); /* Ablauf AUS*/
					FU.IdxIntern = 0;
				}
			} /*else*/
		} /*if (!FU.Activ)*/
	} /*if(FU.cmd_Start)*/
	else
/* Befehl aus App: Reglerfreigabe AUS */
	{
	/* Regelung aktiv */
		if (FU.Activ)
		{
	/* Regelung gezielt AUS schalten*/
			FU.IdxIntern++;
			if(FU.IdxIntern == WAITCYCLES)
			{
				Can_FU.SteuReq[0] = BIT_CLR(Can_FU.SteuReq[0],0); /* Regelung AUS */
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],0); /* Auto AUS*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],1); /* Ablauf AUS*/
				FU.IdxIntern = 0;
			} /*if(FU.IdxIntern == 8)*/
		}/* if (FU.Activ)*/
	} /*else*/


/********************************************************************/
/* nur wenn aus App AUTO Befehl kommt... */
	if (FU.cmd_Auto && !FU.state_tipp_plus && !FU.state_tipp_minus)
	{
/*nach kurzer Zeit Automatik EIN schalten*/
		if (FU.Activ && !FU.Auto)
		{
			FU.IdxIntern++;
			if(FU.IdxIntern == WAITCYCLES)
			{
				Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],0); /*Auto EIN*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],1); /*Ablauf AUS*/

				FU.IdxIntern = 0;
				FU.cmd_tipp_schnell = 0;
			}
		}

/*nach kurzer Zeit Ablaufprogramm Start EIN schalten*/
		if (FU.Activ && FU.Auto && !FU.Ablauf)
		{
			FU.IdxIntern++;
			if(FU.IdxIntern == WAITCYCLES)
			{
				Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],1); /*Ablauf EIN*/
				FU.IdxIntern = 0;
			}
		}
	} /* if cmd_Auto */
/* sonst Handbetrieb */
	else
/*nach kurzer Zeit Automatik AUS schalten*/
	{
		if (FU.Auto && !FU.state_tipp_plus && !FU.state_tipp_minus)
		{
			FU.IdxIntern++;
			if(FU.IdxIntern == WAITCYCLES)
			{
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],0); /*Auto AUS*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],1); /*Ablauf AUS*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],6); /*TIPP+*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],7); /*TIPP-*/
				FU.cmd_tipp_plus = 0;
				FU.cmd_tipp_minus = 0;
				FU.state_tipp_plus = 0;
				FU.state_tipp_minus = 0;
				FU.state_tipp_schnell = 0;

				FU.IdxIntern = 0;
			}
		}
		else /* wenn Auto AUS ist */
		{
/*  TIPP Betrieb auswerten */

/****** PLUS ****/
/* Kommando TIPP + gekommen: einmal TIPP+ Bit setzen */
			if (FU.cmd_tipp_plus && !FU.state_tipp_plus )
			{
				if (MirroredMachine)
					Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],7); /*TIPP-*/
				else
					Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],6); /*TIPP+*/
				FU.state_tipp_plus = 1;
				FU.IdxIntern = 0;
			}

/****** MINUS ****/
/* Kommando TIPP- gekommen: einmal TIPP- Bit setzen */
			if (FU.cmd_tipp_minus && !FU.state_tipp_minus )
			{
				if (MirroredMachine)
					Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],6); /*TIPP+*/
				else
					Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],7); /*TIPP-*/
				FU.state_tipp_minus = 1;
				FU.IdxIntern = 0;
			}

/* Schnell: zusätzlich das andere Bit setzen */
			if (FU.state_tipp_plus && FU.cmd_tipp_schnell && !FU.state_tipp_schnell)
			{
				FU.IdxIntern ++;
				if(	FU.IdxIntern == WAITCYCLES )
				{
					if (MirroredMachine)
						Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],6); /*TIPP-*/
					else
						Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],7); /*TIPP-*/
					FU.state_tipp_schnell = TRUE;
					FU.IdxIntern = 0;
				}
			}
			if (FU.state_tipp_minus && FU.cmd_tipp_schnell && !FU.state_tipp_schnell)
			{
				FU.IdxIntern ++;
				if(	FU.IdxIntern == WAITCYCLES)
				{
					if (MirroredMachine)
						Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],7); /*TIPP-*/
					else
						Can_FU.SteuReq[2] = BIT_SET(Can_FU.SteuReq[2],6); /*TIPP-*/
					FU.state_tipp_schnell = TRUE;
					FU.IdxIntern = 0;
				}
			}

/* Kommando TIPP- oder + gegangen: einmal TIPP Bits rücksetzen */
			if (!FU.cmd_tipp_minus && !FU.cmd_tipp_plus)
			{
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],6); /*TIPP+*/
				Can_FU.SteuReq[2] = BIT_CLR(Can_FU.SteuReq[2],7); /*TIPP-*/
				FU.state_tipp_minus = 0;
				FU.state_tipp_plus = 0;
				FU.state_tipp_schnell = 0;
			}
		} /*else*/
	}

/********************************************************************/
/* Befehlsmerker senden*/
/********************************************************************/

	/* die Einzelmerker in das Steuerwort schreiben */
	Can_FU.SteuReq[3] = 0;
	j = 1;
	for(i=0;i<8;i++)
	{
		Can_FU.SteuReq[3] = Can_FU.SteuReq[3] | (j * FU.BefehlsMerker[i]);
		j *= 2;
	}

/*in jedem Zyklus das Schreibevent auslösen*/
	Can_FU.SteuReqEv = 1;


/********************************************************************/
/********************************************************************/
/********************************************************************/

/* Parameter SCHREIBEN Anforderung durch Applikation */
	if (FU.SetPara == 1)
	{
		switch (FU.ParaStep)
		{
			case 0:
			{
						/* Daten Eintragen */
				Can_FU.ParaReq[0] = FU_Param.ParaNr & 0x00FF;
				Can_FU.ParaReq[1] = (USINT)(FU_Param.ParaNr >> 8) & 0x00FF;
				/*FeldParameter -> andere Kennung und Index eintragen*/
				if(FU_Param.ParaNr == 728)
				{
					Can_FU.ParaReq[2] = FELD_PARAM_SEL;
					Can_FU.ParaReq[7] = FU_Param.Index;
				}
				/*StandardParameter */
				else
				{
					Can_FU.ParaReq[2] = STANDARD_PARAM_SEL;
					Can_FU.ParaReq[7] = 0;
				}
				tmpPtr = (UDINT*) &Can_FU.ParaReq[3];
				*tmpPtr = FU_Param.Wert;

				Can_FU.ParaReqEv = 1; 	/* Daten senden */
				FU.ParaStep = 1;
				FU_Param.Ready = 0;
				break;
			}
			case 1:
			{
			/*Rückmeldung vom FU erwarten und auswerten*/
				if(Can_FU.ParaResEv == 1)
				{
					Can_FU.ParaResEv = 0;
					/*Status OK?*/
					if (Can_FU.ParaRes[2] == 0)
					{
						FU.ParaStep = 2;
						FU_Param.Error = 0;
					}
					else
						FU_Param.Error = 1;
				}
				break;
			}
			case 2:
			{
				FU.SetPara = 0;
				FU.ParaStep = 0;
				FU_Param.Ready = 1;
			}
		} /*switch*/
	} /*if (FU.SetPara == 1)*/
}	/* Endcyclic */


