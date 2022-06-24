#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : BlueFin                                                      **
** filename : REPLENISHING.C                                               **
** version  : 1.01                                                         **
** date     : 29.10.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Replenishing developer automatically controlled by sqm, number of       **
** plates, or (in Standby) by time                                         **
**                                                                         **
** Copyright (c) 2007, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 05.07.2007
Revised by  : Herbert Aden
Description : Original version, moved functionality from
              developertank module to here.

Version     : 1.01
Date        : 29.10.2007
Revised by  : Herbert Aden
Description : - locking of button "manual replenishing" didn't work correctly
              ->new variable LockManualReplenishment locks only the input for
                amount of replenisher
              - Button Manual Replenishment is not locked anymore, the user
                can now stop replenishing by pressing it a 2nd time

*/
#define _REPLENISHING_C_SRC

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
 ShutDownTime enthält den letzten Zeitstempel vor dem letzten Ausschalten
 wird im INIT in OffTime umkopiert, weil ShutDownTime ja zyklisch beschrieben wird
 OnTime wird im INIT mit der aktuellen Zeit gesetzt, damit kann man aus der
 Differenz zwischen OnTime und OffTime ermitteln, wie lange die Maschine aus war
***/
_GLOBAL   DATE_AND_TIME       OffTime;

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    UINT                NextReplenishingIn;	/* for display*/
/* Variables for manual replenishing*/
_LOCAL    UINT                ManualReplenishment,ManualReplenishmentMinValue;
_LOCAL    BOOL                StartManualReplenishment;
_LOCAL    BOOL                LockManualReplenishment; /* to lock the button
                                                        * while replenishing
                                                        * active
                                                        */
_LOCAL    UINT                PumpTime,PumpTimeToGo;
_LOCAL    UINT                TopUpPumpTime;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    UINT                ReplStep,TopUpStep;
static    UDINT               StandbyReplenishmentSeconds; /*
                                                            * count seconds
                                                            * until next
                                                            * standbyrepl.
                                                            */
static    BOOL                OffReplenishingDone,ManualReplenishmentStarted;
static    REAL                ReplAmount;
static    TON_10ms_typ        ReplTimer,ReplenishmentFlowCheckTimer;
static    REAL                TopUpAmount;
static    TON_10ms_typ        TopUpTimer,TopUpFlowCheckTimer;

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

void InitRepl(void)
{
	StandbyReplenishmentSeconds = 0;
	OffReplenishingDone = 0;
	ReplTimer.IN = 0;
	ReplTimer.PT = 1000;
	LockManualReplenishment = FALSE;
}

void InitTopUp(void)
{
	StandbyReplenishmentSeconds = 0;
	TopUpTimer.IN = 0;
	TopUpTimer.PT = 1000;
}

void Replenish(void)
{

	TON_10ms(&ReplTimer);

	switch (ReplStep)
	{
/*
 * Wait for condition to start replenishing
 * and calculate amount and pump running time
 */
		case 0:
		{
			ManualReplenishmentStarted = FALSE;
/* a) Standby*/
			if( EGMDeveloperTankParam.EnableStandbyReplenishing
		     && (CurrentState != S_PROCESSING)
		     )
			{
				if( Pulse_1s )
					StandbyReplenishmentSeconds++;

				if (StandbyReplenishmentSeconds >
						(EGMDeveloperTankParam.StandbyReplenishmentIntervall * 60))
				{
				/* calc. running time of pump:
				 * ((ml/h) * hours) / (ml/s)
				 */

					PumpTime =
						((EGMDeveloperTankParam.StandbyReplenishment  * ( StandbyReplenishmentSeconds/3600.0)) / EGMDeveloperTankParam.PumpMlPerSec)*100;
					if (PumpTime >= EGMDeveloperTankParam.MinPumpOnTimeStandby)
					{
						ReplAmount =
							(EGMDeveloperTankParam.StandbyReplenishment  * ( StandbyReplenishmentSeconds/3600.0));
						ReplStep = 10;
						StandbyReplenishmentSeconds = 0;
					}
					NextReplenishingIn = 0;
				}
				else	/* calc display value "next replenishinh in XX min"*/
					NextReplenishingIn = EGMDeveloperTankParam.StandbyReplenishmentIntervall -
											(StandbyReplenishmentSeconds/60.0);

			}

/**************************************************************************************/
/* OFF Replenishing, only once on startup*/
			if (EGMDeveloperTankParam.EnableOffReplenishing
				&&(!OffReplenishingDone) )
			{

				PumpTime =
						((EGMDeveloperTankParam.OffReplenishment  * ( OffTime/3600.0)) / EGMDeveloperTankParam.PumpMlPerSec)*100;
				if (PumpTime >= EGMDeveloperTankParam.MinPumpOnTimeStandby
				/*mehr als 5 l Regenerat -> da ist was faul, wir regenieren nicht*/
				&& ((EGMDeveloperTankParam.OffReplenishment  * ( OffTime/3600.0)) < 5000)
				)
				{
					ReplAmount =
						(EGMDeveloperTankParam.OffReplenishment  * ( OffTime/3600.0));
					ReplStep = 10;
				}
				OffReplenishingDone = 1;
			}

/**************************************************************************************/
/* b) per plate*/
			if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMDeveloperTankParam.ReplenishingMode == REPLPERPLATE))
			{
				PumpTime =
						((ReplenishmentPlateCounter * EGMDeveloperTankParam.ReplenishmentPerPlate) / EGMDeveloperTankParam.PumpMlPerSec)*100;
				if (PumpTime >= EGMDeveloperTankParam.MinPumpOnTime)
				{
					ReplAmount =
						ReplenishmentPlateCounter * EGMDeveloperTankParam.ReplenishmentPerPlate;
					ReplStep = 10;
					ReplenishmentPlateCounter = 0;
				}
			}

/**************************************************************************************/
/* c) per area*/
			if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMDeveloperTankParam.ReplenishingMode == REPLPERSQM))
			{
				PumpTime =
						((ReplenishmentSqmCounter * EGMDeveloperTankParam.ReplenishmentPerSqm) / EGMDeveloperTankParam.PumpMlPerSec)*100;
				if (PumpTime >= EGMDeveloperTankParam.MinPumpOnTime)
				{
					ReplAmount =
						ReplenishmentSqmCounter * EGMDeveloperTankParam.ReplenishmentPerSqm;
					ReplStep = 10;

					ReplenishmentSqmCounter = 0;
				}
			}


