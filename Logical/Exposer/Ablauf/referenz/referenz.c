#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Referenzfahrt */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.00		25.09.02	erste Implementation					HA		*/
/*	*/
/*		*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"
#include "in_out.h"
#include "standard.h"
#include "motorfunc.h"
#include "auxfunc.h"

#define ROT 0x002D
#define GRUEN 0x000A
#define GRAU 0x0007
#define GRAU59 0x003B
#define SCHWARZ 0x0000
#define WEISS 0x1010

#define RAHMEN_ROT 0x2D00
#define RAHMEN_SCHWARZ 0x0000
#define RAHMEN_GRAU 0x0700

_LOCAL	USINT	ReferenceStep;
_LOCAL TON_10ms_typ ReferenceTimer,CoverLockTimer;
TP_10ms_typ ColorTimer;


_LOCAL	UINT	FeederVerticalColor;
_LOCAL	UINT	FeederHorizontalColor;
_LOCAL	UINT	PaperRemoveVerticalColor;
_LOCAL	UINT	PaperRemoveHorizontalColor;
_LOCAL	UINT	DeloaderHorizontalColor;
_LOCAL	UINT	DeloaderTurnColor;
_LOCAL	UINT	FUColor;
BOOL	ColorToggle;
BOOL	DeloaderReady;
_GLOBAL	UINT	ConveyorBeltOn;
UDINT OriginalDeloaderRefSpeed;
_GLOBAL USINT	ClearLine;
_GLOBAL BOOL	DontCloseTrolley;
/*V1.95 sequenzen disablen, solange Fehlplatte abgeworfen wird*/
_GLOBAL BOOL	DisablePaperRemove;
_GLOBAL UINT MainPicNumber;

static    USINT               PinsStep;       /* holds current step of fct AdjPins */
static    TON_10ms_typ        PinsTimer;

_INIT void Init(void)
{
	PinsStep = 0;
	PinsTimer.IN = FALSE;
	ColorToggle = 0;
	ColorTimer.IN = 1;
	ReferenceStep = 0;
	ReferenceStart = 0;
	ReferenceTimer.IN = 0;
	ReferenceTimer.PT = 1;
	CoverLockTimer.IN = 0;
}


