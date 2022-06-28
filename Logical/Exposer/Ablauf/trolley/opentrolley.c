#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Plattenwagen öffnen */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.00		10.09.02	erste Implementation					HA		*/
/*	1.10		16.09.02	Trolley offen/geschlossen jetzt mit		HA		*/
/*	1.20		02.01.03	vor oeffnen/schließen wird jetzt der	HA		*/
/*							Vertikalantrieb Referenz gefahren				*/
/*	1.30		06.01.03	Trolley erkennung implementiert			HA		*/
/*																			*/
/****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "glob_var.h"
#include "in_out.h"
#include "Motorfunc.h"
#include "auxfunc.h"

#define PAPERREMOVEHORIZONTAL_MAXPOS (250000) /* Increments */
#define NO_TROLLEY (0)

_LOCAL	UINT	OpenTrolleyStep;
_GLOBAL	BOOL	OpenTrolleyStart;
_GLOBAL	BOOL	CloseTrolleyStart;
_LOCAL TON_10ms_typ OpenTrolleyTimer;

USINT			tmp[20];

USINT	OpenCounter; /* for max 2 cycles (2 single trolleys)*/
_LOCAL CTUtyp TrolleyCodeCounter;
_LOCAL	UINT	DetectedTrolley,DetectionError,DetectCounter;
		DINT	StartPos,EndPos;
_GLOBAL	UINT	OrgBild,IgnoreButtons;

extern	STRING	NameTrolleyLeft[32],NameTrolleyRight[32];

static    BOOL                PaletteDetection;
static    BOOL                CheckForPalette;


