#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Stoppen aller Motoren (quasi NOTAUS)*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			30.06.03	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include "glob_var.h"
#include "in_out.h"
#include "asstring.h"
#include <string.h>
#include "motorfunc.h"

#define BROADCAST 127

_LOCAL 	USINT	StopStep;
_LOCAL F_TRIGtyp F_DoorSensor;

_GLOBAL	BOOL	StopMotors;
_GLOBAL	BOOL	ReferenceStart;

USINT	 		tmp[20];
USINT	i;


void MotorStop(void)
{

	AlarmBitField[27] = !DoorsOK;

	F_DoorSensor.CLK = DoorsOK;
/*CoverLockOn ist das Zeichen dafür, dass ein Ablauf aktiv sein muss, nur dann muss alles gestoppt werden!*/
	F_TRIG(&F_DoorSensor);

	if( F_DoorSensor.Q && Out_CoverLockOn ) StopMotors = 1;

	switch (StopStep)
	{
		case 0:
		{
			i = 0;
			if( StopMotors )
				StopStep = 2;
			break;
		}

		case 2:
		{
/*stop all sequences*/
			START = 0;
			AUTO = 0;
			PaperRemoveStart = 0;
			PaperRemoveStart = 0;
			PlateTakingStart = 0;
			AdjustStart = 0;
			DeloadStart = 0;
			ExposeStart = 0;
			CloseTrolleyStart = 0;
			OpenTrolleyStart = 0;
			ReferenceStart = 0;

/*release cover lock*/
			Out_CoverLockOn = 0;

/*stop LUST Controller*/
			FU.cmd_Start = 0;
			FU.cmd_Auto = 0;

			for(i=0;i<MotorsConnected;i++)
			{
				Motors[i].ReferenceOk = 0;
				Motors[i].StartRef = 0;
				Motors[i].ReferenceStep = 0;
			}

			StopStep = 5;
			break;
		}

		case 5:
		{
/*wait, till no other sending active*/
/*send V0 to all motors at once (BROADCAST)*/
			if (SendMotorCmd(BROADCAST,"V0",0,0))
				StopStep = 10;
			break;
		}
/*reset reference ok flags*/
		case 10:
		{
			StopStep = 15;
			break;
		}

		case 15:
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
				AbfrageText2 = 45;
				AbfrageText3 = 46;
				AbfrageIcon = CAUTION;
				StopStep = 20;
			break;
		}
		case 20:
		{
			if(AbfrageOK)
			{
				AbfrageOK = 0;
				StopStep = 0;
				StopMotors = 0;
			}
			break;
		}
	}
}


