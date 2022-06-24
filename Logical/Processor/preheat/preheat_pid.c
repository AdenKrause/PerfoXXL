#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*****************************************************************************************/
/*	Preheat regelung mit LoopCont Library*/
/*	wird aus EGMPreheat.c aufgerufen*/
/*****************************************************************************************/

/*
#define TESTCURVE
*/
/**/
#undef TESTCURVE
/**/

#include <math.h>
#include "EGMglob_var.h"
#include "auxfunc.h"

/*für Sollwertnachführung*/
#define NUMBER_OF_POINTS		720 /*2h lang alle 10 sec ein wert*/
#define NUMBER_OF_SECONDS	7200 /*2h */
/*für Stabilitätscheck*/
#define NUMBER_OF_RANGECHECK_SECONDS	30
#define OVERHEATTEMP 500	/* 50 °C über der Solltemp.*/

_GLOBAL SINT IOConfiguration	_VAR_RETAIN;

_LOCAL	LCPWM_DMod_typ		LCPWM_DMod_var[MAX_NUMBER_OF_CONTROLLERS];
_LOCAL	LCPIDpara_typ			LCPIDpara_var[MAX_NUMBER_OF_CONTROLLERS];
_LOCAL	LCPID_typ				LCPID_var[MAX_NUMBER_OF_CONTROLLERS];
_LOCAL	LCPIDAutoTune_typ		LCPIDAutoTune_var[MAX_NUMBER_OF_CONTROLLERS];
_LOCAL	LCCurveByPoints_typ	LCCurveByPoints_var,LCCurveByPoints_var2;

/*Feld für Kurve der Sollwertnachführung*/
/* e-Funktion*/
_LOCAL lcCurveByPoints_TabEntry_type   CurveByPoints_Table[NUMBER_OF_POINTS];
/* Korrekturen der e-Funktion*/
_LOCAL lcCurveByPoints_TabEntry_type   CurveByPoints_Table2[15];

_LOCAL	BOOL 	StartAutoTune;
_LOCAL	BOOL	TriggerLogging,TriggerLoggingOld;
TON_10ms_typ	Pulse,PulseTimer1;
TON_10ms_typ		HeatingFailureTimer[MAX_NUMBER_OF_CONTROLLERS];

int i;
_GLOBAL int	Values[350];
_GLOBAL int	AmbientValues[350];

int	RangeCheckArray[MAX_NUMBER_OF_CONTROLLERS][NUMBER_OF_RANGECHECK_SECONDS];
int ValueCount,RangeCheckCount,SPTrackCount;
BOOL flag[MAX_NUMBER_OF_CONTROLLERS];
_GLOBAL BOOL RedrawCurve,PreheatParamEnter;
_GLOBAL	UINT	SamplingTime;
BOOL	StopSPTracking;
BOOL	Overheated;
INT LastTemp[MAX_NUMBER_OF_CONTROLLERS];



R_TRIGtyp	InputSensorEdgeR;
F_TRIGtyp	InputSensorEdgeF;
TON_10ms_typ	InputSensorOnDelay;
TOF_10ms_typ	InputSensorOffDelay;
int	PlateInputCounter;
int TempCorrectionValue;
int SecWithoutPlate;

/* depending on Bluefin-Variant: Standard or 1250 */
_LOCAL    INT                 ActualNumberOfControllers;

void Preset_CorrCurve(void)
{
	int i;
	REAL k;

/*e-Funktion für Startup erzeugen*/
	for (i=0;i<NUMBER_OF_POINTS;i++)
	{
		CurveByPoints_Table[i].x = i*(NUMBER_OF_SECONDS / NUMBER_OF_POINTS) ;

		k = -((float)NUMBER_OF_SECONDS / (float) NUMBER_OF_POINTS)  / (60.0* EGMPreheatParam.SetPointCorrTMin);
		CurveByPoints_Table[i].y = EGMPreheatParam.SetPointCorrMax * exp(k*i);
	}

/*initialisieren der Korrekturkurve für die e-Funktion*/
	for (i=0;i<15;i++)
	{
		CurveByPoints_Table2[i].x = 0;
		CurveByPoints_Table2[i].y = 0;
	}
/*und mit Stützpunkten füllen*/
/* 1 */
	CurveByPoints_Table2[0].x = 0;
	CurveByPoints_Table2[0].y = 0;
/* 2 */
	CurveByPoints_Table2[1].x = 180;
	CurveByPoints_Table2[1].y = -50;
/* 3 */
	CurveByPoints_Table2[2].x = 300;
	CurveByPoints_Table2[2].y = -80;
/* 4 */
	CurveByPoints_Table2[3].x = 540;
	CurveByPoints_Table2[3].y = -90;
/* 5 */
	CurveByPoints_Table2[4].x = 840;
	CurveByPoints_Table2[4].y = -110;
/* 6 */
	CurveByPoints_Table2[5].x = 1200;
	CurveByPoints_Table2[5].y = -80;
/* 7 */
	CurveByPoints_Table2[6].x = 1740;
	CurveByPoints_Table2[6].y = -60;
/* 8 */
	CurveByPoints_Table2[7].x = 2700;
	CurveByPoints_Table2[7].y = -30;
/* 9 */
	CurveByPoints_Table2[8].x = 3480;
	CurveByPoints_Table2[8].y = -10;
/* 10 */
	CurveByPoints_Table2[9].x = 3660;
	CurveByPoints_Table2[9].y = 0;
/* 11 */
	CurveByPoints_Table2[10].x = 4860;
	CurveByPoints_Table2[10].y = 0;
/* 12 */
	CurveByPoints_Table2[11].x = 5400;
	CurveByPoints_Table2[11].y = 10;
/* 13 */
	CurveByPoints_Table2[12].x = 6000;
	CurveByPoints_Table2[12].y = 25;
/* 14 */
	CurveByPoints_Table2[13].x = 6660;
	CurveByPoints_Table2[13].y = 19;

}