void OpenTrolley(void)
{
	/* Motoren keine Referenz? -> Ende*/
	if( !Motors[PAPERREMOVE_VERTICAL].ReferenceOk
	||  !Motors[PAPERREMOVE_VERTICAL2].ReferenceOk
	)
		return;
	if( !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk ) return;
	if( !Motors[FEEDER_VERTICAL].ReferenceOk ) return;

	TON_10ms(&OpenTrolleyTimer);

	if (!OpenTrolleyStart) OpenTrolleyStep=0;

	switch (OpenTrolleyStep)
	{
		case 0:
		{
			DetectionError = 0;
			DetectCounter = 0;

/* wait for start command */
			if (!OpenTrolleyStart || PaperRemoveStart || CloseTrolleyStart) break;

/*enable conditions*/
/*check, if Feeder is out of collision area (should be the case here)*/
			if( abs(Motors[FEEDER_VERTICAL].Position) > abs(FeederParameter.VerticalPositions.EnablePaperRemove)+20 )
				break;

			OpenTrolleyTimer.IN = 0;

			OpenCounter = 1;

/*neuer Versuch: Alarme löschen */
			AlarmBitField[13] = 0;
			AlarmBitField[14] = 0;

/*NEW always do a reference of paperremove vert. at first */
			OpenTrolleyStep = 2;
			break;
		}

/*NEW paperremove vert reference at first*/
		case 2:
		{
			Motors[PAPERREMOVE_VERTICAL].StartRef = 1;
			Motors[PAPERREMOVE_VERTICAL].ReferenceOk = 0;
			Motors[PAPERREMOVE_VERTICAL2].StartRef = 1;
			Motors[PAPERREMOVE_VERTICAL2].ReferenceOk = 0;
			OpenTrolleyStep = 3;
			break;
		}
/*Wait for reference OK*/
/* then jump to 50 for trolley detection, if enabled, else go on with step 8*/
		case 3:
		{
			if( Motors[PAPERREMOVE_VERTICAL].ReferenceOk == 0
				|| Motors[PAPERREMOVE_VERTICAL2].ReferenceOk == 0
			)
				break;

			if( GlobalParameter.AutomaticTrolleyOpenClose)
				OpenTrolleyStep = 50;
			else
				OpenTrolleyStep = 8;
			break;
		}

		case 8:
		{
			if( (Out_PaperGripOn[LEFT] == 0)
			 && (Out_PaperGripOn[RIGHT] == 0)
			 && OpenTrolleyTimer.Q )
			{
				OpenTrolleyStep = 10;
				OpenTrolleyTimer.IN = 0;
				break;
			}


			Out_PaperGripOn[LEFT] = 0;
			Out_PaperGripOn[RIGHT] = 0;
			OpenTrolleyTimer.IN = 1;
			OpenTrolleyTimer.PT = PaperRemoveParameter.GripOffTime;
			break;
		}
		case 10:
		{

/*NEW*/
			if( (GlobalParameter.TrolleyLeft != 0) && OpenCounter == 1)
			{
			/* Palette: no opening -> ready, jump to final steps for moving back to parkpos */
				if(GlobalParameter.EnablePaletteLoading && Trolleys[GlobalParameter.TrolleyLeft].NoCover)
				{
					OpenTrolleyStep = 29;
					OpenTrolleyTimer.IN = FALSE;
					break;
				}
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart = Trolleys[GlobalParameter.TrolleyLeft].OpenStart;
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop = Trolleys[GlobalParameter.TrolleyLeft].OpenStop;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart = Trolleys[GlobalParameter.TrolleyLeft].CloseStart;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop = Trolleys[GlobalParameter.TrolleyLeft].CloseStop;
			}
			if( (GlobalParameter.TrolleyRight != 0 && OpenCounter == 2)
			||  (GlobalParameter.TrolleyLeft == 0 && GlobalParameter.TrolleyRight != 0 && OpenCounter == 1 ) )
			{
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart = abs(Trolleys[GlobalParameter.TrolleyRight].OpenStart) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop = abs(Trolleys[GlobalParameter.TrolleyRight].OpenStop) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart = abs(Trolleys[GlobalParameter.TrolleyRight].CloseStart) + GlobalParameter.TrolleyRightOffset;
				PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop = abs(Trolleys[GlobalParameter.TrolleyRight].CloseStop) + GlobalParameter.TrolleyRightOffset;
			}

/*check, if Feeder is out of collision area (should be the case here)*/
			if( abs(Motors[FEEDER_VERTICAL].Position) > abs(FeederParameter.VerticalPositions.EnablePaperRemove)+20 )
				break;

/*Move Paper remove horizontically to closed trolley pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",TRUE,PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart) )
				OpenTrolleyStep = 11;
			break;
		}
		case 11:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",FALSE,0) )
				OpenTrolleyStep = 12;
			break;
		}
		case 12:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart,20) )
				OpenTrolleyStep = 14;

			break;
		}
		case 14:
		{
/* Paper remove down on trolley*/
			if( PapRemVerticalCmd(BOTH,CMD_SPEED,1,PaperRemoveParameter.TrolleyOpenDownSpeed) )
			{
				OpenTrolleyStep = 15;
			}
			break;
		}
		case 15:
		{
/* Paper remove down on trolley*/
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.OnTrolley) )
			{
				OpenTrolleyStep = 16;
			}
			break;
		}
		case 16:
		{
/* Bewegung starten */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				OpenTrolleyStep = 17;
			}
			break;
		}
		case 17:
		{
/*Wait for Pos*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.OnTrolley,20) )
				OpenTrolleyStep = 19;
			break;
		}
		case 19:
		{
/*Move Paper remove horizontically to open trolley pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop) )
				OpenTrolleyStep = 20;
			break;
		}
		case 20:
		{
/*set speed */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,CMD_SPEED,TRUE,PaperRemoveParameter.TrolleyOpenCloseSpeed) )
				OpenTrolleyStep = 21;
			break;
		}
		case 21:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",FALSE,0) )
				OpenTrolleyStep = 22;
			break;
		}
		case 22:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop,20) )
				OpenTrolleyStep = 24;

			break;
		}
		case 24:
		{
/*  move up */
			if( PapRemVerticalCmd(BOTH,CMD_SPEED,TRUE,PaperRemoveParameter.TrolleyOpenUpSpeed) )
			{
				OpenTrolleyStep = 25;
			}
			break;
		}
		case 25:
		{
/*  move up */
			if( PapRemVerticalCmd(BOTH,CMD_ABSOLUTE_POS,TRUE,PaperRemoveParameter.VerticalPositions.Up) )
			{
				OpenTrolleyStep = 26;
			}
			break;
		}
		case 26:
		{
/* Bewegung starten */
			if( PapRemVerticalCmd(BOTH,CMD_START_MOTION,FALSE,0L) )
			{
				OpenTrolleyStep = 27;
			}
			break;
		}
		case 27:
		{
/*Wait for UP*/
			if( isPapRemVerticalOk(BOTH,PaperRemoveParameter.VerticalPositions.Up,20) )
			{
				if( (GlobalParameter.TrolleyLeft != 0) && (GlobalParameter.TrolleyRight != 0) && OpenCounter ==1)
				{
					OpenCounter=2;
					OpenTrolleyStep = 10;
				}
				else
				OpenTrolleyStep = 29;
			}
			break;
		}
		case 29:
		{
/*set speed */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaperRemoveSpeed) )
				OpenTrolleyStep = 30;
			break;
		}
		case 30:
		{
/*Paperremove is the next to do, so dont move back*/
			if (!PaperRemoveReady && GlobalParameter.PaperRemoveEnabled && START)
			{
				OpenTrolleyStep = 0;
				OpenTrolleyStart = 0;
				if( !DetectionError )
					TrolleyOpen = 1;
				break;
			}
/*Move Paper remove horizontically to wait pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,PaperRemoveParameter.HorizontalPositions.EnableFeeder) )
				OpenTrolleyStep = 31;
			break;
		}
		case 31:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				OpenTrolleyStep = 32;
			break;
		}
		case 32:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,PaperRemoveParameter.HorizontalPositions.EnableFeeder,20) )
			{
				PaperRemoveReady = 0;
				OpenTrolleyStep = 0;
				OpenTrolleyStart = 0;
				if( !DetectionError )
					TrolleyOpen = 1;
			}
			break;
		}

/*****************************************/
/* special steps for trolley detection */
/*****************************************/
		case 50:
		{
			int i;
			CheckForPalette = FALSE;
			if(GlobalParameter.EnablePaletteLoading)
			{
				for(i=0;i<MAXTROLLEYS && !CheckForPalette;i++)
					if(Trolleys[i].NoCover)
						CheckForPalette = TRUE;
			}
			OpenTrolleyStep = 51;
			if(DetectCounter == 0)
			{
				StartPos = PaperRemoveParameter.TrolleyDetectStartPos;
				EndPos = PaperRemoveParameter.TrolleyDetectEndPos;
			}
			else
			{
				StartPos = abs(PaperRemoveParameter.TrolleyDetectStartPos) + GlobalParameter.TrolleyRightOffset;
				EndPos = abs(PaperRemoveParameter.TrolleyDetectEndPos) + GlobalParameter.TrolleyRightOffset;
			}
			/*
			 * safety check: Endpos must always be smaller than maximum position of Motor
			 */
			if( abs(EndPos) > abs(PAPERREMOVEHORIZONTAL_MAXPOS) )
				EndPos = PAPERREMOVEHORIZONTAL_MAXPOS;
			break;
		}
		case 51:
		{
/*Move Paper remove horizontically to trolley detect pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,StartPos) )
				OpenTrolleyStep = 52;
			break;
		}

		case 52:
		{
/*set speed FAST*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaperRemoveSpeed) )
				OpenTrolleyStep = 55;
			break;
		}

		case 55:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
				OpenTrolleyStep = 60;
			break;
		}
		case 60:
		{
/*Wait til Position is reached */
			if( !isPositionOk(PAPERREMOVE_HORIZONTAL,StartPos,20) )
				break;

			if(TrolleyCodeSensor)
			{
				OpenTrolleyStep = 65;
				DetectionError = 0;
			}
			else
			{
		/* FEHLER: kein Wagen */
				if(DetectCounter == 0)
				{
/*Wagen 1 (links) nicht gefunden: rechts probieren*/
					DetectCounter = 1;
					OpenTrolleyStep = 50;
					GlobalParameter.TrolleyLeft = 0;
				}
				else
				{
/*Wagen 2 (rechts) nicht gefunden*/
					DetectCounter = 0;
					GlobalParameter.TrolleyRight = 0;
/*no trolley found!*/
					if(GlobalParameter.TrolleyLeft == 0)
					{
/* check for palette*/
						if(CheckForPalette)
							OpenTrolleyStep = 150;
						else
						{
							AlarmBitField[13] = 1;
							DetectionError = 1;
							OrgBild = wBildAktuell;
							wBildNeu = 41;
							IgnoreButtons = 1;
							AbfrageOK = 0;
							OK_ButtonInv = 0;
							OK_CancelButtonInv = 1;
							AbfrageCancel = 0;
							AbfrageText1 = 5;
							AbfrageText2 = 0;
							AbfrageText3 = 7;
							AbfrageIcon = CAUTION;
							OpenTrolleyStep = 29;
						}
					}
					else
					{
/*nur Wagen links vorhanden: weiter mit Öffnen*/
						OpenTrolleyStep = 85;
					}
				}
			}
			break;
		}

		case 65:
		{
/*set speed */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.TrolleyDetectSpeed) )
				OpenTrolleyStep = 70;
			break;
		}
		case 70:
		{
/*Move Paper remove horizontically to trolley detect end pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,EndPos) )
				OpenTrolleyStep = 75;
			break;
		}

		case 75:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
			{
				OpenTrolleyStep = 80;
				TrolleyCodeCounter.RESET = 1;
			}
			break;
		}
/* detect the edges of trolley sensor and wait til pos reached*/
		case 80:
		{
			TrolleyCodeCounter.RESET = 0;
			TrolleyCodeCounter.CU = !TrolleyCodeSensor;
			DetectedTrolley = TrolleyCodeCounter.CV;

/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,EndPos,20) )
				OpenTrolleyStep = 84;