_CYCLIC void Cyclic(void)
{

	SequenceSteps[8] = ReferenceStep;

	TON_10ms(&PinsTimer);

	TON_10ms(&ReferenceTimer);
	TON_10ms(&CoverLockTimer);

	if (!ReferenceStart) ReferenceStep = 0;

	switch (ReferenceStep)
	{
		case 0:
		{

/* wait for start command */
			if (!ReferenceStart) break;

			if(	Motors[FEEDER_VERTICAL].Error
			||	(Motors[FEEDER_HORIZONTAL].Error && GlobalParameter.FlexibleFeeder)
			||	Motors[PAPERREMOVE_VERTICAL].Error
			||	Motors[PAPERREMOVE_VERTICAL2].Error
			||	Motors[PAPERREMOVE_HORIZONTAL].Error
			||	(Motors[DELOADER_HORIZONTAL].Error && GlobalParameter.DeloaderTurnStation>0)
			||	(Motors[DELOADER_TURN].Error && GlobalParameter.DeloaderTurnStation==2) )
			{
				ReferenceStart = 0;
				break;
			}
			ReferenceStep = 100;
			break;
		}

		case 2:
		{

			START = 0;
			PlateTakingStart = 0;
			PaperRemoveStart = 0;
			AdjustStart = 0;
			DeloadStart = 0;
			ExposeStart = 0;
			OpenTrolleyStart = 0;
			CloseTrolleyStart = 0;

			ClearStation(&PlateAtFeeder);
			AdjustReady = 0;
			ExposureReady = 0;
			DeloadReady = 0;
			ClearLine = 0;
			PlateTransportSim = 0;

			Motors[FEEDER_VERTICAL].ReferenceOk = 0;
			Motors[FEEDER_HORIZONTAL].ReferenceOk = 0;
			Motors[PAPERREMOVE_VERTICAL].ReferenceOk = 0;
			Motors[PAPERREMOVE_VERTICAL2].ReferenceOk = 0;
			Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk = 0;
			Motors[DELOADER_HORIZONTAL].ReferenceOk = 0;
			Motors[DELOADER_TURN].ReferenceOk = 0;
			DeloaderReady = 0;
			ReferenceStep = 5;

			break;
		}
		case 5:
		{
			int i;
/*Feeder vacuum off and Feeder vert. reference*/
			for(i=0;i<5;i++)
				Out_FeederVacuumOn[i] = OFF;

			SwitchAdjustTableVacuum(0,ALL,OFF);
			AdjustBlowairOn = 0;
			Out_AdjustSuckerVacuumOn[0] = OFF;
			Out_AdjustSuckerVacuumOn[1] = OFF;
			Out_AdjustSuckerVacuumOn[2] = OFF;
			Out_AdjustSignalCross = OFF;
			Out_LiftingPinsUp = OFF;
			Out_LiftingPinsDown = ON;

			Motors[FEEDER_VERTICAL].StartRef = 1;

/*Deloader Horiz. pre reference very slowly*/
			if(GlobalParameter.DeloaderTurnStation>0)
			{
				Motors[DELOADER_HORIZONTAL].StartRef = 1;
/*HA 02.08.04 V1.80 bugfix: if no turning motor, do not set slow speed*/
				if (GlobalParameter.DeloaderTurnStation==2)
				{
					OriginalDeloaderRefSpeed = Motors[DELOADER_HORIZONTAL].Parameter.ReferenceSpeed;
					Motors[DELOADER_HORIZONTAL].Parameter.ReferenceSpeed = Motors[DELOADER_HORIZONTAL].Parameter.ReferenceSpeed / 2;
				}
			}
			ReferenceStep = 7;
			break;
		}

		case 7:
		{
			if(AdjPins(PINS_DOWN,&PinsStep,&PinsTimer))
				ReferenceStep = 10;
			break;
		}

		case 10:
		{
/*wait for Feeder vert. ref ready, then Feeder Horiz. ref*/

/*if deloader hor is ready, start turn*/
			if(GlobalParameter.DeloaderTurnStation==2)
			{
				if(Motors[DELOADER_HORIZONTAL].ReferenceOk && !Motors[DELOADER_TURN].ReferenceOk)
				{
					Motors[DELOADER_TURN].StartRef = 1;
				}
			}

			if( Motors[FEEDER_VERTICAL].ReferenceOk )
			{
				Out_FeederHorActive = OFF;
				Out_AdjustSuckerActive[0] = OFF;
				Out_AdjustSuckerActive[1] = OFF;
				Out_AdjustSuckerActive[2] = OFF;
				if(GlobalParameter.FlexibleFeeder)
					Motors[FEEDER_HORIZONTAL].StartRef = 1;

				Motors[PAPERREMOVE_VERTICAL].StartRef = 1;
				ReferenceStep = 11;
				break;
			}
			break;
		}

		case 11:
		{
/*X ok? -> start reference*/
			if( !FU.Error && !FU.CANError && FU.ENPO)
			{
				FU.cmd_Ref = 1;
/*wait for reference to start*/
				if(!FU.RefOk)
					ReferenceStep = 12;
			}
			else
				ReferenceStep = 12;
			break;
		}

		case 12:
		{
/*both motors ref ok? -> go on*/
			if(Motors[PAPERREMOVE_VERTICAL].ReferenceOk && Motors[PAPERREMOVE_VERTICAL2].ReferenceOk)
			{
				ReferenceStep = 15;
				break;
			}
/*wait for 1st motor, then start 2nd to avoid current peak*/
			if(Motors[PAPERREMOVE_VERTICAL].ReferenceOk)
			{
				Motors[PAPERREMOVE_VERTICAL2].StartRef = 1;
			}
			break;
		}

		case 15:
		{
/*wait for Feeder Horiz ref  ready, then paper remove vert ref */
/*if deloader hor is ready, start turn*/
			if(GlobalParameter.DeloaderTurnStation==2)
			{
				if(Motors[DELOADER_HORIZONTAL].ReferenceOk && !Motors[DELOADER_TURN].ReferenceOk)
				{
					Motors[DELOADER_TURN].StartRef = 1;
				}
			}
/*if Paperremove vertical ready start horiz*/
			if( Motors[PAPERREMOVE_VERTICAL].ReferenceOk && Motors[PAPERREMOVE_VERTICAL2].ReferenceOk && !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk )
			{
				Motors[PAPERREMOVE_HORIZONTAL].StartRef = 1;
			}

			if( (Motors[FEEDER_HORIZONTAL].ReferenceOk || !GlobalParameter.FlexibleFeeder) )
			{

				ReferenceStep = 20;
				break;
			}
			break;
		}
		case 20:
		{
/*if deloader hor is ready, start turn*/
			if(GlobalParameter.DeloaderTurnStation==2)
			{
				if(Motors[DELOADER_HORIZONTAL].ReferenceOk && !Motors[DELOADER_TURN].ReferenceOk)
				{
					Motors[DELOADER_TURN].StartRef = 1;
				}
			}

/*wait for Paperrem vert ref ready, then paper remove horiz. ref */
			if( Motors[PAPERREMOVE_VERTICAL].ReferenceOk && Motors[PAPERREMOVE_VERTICAL2].ReferenceOk )
			{
				if( !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk )
					Motors[PAPERREMOVE_HORIZONTAL].StartRef = 1;
				ReferenceStep = 22;
				break;
			}
			break;
		}
		case 22:
		{

/*if deloader hor is ready, start turn*/
			if(GlobalParameter.DeloaderTurnStation==2)
			{
				if(Motors[DELOADER_HORIZONTAL].ReferenceOk && !Motors[DELOADER_TURN].ReferenceOk)
				{
					Motors[DELOADER_TURN].StartRef = 1;
				}
			}

/*if deloader turn is ready, start hor again*/
			if(GlobalParameter.DeloaderTurnStation==2)
			{
				if(Motors[DELOADER_TURN].ReferenceOk)
				{
					Motors[DELOADER_HORIZONTAL].Parameter.ReferenceSpeed = OriginalDeloaderRefSpeed;
					Motors[DELOADER_HORIZONTAL].ReferenceOk = 0;
					Motors[DELOADER_HORIZONTAL].StartRef = 1;
					ReferenceStep = 23;
				}
			}
			else
				ReferenceStep = 23;

			break;
		}

		case 23:
		{
/*wait for Paperremove horiz. ref ready*/
			if(Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk)
			{
				Out_PaperGripOn[LEFT] = OFF;
				Out_PaperGripOn[RIGHT] = OFF;
				ReferenceStep = 25;
			}
			break;
		}

		case 25:
		{

			switch (GlobalParameter.DeloaderTurnStation)
			{
				case 0:
				{
					ReferenceStep = 45;
					break;
				}
				case 1:
				{
					if (Motors[DELOADER_HORIZONTAL].ReferenceOk)
						ReferenceStep = 45;
					break;
				}
				case 2:
				{
					if (Motors[DELOADER_HORIZONTAL].ReferenceOk && Motors[DELOADER_TURN].ReferenceOk)
						ReferenceStep = 45;
					break;
				}
			}
			break;
		}
		case 45:
		{
/*X ok? -> check if ref ready*/
			if( !FU.Error && !FU.CANError && FU.ENPO)
			{
				if(FU.RefOk)
				{
					ReferenceStep = 46;
				}
			}
/*X not ok? ->ready*/
			else
			{
				ReferenceStep = 50;
			}
			break;
		}

/*HA 10.04.03 new steps 46 and 47 to move to axis parkpos after reference ok*/
/*move X to parkposition*/
		case 46:
		{
			FU.cmd_Park = 1;
			ReferenceStep = 47;
			break;
		}

/*wait for parkposition ready*/
		case 47:
		{
			if(FU.cmd_Park) break;
			ReferenceStep = 50;
			break;
		}

		case 50:
		{
/* Stop Conveyor Belt, Adjuster down */
			if(SendMotorCmd(CONVEYORBELT,"V0",0,0))
			{
				ReferenceStep = 57;
				ConveyorBeltOn = 0;
			}
			break;
		}
		case 57:
		{
			if(!Motors[CONVEYORBELT].Error)
				Motors[CONVEYORBELT].StartRef = 1;

			if( TCPConnected )
				BeamOFF = TRUE;

			ReferenceStep = 60;
			break;
		}
		case 60:
		{
			ClearStation(&PlateAtFeeder);
			AdjustReady = 0;
			ExposureReady = 0;
			DeloadReady = 0;
			PaperRemoveReady = 0;
			ReferenceStep = 0;
			ReferenceStart = 0;
			DontCloseTrolley = 0;
			DisablePaperRemove = 0;
			if(wBildNr == 26)
				wBildNeu = 1;
			break;
		}
/****************************************************************************/
/* HA 26.06.03 V1.11*/
/* SPECIAL STEPS FOR COVER LOCKING/CHECK */
/****************************************************************************/
		case 100:
		{
			if( DoorsOK || UserLevel>=1)
			{
				ReferenceStep = 104;
				CoverLockTimer.IN = 0;
				ReferenceTimer.IN = 0;
				break;
			}
			else
			{
				if(wBildAktuell != 41 )
					OrgBild = wBildAktuell;
				wBildNeu = 41;
				AbfrageOK = 0;
				AbfrageCancel = 0;
				IgnoreButtons = 0;
				OK_CancelButtonInv = 1;
				OK_ButtonInv = 0;
				AbfrageText1 = 43;
				AbfrageText2 = 44;
				AbfrageText3 = 0;
				AbfrageIcon = CAUTION;
				ReferenceStep = 106;
			}
			break;
		}

		case 104:
		{
/*timeout*/
			if( ReferenceTimer.Q )
			{
				CoverLockTimer.IN = 0;
				ReferenceTimer.IN = 0;
				ReferenceStep = 105;
				Out_CoverLockOn = 0;
				break;
			}
/*OK signal in time, or servicelevel 2*/
			if( CoverLockTimer.Q || UserLevel>=1 )
			{
				CoverLockTimer.IN = 0;
				ReferenceTimer.IN = 0;
				ReferenceStep = 2;
				break;
			}
			Out_CoverLockOn = 1;
			CoverLockTimer.IN = CoverLockOK && DoorsOK;
			CoverLockTimer.PT = 100;
			ReferenceTimer.IN = 1;
			ReferenceTimer.PT = 200;
			break;
		}

		case 105:
		{
				if(wBildAktuell != 41 )
					OrgBild = wBildAktuell;
				wBildNeu = 41;
				AbfrageOK = 0;
				AbfrageCancel = 0;
				IgnoreButtons = 0;
				OK_CancelButtonInv = 1;
				OK_ButtonInv = 0;
				AbfrageText1 = 41;
				AbfrageText2 = 42;
				AbfrageText3 = 0;
				AbfrageIcon = CAUTION;
				ReferenceStep = 106;
			break;
		}
/*wait for input */
		case 106:
		{
			if(AbfrageOK)
			{
				AbfrageOK = 0;
				ReferenceStep = 0;
				ReferenceStart = 0;
			}
			break;
		}


	} /*switch*/


/*colors for "LEDs"*/
	ColorTimer.PT = 50;

	if( ColorTimer.IN == 1 && ColorTimer.Q==1)
		ColorTimer.IN = 0;

	if( ColorTimer.IN == 0 && ColorTimer.Q==0)
	{
		ColorToggle = !ColorToggle;
		ColorTimer.IN = 1;
	}

	TP_10ms(&ColorTimer);


	if( Motors[FEEDER_VERTICAL].ReferenceOk)
	{
		FeederVerticalColor = SCHWARZ;
	}
	else
	{
		if(Motors[FEEDER_VERTICAL].Error)
			FeederVerticalColor = GRAU59;
		else
			if(Motors[FEEDER_VERTICAL].StartRef)
			{
				if(ColorToggle)
					FeederVerticalColor = SCHWARZ;
				else
					FeederVerticalColor = WEISS;
			}
			else /*no ref and ref not running*/
				FeederVerticalColor = WEISS;
	}

/*HA 20.01.04 V1.73 */
	if (!GlobalParameter.FlexibleFeeder)
		FeederHorizontalColor = WEISS;
	else
/*END HA 20.01.04 V1.73 */
	if( Motors[FEEDER_HORIZONTAL].ReferenceOk)
	{
		FeederHorizontalColor = SCHWARZ;
	}
	else
	{
		if(Motors[FEEDER_HORIZONTAL].Error)
			FeederHorizontalColor = GRAU59;
		else
			if(Motors[FEEDER_HORIZONTAL].StartRef)
			{
				if(ColorToggle)
					FeederHorizontalColor = SCHWARZ;
				else
					FeederHorizontalColor = WEISS;
			}
			else /*no ref and ref not running*/
				FeederHorizontalColor = WEISS;
	}

	if( Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk)
	{
		PaperRemoveHorizontalColor = SCHWARZ;
	}
	else
	{
		if(Motors[PAPERREMOVE_HORIZONTAL].Error)
			PaperRemoveHorizontalColor = GRAU59;
		else
			if(Motors[PAPERREMOVE_HORIZONTAL].StartRef)
			{
				if(ColorToggle)
					PaperRemoveHorizontalColor = SCHWARZ;
				else
					PaperRemoveHorizontalColor = WEISS;
			}
			else /*no ref and ref not running*/
				PaperRemoveHorizontalColor = WEISS;
	}

	if( Motors[PAPERREMOVE_VERTICAL].ReferenceOk)
	{
		PaperRemoveVerticalColor = SCHWARZ;
	}
	else
	{
		if(Motors[PAPERREMOVE_VERTICAL].Error)
			PaperRemoveVerticalColor = GRAU59;
		else
			if(Motors[PAPERREMOVE_VERTICAL].StartRef)
			{
				if(ColorToggle)
					PaperRemoveVerticalColor = SCHWARZ;
				else
					PaperRemoveVerticalColor = WEISS;
			}
			else /*no ref and ref not running*/
				PaperRemoveVerticalColor = WEISS;
	}


/*HA 20.01.04 V1.73 */
	if (!GlobalParameter.DeloaderTurnStation)
		DeloaderHorizontalColor = WEISS;
	else
/*END HA 20.01.04 V1.73 */
	if( Motors[DELOADER_HORIZONTAL].ReferenceOk)
	{
		DeloaderHorizontalColor = SCHWARZ;
	}
	else
	{
		if(Motors[DELOADER_HORIZONTAL].Error && GlobalParameter.DeloaderTurnStation)
			DeloaderHorizontalColor = GRAU59;
		else
			if(Motors[DELOADER_HORIZONTAL].StartRef)
			{
				if(ColorToggle)
					DeloaderHorizontalColor = SCHWARZ;
				else
					DeloaderHorizontalColor = WEISS;
			}
			else /*no ref and ref not running*/
				DeloaderHorizontalColor = WEISS;
	}

/*HA 20.01.04 V1.73 */
	if (GlobalParameter.DeloaderTurnStation!=2)
		DeloaderTurnColor = WEISS;
	else
/*END HA 20.01.04 V1.73 */
	if( Motors[DELOADER_TURN].ReferenceOk)
	{
		DeloaderTurnColor = SCHWARZ;
	}
	else
	{
		if(Motors[DELOADER_TURN].Error)
			DeloaderTurnColor = GRAU59;
		else
			if(Motors[DELOADER_TURN].StartRef)
			{
				if(ColorToggle)
					DeloaderTurnColor = SCHWARZ;
				else
					DeloaderTurnColor = WEISS;
			}
			else /*no ref and ref not running*/
				DeloaderTurnColor = WEISS;
	}

	if(FU.Error || FU.CANError)
		FUColor = GRAU59;
	else
	if(FU.cmd_Ref && !FU.RefOk)
	{
		if (ColorToggle)
			FUColor = SCHWARZ;
		else
			FUColor = WEISS;
	}
	else
		if(FU.RefOk)
			FUColor = SCHWARZ;
		else
			FUColor = WEISS;


}