void PreheatPID_DefaultParam(void)
{
	int j;

/* depending on IO config we have 2 or 4 Controllers running*/
	switch (IOConfiguration)
	{
		case 1 /* Standard Bluefin */:
		{
			ActualNumberOfControllers = 2;
			break;
		}
		case 2 /* Bluefin 1250 */:
		{
			ActualNumberOfControllers = 4;
			break;
		}
		default:
		{
			ActualNumberOfControllers = 2;
			break;
		}
	}
	PreheatParamEnter = 0;
	StopSPTracking = 0;
	SamplingTime = 100;
	SPTrackCount = 0;
	PulseTimer1.PT = 100;

	LCCurveByPoints_var.ptr_table = &CurveByPoints_Table[0];
	LCCurveByPoints_var.NoOfPoints = NUMBER_OF_POINTS;

	LCCurveByPoints_var2.ptr_table = &CurveByPoints_Table2[0];
	LCCurveByPoints_var2.NoOfPoints = 14;

	Preset_CorrCurve();

	for (i=0;i<ActualNumberOfControllers;i++)
	{
		LCPWM_DMod_var[i].tminpuls			= 10;  /* 10 ms kürzester Puls bzw pause*/
		LCPWM_DMod_var[i].tperiod			= 200;  /*Periodendauer 0,2 sec*/
		LCPWM_DMod_var[i].enable			= 1;
		LCPWM_DMod_var[i].max_value	= 100;
		LCPWM_DMod_var[i].min_value		= 0;

/*Sollwertabschwächung (1=AUS)*/
		LCPIDpara_var[i].Kw						= 1;
/*windup Dämpfung: 0=AUS*/
		LCPIDpara_var[i].Kfbk						= 0;
		LCPIDpara_var[i].enable					= 1;
		LCPIDpara_var[i].Y_max				= 100;
		LCPIDpara_var[i].Y_min					= 0;
		LCPIDpara_var[i].dY_max				= 0; /* 0= keine Rampe*/
	/*
	Rückführmodus der Stellgröße
	LCPID_FBK_MODE_INTERN: intern
	LCPID_FBK_MODE_EXTERN: extern für Ablöseregelung
	LCPID_FBK_MODE_EXT_SELECTOR: extern für Selektorregelung. In diesem Modus wird der I-Anteil des momentan nicht selektierten Reglers (Y_fbk ? Y) nach der
	Formel Yi = Y_fbk – Yp – Yd – A dem Eingang Y_fbk nachgeführt.
	*/
		LCPIDpara_var[i].fbk_mode				= LCPID_FBK_MODE_INTERN;
	/*
	Differenziermodus
	LCPID_D_MODE_X: nur -X (Istwert)
	LCPID_D_MODE_E: e (Regelabweichung, wie bei einem Standard- PID- Regler )
	*/
		LCPIDpara_var[i].d_mode				= LCPID_D_MODE_X;
	/*
	Rechenmodus:
	LCPID_CALC_MODE_EXACT: Genau (Alle Berechnungen erfolgen in Double Float - keine Rundungsfehler, auf SG3-Zentraleinheiten lange Rechenzeiten).
	LCPID_CALC_MODE_FAST: Schnell (Alle Berechnungen erfolgen in Integer - es kann zu geringfügigen Rundungsfehlern kommen, wird auch auf SG3-Zentraleinheiten sehr schnell gerechnet)
	*/
		LCPIDpara_var[i].calc_mode			= LCPID_CALC_MODE_EXACT;

/* Reglerbaustein LCPID*/
		LCPID_var[i].enable					= 1;
/*Stellgröße Handbetrieb */
		LCPID_var[i].Y_man					= 0;
	/*Aufschaltgröße*/
		LCPID_var[i].A							= 0;
	/*I-Anteil einfrieren*/
		LCPID_var[i].hold_I					= 0;

/* Autotune*/
		LCPIDAutoTune_var[i].enable				= 1;
		LCPIDAutoTune_var[i].Y_max				= 100;
		LCPIDAutoTune_var[i].Y_min				= 0;
		LCPIDAutoTune_var[i].P_manualAdjust		= 0;
		LCPIDAutoTune_var[i].I_manualAdjust		= 0;
		LCPIDAutoTune_var[i].D_manualAdjust		= 0;

		EGMPreheat.Auto[i] = 0;

		TriggerLogging = 0;
		for(j=0;j<NUMBER_OF_RANGECHECK_SECONDS;j++)
			RangeCheckArray[i][j] = 200;
	}
	RangeCheckCount = 0;
	i=0;
	ValueCount = 0;
	InputSensorOnDelay.IN = 0;
	InputSensorOffDelay.IN = 0;
	SecWithoutPlate = 0;
	TempCorrectionValue = 0;
	for(i=0;i<ActualNumberOfControllers;i++)
	{
		LastTemp[i] = 0;
		HeatingFailureTimer[i].IN = 0;
		HeatingFailureTimer[i].PT = 1000;
	}
}






