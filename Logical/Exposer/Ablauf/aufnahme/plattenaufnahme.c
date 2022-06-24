#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Platte holen aus Trolley */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			09.09.02	erste Implementation					HA		*/
/*	1.10		02.01.03	Drehrichtung Feeder up/down geändert	 HA		*/
/*	1.20		25.02.03	Shuttle wird jetzt zur Übernahmepos 	 HA		*/
/*							gefahren, wenn Ablauf Übergabe nicht 			*/
/*							gestartet ist 									*/
/*	1.21		28.03.03	bugfix Plattenzaehler				 HA		*/
/*	1.22		02.04.03	wenn kein Panoramaadapter gefunden wird: Verhalten wie	 HA		*/
/*							bei leerem Wagen und Fehlermeldung */
/*	1.30		03.02.04	- nach Abtastung noch etwas weiter runterfahren um	 HA		*/
/*							  guten Saugerkontakt zu gewährleisten */
/*				04.02.04	- optimiertes Runterfahren (bremsen) */
/*																			*/
/****************************************************************************/
/****************************************************************************/
/*	2.00		26.09.05	- erste LS-Performance Version	*/
/****************************************************************************/

#include "glob_var.h"
#include "asstring.h"
#include <string.h>
#include <math.h>
#include "in_out.h"
#include "Motorfunc.h"
#include "auxfunc.h"

#define PINSDISTANCE	(70) /*Abstand von Stiften zu Rollenmittelpunkt in mm */
#define HALFSUCKERDISTANCE (150) /* Halber Saugerabstand */

/* HA 03.08.04 V1.82 Zeitmessung Vakuum ein/aus*/
_GLOBAL UDINT VacuumSwitchMinTime,VacuumSwitchMaxTime,VacuumSwitchTime;
_GLOBAL UDINT	VacuumSwitchAV,VacuumSwitchAVCNT;
_GLOBAL UDINT VacuumSwitchOffMinTime,VacuumSwitchOffMaxTime,VacuumSwitchOffTime;
_GLOBAL UDINT	VacuumSwitchOffAV,VacuumSwitchOffAVCNT;
_GLOBAL BOOL	ResetMinMaxAverage;

_LOCAL BOOL ToggleFlag;

_LOCAL	USINT	PlateTakingStep,SuckingCounter,ShakeCounter;
_LOCAL TON_10ms_typ PlateTakingTimer;
USINT			tmp[20];
_LOCAL	DINT	StackDetectPosition,LastStackPos,LastStackPosLeft,LastStackPosRight;

_LOCAL	BOOL	TwoStacks;
_GLOBAL	BOOL	StackDetect[2];
_GLOBAL BOOL	StopPlateTaking;

_LOCAL	TON_10ms_typ	TimeoutTimer;

_LOCAL	USINT	RetryCounter;
_LOCAL	USINT	ErrorCounterHorizontal,ErrorCounterVertical;

			BOOL	FeederVacuumError;

_GLOBAL	BOOL	PanoramaAdapter,DontCloseTrolley;
_GLOBAL BOOL	ExposingStarted;	/*HA 08.07.03 V1.13 Flag to inform plate taking that exposing has started*/

_GLOBAL BOOL	AlternatingTrolleys;


/*V1.95 bugfix: DINT statt UDINT (wie konnte das funktionieren?)*/
static    DINT                TargetPosition,TargetSpeed;
DINT tmpval;
REAL tmppos;
_GLOBAL	REAL	TablePosDrop;
REAL LocalTableMaxPos;
static    USINT               MotStep;       /* holds current step of fct MoveToAbsPosition */
static    USINT               PinsStep;       /* holds current step of fct AdjPins */
static    TON_10ms_typ        PinsTimer;
static USINT PaperDetectCounter;

/****************************************************************************/
/* Hilfsfunktion: gar nix tun */
/****************************************************************************/
void nop(void)
{
	return;
}




/*Hilfsfunktion zur Berechnung des Plattenstapels*/
/*Berechnet die Anzahl der Platten aus der aktuell ermittelten Stapel-Pos.,
der Trolley-Leer Pos und der Plattendicke

Parameter:
	- Trolley-Nummer
	- bei breitem Trolley: linker oder rechter Stapel
	  bei single Trolley: steht er links oder rechts (0=links 1=rechts)
*/

UDINT NumberOfPlates(USINT Trolley,BOOL Stack)
{
	REAL IncPerPlate = Motors[FEEDER_VERTICAL].Parameter.IncrementsPerMm * PlateParameter.Thickness;
	DINT Pos0Plates = abs(pTrolley->EmptyPositions[CurrentStack]);

	if (IncPerPlate>1)
		return floor(((abs(Pos0Plates-StackDetectPosition))  / (IncPerPlate))+0.5);
	else
		return 0;
}


void DecrementPlatesInTrolley(void)
{
				if(GlobalParameter.TrolleyLeft != 0)
				{
					if( Trolleys[GlobalParameter.TrolleyLeft].Double)
					{
					/*one double size trolley*/
						if(Trolleys[GlobalParameter.TrolleyLeft].RightStack == 0)
						{
							if(StackDetect[LEFT])
							{
								Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft =
									NumberOfPlates(GlobalParameter.TrolleyLeft,LEFT);
								StackDetect[LEFT] = 0;
								StackDetect[RIGHT] = 0;
							}
							if(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft>0)
								Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft--;
						}
						else /*2 stacks*/
						{
							TwoStacks = 1;
							if(CurrentStack == LEFT )
							{
								if(StackDetect[LEFT])
								{
									Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft =
										NumberOfPlates(GlobalParameter.TrolleyLeft,LEFT);
									StackDetect[LEFT] = 0;
								}
								if(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft>0)
									Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft--;
							}
							else /*RIGHT*/
							{
								if(StackDetect[RIGHT])
								{
									Trolleys[GlobalParameter.TrolleyLeft].PlatesRight =
										NumberOfPlates(GlobalParameter.TrolleyLeft,RIGHT);
									StackDetect[RIGHT] = 0;
								}

								if(Trolleys[GlobalParameter.TrolleyLeft].PlatesRight>0)
									Trolleys[GlobalParameter.TrolleyLeft].PlatesRight--;
							}
						}
					}
					else /*Trolley Left == Single*/
					{
						if( GlobalParameter.TrolleyRight != 0)
						{
						/*2 single trolleys*/
							TwoStacks = 1;
							if(CurrentStack == LEFT)
							{
								if(StackDetect[LEFT])
								{
									Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft =
										NumberOfPlates(GlobalParameter.TrolleyLeft,LEFT);
									StackDetect[LEFT] = 0;
								}
								if(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft>0)
									Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft--;
							}
							else /*RIGHT*/
							{
								if(StackDetect[RIGHT])
								{
									Trolleys[GlobalParameter.TrolleyRight].PlatesLeft =
										NumberOfPlates(GlobalParameter.TrolleyRight,RIGHT);
									StackDetect[RIGHT] = 0;
								}
								if(Trolleys[GlobalParameter.TrolleyRight].PlatesLeft>0)
									Trolleys[GlobalParameter.TrolleyRight].PlatesLeft--;
							}
						}
						else
						{
						/* 1 single trolley LEFT*/
							if(StackDetect[LEFT])
							{
								Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft =
									NumberOfPlates(GlobalParameter.TrolleyLeft,LEFT);
								StackDetect[LEFT] = 0;
								StackDetect[RIGHT] = 0;
							}
							if(Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft>0)
								Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft--;
						}
					} /*else TrolleyLeft==Single*/
				}
				else /*Trolley Left == 0*/
				{
					if( GlobalParameter.TrolleyRight != 0)
					{
					/* 1 single trolley RIGHT*/
							if(StackDetect[RIGHT])
							{
								Trolleys[GlobalParameter.TrolleyRight].PlatesLeft =
									NumberOfPlates(GlobalParameter.TrolleyRight,RIGHT);
								StackDetect[RIGHT] = 0;
								StackDetect[LEFT] = 0;
							}

							if(Trolleys[GlobalParameter.TrolleyRight].PlatesLeft>0)
								Trolleys[GlobalParameter.TrolleyRight].PlatesLeft--;
					}
				}

}

