#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/**
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : PAPIERENTFERNUNG.C                                           **
** version  : 1.24                                                         **
** date     : 26.06.2008                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Sequence for removing paper from plate                                  **
**                                                                         **
**                                                                         **
** Copyright (c) 2008, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 09.09.2002
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.10
Date        : 30.09.2002
Revised by  : Herbert Aden
Description : Added timeout checking for movements

Version     : 1.11
Date        : 28.03.2003
Revised by  : Herbert Aden
Description : bugfix plate coounter

Version     : 1.12
Date        : 24.06.2003
Revised by  : Herbert Aden
Description : plate stack detection at every start

Version     : 1.20
Date        : 20.10.2006
Revised by  : Herbert Aden
Description : code review and using of MotorFunc source lib

Version     : 1.21
Date        : 12.10.2007
Revised by  : Herbert Aden
Description : MoveToAbsPosition has changed, so we must provide a pointer
              to an USINT variable that can hold the internal state of that fct.

Version     : 1.22
Date        : 15.05.2008
Revised by  : Herbert Aden
Description : bugfix: if no conn. to TB AND no automatic trolley open/close, then
                      2 messages are shown during startup. A timing problem resulted
                      in the 2nd message not to be shown. fix: a wait time of 1/2 sec

Version     : 1.23
Date        : 30.05.2008
Revised by  : Herbert Aden
Description : bugfix: speed setting for horizontal motor was missing

Version     : 1.24
Date        : 26.06.2008
Revised by  : Herbert Aden
Description : integrated magazine trolley functionality

*/

#define _PAPIERENTFERNUNG_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "glob_var.h"
#include "auxfunc.h"
#include "in_out.h"
#include "motorfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define TROLLEYCLOSED 1800

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
BOOL  PapRemHorizontalCmd(const char *Cmd, const BOOL UseVal, const DINT Value);
BOOL  PapRemHorizontalOk(DINT Pos, DINT Dev );

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
_GLOBAL   BOOL                LastPlateIsReady;
_GLOBAL   BOOL                WaitForLastPlate;
_GLOBAL   BOOL                AdjustFailed;
_GLOBAL   BOOL                AlternatingTrolleys;
_GLOBAL   BOOL                DisablePaperRemove;

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    USINT               PaperRemoveStep;
_LOCAL    BOOL                AUTOSTART;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    BOOL                DisableReady;
static    DINT                PaperReleasePosition;
static    USINT               GripCounter;
static    USINT               PaperRemoveReturnStep;  /*aux var for returning from "subroutine" */
static    TON_10ms_typ        PaperRemoveTimer;
static    USINT               RetryCounter;
static    UINT                StartUpStep;
static    USINT               MotStep;       /* holds current step of fct MoveToAbsPosition */

static    TON_10ms_typ        TimeoutTimer;
static    TON_10ms_typ        CoverLockTimer;
static    TON_10ms_typ        StartupTimer;
static    BOOL                PapRem1Moved,PapRem2Moved;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/





/****************************************************************************/
/* liefert die Anzahl unbelichteter Platten im System */
/* jedoch ohne die, die ausgerichtet auf Belichtung wartet*/
/****************************************************************************/
USINT GetPlatesInSystem()
{
	int i=0;
	if (PlateAtFeeder.present) i++;
	if (AdjustReady) i++;
	return i;
}


/****************************************************************************/
/* INIT function */
/****************************************************************************/
_INIT void init(void)
{
	PapRem1Moved = FALSE;
	PapRem2Moved = FALSE;
	MotStep = 0;
	LastPlateIsReady = FALSE;
	WaitForLastPlate = FALSE;
	DisableReady = FALSE;
	StartUpStep = 0;
	PaperRemoveStep		= 0;
	PaperRemoveStart	= FALSE;
	PaperRemoveTimer.IN	= FALSE;
	PaperRemoveTimer.PT	= 10; /*just to have any value in there...*/
	TimeoutTimer.IN = FALSE;
	TimeoutTimer.PT = 1;
	RetryCounter = 0;
	CoverLockTimer.IN = FALSE;
	StartupTimer.IN = FALSE;
}