/***********************************************************************
*                                                                      *
*                                                                      *
*     Sollwert pro Platte sukzessive erhöhen, nach Zeit ohne Platte    *
*     wieder verringern                                                *
*                                                                      *
*                                                                      *
***********************************************************************/
void SetpointCorrectionPerPlate(void)
{
/*Flankenerkennung Inputsensor */
	InputSensorEdgeR.CLK = InputSensorOnDelay.Q;
	InputSensorEdgeF.CLK = InputSensorOffDelay.Q;
	R_TRIG(&InputSensorEdgeR);
	F_TRIG(&InputSensorEdgeF);

	InputSensorOnDelay.PT = 100;
	InputSensorOffDelay.PT = 100;
/*Entprellen je 1 sec*/
	InputSensorOnDelay.IN = InputSensor;
	InputSensorOffDelay.IN = InputSensor;
	TON_10ms(&InputSensorOnDelay);
	TOF_10ms(&InputSensorOffDelay);


	if( InputSensor )
		SecWithoutPlate = 0;
	else
	if( PulseTimer1.Q ) /* Pulse every sec */
	{
		SecWithoutPlate++;
		if( (SecWithoutPlate % 60) == 0) /* every minute */
		{
			TempCorrectionValue += EGMPreheatParam.IncPerMinute;
			if( TempCorrectionValue > 0 )
				TempCorrectionValue = 0;
		}
	}

/* Platte läuft ein */
	if( InputSensorEdgeF.Q )
	{
		SecWithoutPlate = 0;	/* nur zur Sicherheit */
/* wenn Umgebunstemp. > Schwellwert*/
		if( EGMPreheat.AmbientTemp[0] > EGMPreheatParam.AmbientThreshold )
		{
			TempCorrectionValue -= EGMPreheatParam.DecPerPlate;
			if( abs(TempCorrectionValue) > abs(EGMPreheatParam.MaxInc) )
				TempCorrectionValue = EGMPreheatParam.MaxInc;
		}
	}




}





