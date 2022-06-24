#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : BlueFin                                                      **
** filename : PREWASH.C                                                    **
** version  : 1.11                                                         **
** date     : 20.11.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Handling of prewash functions: Circulation, refill, replenishing,       **
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
#define _PREWASH_C_SRC

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
_LOCAL    UINT                PrewashValveTime,PrewashValveTimeToGo;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    TP_10ms_typ         PrewashReplenishmentValveTimer;

static    TON_10ms_typ        PrewashLevelOKTimer,PrewashLevelNOTOKTimer,
                              PrewashLevelErrorTimer,
                              PrewashPumpStartTimer,
                              PrewashFlowDelay,
                              PrewashRefillTimer,
                              PrewashPumpOffTimer,
                              RefillValveSafetyTimer;

static    R_TRIGtyp           PrewashPumpCmdR,
                              PrewashValveCmdR,
                              LevelOKR,
                              LevelNOTOKR;
static    F_TRIGtyp           PrewashValveCmdF;

static    BOOL                PrewashRefillActive;
static    UINT                TmpValveTimePrewash;

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
void Prewash(void);


/******************************************************************************/
void Prewash(void)
{
/*Level*/
	PrewashLevelOKTimer.IN = EGMPrewash.TankFull;
	PrewashLevelOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&PrewashLevelOKTimer);

	PrewashLevelNOTOKTimer.IN = !EGMPrewash.TankFull;
	PrewashLevelNOTOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&PrewashLevelNOTOKTimer);

	LevelOKR.CLK = PrewashLevelOKTimer.Q;
	LevelNOTOKR.CLK = PrewashLevelNOTOKTimer.Q;
	R_TRIG(&LevelOKR);
	R_TRIG(&LevelNOTOKR);
	if(LevelOKR.Q)
		EGMPrewash.LevelNotInRange = FALSE;
	if(LevelNOTOKR.Q)
		EGMPrewash.LevelNotInRange = TRUE;

/* Fehlermeldung verzögern, erst wird automatisch befüllt */
	PrewashLevelErrorTimer.IN = EGMPrewash.LevelNotInRange;
	PrewashLevelErrorTimer.PT = 2000; /* 20 s */
	TON_10ms(&PrewashLevelErrorTimer);
	EGM_AlarmBitField[3] = (ModulStatus_DI == 1) && PrewashLevelErrorTimer.Q && (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY);

/* REPLENISHING Prewash*/
/* a) per plate*/
	if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMPrewashParam.ReplenishingMode == REPLPERPLATE))
	{
		TmpValveTimePrewash =
				((PrewashReplenishmentPlateCounter * EGMPrewashParam.ReplenishmentPerPlate) / EGMPrewashParam.PumpMlPerSec)*100;
		if (TmpValveTimePrewash >= EGMPrewashParam.MinPumpOnTime)
		{
			PrewashReplenishmentValveTimer.IN = 1;
			PrewashReplenishmentPlateCounter = 0;
		}
	}

/* b) per area*/
	if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMPrewashParam.ReplenishingMode == REPLPERSQM))
	{
		TmpValveTimePrewash =
				((PrewashReplenishmentSqmCounter * EGMPrewashParam.ReplenishmentPerSqm) / EGMPrewashParam.PumpMlPerSec)*100;
		if (TmpValveTimePrewash >= EGMPrewashParam.MinPumpOnTime)
		{
			PrewashReplenishmentValveTimer.IN = 1;
			PrewashReplenishmentSqmCounter = 0;
		}
	}

	if (EGMPrewash.Valve != OPEN )
		PrewashValveTime = TmpValveTimePrewash;

	if (PrewashReplenishmentValveTimer.IN && (EGMPrewash.Valve != OPEN ))
		PrewashReplenishmentValveTimer.PT = PrewashValveTime;

	if (EGMPrewash.Valve == OPEN )
		PrewashValveTimeToGo = PrewashReplenishmentValveTimer.PT - PrewashReplenishmentValveTimer.ET;
	else
		PrewashValveTimeToGo = 0;

	TP_10ms(&PrewashReplenishmentValveTimer);
	PrewashReplenishmentValveTimer.IN = 0;

	PrewashValveCmdR.CLK = PrewashReplenishmentValveTimer.Q;
	R_TRIG(&PrewashValveCmdR);
	if (PrewashValveCmdR.Q && (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY))
		EGMPrewash.Valve = OPEN;

