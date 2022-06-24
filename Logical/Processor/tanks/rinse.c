#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : BlueFin                                                      **
** filename : RINSE.C                                                      **
** version  : 1.11                                                         **
** date     : 20.11.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Handling of rinse functions: Circulation, refill, replenishing,         **
** monitoring Level and (optional) circulation switch                      **
**                                                                         **
** Copyright (c) 2007, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 06.03.2007
Revised by  : Herbert Aden
Description : Original version, moved functionality from
              developertank task to here.

Version     : 1.10
Date        : 03.05.2007
Revised by  : Herbert Aden
Description : added delay for Level not ok error and delays for
              level switch on and off signal

Version     : 1.11
Date        : 20.11.2007
Revised by  : Herbert Aden
Description : added flow control functionality in mode "freshwater only",
              requires the sensor to be mounted in the inflow tube
*/
#define _RINSE_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/

#include "egmglob_var.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

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
/*NONE*/

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    UINT                RinseValveTime,RinseValveTimeToGo;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    TP_10ms_typ         RinseReplenishmentValveTimer;

static    TON_10ms_typ        RinseLevelOKTimer,RinseLevelNOTOKTimer,
                              RinseLevelErrorTimer,
                              RinsePumpStartTimer,
                              RinseFlowDelay,
                              RinseRefillTimer,
                              RinsePumpOffTimer,
                              RefillValveSafetyTimer;

static    R_TRIGtyp           RinsePumpCmdR,
                              RinseValveCmdR,
                              LevelOKR,
                              LevelNOTOKR;
static    F_TRIGtyp           RinseValveCmdF;

static    UINT                TmpValveTimeRinse;
static    BOOL                RinseRefillActive;


/****************************************************************************/
/**                                                                        **/
/**                      EXTERNAL VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
extern BOOL	Pulse_1s;



/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
void Rinse(void);

/******************************************************************************/
void Rinse(void)
{
/*Level*/
	RinseLevelOKTimer.IN = EGMRinse.TankFull;
	RinseLevelOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&RinseLevelOKTimer);

	RinseLevelNOTOKTimer.IN = !EGMRinse.TankFull;
	RinseLevelNOTOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&RinseLevelNOTOKTimer);

	LevelOKR.CLK = RinseLevelOKTimer.Q;
	LevelNOTOKR.CLK = RinseLevelNOTOKTimer.Q;
	R_TRIG(&LevelOKR);
	R_TRIG(&LevelNOTOKR);
	if(LevelOKR.Q)
		EGMRinse.LevelNotInRange = FALSE;
	if(LevelNOTOKR.Q)
		EGMRinse.LevelNotInRange = TRUE;

/* Fehlermeldung verzögern, erst wird automatisch befüllt */
	RinseLevelErrorTimer.IN = EGMRinse.LevelNotInRange;
	RinseLevelErrorTimer.PT = 2000; /* 20 s */
	TON_10ms(&RinseLevelErrorTimer);
	EGM_AlarmBitField[4] = (ModulStatus_DI == 1) && RinseLevelErrorTimer.Q && (EGMRinseParam.ReplenishingMode != FRESHWATERONLY);



/* REPLENISHING Rinse*/
/* a) per plate*/
	if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMRinseParam.ReplenishingMode == REPLPERPLATE))
	{
		TmpValveTimeRinse =
				((RinseReplenishmentPlateCounter * EGMRinseParam.ReplenishmentPerPlate) / EGMRinseParam.PumpMlPerSec)*100;
		if (TmpValveTimeRinse >= EGMRinseParam.MinPumpOnTime)
		{
			RinseReplenishmentValveTimer.IN = 1;
			RinseReplenishmentPlateCounter = 0;
		}
	}

/* b) per area*/
	if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMRinseParam.ReplenishingMode == REPLPERSQM))
	{
		TmpValveTimeRinse =
				((RinseReplenishmentSqmCounter * EGMRinseParam.ReplenishmentPerSqm) / EGMRinseParam.PumpMlPerSec)*100;
		if (TmpValveTimeRinse >= EGMRinseParam.MinPumpOnTime)
		{
			RinseReplenishmentValveTimer.IN = 1;
			RinseReplenishmentSqmCounter = 0;
		}
	}

	if (EGMRinse.Valve != OPEN )
		RinseValveTime = TmpValveTimeRinse;

	if (RinseReplenishmentValveTimer.IN && (EGMRinse.Valve != OPEN ))
		RinseReplenishmentValveTimer.PT = RinseValveTime;

	if (EGMRinse.Valve == OPEN )
		RinseValveTimeToGo = RinseReplenishmentValveTimer.PT - RinseReplenishmentValveTimer.ET;
	else
		RinseValveTimeToGo = 0;

	TP_10ms(&RinseReplenishmentValveTimer);
	RinseReplenishmentValveTimer.IN = 0;

	RinseValveCmdR.CLK = RinseReplenishmentValveTimer.Q;
	R_TRIG(&RinseValveCmdR);
	if (RinseValveCmdR.Q && (EGMRinseParam.ReplenishingMode != FRESHWATERONLY))
		EGMRinse.Valve = OPEN;

