#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
 Handling of Motors
 - Main Motor (Propulsion of plate)
 - Brush Motor

*************************************************************************************************/

#include "glob_var.h"
#include "egmglob_var.h"
#include <math.h>


TP_10ms_typ		RPMCheckTimer;
F_TRIGtyp 			RPMCheckEdge;
CTUtyp	RPMEdgeCounter;
_LOCAL	UINT	CounterValue;
TON_10ms_typ		BrushMotorOnTimer1,BrushMotorOnTimer2;
TON_10ms_typ		RPMSignalCheckTimer,RPMDeviationTimer;
/* Input delay timer for ControlVoltage signal */
TON_10ms_typ		ControlVoltageOkDelay;

REAL	tmpval;

TON_10ms_typ	PulseTimer1;
BOOL	Pulse_1s;
_LOCAL REAL MainMotorFactorFeet;

_INIT void init()
{
	CounterValue = 0;
	EGMMainMotor.RatedRpm = 0;
	EGMMainMotor.RealRpm = 0;
	EGMMainMotor.Auto = 0;
	EGMMainMotor.Enable = 0;

	EGMBrushMotor.RatedRpm = 0;
	EGMBrushMotor.RealRpm = 0;
	EGMBrushMotor.Auto = 0;
	EGMBrushMotor.Enable = 0;

	BrushMotorOnTimer1.IN = 0;
	BrushMotorOnTimer2.IN = 0;

	PulseTimer1.IN = 0;
	PulseTimer1.PT = 100; /* 1 sec */

}


