#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Plattenwagen schliessen */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.00		10.09.02	erste Implementation					HA		*/
/*	1.10		16.09.02	Trolley offen/geschlossen jetzt mit		HA		*/
/*							2 Positionen (start und ende der Bewegung		*/
/*	1.20		02.01.03	vor oeffnen/schließen wird jetzt der	HA		*/
/*							Vertikalantrieb Referenz gefahren				*/
/*	1.30		06.01.03	Verschluß "nachdruecken" rausgenommen	HA		*/
/*							(neuer Verschluß -> nicht mehr nötig)			*/
/*																			*/
/*																			*/
/****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "glob_var.h"
#include "in_out.h"
#include "motorfunc.h"
#include "auxfunc.h"

#define PRELIFT	300
#define PUSHPOS_H 3157
#define PUSHPOS_V 400
#define BACKWAY_AFTER_CLOSING (100) /* relativ in Incr. */

_LOCAL	USINT	CloseTrolleyStep;
_GLOBAL	BOOL	CloseTrolleyStart;
_LOCAL TON_10ms_typ CloseTrolleyTimer;
USINT			tmp[20];

_GLOBAL	BOOL	OpenTrolleyStart;
_GLOBAL 	BOOL	CloseTrolleyTriggeredCloseDrawer;

USINT	CloseCounter; /* for max 2 cycles (2 single trolleys)*/
BOOL Small;


void CloseTrolley(void)
{
	/* Motoren keine Referenz? -> Ende*/
	if( !Motors[PAPERREMOVE_VERTICAL].ReferenceOk
	||  !Motors[PAPERREMOVE_VERTICAL2].ReferenceOk
	)
		 return;

	if( !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk ) return;
	if( !Motors[FEEDER_VERTICAL].ReferenceOk ) return;

	TON_10ms(&CloseTrolleyTimer);

	if (!CloseTrolleyStart)
	{
		CloseTrolleyStep=0;
		CloseTrolleyTriggeredCloseDrawer = 0;
	}

	switch (CloseTrolleyStep)
	{
		case 0:
		{
/* wait for start command */
			if (!CloseTrolleyStart || PaperRemoveStart || OpenTrolleyStart) break;
/*enable conditions*/

/*check, if Feeder is out of collision area (should be the case here)*/
			if( abs(Motors[FEEDER_VERTICAL].Position) > abs(FeederParameter.VerticalPositions.EnablePaperRemove)+20 )
				break;

/*NEW*/
			CloseCounter = 1;

/*NEW always do a reference of paperremove vert. at first */
			CloseTrolleyStep = 4;
			break;
		}

		case 1:
		{
/* else move up */
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up) )
			{
				CloseTrolleyStep = 2;
			}
			break;
		}
		case 2:
		{
/* Bewegung starten */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				CloseTrolleyStep = 3;
			}
			break;
		}
		case 3:
		{
/*Wait for UP*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.Up,20) )
				CloseTrolleyStep = 8;
			break;
		}

/*NEW paperremove vert reference at first*/
		case 4:
		{
			Motors[PAPERREMOVE_VERTICAL].StartRef = 1;
			Motors[PAPERREMOVE_VERTICAL].ReferenceOk = 0;
			Motors[PAPERREMOVE_VERTICAL2].StartRef = 1;
			Motors[PAPERREMOVE_VERTICAL2].ReferenceOk = 0;
			CloseTrolleyStep = 5;
			break;
		}
		case 5:
		{
			if( Motors[PAPERREMOVE_VERTICAL].ReferenceOk == 0
			 || Motors[PAPERREMOVE_VERTICAL2].ReferenceOk == 0
			)
				break;

			CloseTrolleyStep = 8;

			break;
		}

		case 8:
		{
			if(  (Out_PaperGripOn[LEFT] == OFF)
			  && (Out_PaperGripOn[RIGHT] == OFF)
			  && CloseTrolleyTimer.Q )
			{
				CloseTrolleyStep = 10;
				CloseTrolleyTimer.IN = 0;
				break;
			}

			Out_PaperGripOn[LEFT] = OFF;
			Out_PaperGripOn[RIGHT] = OFF;
			CloseTrolleyTimer.IN = TRUE;
			CloseTrolleyTimer.PT = PaperRemoveParameter.GripOffTime;
			break;
		}


/*********************************************************/
/*move fast to "trolley open" pos*/
/*********************************************************/
		case 10:
		{
/*NEW*/
			if( (GlobalParameter.TrolleyLeft != 0) && CloseCounter == 1)
			{
				if(GlobalParameter.EnablePaletteLoading && Trolleys[GlobalParameter.TrolleyLeft].NoCover)
				{
					CloseTrolleyStart = FALSE;
					TrolleyOpen = FALSE;
					CloseTrolleyStep = 0;
					CloseTrolleyTimer.IN = 0;
					break;
				}
				
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart = Trolleys[GlobalParameter.TrolleyLeft].OpenStart;
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop = Trolleys[GlobalParameter.TrolleyLeft].OpenStop;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart = Trolleys[GlobalParameter.TrolleyLeft].CloseStart;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop = Trolleys[GlobalParameter.TrolleyLeft].CloseStop;
				if(Trolleys[GlobalParameter.TrolleyLeft].Double)
					Small = 0;
				else
					Small = 1;
			}
			if( (GlobalParameter.TrolleyRight != 0 && CloseCounter == 2)
			||  (GlobalParameter.TrolleyLeft == 0 && GlobalParameter.TrolleyRight != 0 && CloseCounter == 1 ) )
			{
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart = abs(Trolleys[GlobalParameter.TrolleyRight].OpenStart) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop = abs(Trolleys[GlobalParameter.TrolleyRight].OpenStop) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart = abs(Trolleys[GlobalParameter.TrolleyRight].CloseStart) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop = abs(Trolleys[GlobalParameter.TrolleyRight].CloseStop) + GlobalParameter.TrolleyRightOffset;
				Small = 1;
			}


/*check, if Feeder is out of collision area (should be the case here)*/
			if( abs(Motors[FEEDER_VERTICAL].Position) > abs(FeederParameter.VerticalPositions.EnablePaperRemove)+20 )
				break;

/*Move Paper remove horizontically to open trolley pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart) )
				CloseTrolleyStep = 11;
			break;
		}
		case 11:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				CloseTrolleyStep = 12;
			break;
		}
		case 12:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart,20) )
				CloseTrolleyStep = 14;

			break;
		}

/*********************************************************/
/*move down slowly on trolley */
/*********************************************************/
		case 14:
		{
/* Paper remove down on trolley*/
			if( PapRemVerticalCmd(BOTH,CMD_SPEED,TRUE,PaperRemoveParameter.TrolleyOpenDownSpeed) )
			{
				CloseTrolleyStep = 15;
			}
			break;
		}
		case 15:
		{
/* Paper remove down on trolley*/
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.OnTrolley) )
			{
				CloseTrolleyStep = 16;
			}
			break;
		}
		case 16:
		{
/* Bewegung starten */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				CloseTrolleyStep = 17;
			}
			break;
		}
		case 17:
		{
/*Wait for pos on trolley*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.OnTrolley,20)
			|| ( CloseTrolleyTimer.Q ))
			{
				CloseTrolleyStep = 19;
				CloseTrolleyTimer.IN = 0;
				break;
			}

			CloseTrolleyTimer.IN = 1;
			CloseTrolleyTimer.PT = PaperRemoveParameter.TimeoutOnTrolley;
			break;
		}

/*********************************************************/
/*move slowly to "trolley closed" pos */
/*********************************************************/
		case 19:
		{
/*Move Paper remove horizontically to closed trolley pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop) )
				CloseTrolleyStep = 20;
			break;
		}
		case 20:
		{
/*set speed */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.TrolleyOpenCloseSpeed) )
				CloseTrolleyStep = 21;
			break;
		}
		case 21:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				CloseTrolleyStep = 22;
			break;
		}
		case 22:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop,20)
			|| ( CloseTrolleyTimer.Q ))
			{
				CloseTrolleyStep = 28;
				CloseTrolleyTimer.IN = 0;
				break;
			}

			CloseTrolleyTimer.IN = 1;
			if(Small)
				CloseTrolleyTimer.PT = PaperRemoveParameter.TimeoutOpenTrolleySmall;
			else
				CloseTrolleyTimer.PT = PaperRemoveParameter.TimeoutOpenTrolleyBig;
			break;

		}

