#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf Platte Entladen */
/*																						*/
/*																						*/
/*	Versionsgeschichte:																*/
/*  Version		Datum		Änderung										von		*/
/*	1.0			17.09.02	erste Implementation								HA		*/
/*																						*/
/*																						*/
/****************************************************************************/
#include "glob_var.h"
#include "asstring.h"
#include <string.h>
#include "in_out.h"
#include "Motorfunc.h"
#include "auxfunc.h"

#define CONVEYORBELT_BACKWAY	5000

_LOCAL	USINT	DeloadStep;
_LOCAL TON_10ms_typ DeloadTimer;
USINT			tmp[20];

_LOCAL TON_10ms_typ CycleTimer,CycleTimer2,TestTimer;
_LOCAL R_TRIGtyp R_TRIG_01;
_LOCAL F_TRIGtyp F_TRIG_01,F_TRIG_02;

BOOL	Started;
_GLOBAL UDINT PlatesPerHour;
/*HA 21.01.04 V1.73*/
_GLOBAL BOOL	AdjustFailed;

_LOCAL	UINT	CurrentTime;
_LOCAL	REAL	LastTime,HandlingTime;
DINT	Result1,Result2;
UDINT	TurnSpeedOld;
		TOF_10ms_typ	ConveyorBeltDelay;
		TON_10ms_typ	EGMErrorDelay;
		TON_10ms_typ	PinsDownWait;
		TON_10ms_typ	TurnOK;
		TOF_10ms_typ	ConveyorBeltSensorDelay1,ConveyorBeltSensorDelay2;

_GLOBAL	UINT			ConveyorBeltOn;

/*HA 14.01.04 V1.71 implemented Stop pins on conveyor */
_GLOBAL	BOOL			StopPinsUp;
		TON_10ms_typ	PinsDownDelay;
		TOF_10ms_typ	PinsUpDelay;

/*HA 05.09.03 V1.51 to avoid adjustsensor triggered by deloaded plate*/
/*HA 08.09.03 V1.52 disable-flags for right adjuster*/
_GLOBAL 	BOOL			DisableAdjustSensor,DisableAdjusterUp,EnableShuttle;
_LOCAL	BOOL			StartConveyorBelt,StopConveyorBelt;
		BOOL			LayOffPos2;
		UDINT			TempPlateReleasePosition,TempTurnPosition,TempTakePosition;

		REAL			PlateDeloadLength; /*holds calculated value Length*LengthFactor*/
BOOL TurnPosOK; /*Hilfsvariable für drehen ohne Zeit*/
_GLOBAL	BOOL	ConveyorbeltSimulation;
int DeloadSpeed,StopPinsStep;
BOOL	DisableConveyorBelt;
DINT ConveyorBeltPos,ConveyorBeltBack;
BOOL dummyflag;
USINT	ReturnStep;
_GLOBAL UINT	HideKrause;
USINT ConveyorBeltStep;
TON_10ms_typ	ConveyorBeltTimer;
UDINT	ConveyorTime;

_GLOBAL	REAL	TablePosDrop;
BOOL	MovingToParkpos,NoSpeedChange;
static USINT ConvMotStep;
TON_10ms_typ LiftpinsDownTimer;
static BOOL FirstTime;
static REAL OldParkSpeed;
static BOOL ParkSpeedChanged;

_LOCAL BOOL  JetValuesValid;
_LOCAL REAL  JetSqmValue; /* in m² for the plate set, i.e. no matter if 1 or 2 plates,
                             sum up must be done by jet */
_LOCAL USINT JetPlateCnt; /* 1 or 2 plates*/
static TON_10ms_typ JetValuesValidDelay;



void InformBluefin(void)
{
	if(!JetValuesValid)
	{
		if(PlateOnConveyorBelt.PlateConfig == BOTH)
			JetPlateCnt = 2;
		else
			JetPlateCnt = 1;

		if( (PlateOnConveyorBelt.PlateType > 0)
		 && (PlateOnConveyorBelt.PlateType < MAXPLATETYPES)
		  )
		{
/* /1000 weil Length und Width in mm sind, die Fläche aber in QadratMETER*/
			JetSqmValue = (PlateTypes[PlateOnConveyorBelt.PlateType].Length / 1000.0) *
			              (PlateTypes[PlateOnConveyorBelt.PlateType].Width / 1000.0);
				JetSqmValue *= JetPlateCnt;
		}
/* little delay to make sure the values are transmitted before the valid flag is set*/
		JetValuesValidDelay.IN = TRUE;
	}
	/* no else: Bluefin did not reset Valid-flag...do nothing */
}