/****************************************************************************/
/* Hilfsfunktion: Initialisieren der Zeitmesswerte*/
/****************************************************************************/
void InitTimerValues(void)
{
	ResetMinMaxAverage = 0;
	VacuumSwitchTime = 0;
	VacuumSwitchMinTime = 1000;
	VacuumSwitchMaxTime = 0;
	VacuumSwitchOffTime = 0;
	VacuumSwitchOffMinTime = 1000;
	VacuumSwitchOffMaxTime = 0;
	VacuumSwitchAVCNT = 0;
	VacuumSwitchAV = 0;
	VacuumSwitchOffAVCNT = 0;
	VacuumSwitchOffAV = 0;
}

/****************************************************************************/
/* INIT Teil*/
/****************************************************************************/
_INIT void init(void)
{
	InitTimerValues();
	PaperDetectCounter = 0;
	PinsStep = 0;
	PinsTimer.IN = FALSE;
	FeederVacuumError = 0;
	AUTO = 0;
	StopPlateTaking		= 0;
	PlateTakingStep		= 0;
	PlateTakingStart	= 0;
	PlateTakingTimer.IN	= 0;
	PlateTakingTimer.PT	= 10; /*just to have any value in there...*/
	START=0;
	ErrorCounterHorizontal = 0;
	ErrorCounterVertical = 0;
	if(FeederParameter.StackDetectSpeed <5)
		FeederParameter.StackDetectSpeed = 200;
	LastStackPos = -500000;
	LastStackPosLeft = -500000;
	LastStackPosRight = -500000;
	StackDetect[0] = 1;
	StackDetect[1] = 1;
	CurrentStack = LEFT;
	DisableTable = FALSE;
}