/**************************************************************************************/
/* d) manually*/
			ManualReplenishmentMinValue = EGMDeveloperTankParam.PumpMlPerSec * (EGMDeveloperTankParam.MinPumpOnTime/100.0);
			if (StartManualReplenishment)
			{
				PumpTime =
						(ManualReplenishment / EGMDeveloperTankParam.PumpMlPerSec)*100;
				if (PumpTime >= EGMDeveloperTankParam.MinPumpOnTime)
				{
					ReplAmount = ManualReplenishment;
					ReplStep = 10;
					ManualReplenishmentStarted = TRUE;
					LockManualReplenishment = TRUE;
				}
			}

			ReplTimer.IN = 0;
			break;
		}

/*
 * - if pump is running, wait
 * - reset flow counter if flow control is in use
 *
 */
		case 10:
		{
			if( EGMDeveloperTank.RegenerationPump == ON )
				break;
/* reset Error message */
			EGM_AlarmBitField[37] = FALSE;
			if( EGMGlobalParam.UseReplFlowControl )
			{
				ReplFC.ResetMlFlown = TRUE;
				ReplStep = 12;
			}
			else
				ReplStep = 20;

			break;
		}
/*
 * if flow control is in use, wait for counter reset
 */
		case 12:
		{
			if( ReplFC.ResetMlFlown == FALSE )
				ReplStep = 20;
			break;
		}

/*
 * - start pump
 * - start timer
 */
		case 20:
		{
			EGMDeveloperTank.RegenerationPump = ON;
			if( EGMGlobalParam.UseReplFlowControl )
				ReplTimer.PT = 6000; /* 60 sec timeout */
			else
			/* no flow control: pump is timer controlled */
				ReplTimer.PT = PumpTime;

			ReplTimer.IN = 1;
		 	ReplStep = 30;
			break;
		}