_INIT void init(void)
{
	JetValuesValidDelay.IN = FALSE;
	JetValuesValid = FALSE;
	JetSqmValue = 0.0;
	JetPlateCnt = 1;
	ParkSpeedChanged = FALSE;
	FirstTime = TRUE;
	ConvMotStep = 0;
	LayOffPos2 = 0;
	ClearStation(&PlateOnConveyorBelt);
	DisableConveyorBelt = 0;
	StopPinsStep = 0;
	ClearStation(&PlateAtDeloader);
	TurnSpeedOld	= 0;
	DeloadStep		= 0;
	DeloadStart	= 0;
	DeloadReady		= 0;
	DeloadTimer.IN	= 0;
	DeloadTimer.PT	= 10; /*just to have any value in there...*/
	CycleTimer.IN	= 0;
	CycleTimer.PT	= 10; /*just to have any value in there...*/
	ConveyorBeltDelay.IN = 0;
	ConveyorBeltDelay.PT = 1000;
	ConveyorBeltOn = FALSE;
	EnableShuttle = 0;
	TurnOK.PT = 50;
	TurnOK.IN = 0;
	TurnPosOK = 0;
	LiftpinsDownTimer.IN = FALSE;

}


/****************************************************************************/
/* zyklischer Teil*/
/****************************************************************************/
_CYCLIC void cyclic(void)
{
	GlobalParameter.DeloaderTurnStation = 0;

	if( GlobalParameter.DisableSlowMode )
		SlowMode = 0;
	if( GlobalParameter.SlowModeWaitTime > 9900 )
		GlobalParameter.SlowModeWaitTime = 2000;

	SequenceSteps[5] = DeloadStep;

	if(ExposureReady && AUTO)
		DeloadStart = 1;

	TON_10ms(&DeloadTimer);
	TON_10ms(&TurnOK);

	LiftpinsDownTimer.IN = Out_LiftingPinsDown;
	LiftpinsDownTimer.PT = 12000; /* 2 min */
	TON_10ms(&LiftpinsDownTimer);

/* little delay for Bluefin comm handshake flag*/
	JetValuesValidDelay.PT = 15; /* 150ms */
	TON_10ms(&JetValuesValidDelay);
	if(JetValuesValidDelay.Q)
	{
		JetValuesValidDelay.IN = FALSE;
		JetValuesValid = TRUE;
	}
/* time measurement*/

/*detect edge of expose command*/
R_TRIG_01.CLK = FU.cmd_Expose;
R_TRIG(&R_TRIG_01);
if(R_TRIG_01.Q && Started)
{
	Started = 0;
	CycleTimer.IN = 0;
	Result1 = CycleTimer.ET;
	CycleTimer2.IN = 1;
	LastTime = (REAL)(Result1)/100.0;
	PlatesPerHour = (UDINT) ((3600.0 / ( ((REAL) (Result1))/100.0))+0.5);
}
else
if(R_TRIG_01.Q && !Started)
{
	Started = 1;
	CycleTimer.IN = 1;
	CycleTimer2.IN = 0;
	Result2 = CycleTimer2.ET;
	LastTime = (REAL)(Result2)/100.0;
	PlatesPerHour = (UDINT) ((3600.0 / ( ((REAL) (Result2))/100.0))+0.5);
}

if(CycleTimer.IN)
{
	CurrentTime = CycleTimer.ET;
}
if(CycleTimer2.IN)
{
	CurrentTime = CycleTimer2.ET;
}

CycleTimer.PT = 60000; /*max 1 min*/
TON_10ms(&CycleTimer);
CycleTimer2.PT = 60000; /*max 1 min*/
TON_10ms(&CycleTimer2);


/*time measurement handling time (end of exposure to start of exposure*/
TestTimer.IN = !FU.cmd_Expose;
if( TestTimer.IN )
{
	HandlingTime = ((REAL) TestTimer.ET) / 100.0;
}
TestTimer.PT = 60000;
TON_10ms(&TestTimer);



/*safety*/
	if( !DeloadStart )
	{
		DeloadStep = 0;
		DisableAdjustSensor = 0;
		EnableShuttle = 0;
		MovingToParkpos = FALSE;
		if(ParkSpeedChanged)
		{
			X_Param.ParkSpeed = OldParkSpeed;
			ParkSpeedChanged = FALSE;
		}

	}

	switch (DeloadStep)
	{
		case 0:
		{
			if( !DeloadStart
			 ||  AdjustStart
			  )
				break;

			DeloadTimer.IN = 0;
			DeloadStep = 1;
/*HA 05.09.03 V1.51 to avoid adjustsensor triggered by deloaded plate*/
			DisableAdjustSensor = 0;

			break;
		}
		case 1:
		{
/*Wait for Exposure to finish*/
			if(ExposureReady || !ExposeStart || !AUTO)
			{
				DeloadStep = 170;
				break;
			}

			break;
		}


/****************************************************************************/
/*Deloading without turn/positioning device*/
/****************************************************************************/
		case 170:
		{
/*switch off adjuster vacuum*/
			if( DeloadTimer.Q)
			{
				DeloadStep = 175;
				DeloadTimer.IN = FALSE;
				PlateAtDeloader = PlateAtAdjustedPosition;
				ClearStation(&PlateAtAdjustedPosition);
				break;
			}
			SwitchAdjustTableVacuum(0,ALL,OFF);
			DeloadTimer.IN = TRUE;
			DeloadTimer.PT = AdjusterParameter.TableVacuumOffDelay;
			break;
		}

/* Table to startposition */
		case 175:
		{
			if (DisableTable)
				break;

			if ( FU.cmd_Target == FALSE )
			{
				if( (FU.aktuellePosition_mm < (DeloaderParameter.StartTablePosition)))
					DeloadStep = 182;
				else
				{
					FU.TargetPosition_mm = DeloaderParameter.StartTablePosition;
					FU.cmd_Target = TRUE;
					DeloadStep = 180;
				}
				DeloadTimer.IN = FALSE;
			}
			break;
		}

		case 180:
		{
			if( (abs(FU.aktuellePosition_mm - DeloaderParameter.StartTablePosition) < 1.0)
			 && !FU.cmd_Target
			  )
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 182;
				break;
			}
			break;
		}
/*liftpins up*/
		case 182:
		{
			if(DeloadTimer.Q)
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 183;
				break;
			}
			DeloadTimer.IN = !In_LiftingPinsDown;
/* make sure, this code is only executed once */
			if(Out_LiftingPinsDown)
			{
/* pins were down longer than 2 min or first time after power on:
   give them more time
 */
				if(LiftpinsDownTimer.Q || FirstTime)
					DeloadTimer.PT = 500;
				else
					DeloadTimer.PT = 20;
			}
			FirstTime = FALSE;
			Out_LiftingPinsUp = ON;
			Out_LiftingPinsDown = OFF;
			break;
		}