/****************************************************************************/
/* zyklischer Teil*/
/****************************************************************************/
_CYCLIC void cyclic(void)
{
	GlobalParameter.FlexibleFeeder = FALSE;

	if (ResetMinMaxAverage) InitTimerValues();

	SequenceSteps[1] = PlateTakingStep;

	/* Motoren keine Referenz? -> Ende*/
	if( !Motors[FEEDER_VERTICAL].ReferenceOk ) return;
	if( !Motors[FEEDER_HORIZONTAL].ReferenceOk && GlobalParameter.FlexibleFeeder ) return;
	if( !Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk ) return;

	TON_10ms(&PlateTakingTimer);
	TON_10ms(&TimeoutTimer);
	TON_10ms(&PinsTimer);


/*safety*/
	if( !PlateTakingStart )
	{
		PlateTakingStep = 0;
		FeederVacuumError = 0;
		if ( abs(Motors[FEEDER_VERTICAL].Position) < abs(FeederParameter.VerticalPositions.ReleasePlate)+100)
			DisableTable = FALSE;
	}

	switch (PlateTakingStep)
	{
		case 0:
		{

			if( isPositionHigherThan(FEEDER_VERTICAL,
		                            labs(FeederParameter.VerticalPositions.ReleasePlate) + 20L)
		     || AdjustStart
		     || ManualMode
		     )
		    {
		    	PlateTakingStart = FALSE;
		    	break;
		    }


			if( !PlateTakingStart ) break;

			if(FeederParameter.ShakeTime > 99) FeederParameter.ShakeTime = 10;
			if(FeederParameter.ShakeRepitions > 99) FeederParameter.ShakeRepitions = 10;
			if(FeederParameter.SuckRepitions > 99) FeederParameter.SuckRepitions = 2;

/*no trolley in machine->break*/
			if( (GlobalParameter.TrolleyLeft == 0 && GlobalParameter.TrolleyRight == 0) ||
				(!TrolleyOpen && GlobalParameter.AutomaticTrolleyOpenClose))
			{
				START = 0;
				PaperRemoveStart= 0;
				PlateTakingStart = 0;
				break;
			}

			TwoStacks = 0;

			if((GlobalParameter.TrolleyLeft == 0) || (GlobalParameter.TrolleyLeft >= MAXTROLLEYS))
				GlobalParameter.TrolleyLeft = 1;

			pTrolley = &Trolleys[GlobalParameter.TrolleyLeft];

			CurrentData.TrolleyNumber = GlobalParameter.TrolleyLeft;
			CurrentData.PlateType = Trolleys[GlobalParameter.TrolleyLeft].PlateType;
			CurrentData.FeederHorizontalPos = Trolleys[CurrentData.TrolleyNumber].TakePlateLeft;


			if( PanoramaAdapter )
			{
				if(PlateType != 0)
				{
					memcpy(&PlateParameter,&PlateTypes[PlateType],sizeof(PlateParameter));
/*HA 01.10.03 V1.64 new Param for Panoadapter independent from Trolley*/
					FeederParameter.HorizontalPositions.TakePlate = FeederParameter.HorPositionPano;
				}
				else
				{
/* wrong plate type!*/
					START = 0;
					PaperRemoveStart= 0;
					PlateTakingStart = 0;
					break;
				}
			}
			else
			{
				CurrentStack = LEFT;
				memcpy(&PlateParameter,&PlateTypes[pTrolley->PlateType],sizeof(PlateParameter));
			}


			MoveAdjustSucker(ALL,INACTIVE);
			SwitchAdjustVacuum(0,ALL,OFF);
/* Schritte 180 ff um ggf den Tisch wegzufahren*/
			PlateTakingStep = 180;
			Out_FeederHorActive = OFF;
			PaperDetectCounter = 0;

			break;
		}

/*******************************************************************************/
		case 1:
		{
			if( !Out_FeederHorActive
			 && In_FeederHorInactive
			 && !In_FeederHorActive
			  )
			{
				PlateTakingStep = 2;
				break;
			}
			Out_FeederHorActive = OFF;
			break;
		}


/*runterfahren bis über Papierentfernung*/
		case 2:
		{
/* safety!*/
			if(DeloadStart || FU.cmd_Target || FU.cmd_Park)
				break;
/**************************/
/*erstmal auf null setzen*/
			DontCloseTrolley = 0;

/* move down to Pos above Paper remove*/
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.EnablePaperRemove,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
			{
				PlateTakingStep = 4;
			}
			break;
		}

		case 4:
		{
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.EnablePaperRemove,100)
			||	(Motors[FEEDER_VERTICAL].Moving))
			{
				PlateTakingStep = 5;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 1;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterVertical++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}

		case 5:
		{
			if( !GlobalParameter.FlexibleFeeder )
			{
				PlateTakingStep = 9;
				break;
			}
/*move Feeder horizontally in plate taking pos.*/
			if( MoveToAbsPosition(FEEDER_HORIZONTAL,
			                      CurrentData.FeederHorizontalPos,
			                      DONT_SEND_SPEED,
			                      &MotStep) )
			{
				PlateTakingStep = 9;
			}
			break;
		}
		case 9:
		{
/*Warten, bis Paierentfernung aus dem Weg ist und Papier entfernt wurde*/
			if(abs(FeederParameter.HorizontalPositions.TakePlate) > 50000 && GlobalParameter.FlexibleFeeder )
			{
				if( 	abs(Motors[PAPERREMOVE_HORIZONTAL].Position) < abs(PaperRemoveParameter.HorizontalPositions.EnableFeeder)+90000
					&&	PaperRemoveReady )
					PlateTakingStep = 10;
			}
			else
			{
				UDINT tmpval;
				if(PaperRemoveStart && (Out_PaperGripOn[LEFT] || Out_PaperGripOn[RIGHT]) )
				{
					if( PlateParameter.Length > 400 )
						tmpval = PaperRemoveParameter.HorizontalPositions.PaperReleasePanorama;
					else
						tmpval = PaperRemoveParameter.HorizontalPositions.PaperReleaseSingle;
				}
				else
					tmpval = PaperRemoveParameter.HorizontalPositions.EnableFeeder;

				if( isPositionLowerThan(PAPERREMOVE_HORIZONTAL,
				                        labs(tmpval) + 500L )
					&&	PaperRemoveReady )
				{
					if( !GlobalParameter.FlexibleFeeder )
						PlateTakingStep = 16;
					else
						PlateTakingStep = 10;
				}
			}
			break;
		}
		case 10:
		{
/*Down to Pos above Trolley*/
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.AboveTrolley,
			                      DONT_SEND_SPEED,
			                      &MotStep) )
			{
				PlateTakingStep = 12;
				PlateTakingTimer.IN = 0;
			}
			break;
		}
		case 11:
		{
/*wait, till no other sending active*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L))
				PlateTakingStep = 12;
			break;
		}
		case 12:
		{
			if( isPositionOk(FEEDER_VERTICAL,
				            FeederParameter.VerticalPositions.AboveTrolley, 100L )
			||	(Motors[FEEDER_VERTICAL].Moving))
			{
				PlateTakingStep = 15;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				PlateTakingTimer.IN = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 10;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}
/* Warten auf Feeder Horiz. in Entnahmepos.*/
		case 15:
		{
			if( isPositionOk(FEEDER_HORIZONTAL,FeederParameter.HorizontalPositions.TakePlate,0) )
			{
				PlateTakingStep = 16;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 5;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterHorizontal++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 500;
			break;
		}
		case 16:
		{

/*runterfahren bis auf Bremsposition*/

			if(PanoramaAdapter)
				TargetPosition = labs(FeederParameter.PanoramaAdapter) - FeederParameter.SlowDownPos;
			else
/* erste Platte? Bremsposition noch unbekannt */
				if( StackDetect[0] || StackDetect[1] )
					TargetPosition = pTrolley->EmptyPositions[CurrentStack];
				else
					TargetPosition = LastStackPos - abs(FeederParameter.SlowDownPos);

			if ( PanoramaAdapter || StackDetect[0] || StackDetect[1])
				TargetSpeed = FeederParameter.SpeedUpFast/3;
			else
				TargetSpeed = FeederParameter.SpeedUpFast;

			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      TargetPosition,
			                      TargetSpeed,
			                      &MotStep) )
			{
				PlateTakingStep = 230;
			}
			break;
		}