/*
 * - if flow control active, wait for amount to be flown and check for timeout
 * - else wait for timer to finish
 * - then Switch off pump
 * - calculate PumpTimeToGo
 */
		case 30:
		{
			if( !StartManualReplenishment && ManualReplenishmentStarted)
			{
				EGMDeveloperTank.RegenerationPump = OFF;
				ReplTimer.IN = 0;
				ReplStep = 40;
			}


			if( PumpTime > ReplTimer.ET )
				PumpTimeToGo = PumpTime - ReplTimer.ET;
			else
				PumpTimeToGo = 0;

			if( EGMGlobalParam.UseReplFlowControl )
			{
				if( ReplFC.MlFlown >= ReplAmount ) /* Menge erreicht */
				{
					REAL tmpval;
					if( (ReplFC.MlFlown > 0.1)
					 && (ReplTimer.ET > 0)
					 )
					 {
						tmpval = ReplFC.MlFlown / ((REAL)ReplTimer.ET / 100.0);
						if( tmpval < 1.0 )
							EGMDeveloperTankParam.PumpMlPerSec = 19.0;
						else
							EGMDeveloperTankParam.PumpMlPerSec = tmpval;
					}
					else
						EGMDeveloperTankParam.PumpMlPerSec = 20.0;

					saveParameterData = 1;
					EGMDeveloperTank.RegenerationPump = OFF;
					ReplTimer.IN = 0;
					ReplStep = 40;
					OverallReplenisherCounter += ReplFC.MlFlown;
					ReplenisherCounter += ReplFC.MlFlown;
					ReplenisherUsedSinceDevChg += ReplFC.MlFlown;
				}
				else
				/* Menge noch nicht erreicht: warten und timeout checken*/
				{
					if( (EGM_AlarmBitField[37] == TRUE) /* no flow error */
					||  ReplTimer.Q ) /* Timeout -> Error */
					{
						ReplTimer.IN = 0;
						ReplStep = 40;
						EGMDeveloperTank.RegenerationPump = OFF;
						EGM_AlarmBitField[37] = TRUE;
						ReplFC.ResetMlFlown = TRUE;
						break;
					}
					else
					/* pump switched off manually */
					if( EGMDeveloperTank.RegenerationPump == OFF )
					{
						ReplTimer.IN = 0;
						ReplStep = 40;
						break;
					}
				}
			}
			else /* no flow control */
			{
				if( ReplTimer.Q ) /* Zeit abgelaufen -> dosieren fertig */
				{
					ReplTimer.IN = 0;
					ReplStep = 40;
					EGMDeveloperTank.RegenerationPump = OFF;
					OverallReplenisherPumpTime += (ReplTimer.PT/100.0);
					OverallReplenisherCounter += ((ReplTimer.PT/100.0)* EGMDeveloperTankParam.PumpMlPerSec);
					ReplenisherCounter += ((ReplTimer.PT/100.0)* EGMDeveloperTankParam.PumpMlPerSec);
					ReplenisherUsedSinceDevChg += ((ReplTimer.PT/100.0)* EGMDeveloperTankParam.PumpMlPerSec);
				}
			}
			break;
		}

/*
 * finished: cleanup
 */
		case 40:
		{
			StartManualReplenishment = FALSE;
			LockManualReplenishment = FALSE;
			ReplTimer.IN = 0;
			ReplStep = 0;
			PumpTimeToGo = 0;
			break;
		}
	}


	if( EGMGlobalParam.UseReplFlowControl)
	{
		/* Überwachen, ob auch was kommt */
		ReplenishmentFlowCheckTimer.IN = EGMDeveloperTank.RegenerationPump;
		ReplenishmentFlowCheckTimer.PT = 1000; /* 10 sec*/
		TON_10ms(&ReplenishmentFlowCheckTimer);
		if( ReplenishmentFlowCheckTimer.Q && !ReplFC.Flowing )
		{
		/* no flow detected: stop pump and trigger error message */
			EGM_AlarmBitField[37] = TRUE;
			EGMDeveloperTank.RegenerationPump = OFF;
			StartManualReplenishment = FALSE;
		}
	}
	else /* No Flow Control */
	{
		EGM_AlarmBitField[37] = FALSE;
	} /* else */

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void TopUp(void)
{

	TON_10ms(&TopUpTimer);

	if(isnan(EGMDeveloperTankParam.TopUpPumpMlPerSec))
	{
		EGMDeveloperTankParam.TopUpPumpMlPerSec = 20.0;
		saveParameterData = 1;
	}

	switch (TopUpStep)
	{
/*
 * Wait for condition to start topup
 * and calculate amount and pump running time
 */
		case 0:
		{

/**************************************************************************************/
/* a) per plate*/
			if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMDeveloperTankParam.TopUpMode == REPLPERPLATE))
			{
				TopUpPumpTime =
						((TopUpPlateCounter * EGMDeveloperTankParam.TopUpPerPlate) / EGMDeveloperTankParam.TopUpPumpMlPerSec)*100;
				if (TopUpPumpTime >= EGMDeveloperTankParam.TopUpMinPumpOnTime)
				{
					/* value for Sensorcontrolled replenishing */
					TopUpAmount =
						TopUpPlateCounter * EGMDeveloperTankParam.TopUpPerPlate;
					TopUpStep = 10;
					TopUpPlateCounter = 0;
				}
			}

/**************************************************************************************/
/* b) per area*/
			if (Pulse_1s &&(CurrentState == S_PROCESSING)&& (EGMDeveloperTankParam.TopUpMode == REPLPERSQM))
			{
				TopUpPumpTime =
						((TopUpSqmCounter * EGMDeveloperTankParam.TopUpPerSqm) / EGMDeveloperTankParam.TopUpPumpMlPerSec)*100;
				if (TopUpPumpTime >= EGMDeveloperTankParam.TopUpMinPumpOnTime)
				{
					/* value for Sensorcontrolled replenishing */
					TopUpAmount =
						TopUpSqmCounter * EGMDeveloperTankParam.TopUpPerSqm;
					TopUpStep = 10;
					TopUpSqmCounter = 0;
				}
			}

			TopUpTimer.IN = 0;
			break;
		}