/* Blowair on */
		case 183:
		{
			if (DisableTable)
				break;
/*wait, til conveyor belt free*/
			if( PlateOnConveyorBelt.present)
				break;

			AdjustBlowairOn = ON;
			DeloadTimer.IN = TRUE;
			DeloadTimer.PT = 50;
			DeloadStep = 184;

			break;
		}

/* wait with Blowair on */
		case 184:
		{
			if(DeloadTimer.Q)
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 185;
			}

			break;
		}


/* Table to endposition */
		case 185:
		{
			if (DisableTable)
				break;
/*wait, til conveyor belt free*/
			if( PlateOnConveyorBelt.present)
				break;

			if ( FU.cmd_Target == FALSE )
			{
				OldParkSpeed = X_Param.ParkSpeed;
				ParkSpeedChanged = TRUE;
				if(ManualMode)
					X_Param.ParkSpeed = DeloaderParameter.TableDeloadSpeedManual;
				else
					X_Param.ParkSpeed = DeloaderParameter.TableDeloadSpeed;
				FU.TargetPosition_mm = DeloaderParameter.EndTablePosition;
				FU.cmd_Target = TRUE;
/*				AdjustBlowairOn = (PlateAtDeloader.PlateConfig != BROADSHEET);*/
				AdjustBlowairOn = ON;
				DeloadTimer.IN = FALSE;
				DeloadStep = 190;
			}
			break;
		}