/*HA 17.02.04 V1.74 new step to avoid hanging in step 18 */
		case 230:
		{

			if( 	isPositionOk(FEEDER_VERTICAL,TargetPosition,100)
				||	(Motors[FEEDER_VERTICAL].Moving)
				|| BeltLoose)
			{
				PlateTakingStep = 18;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 16;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterVertical++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}

		case 18:
		{
			if (BeltLoose) /*Stapel erreicht*/
			{
				PlateTakingStep = 22;
				break;
			}

			if(TwoStacks)
			{
				if(CurrentStack == LEFT)
				{
					LastStackPos = LastStackPosLeft;
				}
				else
				{
					LastStackPos = LastStackPosRight;
				}
			}

/* Pos der letzten Entnahme erreicht-> bremsen*/

/*HA 04.02.04 V1.75 erstmal nur auf Bremsposition fahren*/
			if( isPositionOk(FEEDER_VERTICAL,TargetPosition,0) )
			{
				if (SendMotorCmd(FEEDER_VERTICAL,CMD_SPEED,TRUE,FeederParameter.SpeedUpSlow))
					PlateTakingStep = 19;
			}
			break;
		}



		case 19:
		{
/*runterfahren bis auf Plattenstapel*/
/* move finally down onto platestack */
			if(PanoramaAdapter)
				TargetPosition = FeederParameter.PanoramaAdapter;
			else
				TargetPosition = pTrolley->EmptyPositions[CurrentStack];

			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      TargetPosition,
			                      DONT_SEND_SPEED,
			                      &MotStep) )
			{
				PlateTakingStep = 22;
			}
			break;
		}

		case 22:
		{
/*Panorama Adapter: no Stack height detection*/
			if(PanoramaAdapter)
			{
				StackDetect[0]=0;
				StackDetect[1]=0;
			}

/* Position erreicht und Riemen noch stramm -> Wagen leer!*/
			if ( isPositionOk(FEEDER_VERTICAL,TargetPosition,20)
					&& !BeltLoose)
			{
		/* WAGEN LEER*/
				PlateTakingStep = 75;
				break;
			}

/* belt is loose (feeder is on platestack)*/
			if(BeltLoose)
			{
/* stack a lot higher than before? -> new height detection */
				if(    isPositionLowerThan(FEEDER_VERTICAL, labs(CurrentData.LastStackPos) - 50000)
				    && !PanoramaAdapter )
				{
					StackDetect[0] = 1;
					StackDetect[1] = 1;
				}
				PlateTakingStep = 212;
			}
			break;
		}


		case 212:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_STOP,FALSE,0L))
				PlateTakingStep = 213;
			break;
		}

/*HA 03.02.04 V1.75 new steps for moving down a little more after stopping*/
		case 213:
		{
		/*ca 20 mm extra weg*/
			if ( SendMotorCmd(FEEDER_VERTICAL,CMD_RELATIVE_POS,TRUE,
			                 10*Motors[FEEDER_VERTICAL].Parameter.IncrementsPerMm) )
				PlateTakingStep = 214;
			break;
		}

/*set to slow Speed */
		case 214:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_SPEED,TRUE,FeederParameter.SpeedUpSlow) )
				PlateTakingStep = 215;
			break;
		}

		case 215:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L) )
			{
				PlateTakingStep = 216;
				PlateTakingTimer.IN = 0;
			}
			break;
		}

		case 216:
		{
/*wait shortly*/
			PlateTakingTimer.PT = 10;	/* 100ms should be enough, vacuum on time */
			PlateTakingTimer.IN = 1;

			if( PlateTakingTimer.Q )
			{
				PlateTakingTimer.IN = 0;
				if(CheckForPaper(PlateToDo.PlateConfig))
				{
					PaperDetectCounter++;
					if(PaperDetectCounter < 3)
					{
						PlateTakingStep = 240;
					}
					else
					{
						PaperDetectCounter = 0;
						PlateTakingStep = 76;
						START = 0;
						FeederVacuumError = TRUE;
						AlarmBitField[33] = TRUE;
						AbfrageText1 = 57;
						AbfrageText2 = 0;
						AbfrageText3 = 58;
						break;
					}
				}
				else
				{
					PlateTakingStep = 23;
				}
			}
			break;
		}
/***************************************************************************/
/***************************************************************************/

/* move up to Pos above Paper remove*/
		case 240:
		{
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.EnablePaperRemove,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
			{
				PlateTakingStep = 243;
			}
			break;
		}

		case 243:
		{
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.EnablePaperRemove,100)
			||	(Motors[FEEDER_VERTICAL].Moving))
			{
				PlateTakingStep = 244;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 240;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterVertical++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}

		case 244:
		{
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.EnablePaperRemove,100) )
			{
				PaperRemoveReady = FALSE;
				PlateTakingStep = 9;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
			break;
		}


/***************************************************************************/
/***************************************************************************/