/****************************************************************************/
/* cyclic function */
/****************************************************************************/
_CYCLIC void cyclic(void)
{
	SequenceSteps[0] = PaperRemoveStep;

	UnexposedPlatesInSystem = GetPlatesInSystem();

/* motors no reference? -> End*/

	if( !Motors[FEEDER_VERTICAL].ReferenceOk || !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk
		|| ( (!Motors[PAPERREMOVE_VERTICAL].ReferenceOk || !Motors[PAPERREMOVE_VERTICAL2].ReferenceOk) && !OpenTrolleyStart && !CloseTrolleyStart) )
	{
		PaperRemoveStart = FALSE;
		PaperRemoveStep = 0;
		START = FALSE;
		return;
	}

	TON_10ms(&CoverLockTimer);
	TON_10ms(&StartupTimer);

	if(ResetStartupSeq)
	{
		ResetStartupSeq = FALSE;
		StartUpStep = 0;
	}

	switch (StartUpStep)
	{
		case 0:
		{
			StartupTimer.IN = FALSE;
			if(START && !PlateTakingStart && !PaperRemoveStart && !OpenTrolleyStart && !CloseTrolleyStart)
			{
				if(ManualMode)
				{
					StartUpStep = 2;
					break;
				}
			/*no automatic trolley detect AND no trolley in machine selected->error*/
				if( !GlobalParameter.AutomaticTrolleyOpenClose && !GlobalParameter.TrolleyLeft && !GlobalParameter.TrolleyRight)
				{
					ShowMessage(12,13,14,CAUTION,OKONLY,FALSE);
					StartUpStep = 1;
				}
				else
				{
					if(PanoramaAdapter && PlateType == 0)
					{
						ShowMessage(37,38,0,CAUTION,OKONLY,FALSE);
						StartUpStep = 1;
					}
					else
						StartUpStep = 2;
				}
			}
			break;
		}
/*wait for input */
		case 1:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				AbfrageCancel = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			break;
		}

		case 2:
		{
			if(!TCPConnected && !GlobalParameter.TBSimulation)
			{
				ShowMessage(3,4,0,REQUEST,OKCANCEL,FALSE);
				StartUpStep = 3;
				StartupTimer.IN = FALSE;
			}
			else
				StartUpStep = 30;
			break;
		}
/*wait for input */
		case 3:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				StartUpStep = 4;
				StartupTimer.IN = TRUE;
				StartupTimer.PT = 50;
			}
			if(AbfrageCancel)
			{
				StartupTimer.IN = FALSE;
				AbfrageCancel = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			break;
		}

		case 4:
		{
			if(StartupTimer.Q)
			{
				StartupTimer.IN = FALSE;
				StartUpStep = 30;
			}

			break;
		}


		case 5:
		{

			if(ManualMode)
			{
				StartUpStep = 15;
				break;
			}


/*no automatic open/close: just go on with starting paperremove/platetaking*/
			if( !GlobalParameter.AutomaticTrolleyOpenClose)
			{
	/*wait, til query picture is not active*/
				if(wBildAktuell != 41)
				{
					ShowMessage(8,9,10,INFO,OKCANCEL,FALSE);
					StartUpStep = 12;
				}
				break;
			}

			if(START)
			{
				if( !TrolleyOpen )
				{
					OpenTrolleyStart = TRUE;
					StartupTimer.IN = TRUE;
					StartupTimer.PT = 6000;
					AlarmBitField[13] = FALSE;
					AlarmBitField[14] = FALSE;
					AlarmBitField[15] = FALSE;
				}
				StartUpStep = 10;
			}
			else
				StartUpStep = 0;
			break;
		}
		case 10:
		{
			if(TrolleyOpen)
			{
				StartUpStep = 15;
				StartupTimer.IN = FALSE;
			}

			if(StartupTimer.Q)
			{
				StartupTimer.IN = FALSE;
				START = 0;
				StartUpStep = 0;
				break;
			}

			if( AlarmBitField[13] || AlarmBitField[14] )
			{
				START = FALSE;
				StartUpStep = 11;
				StartupTimer.IN = FALSE;
			}

			break;
		}