/*
 * - if pump is running, wait
 * - reset flow counter if flow control is in use
 *
 */
		case 10:
		{
			if( EGMDeveloperTank.Refill == ON )
				break;
/* reset Error message */
			EGM_AlarmBitField[38] = FALSE;
			if( EGMGlobalParam.UseTopUpFlowControl )
			{
				TopUpFC.ResetMlFlown = TRUE;
				TopUpStep = 12;
			}
			else
				TopUpStep = 20;

			break;
		}
/*
 * if flow control is in use, wait for counter reset
 */
		case 12:
		{
			if( TopUpFC.ResetMlFlown == FALSE )
				TopUpStep = 20;
			break;
		}

/*
 * - start pump
 * - start timer
 */
		case 20:
		{
			EGMDeveloperTank.Refill = ON;
			if( EGMGlobalParam.UseTopUpFlowControl )
				TopUpTimer.PT = 6000; /* 60 sec timeout */
			else
			/* no flow control: pump is timer controlled */
				TopUpTimer.PT = TopUpPumpTime;

			TopUpTimer.IN = 1;
		 	TopUpStep = 30;
			break;
		}

/*
 * - if flow control active, wait for amount to be flown and check for timeout
 * - else wait for timer to finish
 * - then Switch off pump
 */
		case 30:
		{
			if( EGMGlobalParam.UseTopUpFlowControl )
			{
				if( TopUpFC.MlFlown >= TopUpAmount ) /* Menge erreicht */
				{
					REAL tmpval;
					if( (TopUpFC.MlFlown > 0.1)
					 && (TopUpTimer.ET > 0)
					 )
					 {
						tmpval = TopUpFC.MlFlown / ((REAL)TopUpTimer.ET / 100.0);
						if( tmpval < 1.0 )
							EGMDeveloperTankParam.TopUpPumpMlPerSec = 19.0;
						else
							EGMDeveloperTankParam.TopUpPumpMlPerSec = tmpval;
					}
					else
						EGMDeveloperTankParam.TopUpPumpMlPerSec = 20.0;

					saveParameterData = 1;
					EGMDeveloperTank.Refill = OFF;
					TopUpTimer.IN = 0;
					TopUpStep = 40;
				}
				else
				/* Menge noch nicht erreicht: warten und timeout checken*/
				{
					if( (EGM_AlarmBitField[38] == TRUE) /* no flow error */
					||  TopUpTimer.Q ) /* Timeout -> Error */
					{
						TopUpTimer.IN = 0;
						TopUpStep = 40;
						EGMDeveloperTank.Refill = OFF;
						EGM_AlarmBitField[38] = TRUE;
						TopUpFC.ResetMlFlown = TRUE;
						break;
					}
					else
					/* pump switched off manually */
					if( EGMDeveloperTank.Refill == OFF )
					{
						TopUpTimer.IN = 0;
						TopUpStep = 40;
						break;
					}
				}
			}
			else /* no flow control */
			{
				if( TopUpTimer.Q ) /* Zeit abgelaufen -> dosieren fertig */
				{
					TopUpTimer.IN = 0;
					TopUpStep = 40;
					EGMDeveloperTank.Refill = OFF;
				}
			}
			break;
		}

/*
 * finished: cleanup
 */
		case 40:
		{
			TopUpTimer.IN = 0;
			TopUpStep = 0;
			break;
		}
	}


	if( EGMGlobalParam.UseTopUpFlowControl)
	{
		/* Überwachen, ob auch was kommt */
		TopUpFlowCheckTimer.IN = EGMDeveloperTank.Refill;
		TopUpFlowCheckTimer.PT = 1000; /* 10 sec*/
		TON_10ms(&TopUpFlowCheckTimer);
		if( TopUpFlowCheckTimer.Q && !TopUpFC.Flowing )
		{
		/* no flow detected: stop pump and trigger error message */
			EGM_AlarmBitField[38] = TRUE;
			EGMDeveloperTank.Refill = OFF;
		}
	}
	else /* No Flow Control */
	{
		EGM_AlarmBitField[38] = FALSE;
	} /* else */

}