/*HA 08.07.03 V1.13 new step for waiting on expose start (avoid suction marks)*/
		case 23:
		{
/*HA 20.02.04 V1.76 no more START cmd? don't take plate*/
			if ( (!START) && AUTO )
			{
	/*special steps for lifting feeder to upper pos*/
				PlateTakingStep = 76;
				break;
			}

/*HA 27.11.03 V1.70 wait for exposer automatically if Plate size>500mm */
			if( (FeederParameter.WaitForExposer || (PlateParameter.Length > 500.0) ) && AUTO )
			{
				if ( (AdjustReady || AdjustStart ) && !ExposingStarted )
				{
/* if no START command anymore (Stop button pressed) */
					if ( !START )
					{
			/*special steps for lifting feeder to upper pos*/
						PlateTakingStep = 76;
					}
					break;
				}
			}

/*reset flag*/
			ExposingStarted = 0;

/*decide, if stack detection to be done*/
			if(TwoStacks)
			{
				if(		(CurrentStack == LEFT && StackDetect[LEFT])
					||	(CurrentStack == RIGHT && StackDetect[RIGHT]) )

					PlateTakingStep = 100;
				else
				{
					SuckingCounter = 0;
					PlateTakingStep = 220;
				}
			}
			else /*only one stack*/
			{
				if(StackDetect[LEFT] || StackDetect[RIGHT])
					PlateTakingStep = 100;
				else
				{
					SuckingCounter = 0;
					PlateTakingStep = 220;
				}
			}
			PlateTakingTimer.IN = 0; /*to have an edge in next step*/
			break;
		}

		case 26:
		{
/*Feeder heben bis kurz unter Übergabepos.*/
/* lift feeder up to pre-release position */
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.PreRelease,
			                      FeederParameter.SpeedUpSlow,
			                      &MotStep) )
			{
				PlateTakingStep = 29;
				DecrementPlatesInTrolley();
				if(PlatesToDo > 0 && CounterOn)
					PlatesToDo--;
			}
			break;
		}
		case 29:
		{
			if(Motors[FEEDER_VERTICAL].Moving)
			{
				PlateTakingStep = 30;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 26;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterVertical++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}

/*HA 06.05.03 V1.06 new step 30, original step 30 is now 31*/
/*wait, til belt is tightened again and save that position */
		case 30:
		{
			if(!BeltLoose)	/*wait, till belt is tightened*/
			{
				LastStackPos = labs(Motors[FEEDER_VERTICAL].Position);
/*HA 07.05.03 V1.06 optimization: more accurate LastStackPos*/
				if(TwoStacks)
				{
					if(CurrentStack == LEFT)
					{
						LastStackPosLeft = LastStackPos;
					}
					else
					{
						LastStackPosRight = LastStackPos;
					}
				}
				PlateTakingStep = 31;
			}
			break;
		}

		case 31:
		{
/*wait til feeder is in speed change pos*/
			if( isPositionHigherThan(FEEDER_VERTICAL,
			                         labs(LastStackPos) - labs(FeederParameter.VerticalPositions.SpeedChangePosition) ) )
				break;

			if (FeederParameter.IgnoreVacuumSwitch || PlateTransportSim)
			{
				if(!PlateAtFeeder.present)/* nur einmal hier durch!*/
				{
					PlateAtFeeder = PlateToDo;
					PlateAtFeeder.present = TRUE;
					if( (PlateToDo.NextPlateType == PlateToDo.PlateType)
/* 1.27 160112 Maschine 29110 Manila Bulletin
					 && (PlateToDo.PlateConfig != LEFT)
					 && (PlateToDo.PlateConfig != RIGHT)
*/
					  )
					{
						PlateToDo.NextPlateType = 0;
						strcpy(PlateToDo.ID,"???");
					}
					else
						ClearStation(&PlateToDo);
				}
			}
			else
			{
				if ( !CheckFeederVacuum(PlateToDo.PlateType,PlateToDo.PlateConfig) )
				{
					PlateTakingStep = 76;
					START = FALSE;
					FeederVacuumError = TRUE;
					AlarmBitField[28] = TRUE;
					AbfrageText1 = 47;
					AbfrageText2 = 0;
					AbfrageText3 = 49;
					SwitchFeederVacuum(0,ALL,OFF);
					SwitchAdjustVacuum(0,ALL,OFF);
					ClearStation(&PlateAtFeeder);
					DontCloseTrolley = TRUE;
					break;
				}
				else
				{
					if(!PlateAtFeeder.present)/* nur einmal hier durch!*/
					{
						PlateAtFeeder = PlateToDo;
						PlateAtFeeder.present = TRUE;
						if( (PlateToDo.NextPlateType == PlateToDo.PlateType)

/* 1.27 160112 Maschine 29110 Manila Bulletin
						 && (PlateToDo.PlateConfig != LEFT)
						 && (PlateToDo.PlateConfig != RIGHT)
*/
						  )
						{
							PlateToDo.NextPlateType = 0;
							strcpy(PlateToDo.ID,"???");
						}
						else
							ClearStation(&PlateToDo);
					}
				}
			}


/* switch to fast*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_SPEED,TRUE,FeederParameter.SpeedUpFast))
			{
				if(FeederParameter.ShakeRepitions>0)
					PlateTakingStep = 150;
				else
					PlateTakingStep = 34;
			}

			if(AlternatingTrolleys && TwoStacks)
			{
				CurrentStack = !CurrentStack;
			}

			break;
		}
		case 34:
		{
/*Bewegung starten*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L))
			{
				PlateTakingStep = 35;
				PaperRemoveReady = FALSE;
			}
			break;
		}
		case 35:
		{
/*wait til feeder is above trolley*/
			if( isPositionHigherThan(FEEDER_VERTICAL,
		                            labs(FeederParameter.VerticalPositions.AboveTrolley)) )
				break;

/* don't move horizontically */
			PlateTakingStep = 38;
			break;

		}


/****************************************************/
/*warten, bis Tisch weg ist, dann über den Tisch heben (ReleasePos*/
/****************************************************/
		case 38:
		{
/*Tisch muss soweit reingefahren sein, dass die Platte hochgehoben werden kann */
/* - 50 mm Sicherheit*/

/*
 * wenn Panoramaadapter aktiv ist, liegt die Platte ausser mittig, also müssen wir weiter
 * rein fahren
*/
			REAL tmppos;

			if (PanoramaAdapter)
			{
				tmppos = 75.0;
			}
			else
			{
/* bei flexibler Zuf. hängt der Wert von der aktuellen Motor-Pos. ab*/
				if(GlobalParameter.FlexibleFeeder)
					tmppos = FeederParameter.MinTablePosDrop +
							Motors[FEEDER_HORIZONTAL].Position_mm - (PlateParameter.Length / 2) - 50;
				else
					tmppos = (FeederParameter.MinTablePosDrop - (PlateParameter.Length / 2)- 50);
			}
			if(tmppos < 0)
				tmppos = 0;


			if ( FU.aktuellePosition_mm < tmppos )
				PlateTakingStep = 40;
			else
			{
/* wenn eine Belichtung ansteht, dann wird der Tisch irgendwann von da aus getriggert,
also warten wir*/
				if ( (ExposeStart  || AdjustReady)&& AUTO)
					nop();
				else
					PlateTakingStep = 39;
			}
			break;
		}

		case 39:
		{
/*noch nicht da, ok, dann hinfahren*/

			if ( FU.cmd_Target == 0 )
			{
				if (PanoramaAdapter)
					FU.TargetPosition_mm = 70.0;
				else
					FU.TargetPosition_mm = tmppos;

				FU.cmd_Target = 1;
/*Rücksprung in den Prüfschritt*/
				PlateTakingStep = 38;
			}
			break;
		}

		case 40:
		{
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.ReleasePlate,
			                      DONT_SEND_SPEED,
			                      &MotStep) )
			{
				PlateTakingStep = 42;
			}
			break;
		}
		case 42:
		{
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.ReleasePlate,100))
			{
				if( GlobalParameter.FlexibleFeeder )
					PlateTakingStep = 43;
				else
					PlateTakingStep = 47;
			}
			break;
		}

/* move feeder hor. to plate releasepos*/
		case 43:
		{
			if( MoveToAbsPosition(FEEDER_HORIZONTAL,
			                      PlateParameter.ReleasePlatePosition,
			                      DONT_SEND_SPEED,
			                      &MotStep) )
			{
				PlateTakingStep = 45;
			}

			break;
		}
		case 45:
		{
/*feeder horizontal in position for plate transport*/
			if ( (	isPositionOk(FEEDER_HORIZONTAL,PlateParameter.ReleasePlatePosition,0) ))
			{

/*HA 12.09.03 V1.60 added Vacuum switches*/
				if (!CheckFeederVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig)
				 && !FeederParameter.IgnoreVacuumSwitch
				 && !PlateTransportSim)
				{
					PlateTakingStep = 76;
					START = FALSE;
					AlarmBitField[28]=TRUE;
					AbfrageText1 = 47;
					AbfrageText2 = 0;
					AbfrageText3 = 51;
					FeederVacuumError = TRUE;
					SwitchFeederVacuum(0,ALL,OFF);
					SwitchAdjustVacuum(0,ALL,OFF);
					ClearStation(&PlateAtFeeder);
					DontCloseTrolley = TRUE;
					break;
				}

/*feeder hor pos ok und Vakuum noch da: weiter*/
				PlateTakingStep = 47;
			}
			break;
		}