/*HA 05.05.03 V1.05 steps 28-30*/
/*********************************************************/
/*move back a little to release shutter */
/*********************************************************/
		case 28:
		{
/*Move Paper remove horizontically back a little (to open start position actually)*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop+BACKWAY_AFTER_CLOSING) )
				CloseTrolleyStep = 29;
			break;
		}
		case 29:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				CloseTrolleyStep = 30;
			break;
		}
		case 30:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop+BACKWAY_AFTER_CLOSING,20) )
				CloseTrolleyStep = 44;
			break;
		}


/*********************************************************/
/*move up fast completely */
/*********************************************************/
		case 44:
		{
/*  move up */
			if( PapRemVerticalCmd(BOTH,CMD_SPEED,TRUE,PaperRemoveParameter.TrolleyOpenUpSpeed) )
			{
				CloseTrolleyStep = 45;
			}
			break;
		}
		case 45:
		{
/*  move up */
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up) )
			{
				CloseTrolleyStep = 46;
			}
			break;
		}
		case 46:
		{
/* Bewegung starten */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				CloseTrolleyStep = 47;
			}
			break;
		}
		case 47:
		{
/*Wait for UP*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.Up,20)
			|| ( CloseTrolleyTimer.Q ))
			{

/* HA 24.04.03 V1.05 bugfix: closing 2nd trolley was not activated (if-construct was missing)*/
				if( (GlobalParameter.TrolleyLeft != 0) && (GlobalParameter.TrolleyRight != 0) && CloseCounter ==1)
				{
					CloseCounter = 2;
					CloseTrolleyStep = 10;
				}
				else
					CloseTrolleyStep = 79;
				CloseTrolleyTimer.IN = 0;
				break;
			}
			CloseTrolleyTimer.IN = 1;
			CloseTrolleyTimer.PT = PaperRemoveParameter.TimeoutUpFromTrolley;
			break;

		}


/*********************************************************/
/*move back fast to wait pos*/
/*********************************************************/

		case 79:
		{
/*set speed */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaperRemoveSpeed) )
				CloseTrolleyStep = 80;
			break;
		}
		case 80:
		{
/*Move Paper remove horizontically to wait pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,PaperRemoveParameter.HorizontalPositions.EnableFeeder) )
				CloseTrolleyStep = 81;
			break;
		}
		case 81:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				CloseTrolleyStep = 82;
			break;
		}
		case 82:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.EnableFeeder,20) )
			{
				CloseTrolleyStep = 0;
				CloseTrolleyStart = 0;
				TrolleyOpen = 0;
			}
			break;
		}
	}/*switch*/
}