/*wait for input */
		case 11:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			if(AbfrageCancel)
			{
				AbfrageCancel = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
/*for safety:*/
/*not in ok/cancel pic: end startup seq*/
			if(wBildAktuell != 41)
			{
				AbfrageCancel = FALSE;
				AbfrageOK = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			break;
		}

/*wait for input */
		case 12:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				StartUpStep = 15;
				AlarmBitField[15] = FALSE;
			}
			if(AbfrageCancel)
			{
				AbfrageCancel = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			break;
		}

/*Start actual machine sequence (at least once!)*/
		case 15:
		{
/*HA 01.10.03 V1.64 use AlternatingTrolleys instead of GlobalParameter.AlternatingTrolleys during*/
/*plate taking to avoid resetting of the Global param*/
			AlternatingTrolleys = GlobalParameter.AlternatingTrolleys;

			AlarmBitField[15] = FALSE;
/*HA 04.04.03 */
			AlarmBitField[25] = FALSE;
			AlarmBitField[28] = FALSE;
			AlarmBitField[29] = FALSE;
/*HA 25.08.04 V1.83 new ultrasonic Paper detection sensor at shuttle */
/*HA 19.11.04 V1.93 merged 1.83 and 1.92 */
			AlarmBitField[33] = FALSE;
/*V2.59*/
			AlarmBitField[34] = FALSE;

/*HA 17.02.04 V1.74B */
			AdjustFailed = FALSE;
			AlarmBitField[26] = FALSE;
			DontCloseTrolley = FALSE;


/*reset empty flags of inserted trolleys*/
			if( GlobalParameter.TrolleyLeft!=0 )
			{
				Trolleys[GlobalParameter.TrolleyLeft].EmptyLeft = 0;
				Trolleys[GlobalParameter.TrolleyLeft].EmptyRight = 0;
			}

			if( GlobalParameter.TrolleyRight!=0 )
			{
				Trolleys[GlobalParameter.TrolleyRight].EmptyLeft = FALSE;
				Trolleys[GlobalParameter.TrolleyRight].EmptyRight = FALSE;
			}

			AUTO = TRUE;
			AUTOSTART = 0;
/*HA 24.06.03 V1.10 Stack detection on next plate taking*/
			StackDetect[0] = TRUE;
			StackDetect[1] = TRUE;
			StackDetect[2] = TRUE;
			StartUpStep = 20;
			break;
		}

		case 20:
		{
			if(ManualMode)
			{
				if(START)
				{
					PlateToDo.PlateType = PlateType;
					if((PlateToDo.PlateType > 0) && (PlateToDo.PlateType < MAXPLATETYPES) )
					{
						if(PlateToDo.present || GlobalParameter.TBSimulation)
						{
							memcpy(&PlateParameter,&PlateTypes[PlateToDo.PlateType],sizeof(PlateParameter));
							if( !AdjustReady
							 && !AdjustStart
							 && !ExposeStart
							 && !DeloadStart
							 && !PlateAtAdjustedPosition.present)
								ManualLoadingStart = TRUE;

							if(GlobalParameter.TBSimulation)
							{
								START = FALSE;
							}

						}
					}
				}
				else
				{
					StartUpStep = 27;
				}
				break;
			}
/**************************************************************/
/**************************************************************/

/*start command reset? */
			if ( !START )
			{
/*wait for paper remove and plate taking ready, then end*/
				if(!PlateTakingStart && !PaperRemoveStart)
				{
					StartUpStep = 25;
					StartupTimer.IN = FALSE;
				}
			}

/*HA 01.10.03 V1.64 new Param for Panoadapter independent from Trolley*/
			if (PanoramaAdapter)
			{
				CurrentData.PlateType = PlateType;
				CurrentData.CurrentStack =  0;
				CurrentData.PlateStackInTrolley = 0;
				CurrentData.FeederVerticalPos = FeederParameter.PanoramaAdapter;
				StackDetect[CurrentData.CurrentStack] = 0;
				CurrentData.FeederHorizontalPos = FeederParameter.HorPositionPano;
				CurrentData.PaperRemovePos = PaperRemoveParameter.HorPositionPano;
				AUTOSTART = 1;
			}
			else
			{
				if(PlateToDo.present || GlobalParameter.TBSimulation)
					AUTOSTART = TRUE;
				else
					AUTOSTART = FALSE;
			}

			if( START && AUTOSTART)
			{
				if(!PaperRemoveReady && GlobalParameter.PaperRemoveEnabled && !DisablePaperRemove)
					PaperRemoveStart = 1;
				if( !PlateAtFeeder.present )
					PlateTakingStart = 1;
			}
			break;
		}