/* Tischverriegelung zurücksetzen, um Entladen zu ermöglichen*/
/* und Feeder horizontal verfahren */
		case 47:
		{
			DisableTable = FALSE;
			Out_FeederHorActive = ON;
			if (!DeloadStart && !ExposeStart)
				PlateTakingStep = 48;
			break;
		}

/* Pins up */
		case 48:
		{
			if(AdjPins(PlateAtFeeder.PlateConfig,&PinsStep,&PinsTimer))
				PlateTakingStep = 49;
			break;
		}

/* Tisch in Übergabepos fahren */
		case 49:
		{
			DINT Offset = 0;
		/*
		 * calculate plate dependant offset
		 */
			if(  GlobalParameter.FlexibleFeeder /* do we have a flex. feeder at all? */
			 && (Motors[FEEDER_HORIZONTAL].Parameter.IncrementsPerMm > 0.1) /*avoid div by 0*/
			  )
			{
				Offset =(DINT)(PlateParameter.ReleasePlatePosition /
					           Motors[FEEDER_HORIZONTAL].Parameter.IncrementsPerMm);
			}

			TablePosDrop = FeederParameter.MinTablePosDrop;

/*Sicherheit: Position prüfen und ggf. begrenzen*/
			if ( TablePosDrop > X_Param.MaxPosition )
			{
				TablePosDrop = X_Param.MaxPosition-1;
			}

/*kann zwar nicht vorkommen, aber sicher ist sicher...*/
			if ( TablePosDrop < X_Param.MinPosition )
				TablePosDrop = X_Param.MinPosition+1;

/*schon da? sollte der Fall sein nach Entladen...*/
			if ( abs(FU.aktuellePosition_mm - TablePosDrop)< 2 )
			{
/*Tisch ist in Position -> OK, weiter*/
				PlateTakingStep = 52;
				break;
			}
/*noch nicht da, ok, dann hinfahren*/
			if ( FU.cmd_Target == 0 )
			{
				FU.TargetPosition_mm = TablePosDrop;
				FU.cmd_Target = TRUE;
				PlateTakingStep = 50;
			}
			break;
		}
		case 50:
		{
/*und warten*/
			if( (abs(FU.aktuellePosition_mm - FU.TargetPosition_mm) < 1.0)
			&& 	(FU.cmd_Target == FALSE) )
			{
/*Tisch ist in Position -> OK, weiter*/
				PlateTakingStep = 52;
			}
			break;
		}

/* Adjustsucker Vac on*/
		case 52:
		{
			SwitchAdjustVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig,ON);
			PlateTakingStep = 53;
			break;
		}
/*check for feeder horizonzal*/
		case 53:
		{
			Out_FeederHorActive = ON;
			if(  Out_FeederHorActive
			 &&  In_FeederHorActive
			 && !In_FeederHorInactive
			  )
				PlateTakingStep = 54;
			break;
		}

/* Feeder down onto table */
		case 54:
		{
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      AdjusterParameter.FeederFixposition,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
			{
				Out_LiftingPinsUp = OFF;
				Out_LiftingPinsDown = ON;
				PlateTakingStep = 55;
			}

			break;
		}


		case 55:
		{
			if ( isPositionOk(FEEDER_VERTICAL,AdjusterParameter.FeederFixposition,0) )
			{
				SwitchFeederVacuum(0,ALL,OFF);
				PlateTakingStep = 60;
			}

			break;
		}

		case 60:
		{

/* kein timeout, Schalter geht weg */
/* HA 03.08.04 V1.82 Zeitmessung Vakuum EIN*/
			if (!CheckFeederVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig)
			 || FeederParameter.IgnoreVacuumSwitch
			 || PlateTransportSim) /*alles ok, Zeit messen und in Warteschritt*/
			{
				VacuumSwitchOffTime = PlateTakingTimer.ET;
				if (VacuumSwitchOffTime < VacuumSwitchOffMinTime)
					VacuumSwitchOffMinTime = VacuumSwitchOffTime;
				if (VacuumSwitchOffTime > VacuumSwitchOffMaxTime)
					VacuumSwitchOffMaxTime = VacuumSwitchOffTime;

				VacuumSwitchOffAVCNT++;
				VacuumSwitchOffAV = ((VacuumSwitchOffAV * (VacuumSwitchOffAVCNT-1) ) + VacuumSwitchOffTime) / VacuumSwitchOffAVCNT;

				PlateTakingStep = 70;
				PlateTakingTimer.IN = 0;
			}
			break;
		}

		case 70:
		{
			PlateTakingStart = 0;
			PlateTakingStep = 0;
			if(AUTO)
				AdjustStart = TRUE;
			break;
		}


/*************************************************/
/* special steps for trolley empty condition*/
/*************************************************/
		case 75:
		{
/*trolley empty flag only, if no panoramaadapter*/
			if(!PanoramaAdapter)
			{
				if(TwoStacks)
				{
					if(Trolleys[GlobalParameter.TrolleyLeft].Double)
					{
						if(CurrentStack == LEFT)
						{
							pTrolley->EmptyLeft = 1;
							pTrolley->PlatesLeft = 0;
						}
						else
						{
							pTrolley->EmptyRight = 1;
							pTrolley->PlatesRight = 0;
						}
					}
					else
					{
						pTrolley->EmptyLeft = 1;
						pTrolley->PlatesLeft = 0;
					}

					CurrentStack = !CurrentStack;
/*HA 01.10.03 V1.64 use AlternatingTrolleys instead of GlobalParameter.AlternatingTrolleys during*/
/*plate taking to avoid resetting of the Global param*/
					AlternatingTrolleys = 0;
				}
				else /*one stack*/
					pTrolley->EmptyLeft = 1;
			}

/* Q+D empty flag nicht setzen*/
			pTrolley->EmptyLeft = 0;
			pTrolley->EmptyRight = 0;

			PlateTakingStep = 76;
			break;
		}

		case 76:
		{
/*Feeder heben in pos oben*/
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.Up,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
				PlateTakingStep = 82;
			break;
		}

		case 82:
		{
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.Up,0)
			||	(Motors[FEEDER_VERTICAL].Moving) )
			{
				if ( !START )	/*START was reset, so it's no trolley empty condition*/
					PlateTakingStep = 88;	/*directly to end of sequence*/
				else
					PlateTakingStep = 85;
				TimeoutTimer.IN = 0;
				RetryCounter = 0;
				break;
			}