/*
 * longer than 3 sec no motion: error
 */
			OpenTrolleyTimer.IN = !Motors[PAPERREMOVE_HORIZONTAL].Moving;
			OpenTrolleyTimer.PT = 300; /* 3 sec */
			if( OpenTrolleyTimer.Q )
			{
				OpenTrolleyTimer.IN = 0;
				DetectionError = 1;
				OpenTrolleyStep = 29;
				AlarmBitField[14] = 1;
				OrgBild = wBildAktuell;
				wBildNeu = 41;
				IgnoreButtons = 1;
				AbfrageOK = 0;
				OK_ButtonInv = 0;
				OK_CancelButtonInv = 1;
				AbfrageCancel = 0;
				AbfrageText1 = 76;
				AbfrageText2 = 0;
				AbfrageText3 = 7;
				AbfrageIcon = CAUTION;
				GlobalParameter.TrolleyLeft = 0;
				GlobalParameter.TrolleyRight = 0;
			}
			break;
		}
		case 84:
		{
			DetectedTrolley = TrolleyCodeCounter.CV;
			TrolleyCodeCounter.RESET = 1;
	/* validate the code */
			if(DetectedTrolley>MAXTROLLEYS || DetectedTrolley<1)
			{
		/* FEHLER: ungültiger Code */

				DetectionError = 1;
				OpenTrolleyStep = 29;
				AlarmBitField[14] = 1;
				OrgBild = wBildAktuell;
				wBildNeu = 41;
				IgnoreButtons = 1;
				AbfrageOK = 0;
				OK_ButtonInv = 0;
				OK_CancelButtonInv = 1;
				AbfrageCancel = 0;
				AbfrageText1 = 6;
				AbfrageText2 = 0;
				AbfrageText3 = 7;
				AbfrageIcon = CAUTION;
				GlobalParameter.TrolleyLeft = 0;
				GlobalParameter.TrolleyRight = 0;
			}
			else
			{
		/*code eintragen*/
				if(PaletteDetection)
				{
/* plausibility: Palette code found, but not marked as palette ->Error*/
					if(Trolleys[DetectedTrolley].NoCover)
						GlobalParameter.TrolleyLeft = DetectedTrolley;
					else
					{
				/* ERROR: invalid Code */
						ShowMessage(6,81,7, CAUTION, OKONLY, TRUE);
						DetectionError = TRUE;
						AlarmBitField[14] = TRUE;
						GlobalParameter.TrolleyLeft = NO_TROLLEY;
						GlobalParameter.TrolleyRight = NO_TROLLEY;
						PaletteDetection = FALSE;
						OpenTrolleyStep = 29;
						OpenTrolleyTimer.IN = FALSE;
						break;
					}

					PaletteDetection = FALSE;
					OpenTrolleyStep = 85;
					OpenTrolleyTimer.IN = FALSE;
				}
				else
				if(DetectCounter == 0) /*1st detection: left trolley*/
				{
					GlobalParameter.TrolleyLeft = DetectedTrolley;
/* plausibility: Palette code found during trolley detection ->Error*/
					if(Trolleys[DetectedTrolley].NoCover)
					{
				/* ERROR: invalid Code */
						ShowMessage(6,80,7, CAUTION, OKONLY, TRUE);
						DetectionError = TRUE;
						AlarmBitField[14] = TRUE;
						GlobalParameter.TrolleyLeft = NO_TROLLEY;
						GlobalParameter.TrolleyRight = NO_TROLLEY;
						PaletteDetection = FALSE;
						OpenTrolleyStep = 29;
						OpenTrolleyTimer.IN = FALSE;
						break;
					}

					if(Trolleys[GlobalParameter.TrolleyLeft].Double)
					{
						GlobalParameter.TrolleyRight = 0;
			/*READY, no second detection required*/
						DetectCounter = 0;
						OpenTrolleyStep = 85;
						break;
					}
					else
					{
						DetectCounter = 1;
						OpenTrolleyStep = 50;
						break;
					}
				}
				else  /*2nd detection: right trolley*/
				{
					GlobalParameter.TrolleyRight = DetectedTrolley;
		/*validation*/
					if(Trolleys[GlobalParameter.TrolleyRight].Double)
					{
						DetectionError = 1;
						OpenTrolleyStep = 29;
						DetectCounter = 0;
						GlobalParameter.TrolleyRight = 0;
						GlobalParameter.TrolleyLeft = 0;
						AlarmBitField[14] = 1;
						OrgBild = wBildAktuell;
						wBildNeu = 41;
						OK_ButtonInv = 0;
						OK_CancelButtonInv = 1;
						IgnoreButtons = 1;
						AbfrageOK = 0;
						AbfrageCancel = 0;
						AbfrageText1 = 6;
						AbfrageText2 = 93;
						AbfrageText3 = 7;
						AbfrageIcon = CAUTION;
					}
					else
					{
						DetectCounter = 0;
						OpenTrolleyStep = 85;
					}
				}
			}
			break;
		}

		case 85:
		{
			TrolleyCodeCounter.RESET = 0;

/*reset speed to fast*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaperRemoveSpeed) )
				OpenTrolleyStep = 90;
			break;
		}

		case 90:
		{
			OpenTrolleyStep = 8;
			break;
		}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
		case 150:
		{
			PaletteDetection = TRUE;
			StartPos = labs(PaperRemoveParameter.HorizontalPositions.PaletteDetectStartPos);
			EndPos = labs(PaperRemoveParameter.HorizontalPositions.PaletteDetectEndPos);
			/*
			 * safety check: Endpos must always be smaller than maximum position of Motor
			 */
			if( labs(EndPos) > labs(PAPERREMOVEHORIZONTAL_MAXPOS) )
				EndPos = PAPERREMOVEHORIZONTAL_MAXPOS;

			OpenTrolleyStep = 151;
			OpenTrolleyTimer.IN = FALSE;
			break;
		}
		case 151:
		{
/*Move Paper remove horizontally to Palette detect pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,StartPos) )
			{
				OpenTrolleyStep = 152;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}
		case 152:
		{
/*set speed FAST*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaperRemoveSpeed) )
			{
				OpenTrolleyStep = 155;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}
		case 155:
		{
/* Bewegung starten */
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
			{
				OpenTrolleyStep = 160;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}

		case 160:
		{
/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,StartPos,20) )
			{
			{
				OpenTrolleyStep = 161;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
				break;
			}
			break;
		}
		
		case 161:
		{
/*Move Paper remove horizontally to Palette detect pos*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"LA",1,EndPos) )
			{
				OpenTrolleyStep = 162;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}
		case 162:
		{
/*set speed FAST*/
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"SP",1,PaperRemoveParameter.PaletteDetectSpeed) )
			{
				OpenTrolleyStep = 165;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}
		case 165:
		{
/* Bewegung starten */
			TrolleyCodeCounter.RESET = 1;
			if( SendMotorCmd(PAPERREMOVE_HORIZONTAL,"M",0,0) )
			{
				OpenTrolleyStep = 170;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}

/* detect the edges of trolley sensor and wait til pos reached*/
		case 170:
		{
			TrolleyCodeCounter.RESET = 0;
			TrolleyCodeCounter.CU = TrolleyCodeSensor;

/*Wait til Position is reached */
			if( isPositionOk(PAPERREMOVE_HORIZONTAL,EndPos,20) )
			{
				OpenTrolleyStep = 84;
				OpenTrolleyTimer.IN = FALSE;
				break;
			}
			break;
		}

	}/*switch*/

	CTU(&TrolleyCodeCounter);

}