/*End Sequence with closing trolley*/
		case 25:
		{
			if ( !GlobalParameter.AutomaticTrolleyOpenClose || DontCloseTrolley)
				StartUpStep = 27;
			else
			{
				if ( TrolleyOpen )
				{
					StartupTimer.IN = TRUE;
					StartupTimer.PT = 6000;
					CloseTrolleyStart = 1;
					StartUpStep = 26;
				}
			}
			break;
		}
		case 26:
		{
			if(StartupTimer.Q)
			{
				StartupTimer.IN = FALSE;
				StartUpStep = 0;
				START = 0;
				break;
			}

			if ( !TrolleyOpen && !CloseTrolleyStart)
			{
				StartupTimer.IN = FALSE;
				StartUpStep = 27;
			}

			break;
		}

		case 27:
		{
			DontCloseTrolley = FALSE;
		/*Wait, til all sequences finished*/
			if (		PaperRemoveStart
				||	PlateTakingStart
				||	AdjustStart
				||	DeloadStart
				||	ExposeStart
				||	PlateAtFeeder.present
				||	ManualLoadingStart
				||  AdjustReady )
				break;

			AUTO = 0;
			StartUpStep = 0;
			break;
		}

/****************************************************************************/
/* HA 26.06.03 V1.11 */
/* STEPS FOR LOCKING/CHECKING COVER*/
/****************************************************************************/

/*check door sensors*/
		case 30:
		{
			if( DoorsOK || (UserLevel >= 1))
			{
				StartUpStep = 34;
				CoverLockTimer.IN = FALSE;
				StartupTimer.IN = FALSE;
				break;
			}
			else
			{
				ShowMessage(43,44,0,CAUTION,OKONLY,FALSE);

				StartUpStep = 36;
			}
			break;
		}

		case 34:
		{
/*timeout*/
			if( StartupTimer.Q )
			{
				CoverLockTimer.IN = FALSE;
				StartupTimer.IN = FALSE;
				StartUpStep = 35;
				Out_CoverLockOn = OFF;
				break;
			}
/*OK signal in time, or servicelevel 2*/
			if( CoverLockTimer.Q || (UserLevel >= 1))
			{
				CoverLockTimer.IN = FALSE;
				StartupTimer.IN = FALSE;
				StartUpStep = 5;
				break;
			}
			Out_CoverLockOn = ON;
			CoverLockTimer.IN = CoverLockOK && DoorsOK;
			CoverLockTimer.PT = 200;
			StartupTimer.IN = TRUE;
			StartupTimer.PT = 1000;
			break;
		}

		case 35:
		{
			ShowMessage(41,42,0,CAUTION,OKONLY,FALSE);
			StartUpStep = 36;
			break;
		}
/*wait for input */
		case 36:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				START = FALSE;
				StartUpStep = 0;
			}
			break;
		}

	} /*switch*/

/*work without paper remove*/
	if(!GlobalParameter.PaperRemoveEnabled && !OpenTrolleyStart && !CloseTrolleyStart)
	{
		PaperRemoveReady = TRUE;
		return;
	}

	TON_10ms(&PaperRemoveTimer);
	TON_10ms(&TimeoutTimer);

/*safety*/
	if( !PaperRemoveStart )
	{
		PaperRemoveStep = 0;
		(void) MoveToAbsPosition(0,0L,0L,&MotStep);
	}