_CYCLIC void cyclic()
{
	PulseTimer1.IN = 1;
	if(PulseTimer1.Q)
		PulseTimer1.IN = 0;
	TON_10ms(&PulseTimer1);
	Pulse_1s = PulseTimer1.Q;

/* Skalierungsfaktor muss in VC Einheitengruppe "Vorschub" identisch sein !! */
/*	EGMGlobalParam.MainMotorFactor = 9.30814539017914E-05;*/
	MainMotorFactorFeet = EGMGlobalParam.MainMotorFactor / 0.3048;
/* das gleiche für Bürstenmotor */
/*  "Alter" Motortyp*/
/*	EGMGlobalParam.BrushFactor = 180.0/32767.0;*/
/*  "Neuer" Motortyp Siemens*/
/*	EGMGlobalParam.BrushFactor = 230.0/32767.0;*/

/* safety:
 * 	Not a number or out of reasonable range ->set default*/
	if( isnan(EGMGlobalParam.BrushFactor)
	 || (EGMGlobalParam.BrushFactor > 0.02)  /* > ca 655 rpm */
	 || (EGMGlobalParam.BrushFactor < 0.001) /* < ca 32.7 rpm */
	  )
		EGMGlobalParam.BrushFactor = 230.0/32767.0;

/*Main Motor*/
/* safety:
 * 	Not a number or out of reasonable range ->set default*/
	if( isnan(EGMGlobalParam.MainMotorFactor)
	 || (EGMGlobalParam.MainMotorFactor > (10.0/32767.0))
	 || (EGMGlobalParam.MainMotorFactor < 1.0/32767.0)
	  )
		EGMGlobalParam.MainMotorFactor = 3.0/32767.0;

	EGMMainMotor.Manual = !EGMMainMotor.Auto;

/**
Prüfen, ob überhaupt Signale vom Taktsensor kommen:
Wenn Sollwert > 0,2 m/min, dann muss der Countervalue > 0 sein
*/
	RPMSignalCheckTimer.IN = ((CounterValue == 0) && ((EGMMainMotor.RatedRpm * EGMGlobalParam.MainMotorFactor)> 0.2 ));
	RPMSignalCheckTimer.PT = 1000; /* 10 sec */
	TON_10ms(&RPMSignalCheckTimer);
	EGM_AlarmBitField[1] = RPMSignalCheckTimer.Q;

	RPMDeviationTimer.PT = 1000;
	RPMDeviationTimer.IN = ((fabs((EGMGlobalParam.MainMotorFactor*EGMMainMotor.RatedRpm) - EGMGlobalParam.CurrentPlateSpeed)) > 0.5)
							&& !EGM_AlarmBitField[1] && EGMMainMotor.Enable && !CurrentState==S_OFF && ControlVoltageOk;
	TON_10ms(&RPMDeviationTimer);
	EGM_AlarmBitField[27] = RPMDeviationTimer.Q;

/* Messung der Speed über Taktgeber*/
	RPMEdgeCounter.CU = RPMCheck;
	RPMEdgeCounter.PV = 32000; /*soll nicht erreicht werden*/

	CTU(&RPMEdgeCounter);
	RPMEdgeCounter.RESET = 0;

	TP_10ms(&RPMCheckTimer);

	RPMCheckTimer.IN = Pulse_1s;
	RPMCheckTimer.PT = 1000; /*10 sec Messen*/

/*fallende Flanke des Pulstimers auswerten*/
	RPMCheckEdge.CLK	= RPMCheckTimer.Q;
	F_TRIG(&RPMCheckEdge);

	if (RPMCheckEdge.Q)
	{
		CounterValue = RPMEdgeCounter.CV;
		RPMEdgeCounter.RESET = 1;
	}

/*im Automatik-Modus die Auto-Drehzahl übernehmen*/
/*-> im Hand Modus kann man dann die RatedRpm direkt verändern*/
/*Spannung muß schon 5 sec lang anliegen, sonst erkennt FU das Signal nicht zuverlässig*/
	ControlVoltageOkDelay.IN = ControlVoltageOk;
	ControlVoltageOkDelay.PT = 500;
	TON_10ms(&ControlVoltageOkDelay);
	if (EGMMainMotor.Auto && ControlVoltageOkDelay.Q)
	{
		EGMMainMotor.Enable = 1;	/* Freigabe Ausgang an FU */
		EGMMainMotor.RatedRpm = EGMMainMotor.AutoRatedRpm;	/* Drehzahl Ausgang an FU */
	}

	if (!ControlVoltageOk)
	{
		EGMMainMotor.Enable = 0;	/* Freigabe Ausgang an FU */
		EGMMainMotor.Auto = 0;
	}

/*************************************************************************************************/

/*Brush Motor*/
	EGMBrushMotor.Manual = !EGMBrushMotor.Auto;
/*keine Messung: Ist = Soll*/
	EGMBrushMotor.RealRpm = EGMBrushMotor.RatedRpm;
/*im Automatik-Modus die Auto-Drehzahl übernehmen*/
/*-> im Hand Modus kann man dann die RatedRpm direkt verändern*/
	if (EGMBrushMotor.Auto && ControlVoltageOkDelay.Q)
	{
		EGMBrushMotor.Enable = 1;	/* Freigabe Ausgang an FU */
/*
		if (BrushMotorOnTimer2.IN )
			EGMBrushMotor.RatedRpm = 150.0 / EGMGlobalParam.BrushFactor;
		else
 */
			EGMBrushMotor.RatedRpm = EGMBrushMotor.AutoRatedRpm;	/* Drehzahl Ausgang an FU */
	}

	if (!ControlVoltageOk)
	{
		EGMBrushMotor.Enable = 0;	/* Freigabe Ausgang an FU */
		EGMBrushMotor.Auto = 0;
	}

	if (EGMBrushMotor.Auto && (EGMBrushMotor.RatedRpm < 100.0/EGMGlobalParam.BrushFactor) )
	{
		BrushMotorOnTimer1.IN = 1;
	}
	else
		BrushMotorOnTimer1.IN = 0;

	BrushMotorOnTimer1.PT = 9000; /* alle 1,5 min*/
	TON_10ms(&BrushMotorOnTimer1);

	BrushMotorOnTimer2.PT = 2000; /* 20 sec*/
	TON_10ms(&BrushMotorOnTimer2);

/* nach Zeit die Bürsten hochdrehen*/
	if (BrushMotorOnTimer1.Q  )
	{
		BrushMotorOnTimer2.IN = 1;
	}

	if (BrushMotorOnTimer2.Q)
	{
		BrushMotorOnTimer2.IN = 0;
	}
}



