#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Platte Ausrichten */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			16.09.02	erste Implementation					HA		*/
/*	1.1			30.10.02	Wenn Ausrichten fehlschlägt wird nicht 			*/
/*							belichtet								HA		*/
/*	1.11			02.04.03	Fehlermeldung bei Ausrichtfehler	HA		*/
/*																			*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"
#include "asstring.h"
#include "in_out.h"
#include <string.h>
#include "Motorfunc.h"
#include "auxfunc.h"

_LOCAL	USINT	AdjustStep;
_LOCAL TON_10ms_typ AdjustTimer,AddBlowairValveTimer;
USINT			tmp[20];

_GLOBAL	REAL	DropPosition;
_GLOBAL BOOL	AdjustFailed;
_GLOBAL BOOL	StopPlateTaking;
/*HA 07.05.03 V1.06*/
_GLOBAL BOOL	LastPlateIsReady;
/*HA 05.09.03 V1.51 to avoid adjustsensor triggered by deloaded plate*/
_GLOBAL 	BOOL			DisableAdjustSensor;


_LOCAL	USINT	FailCounter;
static USINT MotStep;
static BOOL tmpfail;
static BOOL LeftWasOK,RightWasOK;
static    USINT               PinsStep;       /* holds current step of fct AdjPins */
static    TON_10ms_typ        PinsTimer;

/****************************************************************/
/****************************************************************/
_LOCAL	UINT ManualLoadingStep;
static	TON_10ms_typ ManualLoadingTimer;
_GLOBAL	REAL	TablePosDrop;

/****************************************************************/
/****************************************************************/

_INIT void init(void)
{
	PinsStep = 0;
	PinsTimer.IN = FALSE;
	ManualLoadingStep = 0;
	ManualLoadingStart = FALSE;
	ManualLoadingTimer.IN = FALSE;
	MotStep = 0;
	AdjustStep		= 0;
	AdjustStart	= 0;
	AdjustReady		= 0;
	AdjustTimer.IN	= 0;
	AdjustTimer.PT	= 10; /*just to have any value in there...*/
	AdjustFailed	= 0;
	FailCounter		= 0;
	AddBlowairValveTimer.IN = 0;
	AddBlowairValveTimer.PT = 50;
	return;
}