/****************************************************************************/
/* PAPER REMOVE SEQUENCE*/
/****************************************************************************/

	switch (PaperRemoveStep)
	{
		case 0:
		{

			if( !PaperRemoveStart
			 || OpenTrolleyStart
			 || CloseTrolleyStart
			 || ManualMode
			 || (!TrolleyOpen && GlobalParameter.AutomaticTrolleyOpenClose))
				break;

/*HA 01.10.03 V1.64 new Param for Panoadapter independent from Trolley*/
			if (PanoramaAdapter)
			{
				CurrentData.PlateType = PlateType;
				CurrentData.CurrentStack =  0;
				CurrentData.PlateStackInTrolley = 0;
				CurrentData.FeederVerticalPos = FeederParameter.PanoramaAdapter;
				StackDetect[CurrentData.CurrentStack] = 0;
/*HA 01.10.03 V1.64 new Param for Panoadapter independent from Trolley*/
				CurrentData.FeederHorizontalPos = FeederParameter.HorPositionPano;
				CurrentData.PaperRemovePos = PaperRemoveParameter.HorPositionPano;
			}
			else
			{
				CurrentData.TrolleyNumber = GlobalParameter.TrolleyLeft;
				CurrentData.PlateType = Trolleys[GlobalParameter.TrolleyLeft].PlateType;
				CurrentData.FeederHorizontalPos = Trolleys[CurrentData.TrolleyNumber].TakePlateLeft;
				CurrentData.PaperRemovePos = Trolleys[CurrentData.TrolleyNumber].TakePaperLeft;
				CurrentData.CurrentStack = LEFT;
			}
			PaperRemoveStep = 181;

			break;
		}


		case 181:
		{
/*Paper remove vert. UP? (should be the case...)*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.Up,20) )
				PaperRemoveStep = 5;
			else
				PaperRemoveStep = 1;
/*at start: open paper grip*/
			Out_PaperGripOn[LEFT] = OFF;
			Out_PaperGripOn[RIGHT] = OFF;
			break;
		}

		case 1:
		{
/* else move up */
/**
  * can't use MoveToAbsPosition here because it's Paprem vertical: must be handled separately
  * because of 2 Paperremove arms
 **/
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up) )
				PaperRemoveStep = 2;
			break;
		}
		case 2:
		{
/* Start motion */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 3;
				TimeoutTimer.IN = FALSE;
			}
			break;
		}
		case 3:
		{
/*Wait for UP*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.Up,20L) )
			{
				PaperRemoveStep = 5;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 1;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;
			break;
		}
		case 5:
		{
/*V1.22 15.4.2008: step is obsolete: Just go ahead*/
			PaperRemoveStep = 10;
			break;
		}

		case 10:
		{
/*check, if Feeder is out of collision area (should be the case here)*/
			if( isPositionHigherThan(FEEDER_VERTICAL, labs(FeederParameter.VerticalPositions.EnablePaperRemove) + 50L) )
				break;

			PaperRemoveStep = 11;
			break;
		}

		case 11:
		{
/*Move Paper remove horizontically to paper taking pos.*/
			if( MoveToAbsPosition(PAPERREMOVE_HORIZONTAL,
			                      CurrentData.PaperRemovePos,
			                      PaperRemoveParameter.PaperRemoveSpeed,
			                      &MotStep) )
				PaperRemoveStep = 12;
			break;
		}
		case 12:
		{
/*send maxdowncurrent to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_PEAK_CURRENT,TRUE,PaperRemoveParameter.MaxCurrentDown) )
			{
				PaperRemoveStep = 13;
			}
			break;
		}
		case 13:
		{
/*send down speed to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_SPEED,TRUE,PaperRemoveParameter.PaperRemoveDownSpeed) )
			{
				PaperRemoveStep = 14;
				DisableReady = 0;
			}
			break;
		}
		case 14:
		{
/*Wait til Position is reached */
			if(isPositionOk(PAPERREMOVE_HORIZONTAL,
			                CurrentData.PaperRemovePos,
			                20L ) )
			{
				PaperRemoveStep = 15;
				RetryCounter=0;
				PapRem1Moved = FALSE;
				PapRem2Moved = FALSE;
				TimeoutTimer.IN = FALSE;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 10;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;

			break;
		}
		case 15:
		{
/* Paper remove down*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Down) )
			{
				PaperRemoveStep = 16;
			}
			break;
		}
		case 16:
		{
/* start motion */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 17;
			}
			break;
		}
		case 17:
		{
/*Wait til moving begins*/
			if( Motors[PAPERREMOVE_VERTICAL].Moving )
				PapRem1Moved = TRUE;

			if( Motors[PAPERREMOVE_VERTICAL2].Moving )
				PapRem2Moved = TRUE;

			if(PlateToDo.PlateConfig == LEFT)
				PapRem2Moved = TRUE;
			if(PlateToDo.PlateConfig == RIGHT)
				PapRem1Moved = TRUE;

			if( PapRem1Moved && PapRem2Moved )
			{
				PaperRemoveStep = 20;
				RetryCounter=0;
				TimeoutTimer.IN = FALSE;
				PapRem1Moved = FALSE;
				PapRem2Moved = FALSE;
				break;
			}

/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 15;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 100;
			break;
		}

		case 20:
		{
/*Wait til moving ends (on platestack*/
			if( !Motors[PAPERREMOVE_VERTICAL].Moving && !Motors[PAPERREMOVE_VERTICAL2].Moving)
			{
/* sonderschritte für "ruckeln"*/
				PaperRemoveStep = 120;
				PaperRemoveTimer.IN = FALSE;
			}
			break;
		}

		case 21:
		{
			if (!PlateTransportSim)
			{
				Out_PaperGripOn[LEFT] = ON;
				Out_PaperGripOn[RIGHT] = ON;
			}

			PaperRemoveTimer.IN = TRUE;
			PaperRemoveTimer.PT = PaperRemoveParameter.GripOnTime;

/* stop motion */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_STOP,FALSE,0L) )
			{
				PaperRemoveStep = 22;
			}
			break;
		}

		case 22:
		{
/*send maxupcurrent to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_PEAK_CURRENT,TRUE,PaperRemoveParameter.MaxCurrentUp) )
			{
				PaperRemoveStep = 23;
			}
			break;
		}
		case 23:
		{
/*send maxupcurrent to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_CONT_CURRENT,TRUE,PaperRemoveParameter.MaxCurrentUp) )
			{
				PaperRemoveStep = 24;
			}
			break;
		}
		case 24:
		{
/*send up speed to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_SPEED,TRUE,PaperRemoveParameter.PaperRemoveUpSpeed) )
			{
				PaperRemoveStep = 25;
			}
			break;
		}

		case 25:
		{
/*wait a little to give gripper time to close*/
			if(	PaperRemoveTimer.Q )
			{
				PaperRemoveStep = 26;
				PaperRemoveTimer.IN = FALSE;
			}
			break;
		}
		case 26:
		{
/* move up */
/*HA 12.09.03 V1.60 move last 500 increments slowly*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up+700) )
			{
				PaperRemoveStep = 27;
			}
			break;
		}

		case 27:
		{
/* start motion  */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 30;
			}
			break;
		}
		case 30:
		{
/*Wait for Pos. UP*/
/*HA 12.09.03 V1.60 move last 500 increments slowly*/
			if( isPapRemVerticalOk(PlateToDo.PlateConfig,PaperRemoveParameter.VerticalPositions.Up+700,0L) )
			{
				PaperRemoveStep = 80;
				PaperRemoveReturnStep = 31;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 26;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;
			break;
		}
		case 31:
		{
/* Move Paper remove horizontically to paper release pos. */
/* decide, where the release position is (depends on platesize) */
			PaperReleasePosition = PaperRemoveParameter.HorizontalPositions.PaperReleaseSingle;

			if( CurrentData.PlateType > 0)
			{
				if( PlateTypes[CurrentData.PlateType].Length > 400 )
					PaperReleasePosition = PaperRemoveParameter.HorizontalPositions.PaperReleasePanorama;
				else
					PaperReleasePosition = PaperRemoveParameter.HorizontalPositions.PaperReleaseSingle;
			}

			if( MoveToAbsPosition(PAPERREMOVE_HORIZONTAL,
			                      PaperReleasePosition,
			                      PaperRemoveParameter.PaperRemoveSpeed,
			                      &MotStep) )
				PaperRemoveStep = 33;
			break;
		}
		case 33:
		{
			if( !DisableReady )
				PaperRemoveReady = TRUE;
/*Wait til Position is reached */
			if(isPositionOk(PAPERREMOVE_HORIZONTAL,
			                PaperReleasePosition,
			                20L ) )
			{
				PaperRemoveStep = 40;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				PapRem1Moved = FALSE;
				PapRem2Moved = FALSE;
				break;
			}

/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 31;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;
			break;
		}


		case 40:
		{
/* Paper remove down*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.PaperReleasePosVertical) )
			{
				PaperRemoveStep = 41;
			}
			break;
		}
/*HA 12.09.03 V1.60 set speed */
		case 41:
		{
/*send down speed to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_SPEED,TRUE,PaperRemoveParameter.PaperRemoveDownSpeed) )
			{
				PaperRemoveStep = 42;
			}
			break;
		}
		case 42:
		{
/* start motion  */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 44;
			}
			break;
		}
		case 44:
		{
/*Wait til moving begins*/
			if( Motors[PAPERREMOVE_VERTICAL].Moving )
				PapRem1Moved = TRUE;

			if( Motors[PAPERREMOVE_VERTICAL2].Moving )
				PapRem2Moved = TRUE;

			if(PlateToDo.PlateConfig == LEFT)
				PapRem2Moved = TRUE;
			if(PlateToDo.PlateConfig == RIGHT)
				PapRem1Moved = TRUE;

			if( PapRem1Moved && PapRem2Moved )
			{
				PaperRemoveStep = 46;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				PapRem1Moved = FALSE;
				PapRem2Moved = FALSE;
				break;
			}

/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 40;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 100;
			break;
		}
		case 46:
		{
/*Wait til moving ends */
			if( !Motors[PAPERREMOVE_VERTICAL].Moving && !Motors[PAPERREMOVE_VERTICAL2].Moving)
			{
				PaperRemoveStep = 47;
				PaperRemoveTimer.IN = TRUE;
			}
			break;
		}

		case 47:
		{
			Out_PaperGripOn[LEFT] = OFF;
			Out_PaperGripOn[RIGHT] = OFF;
/*wait a little to give gripper time to open*/
			PaperRemoveTimer.IN = TRUE;
			PaperRemoveTimer.PT = PaperRemoveParameter.GripOffTime;
			if(	PaperRemoveTimer.Q )
			{
				PaperRemoveStep = 48;
				PaperRemoveTimer.IN = FALSE;
			}
			break;
		}

		case 48:
		{
/* move up */
/* move the last 700 incr slowly */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up+700) )
			{
				PaperRemoveStep = 49;
			}
			break;
		}

