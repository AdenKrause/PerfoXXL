#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Plattenwagen Hauptprogramm */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			10.09.02	erste Implementation					HA		*/
/*	1.31			02.04.03	Fehlerhandling:kein Pano Adapter			HA		*/
/*																			*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"
#include "in_out.h"

_LOCAL	UINT	OpenTrolleyStep;
_LOCAL	USINT	CloseTrolleyStep;

_GLOBAL	BOOL	CloseTrolleyStart;
_GLOBAL	BOOL	OpenTrolleyStart;
_GLOBAL	BOOL	ReferenceStart,DontCloseTrolley;

_LOCAL TON_10ms_typ CloseTrolleyTimer;
_LOCAL TON_10ms_typ OpenTrolleyTimer;
_LOCAL F_TRIGtyp F_TRIG02;

extern void OpenTrolley(void);
extern void CloseTrolley(void);

extern void Test(void);

/****************************************************************************/
/* INIT Teil*/
/****************************************************************************/
_INIT void init(void)
{
	OpenTrolleyStep		= 0;
	OpenTrolleyStart	= 0;
	OpenTrolleyTimer.IN	= 0;
	OpenTrolleyTimer.PT	= 10; /*just to have any value in there...*/

	CloseTrolleyStep		= 0;
	CloseTrolleyStart		= 0;
	CloseTrolleyTimer.IN	= 0;
	CloseTrolleyTimer.PT	= 10; /*just to have any value in there...*/
	GlobalParameter.TrolleyLeft = 0;
	GlobalParameter.TrolleyRight = 0;
}


/****************************************************************************/
/* zyklischer Teil*/
/****************************************************************************/
_CYCLIC void cyclic(void)
{
	SequenceSteps[6] = (USINT) OpenTrolleyStep;
	SequenceSteps[7] = CloseTrolleyStep;


/*HA 26.06.03 V1.11 release cover lock, if machine is safe*/
	F_TRIG02.CLK =  AUTO
		|| START
		|| PaperRemoveStart
		|| PlateTakingStart
		|| AdjustStart
		|| DeloadStart
		|| ExposeStart
		|| CloseTrolleyStart
		|| OpenTrolleyStart
		|| PlateOnConveyorBelt.present
		|| PlateAtAdjustedPosition.present
		|| PlateAtDeloader.present
		|| ReferenceStart;

	F_TRIG(&F_TRIG02);
	if( F_TRIG02.Q )
	{
		Out_CoverLockOn = 0;
	}

/*error message if: trolley empty flag AND
Automatic Open/close and trolley is closed OR
no Automatic Open/close
*/
	if(TrolleyEmpty && ((GlobalParameter.AutomaticTrolleyOpenClose && !TrolleyOpen) || !GlobalParameter.AutomaticTrolleyOpenClose) )
	{
			if(PanoramaAdapter)
			{
				AlarmBitField[25]=1;
				AbfrageText1 = 37;
				AbfrageText2 = 40;
				AbfrageText3 = 0;
			}
			else
			{
				AlarmBitField[15]=1;
				AbfrageText1 = 17;
				AbfrageText2 = 18;
				AbfrageText3 = 19;
			}

			if(wBildAktuell != MESSAGEPIC )
				OrgBild = wBildAktuell;
			wBildNeu = MESSAGEPIC;
			AbfrageOK = 0;
			AbfrageCancel = 0;
			IgnoreButtons = 1;
			OK_CancelButtonInv = 1;
			OK_ButtonInv = 0;
			AbfrageIcon = CAUTION;
			TrolleyEmpty = 0;
	}

	OpenTrolley();
	CloseTrolley();
}