/* Ventil zu nach Zeit oder wenn kein Automatik Modus*/
	RinseValveCmdF.CLK = RinseReplenishmentValveTimer.Q && EGMRinse.Auto;
	F_TRIG(&RinseValveCmdF);
	if ((RinseValveCmdF.Q && (EGMRinseParam.ReplenishingMode != FRESHWATERONLY)) || !ControlVoltageOk)
	{
		EGMRinse.Valve = CLOSE;
	}

/*Refill Rinse*/
	TON_10ms(&RinseRefillTimer);
/* automatisch befüllen nur, wenn nicht ausschliesslich Frischwasser verwendet wird*/
	if (EGMRinseParam.ReplenishingMode != FRESHWATERONLY)
	{
		RinseRefillTimer.IN = !EGMRinse.LevelNotInRange;
		RinseRefillTimer.PT = 200;
/* Füllen nur wenn Automatik modus*/
		if (EGMRinse.LevelNotInRange && ControlVoltageOk && EGMRinse.Auto)
		{
			RinseRefillActive = TRUE;
			EGMRinse.Valve = OPEN;
		}
		if ((RinseRefillTimer.Q || !EGMRinse.Auto) && RinseRefillActive)
		{
			RinseRefillActive = FALSE;
			EGMRinse.Valve = CLOSE;
		}
	}
	else
	{
		RinseRefillTimer.IN = 0;
		RinseRefillActive = FALSE;
	}

/*
 * safety: if Valve is open and Tank full indicated close the valve after 10 sec
 */
	RefillValveSafetyTimer.IN = (EGMRinse.Valve == OPEN) && EGMRinse.TankFull;
	RefillValveSafetyTimer.PT = 1000; /* 10 sec */
	TON_10ms(&RefillValveSafetyTimer);

	if(RefillValveSafetyTimer.Q)
		EGMRinse.Valve = CLOSE;

/*Rinse Pump*/
	RinsePumpCmdR.CLK = EGMRinse.PumpCmd;
	R_TRIG(&RinsePumpCmdR);

/****************************************************************************/
/**                          Flow control                                  **/
/****************************************************************************/
	if(	EGMGlobalParam.UseRinseFlowControl )
	{
	/*
	 * wenn ausschliesslich Frischwasser verwendet wird,
	 * dann darf flow control nur  auf das geöffnete Ventil reagieren
	 */
		if (EGMRinseParam.ReplenishingMode == FRESHWATERONLY)
		{
		/*
		 * 20 sec nach Öffnen muss Durchfluß da sein
		 */
			RinsePumpStartTimer.IN = EGMRinse.Valve;
			RinsePumpStartTimer.PT = 2000; /* 20 sec */
			TON_10ms(&RinsePumpStartTimer);
			if( RinsePumpStartTimer.Q && !RinseFlowInput )
				EGM_AlarmBitField[36] = TRUE;
		}
		else
		{
	/*
	 * 20 sec nach Start der Pumpe muss Durchfluß da sein
	 */
			RinsePumpStartTimer.IN = EGMRinse.Pump;
			RinsePumpStartTimer.PT = 2000; /* 20 sec */
			TON_10ms(&RinsePumpStartTimer);
	/*
	 * wenn Frischwasser zugeführt wird, schaltet der Durchflußwächter ab,
	 * deshalb hier die Verriegelung des Fehlers mit dem Ventil
	 */
			RinseFlowDelay.IN = !RinseFlowInput && !EGMRinse.Valve;
			RinseFlowDelay.PT = 300; /* 3 sec */
			TON_10ms(&RinseFlowDelay);
			if( RinsePumpStartTimer.Q && RinseFlowDelay.Q )
				EGM_AlarmBitField[36] = TRUE;
		}
	}
	else
		EGM_AlarmBitField[36] = FALSE;

/* Rinse Pump
 * wenn 30 sec der Level nicht ok war wird die Pumpe abgeschaltet.
 * Sie kann durch steigende Flanke des Kommandos wieder für 30 s eingeschaltet werden
 * (für Service)
*/
	RinsePumpOffTimer.IN = EGMRinse.LevelNotInRange && !RinsePumpCmdR.Q;
	RinsePumpOffTimer.PT = 3000; /*30 sec*/
	TON_10ms(&RinsePumpOffTimer);
	if (RinsePumpOffTimer.Q || !ControlVoltageOk || EGM_AlarmBitField[36])
	{
		EGMRinse.Pump = OFF;
		EGMRinse.PumpCmd = OFF;
	}
	else
		EGMRinse.Pump = EGMRinse.PumpCmd;

}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