/* set speed */
		case 49:
		{
/* send up speed to vertical motor */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_SPEED,TRUE,PaperRemoveParameter.PaperRemoveUpSpeed) )
			{
				PaperRemoveStep = 50;
			}
			break;
		}

		case 50:
		{
/* start motion  */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 52;
			}
			break;
		}
		case 52:
		{
/* Wait for Pos. UP */
/* move the last 700 incr slowly */
			if( isPapRemVerticalOk(PlateToDo.PlateConfig,PaperRemoveParameter.VerticalPositions.Up+700,0L) )
			{
				PaperRemoveStep = 80;
				PaperRemoveReturnStep = 60;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				break;
			}
/* Timeout check */
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 48;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;
			break;
		}

		case 60:
		{
/*Move Paper remove horizontically to wait pos.*/

/* V1.95 Fehlplatte: nicht in Wartepos fahren, sondern seq hier beenden*/
			if (DisablePaperRemove)
			{
				PaperRemoveStep = 0;
				PaperRemoveStart = FALSE;
				break;
			}

			if( MoveToAbsPosition(PAPERREMOVE_HORIZONTAL,
			                      PaperRemoveParameter.HorizontalPositions.EnableFeeder,
			                      PaperRemoveParameter.PaperRemoveSpeed,
			                      &MotStep) )
			{
				PaperRemoveStep = 70;
			}
			break;
		}
		case 70:
		{
/*Moving or already there*/
			if(isPositionOk(PAPERREMOVE_HORIZONTAL,
			                PaperRemoveParameter.HorizontalPositions.EnableFeeder,
			                0L )
				|| Motors[PAPERREMOVE_HORIZONTAL].Moving )
			{
				PaperRemoveStep = 0;
				PaperRemoveStart = FALSE;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 60;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 100;
			break;
		}

/************************************************************/
/* move the last 700 incr slowly */
		case 80:
		{
/*send up speed/6 to vertical motor*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_SPEED,TRUE,PaperRemoveParameter.PaperRemoveUpSpeed/8) )
			{
				PaperRemoveStep = 85;
			}
			break;
		}

		case 85:
		{
/* move up */
/*HA 12.09.03 V1.60 move last 700 increments slowly*/
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up) )
			{
				PaperRemoveStep = 87;
			}
			break;
		}
		case 87:
		{
/* start motion  */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_START_MOTION,FALSE,0L) )
			{
				PaperRemoveStep = 90;
			}
			break;
		}

		case 90:
		{
/*Wait for Pos. UP*/
			if( isPapRemVerticalOk(PlateToDo.PlateConfig,PaperRemoveParameter.VerticalPositions.Up,0L) )
			{
				PaperRemoveStep = PaperRemoveReturnStep;
				RetryCounter = 0;
				TimeoutTimer.IN = FALSE;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PaperRemoveStep = 85;
				RetryCounter++;
				TimeoutTimer.IN = FALSE;
				break;
			}
			TimeoutTimer.IN = TRUE;
			TimeoutTimer.PT = 500;
			break;
		}