/* wait for table to arrive in end position, lift pins down on the way, Blowair off when there*/
		case 190:
		{
			if( (FU.aktuellePosition_mm > (DeloaderParameter.StartTablePosition + 50.0) )
			 && !In_LiftingPinsDown)
			{
				Out_LiftingPinsUp = OFF;
				Out_LiftingPinsDown = ON;
			}

			if( (abs(FU.aktuellePosition_mm - DeloaderParameter.EndTablePosition) < 1.0)
			 && !FU.cmd_Target
			  )
			{
				X_Param.ParkSpeed = OldParkSpeed;
				ParkSpeedChanged = FALSE;
				AdjustBlowairOn = OFF;
				DeloadStep = 192;
				break;
			}
			break;
		}
/*liftpins down, should be done in step 190 already, just double check...*/
		case 192:
		{
			if(In_LiftingPinsDown)
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 200;
				break;
			}
			Out_LiftingPinsUp = OFF;
			Out_LiftingPinsDown = ON;
			break;
		}

		case 200:
		{
			if(MoveToRelPosition(CONVEYORBELT,
			                     350*Motors[CONVEYORBELT].Parameter.IncrementsPerMm,
			                     3000, &ConvMotStep)
			  )
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 205;
				break;
			}
			break;
		}
		case 205:
		{
			if(Motors[CONVEYORBELT].Moving)
			{
				DeloadTimer.IN = FALSE;
				DeloadStep = 210;
				break;
			}
			break;
		}
		case 210:
		{
			if(!Motors[CONVEYORBELT].Moving)
			{
				DeloadTimer.IN = FALSE;
				AdjustReady = FALSE;
				if(AUTO)
				{
					DeloadReady = TRUE;
					PlateOnConveyorBelt = PlateAtDeloader;
				}
				else
				{
					PlateOnConveyorBelt.present = TRUE;
					PlateOnConveyorBelt.PlateConfig = PlateToDo.PlateConfig;
				}
				ClearStation(&PlateAtDeloader);
				DeloadStep = 0;
				DeloadStart = FALSE;
				break;
			}
			break;
		}
	} /*switch*/


/****************************************************************/
/* CONVEYOR BELT */
/****************************************************************/

/*EGM Fehlermeldung: kein Power-On Signal oder länger keine Einlauffreigabe*/
	EGMErrorDelay.IN = !StatusEGM2 && !GlobalParameter.ProcessorSimulation;
	EGMErrorDelay.PT = DeloaderParameter.EGMStopDelay;
	TON_10ms(&EGMErrorDelay);

	AlarmBitField[22] = (EGMErrorDelay.Q || !StatusEGM1) && !GlobalParameter.ProcessorSimulation;
	AlarmBitField[23] = (!VCP && !GlobalParameter.PunchSimulation);


	if (DeloaderParameter.PinsUpDelay > 999) DeloaderParameter.PinsUpDelay = 999;
	if (DeloaderParameter.PinsDownDelay > 999) DeloaderParameter.PinsDownDelay = 999;

	TON_10ms(&ConveyorBeltTimer);

/*OFF-delay for sensor 200 ms */
	ConveyorBeltSensorDelay1.IN = In_ConveyorBeltSensor[0] || In_ConveyorBeltSensor[1];
	ConveyorBeltSensorDelay1.PT = 20;
	TOF_10ms(&ConveyorBeltSensorDelay1);