/*Timeout check*/
			if( TimeoutTimer.Q )
			{
				PlateTakingStep = 76;
				RetryCounter++;
				TimeoutTimer.IN = 0;
				ErrorCounterVertical++;
				break;
			}
			TimeoutTimer.IN = 1;
			TimeoutTimer.PT = 100;
			break;
		}

		case 85:
		{
			TrolleyEmpty = 1;
			PlateTakingStep = 88;
/*HA 02.04.03 Stack detection on next plate taking*/
			StackDetect[0] = 1;
			StackDetect[1] = 1;
			START = 0;
/*END HA 02.04.03*/
			break;
		}

		case 88:
		{
/* if Feeder is up */
			if ( isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.Up,0) )
			{
				PlateTakingStart = 0;
				PlateTakingStep = 0;
				DisableTable = FALSE;
				PaperRemoveReady = 0;

/*HA 12.09.03 V1.60 added Vacuum switches*/
				if (FeederVacuumError)
				{
					FeederVacuumError = 0;
					if(wBildAktuell != 41 )
						OrgBild = wBildAktuell;
					wBildNeu = 41;
					AbfrageOK = 0;
					AbfrageCancel = 0;
					IgnoreButtons = 1;
					OK_CancelButtonInv = 1;
					OK_ButtonInv = 0;
					AbfrageIcon = CAUTION;
				}

			}
			break;
		}

/********************************************************************************/
/* Special sequence for stack height detection					*/
/********************************************************************************/
		case 100:
		{
/*move up til belt is tightened again*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_RELATIVE_POS,TRUE,-50000))
				PlateTakingStep = 102;
			break;
		}

/*set to slow Speed */
		case 102:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_SPEED,TRUE,FeederParameter.SpeedUpSlow))
				PlateTakingStep = 105;
			break;
		}

		case 105:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L))
				PlateTakingStep = 110;
			break;
		}
		case 110:
		{
/*wait til belt is tightened, then stop motor*/
			if(!BeltLoose) PlateTakingStep = 111;
			break;
		}

		case 111:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_STOP,FALSE,0L))
			{
				PlateTakingStep = 115;
				PlateTakingTimer.IN = 0; /*to have an edge in next step*/
			}
			break;
		}
		case 115:
		{
/*Feeder down very slowly*/
			if( MoveToAbsPosition(FEEDER_VERTICAL,
								  pTrolley->EmptyPositions[CurrentStack],
			                      FeederParameter.StackDetectSpeed,
			                      &MotStep) )
				PlateTakingStep = 120;
			break;
		}

		case 120:
		{
/*wait til belt is loose, then stop motor*/
			if(BeltLoose)
			{
				StackDetectPosition = labs(Motors[FEEDER_VERTICAL].Position);
				if( SendMotorCmd(FEEDER_VERTICAL, CMD_STOP, FALSE, 0L) )
				{
					PlateTakingStep = 132;
					SwitchFeederVacuum(PlateToDo.PlateType,PlateToDo.PlateConfig,ON);
					PlateTakingTimer.IN = FALSE;
				}
			}
			else
			{
/*HA 07.05.03 V1.06 check for empty trolley by position instead of time*/
/* position reached and belt still not loose -> trolley is empty*/
				if( isPositionOk(FEEDER_VERTICAL,
				    pTrolley->EmptyPositions[CurrentStack],
				    20L ) )
				{
					PlateTakingTimer.IN = 0;
					PlateTakingStep = 145;
					break;
				}
			}
			break;
		}

/*HA 03.02.04 V1.75 new steps for moving down a little more after detection*/
		case 132:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_RELATIVE_POS,TRUE,
			                10*Motors[FEEDER_VERTICAL].Parameter.IncrementsPerMm))
				PlateTakingStep = 135;
			break;
		}

/*set to slow Speed */
		case 135:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_SPEED,TRUE,FeederParameter.SpeedUpSlow))
				PlateTakingStep = 138;
			break;
		}

		case 138:
		{
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L))
			{
				PlateTakingStep = 140;
				PlateTakingTimer.IN = FALSE;
			}
			break;
		}

		case 140:
		{
/*wait shortly*/
			PlateTakingTimer.PT = 50;	/* 1/2 sec should be enough*/
			PlateTakingTimer.IN = TRUE;

			if( PlateTakingTimer.Q )
			{
				PlateTakingStep = 220;
				PlateTakingTimer.IN = FALSE;
			}
			break;
		}

/********************************************************************************/
		case 145:
		{
/*error in detection:*/
/*stop motor and jump to step 75 (trolley empty)*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_STOP,FALSE,0L))
			{
				PlateTakingStep = 75;
				SwitchFeederVacuum(0,ALL,OFF);
				SwitchAdjustVacuum(0,ALL,OFF);
				PlateTakingTimer.IN = FALSE;
			}
			break;
		}


/*************************************************************************/
/*************************************************************************/
/*HA 19.05.03 V1.07 new steps for shaking*/
		case 150:
		{
/*stop motor */
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_STOP,FALSE,0L))
			{
				PlateTakingStep = 152;
				PlateTakingTimer.IN = FALSE;
				ToggleFlag = 0;
				ShakeCounter = 0;
			}
			break;
		}
		case 152:
		{
			if( !ToggleFlag )
				tmpval = FeederParameter.ShakeWay;
			else
				tmpval = (-1)*FeederParameter.ShakeWay;

			if (SendMotorCmd(FEEDER_VERTICAL,CMD_RELATIVE_POS,TRUE,tmpval))
			{
				ToggleFlag = !ToggleFlag;
				PlateTakingStep = 155;
			}
			break;
		}
		case 155:
		{
/*Bewegung starten*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_START_MOTION,FALSE,0L))
			{
				PlateTakingStep = 160;
				PlateTakingTimer.PT = FeederParameter.ShakeTime;
				PlateTakingTimer.IN = TRUE;
			}
			break;
		}
		case 160:
		{
			if(PlateTakingTimer.Q)
			{
				PlateTakingTimer.IN = FALSE;
				if( ShakeCounter++ >= FeederParameter.ShakeRepitions )
				{
					PlateTakingStep = 165;
					ShakeCounter = 0;
				}
				else
					PlateTakingStep = 152;
			}
			break;
		}
		case 165:
		{
/*Feeder heben bis kurz unter Übergabepos.*/
			if (SendMotorCmd(FEEDER_VERTICAL,CMD_ABSOLUTE_POS,TRUE,FeederParameter.VerticalPositions.PreRelease))
				PlateTakingStep = 34;
			break;
		}

