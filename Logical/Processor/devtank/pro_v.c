#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : BlueFin                                                      **
** filename : PRO_V.C                                                      **
** version  : 1.01                                                         **
** date     : 16.10.2008                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
**                                                                         **
** Copyright (c) 2008, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 14.10.2008
Revised by  : Herbert Aden
Description : Original version

Version     : 1.01
Date        : 16.10.2008
Revised by  : Herbert Aden
Description : Pumps now only active, if machine is in ON mode


*/
#define _PRO_V_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/

#include "glob_var.h"
#include "egmglob_var.h"
#include <math.h>

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
/***
***/

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    INT                 RefillStep;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    TON_10ms_typ        ReplPumpTimer;
static    TON_10ms_typ        CanisterFullTimer;
static    TOF_10ms_typ        CanisterNotFullTimer;

/****************************************************************************/
/**                                                                        **/
/**                      EXTERNAL VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/



/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/

void InitProV(void)
{
	RefillStep = 0;
	CanisterFullTimer.IN    = FALSE;
	CanisterNotFullTimer.IN = FALSE;
	ReplPumpTimer.IN = FALSE;
}


void ProV(void)
{

/*
 * Umwälzung immer, solange nicht leer (Levelswitch ist scließer!)
 */
	Out_DevCanister_Circulation =  In_DevCanister_Empty
								&& ControlVoltageOk
								&& EGMDeveloperTank.Enable;


	CanisterFullTimer.IN    = In_DevCanister_Full;
	CanisterNotFullTimer.IN = In_DevCanister_Full;
	CanisterFullTimer.PT = 1000;
	CanisterNotFullTimer.PT = 1000;
	TON_10ms(&CanisterFullTimer);
	TOF_10ms(&CanisterNotFullTimer);

	TON_10ms(&ReplPumpTimer);
/*
 * Befüllpumpe an für 1 min, wenn Levelschalter "voll" weggeht
 * Wenn Schalter wiederkommt, dann auf jeden Fall aus
 */

	if( !ControlVoltageOk
 	 || !EGMDeveloperTank.Enable
 	 )
 	{
 		RefillStep = 0;
 		ReplPumpTimer.IN = FALSE;
		Out_DevCanister_Replenish = FALSE;
 	}

	switch (RefillStep)
	{
		case 0:
		{
			if( !ControlVoltageOk
		 	 || !EGMDeveloperTank.Enable
		 	 )
		 		break;

		/* Pumpe an, Wartezeit 0,5 min triggern*/
			if(!CanisterNotFullTimer.Q)
			{
				ReplPumpTimer.IN = TRUE;
				ReplPumpTimer.PT = EGMAutoParam.PROV_PumpOnTime;
				Out_DevCanister_Replenish = TRUE;
				RefillStep = 10;
				break;
			}

			break;
		}
		case 10:
		{
		/* Wartezeit abgelaufen: Pumpe aus,nächster Schritt*/
			if(ReplPumpTimer.Q)
			{
				Out_DevCanister_Replenish = FALSE;
				ReplPumpTimer.IN = FALSE;
				RefillStep = 20;
				break;
			}
		/* Kanister wieder voll: Pumpe aus und fertig*/
			if(CanisterFullTimer.Q)
			{
				ReplPumpTimer.IN = FALSE;
				RefillStep = 0;
				Out_DevCanister_Replenish = FALSE;
				break;
			}

			break;
		}
		case 20:
		{
		/* Warten eine min */
			if(ReplPumpTimer.Q)
			{
				ReplPumpTimer.IN = FALSE;
				RefillStep = 0;
				break;
			}
			ReplPumpTimer.IN = TRUE;
			ReplPumpTimer.PT = EGMAutoParam.PROV_PumpOffTime; /* 5 min */
			break;
		}
	}


}