/*if flag is reset, stop the sequence*/
	if( !PlateOnConveyorBelt.present )
	{
		ConveyorBeltStep = 0;
	}

	if (!DeloaderParameter.EnableStopPins)
	{
		StopPinsUp = 0;
	}

	switch (ConveyorBeltStep)
	{
		case 0:
		{
			ConveyorBeltTimer.IN = FALSE;
/*Simulation: just delete PlateOnConveyorBelt flag, don't start sequence*/
			if (PlateTransportSim || ConveyorbeltSimulation)
			{
				ClearStation(&PlateOnConveyorBelt);
				break;
			}
/*wait for plate on belt (set by deloading sequence)*/
/*and no plate on sensor*/
			if( !PlateOnConveyorBelt.present) break;

			ConveyorBeltTimer.IN = FALSE;

/*Plate arrived, first thing is: stop pins up (should be the case already...)*/
			if (DeloaderParameter.EnableStopPins)
			{
				StopPinsUp = 1;
			}

			if( (PlateOnConveyorBelt.PlateConfig != PANORAMA)
			 && (PlateOnConveyorBelt.PlateConfig != BROADSHEET) )
			{
				ConveyorBeltTimer.IN = FALSE;
				Out_ConveyorbeltLateral = ON;
				ConveyorBeltStep = 2;
				break;
			}
			else
				ConveyorBeltStep = 5;

			break;
		}

		case 2:
		{
			if(ConveyorBeltTimer.Q)
			{
				Out_ConveyorbeltLateral = OFF;
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 3;
				break;
			}
			ConveyorBeltTimer.IN = TRUE;
			ConveyorBeltTimer.PT = 10;
			Out_ConveyorbeltLateral = ON;
			break;
		}

		case 3:
		{
			if(ConveyorBeltTimer.Q)
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 5;
				break;
			}

			ConveyorBeltTimer.IN = TRUE;
			if( (DeloaderParameter.ConvLateralMoveTime > 0) && (DeloaderParameter.ConvLateralMoveTime < 3000) )
				ConveyorBeltTimer.PT = DeloaderParameter.ConvLateralMoveTime;
			else
				ConveyorBeltTimer.PT = 200;
			break;
		}

		case 5:
		{
/* Start the belt to move plate to the sensor/pins*/

			if( SendMotorCmd(CONVEYORBELT,CMD_MOVE_AT,TRUE,DeloaderParameter.ConveyorBeltSpeed) )
			{
				ConveyorBeltOn = TRUE;
				ConveyorBeltStep = 10;
				ConveyorBeltTimer.IN = TRUE;
				ConveyorBeltTimer.PT = 6000; /*60 sec*/
			}
			break;
		}
		case 10:
		{
/* wait, until plate reaches sensor */
/*measure the time*/
			if (In_ConveyorBeltSensor[0] || In_ConveyorBeltSensor[1])
			{
				if (DeloaderParameter.EnableStopPins)
					ConveyorBeltStep = 15;
				else
				{
/*belt is moving, if processor OK just go on to step 40*/
					if ( ((StatusEGM1 && StatusEGM2) || GlobalParameter.ProcessorSimulation	)
					&&  (VCP || GlobalParameter.PunchSimulation) )
						ConveyorBeltStep = 40;
					else
/*processor not ok -> step 20 to stop belt*/
						ConveyorBeltStep = 20;
				}
				ConveyorBeltTimer.IN = FALSE;
				ConveyorTime = ConveyorBeltTimer.ET;
				break;
			}
/* plate after 60 sec not there -> ERROR */
/*consider it's been taken out*/
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 55;
			}
			break;
		}
		case 15:
		{
/* wait a little to let plate reach the pins */
			ConveyorBeltTimer.IN = TRUE;
			ConveyorBeltTimer.PT = DeloaderParameter.PinsDownDelay;
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltStep = 20;
				ConveyorBeltTimer.IN = FALSE;
			}
			break;
		}

/*Band stoppen*/
		case 20:
		{
			if( SendMotorCmd(CONVEYORBELT,CMD_STOP,FALSE,0L) )
			{
				ConveyorBeltOn = FALSE;
				ConveyorBeltStep = 30;
				ConveyorBeltTimer.IN = FALSE;
			}
			break;
		}