/****************************************************************************/
/* zyklischer Teil*/
/****************************************************************************/
_CYCLIC void cyclic(void)
{
	SequenceSteps[3] = AdjustStep;

/*validate*/
	if (AdjusterParameter.MaxFailedPlates > 10)
			AdjusterParameter.MaxFailedPlates = 3;
	if ( (AdjusterParameter.AdjustSim != 0)&& (AdjusterParameter.AdjustSim != 1))
		AdjusterParameter.AdjustSim = 0;


	TON_10ms(&AdjustTimer);
	TON_10ms(&ManualLoadingTimer);
	TON_10ms(&PinsTimer);

/* bei Tischblasluft wird jetzt nach 0,5 sec das Vakuumventil zugeschaltet
 * um den Luftdurchsatz zu erhöhen, Option schaltbar durch Parameter
 * "AdjusterParameter.AddBlowairValveDelay" 0->AUS, 1->EIN
*/
	if (AdjusterParameter.AddBlowairValveDelay > 1)
		AdjusterParameter.AddBlowairValveDelay = 1;

	if (AdjusterParameter.AddBlowairValveDelay > 0)
	{
		AddBlowairValveTimer.PT = 50;
		AddBlowairValveTimer.IN = AdjustStart && AdjustBlowairOn;
		TON_10ms(&AddBlowairValveTimer);

		if (AddBlowairValveTimer.Q )
		{
			AdjustVacuumOn[0] = ON;
			AdjustVacuumOn[1] = ON;
		}
	}
/*safety*/
	if( !AdjustStart )
	{
		AdjustStep = 0;
	}

	switch (AdjustStep)
	{
		case 0:
		{
			LeftWasOK = FALSE;
			RightWasOK = FALSE;
			if( !AdjustStart
			 ||  ExposeStart
			 ||  DeloadStart
			 ||  PlateTakingStart
			  )
				break;
/************************************************/
/*
			if( isPositionHigherThan(FEEDER_VERTICAL,
		                            labs(AdjusterParameter.FeederFixposition) + 20L)
			 || PlateTakingStart
		      )
			{
				AdjustStart = FALSE;
		    	break;
		    }
*/
/****************************/

			AdjustTimer.IN = FALSE;
			AdjustReady = FALSE;
			FailCounter = 0;

			if(   In_FeederHorActive
			  && !In_FeederHorInactive
			  &&  Out_FeederHorActive
			  )
				AdjustStep = 200;
			else
				AdjustStep = 2;
			break;
		}

		case 2:
		{
			Out_FeederHorActive = TRUE;
			if(In_FeederHorActive && !In_FeederHorInactive)
				AdjustStep = 200;

			break;
		}

/* Pins up */
		case 200:
		{
			if(AdjPins(PlateAtFeeder.PlateConfig,&PinsStep,&PinsTimer))
				AdjustStep = 202;
			break;
		}

/* Tisch in Übergabepos fahren */
		case 202:
		{
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
				AdjustStep = 5;
				break;
			}
/*noch nicht da, ok, dann hinfahren*/
			if ( FU.cmd_Target == 0 )
			{
				FU.TargetPosition_mm = TablePosDrop;
				FU.cmd_Target = TRUE;
				AdjustStep = 205;
			}
			break;
		}
		case 205:
		{
/*und warten*/
			if( (abs(FU.aktuellePosition_mm - FU.TargetPosition_mm) < 1.0)
			&& 	(FU.cmd_Target == FALSE) )
			{
/*Tisch ist in Position -> OK, weiter*/
				AdjustStep = 5;
			}
			break;
		}

		case 5:
		{
			if(isPositionOk(FEEDER_VERTICAL,AdjusterParameter.FeederAdjustposition,0))
				AdjustStep = 20;
			else
				AdjustStep = 10;
			break;
		}

/* Feeder down onto table */
		case 10:
		{
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      AdjusterParameter.FeederAdjustposition,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
			{
				AdjustStep = 20;
				Out_AdjustSignalCross = (PlateAtFeeder.PlateConfig == BROADSHEET);
				AdjustTimer.IN = FALSE;
			}

			break;
		}
/* wait for feeder */
		case 20:
		{
			if(isPositionOk(FEEDER_VERTICAL,AdjusterParameter.FeederAdjustposition,0))
			{
				SwitchFeederVacuum(0,ALL,OFF);
				SwitchAdjustVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig,ON);
				AdjustBlowairOn = ON;
				tmpfail = FALSE;
/* damit die Sauger auch bei Transport Simulation bewegt werden*/
				MoveAdjustSucker(PlateAtFeeder.PlateConfig, ACTIVE);
				AdjustStep = 30;
				LeftWasOK = FALSE;
				RightWasOK = FALSE;
				break;
			}
			break;
		}

		case 30:
		{
			BOOL tmpOK;

			if(PlateAtFeeder.present)
			{
				if(PlateAtFeeder.PlateConfig == BOTH)
				{
					if(AdjustOK(LEFT))
						LeftWasOK = TRUE;
					if(AdjustOK(RIGHT))
						RightWasOK = TRUE;

					tmpOK = LeftWasOK && RightWasOK;
				}
				else
					tmpOK = AdjustOK(PlateAtFeeder.PlateConfig);
			}
			else
				tmpOK = AdjustOK(LEFT);

			if(tmpOK)
			{
				AdjustTimer.IN = FALSE;
				tmpfail = FALSE;
				AdjustStep = 40;
				break;
			}
/* timeout, retry */
			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
				tmpfail = TRUE;
				AdjustStep = 40;
				break;
			}
			MoveAdjustSucker(PlateAtFeeder.PlateConfig, ACTIVE);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.TimeoutAdjustedPosition;
			break;
		}


/* Blowair off */
		case 40:
		{
			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
				AdjustStep = 50;
				break;
			}
			AdjustBlowairOn = OFF;
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.BlowairOffDelay;

			break;
		}
/* Vacuum on */
		case 50:
		{

			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
				AdjustStep = 60;
				break;
			}

			SwitchAdjustTableVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig,ON);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.TableVacuumOnDelay;
			break;
		}

		case 60:
		{
/*			if(!CheckAdjustVacuum(0,ALL) || PlateTransportSim)*/
			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
/* adjusting was ok*/
				if(!tmpfail)
				{
					AdjustStep = 130;
					PlateAtAdjustedPosition = PlateAtFeeder;
					break;
				}
				else
					AdjustStep = 70;
				break;
			}
			SwitchAdjustVacuum(0,ALL, OFF);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.AdjVacuumOffDelay;

			break;
		}

		case 70:
		{
			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
				FailCounter++;
				if(FailCounter > AdjusterParameter.MaxAttempts)
				{
					SwitchAdjustTableVacuum(0,ALL,OFF);
					AdjustBlowairOn = OFF;
					AdjustStep = 130;
					tmpfail = TRUE;
					StopPlateTaking = 0;
					START = 0;
					FailCounter = 0;

/* HA 15.09.03 V1.60 Adjusting failed: error message */
					AbfrageText1 = 52;
					if(PlateAtFeeder.PlateConfig == BOTH)
					{
						if(!LeftWasOK && !RightWasOK)
							AbfrageText2 = 22;
						else
						if(!LeftWasOK)
							AbfrageText2 = 20;
						else
						if(!RightWasOK)
							AbfrageText2 = 21;
					}
					else
						AbfrageText2 = 0;

					AbfrageText3 = 53;

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
				else
					AdjustStep = 80;

				break;
			}
			MoveAdjustSucker(ALL, INACTIVE);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.AdjInactiveTime;
 			break;
		}