/***********************************************************************
*                                                                      *
*     PID Regler Funktionalität für beide Preheat Kreise               *
*                                                                      *
*                                                                      *
***********************************************************************/
void PreheatPID(void)
{
	int j;
	int k;
/*
	SetpointCorrectionPerPlate();
*/
	TempCorrectionValue = 0; /* rausnehmen, wenn Aufruf von SetpointCorrectionPerPlate
	                          * wieder einkommentiert wird !!
                              */

/* depending on IO config we have 2 or 4 Controllers running*/
	switch (IOConfiguration)
	{
		case 1 /* Standard Bluefin */:
		{
			ActualNumberOfControllers = 2;
			break;
		}
		case 2 /* Bluefin 1250 */:
		{
			ActualNumberOfControllers = 4;
			break;
		}
		default:
		{
			ActualNumberOfControllers = 2;
			break;
		}
	}


	for (j=0;j<ActualNumberOfControllers;j++)
		EGMPreheat.HeatingOn[j] = HeatingON[j];

/* Preheat enabled: Fans on as soon as Controlvoltage is on*/
	if( EGMPreheat.Enable )
		EGMPreheat.FansOn = ControlVoltageOk;
	else /* Preheat Off and Temp. in both Heatings < 100 °C
	      * or Emergency stop
	      * -> Fans OFF
	      */
	if( !ControlVoltageOk ||
	    (EGMPreheat.RealTemp[0] < 1000
	 &&  EGMPreheat.RealTemp[1] < 1000
	 && (LCPWM_DMod_var[0].x == 0)
	 && (LCPWM_DMod_var[1].x == 0))
	  )
		EGMPreheat.FansOn = 0;
	else	/* Preheat OFF, Controlvoltage ON, but both heatings >110 °C
	         * or one heating is ON -> Fans on
	         */
	if( ((EGMPreheat.RealTemp[0] > 1100) && ( EGMPreheat.RealTemp[1] > 1100 ))
	   || (LCPWM_DMod_var[0].x != 0)
	   || (LCPWM_DMod_var[1].x != 0)
	)
		EGMPreheat.FansOn = 1;


/**
Heizung defekt erkennen:
Ausgang 100% aber Temperatur steigt nicht
Erkennung für beide Heizkreise einzeln
**/
	for (i = 0;i<ActualNumberOfControllers;i++)
	{
		HeatingFailureTimer[i].IN = (LCPWM_DMod_var[i].x == 100) && !HeatingFailureTimer[i].Q;
		TON_10ms(&HeatingFailureTimer[i]);

		if( HeatingFailureTimer[i].Q )
		{
			/*
			 * 10 sec 100% Heating, but no rising temp. -> Alarm on
			*/
/* alarms for 1 and 2 are number 19 and 20*/
			if(i<2)
			{
				if( EGMPreheat.RealTemp[i] <= LastTemp[i]+10)
					EGM_AlarmBitField[19+i] = 1;
				else	/* Temp. did rise: Alarm off*/
					EGM_AlarmBitField[19+i] = 0;
			}
/* alarms for 3 and 4 are number 29 and 30*/
			else
			{
				if( EGMPreheat.RealTemp[i] <= LastTemp[i]+10)
					EGM_AlarmBitField[29+i] = 1;
				else	/* Temp. did rise: Alarm off*/
					EGM_AlarmBitField[29+i] = 0;
			}

			/* save current temp. for next check in 10 sec */
			LastTemp[i] = EGMPreheat.RealTemp[i];
		}
/* Heating off, or controller doesn't heat 100% -> Alarm Off*/
		if (LCPWM_DMod_var[i].x < 100)
		{
			if(i<2)
				EGM_AlarmBitField[19+i] = 0;
			else
				EGM_AlarmBitField[29+i] = 0;
		}
	}


/*Sampling der Werte für die Kurve mit einstellbarer Samplingzeit*/
	if (SamplingTime<10) SamplingTime = 100;
	Pulse.PT = SamplingTime;
	TON_10ms(&Pulse);
	if (Pulse.Q)
	{
/* take curve for showing on screen*/
		if ( TriggerLoggingOld!=TriggerLogging )
			ValueCount = 0;
		TriggerLoggingOld = TriggerLogging;
		if (TriggerLogging)
		{
/*erstes Mal? Kurve löschen*/
			if (ValueCount == 0)
			{
				int j;
				for (j=0;j<302;j++)
#ifndef TESTCURVE
				{
					Values[j] = 0;
					AmbientValues[j] = 0;
				}
#else
				{
					LCCurveByPoints_var.x = 20*j ;
					LCCurveByPoints_var2.x = 20*j;
					LCCurveByPoints(&LCCurveByPoints_var);
					LCCurveByPoints(&LCCurveByPoints_var2);
					Values[j] = EGMPreheatParam.RatedTemp[0] + LCCurveByPoints_var.y - LCCurveByPoints_var2.y;
					AmbientValues[j] = j;
				}
#endif
			}
#ifndef TESTCURVE
			Values[ValueCount] = EGMPreheat.RealTemp[0];
			AmbientValues[ValueCount] = EGMPreheat.AmbientTemp[0];
#endif
			ValueCount++;
			if(ValueCount>301)
			{
				ValueCount = 0;
				TriggerLogging = 0;
			}
			RedrawCurve = 1;
		}
		Pulse.IN = 0;
	}
	else
		Pulse.IN = 1;

/* Fester Pulstimer mit 1 sec Takt*/
	PulseTimer1.IN = 1;
	if(PulseTimer1.Q)
		PulseTimer1.IN = 0;
	TON_10ms(&PulseTimer1);

	if (PulseTimer1.Q)
	{
/*check, deviation was smaller than allowed max deviation during one minute*/
		if(RangeCheckCount>=NUMBER_OF_RANGECHECK_SECONDS)
			RangeCheckCount = 0;

		for (k=0;k<ActualNumberOfControllers;k++)
		{
			RangeCheckArray[k][RangeCheckCount] = EGMPreheat.RealTemp[k];
			EGMPreheatParam.MaxOKTemp = EGMPreheatParam.RatedTempCorr[k] + EGMPreheatParam.ToleratedDevPos;
			EGMPreheatParam.MinOKTemp = EGMPreheatParam.RatedTempCorr[k] - EGMPreheatParam.ToleratedDevNeg;
			flag[k] = TRUE;
			for (j=0;j<NUMBER_OF_RANGECHECK_SECONDS;j++)
			{
				if (	(RangeCheckArray[k][j] > EGMPreheatParam.MaxOKTemp)
				||		(RangeCheckArray[k][j] < EGMPreheatParam.MinOKTemp)	)
				{
						flag[k] = FALSE;
						break;
				}
			}
			if(ActualNumberOfControllers == 2)
				EGMPreheat.TempInRange = flag[0] && flag[1];
			else
				EGMPreheat.TempInRange = flag[0] && flag[1] && flag[2] && flag[3];

			EGMPreheat.TempNotInRange = !EGMPreheat.TempInRange;
		}
		RangeCheckCount++;
/* Alarme */
		if (ModulStatus_AT == 1)
		{

		/*Sensor defekt*/
			EGM_AlarmBitField[15] =	(EGMPreheat.RealTemp[0] > 32000 ) || (EGMPreheat.RealTemp[0] < -500 );
			EGM_AlarmBitField[16] =	(EGMPreheat.RealTemp[1] > 32000 ) || (EGMPreheat.RealTemp[1] < -500 );

			if(ActualNumberOfControllers == 4)
			{
				EGM_AlarmBitField[47] =	(EGMPreheat.RealTemp[2] > 32000 ) || (EGMPreheat.RealTemp[2] < -500 );
				EGM_AlarmBitField[48] =	(EGMPreheat.RealTemp[3] > 32000 ) || (EGMPreheat.RealTemp[3] < -500 );

				EGM_AlarmBitField[41] =	(EGMPreheat.RealTemp[2] < (EGMPreheatParam.RatedTempCorr[2] - EGMPreheatParam.ToleratedDevNeg))
								&& !EGM_AlarmBitField[47];
				EGM_AlarmBitField[42] =	(EGMPreheat.RealTemp[2] > (EGMPreheatParam.RatedTempCorr[2] + EGMPreheatParam.ToleratedDevPos))
								&& !EGM_AlarmBitField[47];

				EGM_AlarmBitField[43] =	(EGMPreheat.RealTemp[3] < (EGMPreheatParam.RatedTempCorr[3] - EGMPreheatParam.ToleratedDevNeg))
								&& !EGM_AlarmBitField[48];
				EGM_AlarmBitField[44] =	(EGMPreheat.RealTemp[3] > (EGMPreheatParam.RatedTempCorr[3] + EGMPreheatParam.ToleratedDevPos))
								&& !EGM_AlarmBitField[48];

				EGM_AlarmBitField[45] = (EGMPreheat.RealTemp[2] > (EGMPreheatParam.RatedTempCorr[2]+OVERHEATTEMP)) && !EGM_AlarmBitField[47];
				EGM_AlarmBitField[46] = (EGMPreheat.RealTemp[3] > (EGMPreheatParam.RatedTempCorr[3]+OVERHEATTEMP)) && !EGM_AlarmBitField[48];
			}
			else
			{
				EGM_AlarmBitField[41] =	0;
				EGM_AlarmBitField[42] =	0;
				EGM_AlarmBitField[43] =	0;
				EGM_AlarmBitField[44] =	0;
				EGM_AlarmBitField[45] =	0;
				EGM_AlarmBitField[46] =	0;
				EGM_AlarmBitField[47] =	0;
				EGM_AlarmBitField[48] =	0;
			}

			EGM_AlarmBitField[17] =	(EGMPreheat.AmbientTemp[0] > 32000 ) || (EGMPreheat.AmbientTemp[0] < -500 );
		/*Temp. nicht im Range*/
			EGM_AlarmBitField[8] =	(EGMPreheat.RealTemp[0] < (EGMPreheatParam.RatedTempCorr[0] - EGMPreheatParam.ToleratedDevNeg))
								&& !EGM_AlarmBitField[15];
			EGM_AlarmBitField[9] =	(EGMPreheat.RealTemp[0] > (EGMPreheatParam.RatedTempCorr[0] + EGMPreheatParam.ToleratedDevPos))
								&& !EGM_AlarmBitField[15];
			EGM_AlarmBitField[10] =	(EGMPreheat.RealTemp[1] < (EGMPreheatParam.RatedTempCorr[1] - EGMPreheatParam.ToleratedDevNeg))
								&& !EGM_AlarmBitField[16];
			EGM_AlarmBitField[11] =	(EGMPreheat.RealTemp[1] > (EGMPreheatParam.RatedTempCorr[1] + EGMPreheatParam.ToleratedDevPos))
								&& !EGM_AlarmBitField[16];
		/* Überhitzt*/
			EGM_AlarmBitField[24] = (EGMPreheat.RealTemp[0] > (EGMPreheatParam.RatedTempCorr[0]+OVERHEATTEMP)) && !EGM_AlarmBitField[15];
			EGM_AlarmBitField[25] = (EGMPreheat.RealTemp[1] > (EGMPreheatParam.RatedTempCorr[1]+OVERHEATTEMP)) && !EGM_AlarmBitField[16];
		}
		else	/* Modulsttatus nicht OK? Werte nicht gültig, also keine Fehlermeldung */
		{
			EGM_AlarmBitField[15] =	0;
			EGM_AlarmBitField[16] =	0;
			EGM_AlarmBitField[17] =	0;
			EGM_AlarmBitField[8] =	0;
			EGM_AlarmBitField[9] =	0;
			EGM_AlarmBitField[10] =	0;
			EGM_AlarmBitField[24] =	0;
			EGM_AlarmBitField[25] =	0;

			EGM_AlarmBitField[41] =	0;
			EGM_AlarmBitField[42] =	0;
			EGM_AlarmBitField[43] =	0;
			EGM_AlarmBitField[44] =	0;
			EGM_AlarmBitField[45] =	0;
			EGM_AlarmBitField[46] =	0;
			EGM_AlarmBitField[47] =	0;
			EGM_AlarmBitField[48] =	0;
		}

/* Zeitmessung f Sollwertnachführung*/
		if(SPTrackCount>NUMBER_OF_SECONDS )
			StopSPTracking = 1;

		if(ActualNumberOfControllers == 2)
		{
			if (EGMPreheat.Auto[0]
			 && EGMPreheat.Auto[1]
			 && EGMPreheat.TempInRange)
				SPTrackCount++;
		}
		else
		{
			if (EGMPreheat.Auto[0]
			 && EGMPreheat.Auto[1]
			 && EGMPreheat.Auto[2]
			 && EGMPreheat.Auto[3]
			 && EGMPreheat.TempInRange)
				SPTrackCount++;
		}
	}

/*Wert f Sollwertnachführung bestimmen (Ausgang wird in der Schleife verarbeitet)*/
	LCCurveByPoints_var.x = SPTrackCount ;
	LCCurveByPoints_var2.x = SPTrackCount ;
	LCCurveByPoints(&LCCurveByPoints_var);
	LCCurveByPoints(&LCCurveByPoints_var2);

/*********************************************************************************************/
/*********************************************************************************************/
	if (PreheatParamEnter)
	{
		PreheatParamEnter = 0;
		for (i = 0;i<ActualNumberOfControllers;i++)
		{
			LCPIDpara_var[i].enter = 1;
			LCPIDpara_var[i].Kp = EGMPreheatParam.ReglerParam[i].Kp;
			LCPIDpara_var[i].Tn = EGMPreheatParam.ReglerParam[i].Tn;
			LCPIDpara_var[i].Tv = EGMPreheatParam.ReglerParam[i].Tv;
			LCPIDpara_var[i].Tf = EGMPreheatParam.ReglerParam[i].Tf;
			LCPIDpara_var[i].Kw = EGMPreheatParam.ReglerParam[i].Kw;
			LCPIDpara_var[i].Kfbk = EGMPreheatParam.ReglerParam[i].Kfbk;
/*
			LCPWM_DMod_var[i].tminpuls = EGMPreheatParam.ReglerParam[i].tminpuls;
			LCPWM_DMod_var[i].tperiod = EGMPreheatParam.ReglerParam[i].tperiod;
*/
			LCPWM_DMod_var[i].tminpuls			= 10;  /* 10 ms kürzester Puls bzw pause*/
			LCPWM_DMod_var[i].tperiod			= 200;  /*Periodendauer 0,2 sec*/

			if ( ( EGMPreheatParam.ReglerParam[i].d_mode != LCPID_D_MODE_X )&&
				( EGMPreheatParam.ReglerParam[i].d_mode != LCPID_D_MODE_E ))
			{
				LCPIDpara_var[i].d_mode = LCPID_D_MODE_X;
				EGMPreheatParam.ReglerParam[i].d_mode = LCPID_D_MODE_X;
			}
			else
				LCPIDpara_var[i].d_mode = EGMPreheatParam.ReglerParam[i].d_mode;

		}
/*für den Fall, daß sich die Parameter geändert haben*/
		Preset_CorrCurve();
	}
	else
		for (i = 0;i<ActualNumberOfControllers;i++)
			LCPIDpara_var[i].enter = 0;

/* wenn eine der Ist-Temperaturen über ein absolutes Maximum geht ist das Preheat
überhitzt, das Overheated flag schaltet dann den Ausgang ab
*/
	if(ActualNumberOfControllers == 2)
		Overheated = (EGMPreheat.RealTemp[0] > (EGMPreheatParam.RatedTempCorr[0]+OVERHEATTEMP))
				  || (EGMPreheat.RealTemp[1] > (EGMPreheatParam.RatedTempCorr[1]+OVERHEATTEMP));
	else
		Overheated = (EGMPreheat.RealTemp[0] > (EGMPreheatParam.RatedTempCorr[0]+OVERHEATTEMP))
				  || (EGMPreheat.RealTemp[1] > (EGMPreheatParam.RatedTempCorr[1]+OVERHEATTEMP))
				  || (EGMPreheat.RealTemp[2] > (EGMPreheatParam.RatedTempCorr[2]+OVERHEATTEMP))
				  || (EGMPreheat.RealTemp[3] > (EGMPreheatParam.RatedTempCorr[3]+OVERHEATTEMP));

	if(Overheated && ControlVoltageOk)
	{
/* Not Aus erzeugen */
		ActivateControlVoltage = OFF;
		ShowMessage(136,137,0,CAUTION,OKONLY, FALSE);
	}



/*********************************************************************************************/
/* Schleife über die implementierten Regelkreise*/
/*********************************************************************************************/
	for (i = 0;i<ActualNumberOfControllers;i++)
	{
/* Parameter ungültig? dann Standard setzen*/
		if ( ( LCPIDpara_var[i].d_mode != LCPID_D_MODE_X )&&
			( LCPIDpara_var[i].d_mode != LCPID_D_MODE_E ))
		{
			LCPIDpara_var[i].d_mode	= LCPID_D_MODE_X;
			EGMPreheatParam.ReglerParam[i].d_mode = LCPID_D_MODE_X;
		}


	/*********************************************************************************************/
	/* Stellgröße auf PWM Modul geben */
		if (ControlVoltageOk && EGMPreheat.FansOn && EGMPreheat.Enable && !Overheated)
			LCPWM_DMod_var[i].x 				= LCPID_var[i].Y;
		else
			LCPWM_DMod_var[i].x 				= 0;
	/* Abtast Takt*/
		LCPWM_DMod_var[i].basetime		= LCCountVar.mscnt;

	/*  PWM Ansteuerung des Ausgangs */
		LCPWM_DMod(&LCPWM_DMod_var[i]);
		HeatingON[i] = LCPWM_DMod_var[i].y
						&& ControlVoltageOk
						&& EGMPreheat.FansOn
						&& EGMPreheat.Enable
						&& !Overheated;

	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/
	/* Parametrierbaustein*/

		LCPIDpara(&LCPIDpara_var[i]);
	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/
	/* AutoTune Baustein*/

	/*
	Anforderung für Autotuning
	LCPIDAUTOTUNE_REQU_OFF: Autotuning inaktiv, Regler ist freigegeben.
	LCPIDAUTOTUNE_REQU_STEPRESPONSE: Autotuning nur des Führungsverhaltens (Aufnahme einer Sprungantwort).
	LCPIDAUTOTUNE_REQU_OSCILLATE: Autotuning des Störverhaltens durch Anregen der Strecke zum Schwingen.
	LCPIDAUTOTUNE_REQU_AUTOTUNE: Autotuning des Führungsverhaltens mit anschließendem Autotuning des Störverhaltens
	*/
		if (StartAutoTune)
			LCPIDAutoTune_var[0].request				= LCPIDAUTOTUNE_REQU_AUTOTUNE;
		else
			LCPIDAutoTune_var[i].request				= LCPIDAUTOTUNE_REQU_OFF;

		LCPIDAutoTune_var[i].basetime				= LCCountVar.mscnt;

/*		LCPIDAutoTune(&LCPIDAutoTune_var[i]);*/


	/*********************************************************************************************/
	/*********************************************************************************************/
	/*********************************************************************************************/
	/* Reglerbaustein LCPID*/
	/* OHNE AUTOTUNE*/
		if( i!=0 )
			LCPID_var[i].ident						= LCPIDpara_var[i].ident;

	/* Mit AUTOTUNE*/
		if (StartAutoTune)
			LCPID_var[0].ident						= LCPIDAutoTune_var[0].ident;
		else
			LCPID_var[0].ident						= LCPIDpara_var[0].ident;


/*Sollwert tracking mit e-funktion*/
/*
		if (EGMPreheatParam.ReglerParam[i].SetpointTracking && !StopSPTracking)
			EGMPreheatParam.RatedTempCorr[i]  = EGMPreheatParam.RatedTemp[i] + LCCurveByPoints_var.y;
		else
			EGMPreheatParam.RatedTempCorr[i]  = EGMPreheatParam.RatedTemp[i];
*/
/* ENDE Sollwert tracking mit e-funktion*/


/*Sollwert tracking mit gerade f(T[ambient])*/
		if (EGMPreheatParam.ReglerParam[i].SetpointTracking )
		{
			REAL m,b;
			int CorrValue;
/* gerade wird durch 2 Punkte gelegt
x1 = EGMPreheatParam.StartAmbient[0]
y1 = (EGMPreheatParam.EndSetpoint[0] + EGMPreheatParam.StartOverheat[0])

x2 = EGMPreheatParam.StartAmbient[0]
y2 = EGMPreheatParam.EndAmbient[0]
*/

/*
	m = (y2-y1)/(x2-x1)
	b  =  y2 - m*x2  ODER  b  =  y1- m*x1
*/
			EGMPreheatParam.EndSetpoint[0] = EGMPreheatParam.RatedTemp[0];

			m = ((REAL)EGMPreheatParam.EndSetpoint[0] - (REAL)(EGMPreheatParam.EndSetpoint[0] + EGMPreheatParam.StartOverheat[0]))
				/ ((REAL)EGMPreheatParam.EndAmbient[0] - (REAL)EGMPreheatParam.StartAmbient[0]);
			b = (REAL)EGMPreheatParam.EndSetpoint[0] - m*EGMPreheatParam.EndAmbient[0];
			CorrValue =10*((m * ((REAL)EGMPreheat.AmbientTemp[0]/10.0) + (b/10.0)))
													-	EGMPreheatParam.RatedTemp[0]
													+	EGMPreheatParam.RatedTemp[i];;
			if (EGMPreheat.AmbientTemp[0] < 250)
				EGMPreheatParam.RatedTempCorr[i] = EGMPreheatParam.RatedTemp[i] + EGMPreheatParam.StartOverheat[0];
			else
/* wenn Korrigierter Wert kleiner wäre als Sollwert
dann direkt auf festen Sollwert schalten,
sonst auf korrigierten Sollwert
*/
			if ( (CorrValue < EGMPreheatParam.RatedTemp[i]) )
				EGMPreheatParam.RatedTempCorr[i] = EGMPreheatParam.RatedTemp[i];
			else
				EGMPreheatParam.RatedTempCorr[i]  = CorrValue;
		}
		else /* kein Setpoint Tracking aktiviert */
			EGMPreheatParam.RatedTempCorr[i]  = EGMPreheatParam.RatedTemp[i];
/*ENDE Sollwert tracking mit f(T[ambient])*/

		LCPID_var[i].W							= EGMPreheatParam.RatedTempCorr[i];

	/*Istwert*/
		LCPID_var[i].X							= EGMPreheat.RealTemp[i];
	/*rückgeführte Stellgröße*/
		LCPID_var[i].Y_fbk					= LCPID_var[i].Y;
	/*
	Reglerbetriebsart:
	LCPID_OUT_MODE_AUTO: Automatikbetrieb

	LCPID_OUT_MODE_MAN: Handbetrieb, Stellgröße fährt auf Y_man.
	LCPID_OUT_MODE_OPEN: Stellgröße fährt auf Y_max.
	LCPID_OUT_MODE_CLOSE: Stellgröße fährt auf Y_min.
	LCPID_OUT_MODE_FREEZE: Stellgröße wird eingefroren.

	Beim Zurückschalten aus diesen Modi auf OUT_MODE_AUTO wird eine evtl. bestehende
	Regelabweichung durch den P-Anteil ausgeregelt.

	LCPID_MODE_xxxx_JOLTFREE: Das Zurückschalten auf OUT_MODE_AUTO erfolgt völlig stoßfrei
	sofern der I-Anteil aktiviert ist!) - eine dann bestehende Regelabweichung kann aber nur sukzessiv durch den I-Anteil ausgeregelt */
		if (EGMPreheat.Auto[i])
			LCPID_var[i].out_mode				= LCPID_OUT_MODE_AUTO;
		else
			LCPID_var[i].out_mode				= LCPID_OUT_MODE_MAN;

		LCPID_var[i].basetime					= LCCountVar.mscnt;
		LCPID(&LCPID_var[i]);

/*
		if(StartAutoTune)
		{
			ReglerParam = LCPIDAutoTune_var[0].processPar;
			LCPIDpara_var[i].Kp				= ReglerParam.Kp;
			LCPIDpara_var[i].Tn				= ReglerParam.Tn;
			LCPIDpara_var[i].Tv				= ReglerParam.Tv;
			LCPIDpara_var[i].Tf				= ReglerParam.Tv/10;
		}
*/
	}/* ENDE der for-schleife über die Regler*/

}