/* 12.07.07 "Slow Mode" */
		case 30:
		{
/* SlowMode aktiviert: Warten*/
			if(SlowMode)
			{
				ConveyorBeltTimer.IN = TRUE;
				ConveyorBeltTimer.PT = GlobalParameter.SlowModeWaitTime;
				ConveyorBeltStep = 31;
			}
			else
			/* kein SlowMode: einfach weiter */
				ConveyorBeltStep = 32;
			break;
		}
/* Warten, bis timer abgelaufen */
		case 31:
		{
			if (ConveyorBeltTimer.Q || !SlowMode)
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 32;
			}
			break;
		}

		case 32:
		{

/*wait for "ready" from Processor*/
/*StatusEGM1 -> Pwr ON*/
/*StatusEGM2 -> Ready*/
			ConveyorBeltTimer.IN = FALSE;
			if ( ((StatusEGM1 && StatusEGM2) || GlobalParameter.ProcessorSimulation	)
			&&  (VCP || GlobalParameter.PunchSimulation) )
				ConveyorBeltStep = 35;
			break;
		}


		case 35:
		{
/* Start the belt to move plate into the processor*/
			if( SendMotorCmd(CONVEYORBELT,CMD_MOVE_AT,TRUE,DeloaderParameter.ConveyorBeltSpeed) )
			{
				ConveyorBeltOn = TRUE;
				ConveyorBeltStep = 36;
				ConveyorBeltTimer.IN = TRUE;
				ConveyorBeltTimer.PT = 4000; /*40 sec*/
			}
			break;
		}

		case 36:
		{
/*wait until plate on sensor (maybe off sensor because it was moved back too far )*/
			if (In_ConveyorBeltSensor[0] || In_ConveyorBeltSensor[1])
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 40;
				break;
			}

/*Error handling: plate doesn't reach sensor in time*/
/*consider it's been taken out*/
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 55;
			}
			break;
		}

		case 40:
		{
/*wait, until processor signals "not ready" : */
/*now the plate moves into the processor*/

/*StatusEGM1 -> Pwr ON*/
/*StatusEGM2 -> Ready*/
			if ( !StatusEGM2 || GlobalParameter.ProcessorSimulation)
			{
				ConveyorBeltTimer.IN = TRUE;
				ConveyorBeltTimer.PT = 12000; /*120 sec*/
				ConveyorBeltStep = 45;
				InformBluefin();
				break;
			}

/*if sensor becomes free before processor signals "not ready" jump to step 50*/
/*stop waiting for processor*/
			if ( !ConveyorBeltSensorDelay1.Q )
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 50;
				InformBluefin();
				break;
			}

			break;
		}

		case 45:
		{
/*wait until plate off sensor */
			if ( !ConveyorBeltSensorDelay1.Q )
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 50;
				break;
			}

/*Error handling: plate doesn't leave sensor in time*/
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltTimer.IN = FALSE;
				ConveyorBeltStep = 55;
			}
			break;
		}

		case 50:
		{
/* no stop pins ->  go on */
			if (!DeloaderParameter.EnableStopPins)
			{
				ConveyorBeltStep = 53;
				ConveyorBeltTimer.IN = FALSE;
				break;
			}

/*wait to let plate be off pins*/
			ConveyorBeltTimer.IN = TRUE;
			ConveyorBeltTimer.PT = DeloaderParameter.PinsUpDelay;
/* after 2 sec: pins up and stop belt*/
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltStep = 55;
				if (DeloaderParameter.EnableStopPins)
					StopPinsUp = 1;
			}
			break;
		}

		case 53:
		{
/* wait a little to let plate leave the belt completely */
			ConveyorBeltTimer.IN = TRUE;
/*adjustable time */
			ConveyorBeltTimer.PT = DeloaderParameter.ConveyorBeltDelay;
			if (ConveyorBeltTimer.Q)
			{
				ConveyorBeltStep = 55;
				ConveyorBeltTimer.IN = FALSE;
			}
			break;
		}

/*Band stoppen, Ablauf beendet*/
		case 55:
		{
			if( SendMotorCmd(CONVEYORBELT,CMD_STOP,FALSE,0L) )
			{
				ConveyorBeltOn = FALSE;
				ClearStation(&PlateOnConveyorBelt);
				ConveyorBeltStep = 0;
			}
			break;
		}

	} /* switch (ConveyorBeltStep) */
}


