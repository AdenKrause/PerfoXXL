#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : BlueFin                                                      **
** filename : GUM.C                                                        **
** version  : 1.12                                                         **
** date     : 21.11.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Handling of gumming functions: Circulation, monitoring Level            **
** and (optional) circulation switch                                       **
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

Version     : 1.11
Date        : 16.11.2007
Revised by  : Herbert Aden
Description : added checking for gum valve output by monitoring an input

Version     : 1.12
Date        : 21.11.2007
Revised by  : Herbert Aden
Description : removed checking for gum valve output by monitoring an input
              (was for testing at Bauer only)
*/
#define _GUM_C_SRC

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
/*NONE*/

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    TON_10ms_typ        GumLevelTimer,GumFlowingDelay;
static    TON_10ms_typ        GumPumpOffTimer,GumPumpStartTimer;

static    R_TRIGtyp           GumPumpCmdR;

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
void Gum(void);
void InitGum(void);

/******************************************************************************/
void InitGum(void)
{
	return;
}

/******************************************************************************/
void Gum(void)
{

	if(!EGMGlobalParam.EnableGumSection && (MachineType != BLUEFIN_XS))
	{
		EGM_AlarmBitField[2] = FALSE;
		EGM_AlarmBitField[33] = FALSE;
		EGMGum.LevelNotInRange = FALSE;
		EGMGum.Pump = OFF;
		EGMGum.PumpCmd = OFF;
		EGMGum.Valve = OFF;
		EGMGum.RinseValve = OFF;
		return;
	}

/*Level Gum*/
	GumLevelTimer.IN = !EGMGum.TankFull;
	GumLevelTimer.PT = 200; /*2 sec*/
	TON_10ms(&GumLevelTimer);
	EGMGum.LevelNotInRange = GumLevelTimer.Q;
	EGM_AlarmBitField[2] = (ModulStatus_DI == 1) && EGMGum.LevelNotInRange;
/*Gum Pump*/
	GumPumpCmdR.CLK = EGMGum.PumpCmd;
	R_TRIG(&GumPumpCmdR);

	if(	EGMGlobalParam.UseGumFlowControl )
	{
	/* flow control starts 20 sec after pump start */
		GumPumpStartTimer.IN = EGMGum.Pump;
		GumPumpStartTimer.PT = 2000; /*20 sec*/
		TON_10ms(&GumPumpStartTimer);

	/* delay the "not flowing" error a bit */
		GumFlowingDelay.IN = !GumFC.Flowing;
		GumFlowingDelay.PT = 300; /* 3sec */
		TON_10ms(&GumFlowingDelay);

		if( GumPumpStartTimer.Q && GumFlowingDelay.Q )
		{
			EGM_AlarmBitField[33] = TRUE;
			GumFC.ResetMlFlown = TRUE;
		}
	}
	else
		EGM_AlarmBitField[33] = FALSE;

/* manual retrigger of the pump is possible, it will run for 5 sec */
	GumPumpOffTimer.IN = EGMGum.LevelNotInRange && !GumPumpCmdR.Q;
	GumPumpOffTimer.PT = 500; /*5 sec*/
	TON_10ms(&GumPumpOffTimer);
	if (GumPumpOffTimer.Q || !ControlVoltageOk || EGM_AlarmBitField[33])
	{
		EGMGum.Pump = OFF;
		EGMGum.PumpCmd = OFF;
	}
	else
		EGMGum.Pump = EGMGum.PumpCmd;

/*
 * safety: if the gum pump is running we always want the gum valve switched
 * and the Rinse valve closed
 */
	if( EGMGum.Pump )
	{
		EGMGum.Valve = ON;
		EGMGum.RinseValve = CLOSE;
	}

/*
 * safety: if gum rinse valve is switched on, then
 * we always want the gum valve to be switched off to make sure the
 * rinsing water cannot flow into the gum tank;
 * the Pump is switched off as well
 */
	if( EGMGum.RinseValve == OPEN )
	{
		EGMGum.Pump = EGMGum.PumpCmd = OFF;
		EGMGum.Valve = OFF;
	}
}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