/*************************************************************************/
/*************************************************************************/
/* Tischpos prüfen und ggf. wegfahren  */
		case 180:
		{
			if(GlobalParameter.FlexibleFeeder)
			{
				if( Motors[FEEDER_HORIZONTAL].Position_mm > HALFSUCKERDISTANCE )
					LocalTableMaxPos = FeederParameter.MaxTablePosition;
				else
					LocalTableMaxPos = FeederParameter.MaxTablePosition - HALFSUCKERDISTANCE
							 			+ Motors[FEEDER_HORIZONTAL].Position_mm;
			}
			else
				LocalTableMaxPos = FeederParameter.MaxTablePosition;

			if ( FU.aktuellePosition_mm < LocalTableMaxPos )
			{
/*Tisch Position ist kleiner als Max -> OK, weiter*/
				if(!DeloadStart && !FU.cmd_Target && !FU.cmd_Park)
				{
					DisableTable = TRUE;
					PlateTakingStep = 1;
				}
			}
			else
			{
/* wenn eine Belichtung ansteht, dann wird der Tisch irgendwann von da aus getriggert,
also warten wir*/
				if ( (ExposeStart  || AdjustReady) && AUTO)
				{
				/*
				 * flexible Zuführung? wenn ja: schonmal Position für Plattenaufnahme anfahren
				 */
					if( GlobalParameter.FlexibleFeeder )
					{
						if( !isPositionOk(FEEDER_HORIZONTAL,FeederParameter.HorizontalPositions.TakePlate,0) )
							PlateTakingStep = 181;
					}

/*no more Start-cmd: Stop, move feeder up */
					if ( (!START) && AUTO )
					{
						PlateTakingStep = 76;
						break;
					}
				}
				else
					PlateTakingStep = 185;
			}
			break;
		}
/* schonmal die horizontalbewegung des Feeders machen */
		case 181:
		{
/*move Feeder horizontally in plate taking pos.*/
			if (SendMotorCmd(FEEDER_HORIZONTAL,CMD_ABSOLUTE_POS,1,FeederParameter.HorizontalPositions.TakePlate))
				PlateTakingStep = 182;
			break;
		}
		case 182:
		{
/*start cmd*/
			if (SendMotorCmd(FEEDER_HORIZONTAL,CMD_START_MOTION,FALSE,0L))
				PlateTakingStep = 183;
			break;
		}
		case 183:
		{
/*start cmd*/
			if( isPositionOk(FEEDER_HORIZONTAL,FeederParameter.HorizontalPositions.TakePlate,0) )
				PlateTakingStep = 180;
			break;
		}

/*wenn Entladen aktiv ist, dann warten wir das ab (crashgefahr!)*/
		case 185:
		{
			if(DeloadStart)
				break;

			PlateTakingStep = 186;

			break;
		}

		case 186:
		{
/* Tisch aus dem Weg fahren */
			if ( FU.cmd_Target == 0 )
			{
				FU.TargetPosition_mm = LocalTableMaxPos;
				FU.cmd_Target = 1;
				PlateTakingStep = 187;
			}
			break;
		}

		case 187:
		{
/*und warten*/
			if( (abs(FU.aktuellePosition_mm - FU.TargetPosition_mm) < 1.0)
			&& 	(FU.cmd_Target == 0) )
			{
/*Tisch Position ist kleiner als Max -> OK, weiter*/
				DisableTable = TRUE;
				PlateTakingStep = 1;
			}
			break;
		}

/*************************************************************************/
/*************************************************************************/
/*HA 19.05.03 V1.07 new steps for several sucking attempts*/
		case 220:
		{
/*vacuum on, check switch*/
			SwitchFeederVacuum(PlateToDo.PlateType,PlateToDo.PlateConfig,ON);

			PlateTakingTimer.PT = 700; /*7 sec timeout-time*/
			PlateTakingTimer.IN = 1;

			if (CheckFeederVacuum(PlateToDo.PlateType,PlateToDo.PlateConfig)
			 || FeederParameter.IgnoreVacuumSwitch
			 || PlateTransportSim) /*Schalter OK*/
			{
/* HA 03.08.04 V1.82 Zeitmessung Vakuum EIN*/
				VacuumSwitchTime = PlateTakingTimer.ET;
				if (VacuumSwitchTime<VacuumSwitchMinTime)
					VacuumSwitchMinTime = VacuumSwitchTime;
				if (VacuumSwitchTime>VacuumSwitchMaxTime)
					VacuumSwitchMaxTime = VacuumSwitchTime;

				VacuumSwitchAVCNT++;
				VacuumSwitchAV = ((VacuumSwitchAV * (VacuumSwitchAVCNT-1) ) + VacuumSwitchTime) / VacuumSwitchAVCNT;

				PlateTakingStep = 221;
				PlateTakingTimer.IN = 0;
				break;
			}

			if(PlateTakingTimer.Q) /*TIMEOUT*/
			{
				PlateTakingStep = 76;
				START = 0;
				FeederVacuumError = 1;
				AlarmBitField[28]=1;
				AbfrageText1 = 47;
				AbfrageText2 = 0;
				AbfrageText3 = 49;
				SwitchFeederVacuum(0,ALL,OFF);
				SwitchAdjustVacuum(0,ALL,OFF);
				ClearStation(&PlateAtFeeder);
				DontCloseTrolley = 1;

				PlateTakingTimer.IN = 0;
				break;
			}
			break;
		}
/*neuer Warteschritt*/
		case 221:
		{
/*wait shortly*/
			PlateTakingTimer.PT = FeederParameter.VacuumOnTime;
			PlateTakingTimer.IN = 1;
			if(PlateTakingTimer.Q)
			{
				if( SuckingCounter++ >= FeederParameter.SuckRepitions )
				{
					SwitchAdjustVacuum(PlateToDo.PlateType,PlateToDo.PlateConfig,ON);
					PlateTakingStep = 26;
					SuckingCounter = 0;
				}
				else
					PlateTakingStep = 225;
				PlateTakingTimer.IN = 0;
			}
			break;
		}

		case 225:
		{
/*vacuum off, wait shortly*/
			SwitchFeederVacuum(0,ALL,OFF);
			PlateTakingTimer.PT = FeederParameter.VacuumOffTime;
			PlateTakingTimer.IN = 1;
			if(PlateTakingTimer.Q)
			{
				PlateTakingStep = 220;
				PlateTakingTimer.IN = 0;
			}
			break;
		}

	}	/*switch*/
}