/*************************************************************************/
		case 80:
		{
/*			if(CheckAdjustVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig))*/
			if(AdjustTimer.Q)
			{
				AdjustTimer.IN = FALSE;
				AdjustStep = 90;
				break;
			}
			SwitchAdjustVacuum(PlateAtFeeder.PlateType,PlateAtFeeder.PlateConfig,ON);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.AdjVacuumOnDelay;
			tmpfail = FALSE;
			break;
		}

		case 90:
		{
			if(AdjustTimer.Q)
			{
				AdjustBlowairOn = ON;
				AdjustTimer.IN = FALSE;
				AdjustStep = 30;
				break;
			}
			SwitchAdjustTableVacuum(0,ALL,OFF);
			AdjustTimer.IN = TRUE;
			AdjustTimer.PT = AdjusterParameter.TableVacuumOffDelay;
			break;
		}
/*************************************************************************/
/* Feeder up */
		case 130:
		{
			if( MoveToAbsPosition(FEEDER_VERTICAL,
			                      FeederParameter.VerticalPositions.Up,
			                      FeederParameter.SpeedUpFast,
			                      &MotStep) )
			{
				AdjustStep = 140;

			}

			break;
		}
/* wait for feeder */
		case 140:
		{
			if(isPositionOk(FEEDER_VERTICAL,FeederParameter.VerticalPositions.Up,0))
			{
				AdjustFailed = tmpfail;
				AlarmBitField[26] = AdjustFailed;
				AdjustReady = !tmpfail;
				ClearStation(&PlateAtFeeder);
				AdjustStep = 150;
				MoveAdjustSucker(ALL, INACTIVE);
				Out_FeederHorActive = FALSE;
				break;
			}
			break;
		}
		case 150:
		{
			if( !Out_FeederHorActive
			 &&  In_FeederHorInactive
			 && !In_FeederHorActive)
			{
				AdjustStart = FALSE;
				AdjustStep = 0;
				break;
			}
			break;
		}

	} /*switch*/
/******************************************************/
/******************************************************/
/* Manual loading */
	switch (ManualLoadingStep)
	{
		case 0:
		{
			if( PaperRemoveStart
			 || PlateTakingStart
			 || AdjustStart
			 || ExposeStart
			 || DeloadStart
			  )
				break;


			if(ManualLoadingStart)
				ManualLoadingStep = 5;
			break;
		}
		case 5:
		{
			if(AdjPins(PlateToDo.PlateConfig,&PinsStep,&PinsTimer))
				ManualLoadingStep = 10;
			break;
		}

		case 10:
		{
/*check, if Feeder is out of collision area (should be the case here)*/
			if( isPositionHigherThan(FEEDER_VERTICAL, labs(AdjusterParameter.FeederAdjustposition) + 50L) )
				break;

			FU.cmd_Park = TRUE;
			ManualLoadingStep = 20;
			break;
		}
		case 20:
		{
			if( (abs(FU.aktuellePosition_mm - X_Param.ParkPosition) < 1.0)
			 && !FU.cmd_Park)
			{
/*				ManualLoadingStep = 30;*/
				ManualLoadingStep = 90;
			}
			break;
		}
		case 30:
		{
			if(!In_ServiceMode)
			{
				ShowMessage(80,81,0,CAUTION,OKONLY,FALSE);
				AbfrageOK = FALSE;
				AbfrageCancel = FALSE;
				ManualLoadingStep = 40;
			}
			else
				ManualLoadingStep = 50;
			break;
		}
		case 40:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				AbfrageCancel = FALSE;
				if(In_ServiceMode)
					ManualLoadingStep = 50;
				else
					ManualLoadingStep = 30;
			}
			break;
		}
		case 50:
		{
			if(In_ServiceMode)
			{
				Out_CoverLockOn = OFF;
				ShowMessage(82,83,84,CAUTION,OKONLY,FALSE);
				AbfrageOK = FALSE;
				ManualLoadingStep = 60;
			}
			break;
		}
		case 60:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				AbfrageCancel = FALSE;
				ManualLoadingStep = 70;
			}
			break;
		}
		case 70:
		{
/* close cover and switch back to auto mode*/
			ShowMessage(85,86,0,CAUTION,OKONLY,FALSE);
			AbfrageOK = FALSE;
			AbfrageCancel = FALSE;
			ManualLoadingStep = 80;
			break;
		}
		case 80:
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				AbfrageCancel = FALSE;
				if(!In_ServiceMode)
					ManualLoadingStep = 90;
				else
					ManualLoadingStep = 70;
			}
			break;
		}
		case 90:
		{
			Out_CoverLockOn = ON;
			PlateAtFeeder = PlateToDo;
			ClearStation(&PlateToDo);
			PlateAtFeeder.present = TRUE;
/*			PlateAtFeeder.PlateConfig = BROADSHEET;*/
			PlateAtFeeder.PlateConfig = PANORAMA;
			AdjustStart = TRUE;
			ManualLoadingStep = 0;
			ManualLoadingStart = FALSE;
			break;
		}

	}

	return;
}