/* Ventil zu nach Zeit oder wenn kein Automatik Modus*/
	PrewashValveCmdF.CLK = PrewashReplenishmentValveTimer.Q && EGMPrewash.Auto;
	F_TRIG(&PrewashValveCmdF);
	if ((PrewashValveCmdF.Q && (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY)) || !ControlVoltageOk)
	{
		EGMPrewash.Valve = CLOSE;
	}

/*Refill Prewash*/
	TON_10ms(&PrewashRefillTimer);

/* automatisch befüllen nur, wenn nicht ausschliesslich Frischwasser verwendet wird*/
	if (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY)
	{
		PrewashRefillTimer.IN = !EGMPrewash.LevelNotInRange;
		PrewashRefillTimer.PT = 200;
/* Füllen nur wenn Automatik modus*/
		if (EGMPrewash.LevelNotInRange && ControlVoltageOk && EGMPrewash.Auto)
		{
			EGMPrewash.Valve = OPEN;
			PrewashRefillActive = TRUE;
		}
		if ((PrewashRefillTimer.Q || !EGMPrewash.Auto) && PrewashRefillActive)
		{
			PrewashRefillActive = FALSE;
			EGMPrewash.Valve = CLOSE;
		}
	}
	else
	{
		PrewashRefillTimer.IN = 0;
		PrewashRefillActive = FALSE;
	}

/*
 * safety: if Valve is open and Tank full indicated close the valve after 10 sec
 */
	RefillValveSafetyTimer.IN = (EGMPrewash.Valve == OPEN) && EGMPrewash.TankFull;
	RefillValveSafetyTimer.PT = 1000; /* 10 sec */
	TON_10ms(&RefillValveSafetyTimer);

	if(RefillValveSafetyTimer.Q)
		EGMPrewash.Valve = CLOSE;

	PrewashPumpCmdR.CLK = EGMPrewash.PumpCmd;
	R_TRIG(&PrewashPumpCmdR);


/****************************************************************************/
/**                          Flow control                                  **/
/****************************************************************************/
	if(	EGMGlobalParam.UsePrewashFlowControl )
	{
	/*
	 * wenn ausschliesslich Frischwasser verwendet wird,
	 * dann darf flow control nur  auf das geöffnete Ventil reagieren
	 */
		if (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY)
		{
		/*
		 * 20 sec nach Öffnen muss Durchfluß da sein
		 */
			PrewashPumpStartTimer.IN = EGMPrewash.Valve;
			PrewashPumpStartTimer.PT = 2000; /* 20 sec */
			TON_10ms(&PrewashPumpStartTimer);
			if( PrewashPumpStartTimer.Q && !PrewashFlowInput )
				EGM_AlarmBitField[35] = TRUE;
		}
		else
		{
	/*
	 * 20 sec nach Start der Pumpe muss Durchfluß da sein
	 */
			PrewashPumpStartTimer.IN = EGMPrewash.Pump;
			PrewashPumpStartTimer.PT = 2000; /* 20 sec */
			TON_10ms(&PrewashPumpStartTimer);
	/*
	 * wenn Frischwasser zugeführt wird, schaltet der Durchflußwächter ab,
	 * deshalb hier die Verriegelung des Fehlers mit dem Ventil
	 */
			PrewashFlowDelay.IN = !PrewashFlowInput && !EGMPrewash.Valve;
			PrewashFlowDelay.PT = 300; /* 3 sec */
			TON_10ms(&PrewashFlowDelay);
			if( PrewashPumpStartTimer.Q && PrewashFlowDelay.Q )
				EGM_AlarmBitField[35] = TRUE;
		}
	}
	else
		EGM_AlarmBitField[35] = FALSE;

/* Prewash Pump
 * wenn 30 sec der Level nicht ok war wird die Pumpe abgeschaltet.
 * Sie kann durch steigende Flanke des Kommandos wieder für 30 s eingeschaltet werden
 * (für Service)
*/
	PrewashPumpOffTimer.IN = EGMPrewash.LevelNotInRange && !PrewashPumpCmdR.Q;
	PrewashPumpOffTimer.PT = 3000; /*30 sec*/
	TON_10ms(&PrewashPumpOffTimer);
	if (PrewashPumpOffTimer.Q || !ControlVoltageOk || EGM_AlarmBitField[35])
	{
		EGMPrewash.Pump = OFF;
		EGMPrewash.PumpCmd = OFF;
	}
	else
		EGMPrewash.Pump = EGMPrewash.PumpCmd;

}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