/************************************************************/
/*Sonderschritte für Ruckeln*/
		case 120:
		{
/*Move Paper remove horizontically a bit*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL, CMD_RELATIVE_POS, TRUE, 500) )
			{
				PaperRemoveStep = 125;
			}
			break;
		}
		case 125:
		{
/* start motion  */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL, CMD_START_MOTION, FALSE, 0L) )
			{
				PaperRemoveStep = 130;
				PaperRemoveTimer.IN = TRUE;
				PaperRemoveTimer.PT = 20;
			}
			break;

		}


		case 130:
		{
			if(	PaperRemoveTimer.Q )
			{
/*HA 09.05.03 V1.12*/
/*several grip movements to make paper removal more reliable*/
				if(PaperRemoveParameter.PaperGripCycles == 0)
					PaperRemoveStep = 21;
				else
				{
					PaperRemoveStep = 140;
					GripCounter = 0;
				}
				PaperRemoveTimer.IN = FALSE;
			}
			break;
		}


/*several grip movements to make paper removal more reliable*/
		case 140:
		{
/* stop movement */
			if( PapRemVerticalCmd(PlateToDo.PlateConfig,CMD_STOP,FALSE,0L) )
			{
				PaperRemoveStep = 142;
			}
			break;
		}

		case 142:
		{
			if (!PlateTransportSim)
			{
				Out_PaperGripOn[LEFT] = ON;
				Out_PaperGripOn[RIGHT] = ON;
			}

			if(PaperRemoveTimer.Q)
			{
				GripCounter++;
				if(GripCounter > PaperRemoveParameter.PaperGripCycles)
					PaperRemoveStep = 22;
				else
				{
					PaperRemoveTimer.IN = FALSE;
					PaperRemoveStep = 145;
				}

				break;
			}

			PaperRemoveTimer.IN = TRUE;
			PaperRemoveTimer.PT = PaperRemoveParameter.GripOnTime;

			break;
		}


		case 145:
		{
			Out_PaperGripOn[LEFT] = OFF;
			Out_PaperGripOn[RIGHT] = OFF;

			if(PaperRemoveTimer.Q)
			{
				PaperRemoveTimer.IN = FALSE;
				PaperRemoveStep = 142;
				break;
			}
			PaperRemoveTimer.IN = TRUE;
			PaperRemoveTimer.PT = PaperRemoveParameter.GripOffTime;
			break;
		}

	}	/*switch*/
}

/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


