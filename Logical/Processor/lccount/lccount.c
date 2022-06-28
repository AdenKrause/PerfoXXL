#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************************/
/*	LCCount aufrufen (Zeitbasis für Regler)*/
/**/
/******************************************************************************************/

#include "EGMglob_var.h"
#include <loopcont.h>

#define MM_PER_PULSE (251.0/17.0)

R_TRIGtyp	RPMSignalEdgeR;
/*timer für die Messung der Periodendauer des Antriebs-Taktes*/
REAL	tmpval,tmpval2,PeriodTime; /* in sec */


CTUtyp	PlateLengthCounter;

/*timer für die Messung der Periodendauer des Antriebs-Taktes*/
TON_typ	PeriodTimeRPM;
/*timer für die Messung der Verzögerung Plattenvorderkante->nächster Takt*/
TON_10ms_typ	PlateStartToRPMDelay;
/*timer für die Messung der Verzögerung Plattenhinterkante->nächster Takt*/
TON_10ms_typ	PlateEndToRPMDelay;
/*Var zum Speichern der gemessenen Zeiten*/
UDINT	StartDelay,EndDelay;

R_TRIGtyp	InputSensorEdgeR;
F_TRIGtyp	InputSensorEdgeF;
BOOL	 EnablePulseCounter;

_GLOBAL REAL PlateLength;
/* zum Entprellen des Eingangssensors*/
TON_10ms_typ	InputSensorOnDelay;
TOF_10ms_typ	InputSensorOffDelay;

/* ohne Mittelwertbildung */
void MeasureCurrentSpeed(void)
{

	RPMSignalEdgeR.CLK = RPMCheck;
	R_TRIG(&RPMSignalEdgeR);
/* steigende Flanke und timer ist gestartet: Zeit nehmen und stoppen*/
	if(RPMSignalEdgeR.Q )
	{
/* 2.02: Messung per Timer statt Zykluszähler */
		PeriodTime = PeriodTimeRPM.ET * 0.001; /* timer Wert 1 ms */
		PeriodTimeRPM.IN = FALSE;
	}
	else
		PeriodTimeRPM.IN = TRUE;
	PeriodTimeRPM.PT = 10000;
/* wenn 10 sec lang kein Takt, dann ist die Periodendauer 0 */
	if(PeriodTimeRPM.Q)
		PeriodTime = 0;

 	TON(&PeriodTimeRPM);
/* ENDE Periodendauer des Taktes messen*/
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/* START Geschwindigkeit und Plattenlänge errechnen*/
/* Periodendauer ist gegen 0: Antrieb läuft nicht*/
/* Abfrage wichtig, da durch PeriodTime geteilt wird !!*/
	if (PeriodTime < 0.05)/* kürzer als 50 ms : ungültig, wäre 12 m/min*/
	{
		EGMGlobalParam.CurrentPlateSpeed = 0;
	}
	else
	{
/* 251mm/U / 17Takte/U =14.76 mm/Takt steht in MM_PER_PULSE */
/*PeriodTime*0.01 weil Timerwert in 10ms */
/* *0.06 weil Speed in m/min, Ergebnis vorher ist in mm/s*/
/* gilt so nur, wenn die Speed konstant ist, um schwankungen zu erfassen müsste man integrieren*/

	 	EGMGlobalParam.CurrentPlateSpeed = (MM_PER_PULSE / (PeriodTime))*0.06;
		tmpval = EGMGlobalParam.CurrentPlateSpeed / EGMGlobalParam.MainMotorFactor;
		if (tmpval <= 32767.0)
			EGMMainMotor.RealRpm = (INT)tmpval;
		else
			EGMMainMotor.RealRpm = 32767;

	}
/* ENDE Geschwindigkeit und Plattenlänge errechnen*/
/*-------------------------------------------------------------------------------------------*/
}


void MeasurePlateLength(void)
{

/**********************************************************************************/
/* Plattenlänge ermitteln*/

/*
Strategie für erhöhte Genauigkeit:
	- kontinuierlich Periodendauer des Taktsignals vom Antrieb messen (jede 2. Periode)
	- Zeit vom Erkennen des Plattenanfangs bis zum nächsten Takt messen
	- Zeit vom Erkennen des Plattenendes bis zum nächsten Takt messen
	- aus den gezählten Takten und den gemessenen Zeiten die Plattenlänge
	  ermitteln
*/

/*Entprellen je 1 sec*/
	InputSensorOnDelay.PT = 100;
	InputSensorOffDelay.PT = 100;
	InputSensorOnDelay.IN = InputSensor;
	InputSensorOffDelay.IN = InputSensor;
	TON_10ms(&InputSensorOnDelay);
	TOF_10ms(&InputSensorOffDelay);
	InputSensorEdgeR.CLK = InputSensorOnDelay.Q;
	InputSensorEdgeF.CLK = InputSensorOffDelay.Q;
	R_TRIG(&InputSensorEdgeR);
	F_TRIG(&InputSensorEdgeF);

/*-------------------------------------------------------------------------------------------*/
/* START Messung Verzögerung Plattenvorderkante->nächster Takt*/
/*mit steigender Flanke des Sensors Zeitmessung starten*/
	PlateStartToRPMDelay.PT = 60000;
	if (InputSensorEdgeR.Q && !PlateStartToRPMDelay.IN)
		PlateStartToRPMDelay.IN = 1;
/*bei der nächsten steigenden Flanke des Taktes die Zeit nehmen*/
/* || PlateStartToRPMDelay.Q nur für den Fall, dass der Antrieb steht o.ä.*/
	if (PlateStartToRPMDelay.IN && (RPMSignalEdgeR.Q || PlateStartToRPMDelay.Q ))
	{
		PlateStartToRPMDelay.IN = 0;
		StartDelay = PlateStartToRPMDelay.ET;
	}
	TON_10ms(&PlateStartToRPMDelay);
/* ENDE Messung Verzögerung Plattenvorderkante->nächster Takt*/
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/* START Messung Verzögerung Plattenhinterkante->nächster Takt*/
/*mit steigender Flanke des Sensors Zeitmessung starten*/
	PlateEndToRPMDelay.PT = 60000;
	if (InputSensorEdgeF.Q && !PlateEndToRPMDelay.IN)
		PlateEndToRPMDelay.IN = 1;
/*bei der nächsten steigenden Flanke des Taktes die Zeit nehmen*/
/* || PlateEndToRPMDelay.Q nur für den Fall, dass der Antrieb steht o.ä.*/
	if (PlateEndToRPMDelay.IN && (RPMSignalEdgeR.Q || PlateEndToRPMDelay.Q ))
	{
		PlateEndToRPMDelay.IN = 0;
		EndDelay = PlateEndToRPMDelay.ET;
	}
	TON_10ms(&PlateEndToRPMDelay);
/* ENDE Messung Verzögerung Plattenhinterkante->nächster Takt*/
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/* START Zählen der Takte, während Platte durchläuft*/
	CTU(&PlateLengthCounter);

/*bei fallender Flanke den Zähler auswerten*/
	if(InputSensorEdgeF.Q)
	{
/* nicht durch 0 teilen... */
		if(PeriodTime < 0.05)
			tmpval2 = PlateLengthCounter.CV;
		else
			tmpval2 = PlateLengthCounter.CV+ ((REAL)StartDelay)/(PeriodTime*100.0) - ((REAL)EndDelay)/(PeriodTime*100.0);
		PlateLength = tmpval2 * MM_PER_PULSE;
		if (PlateLength < 0.0) PlateLength = 0.0;
		if (PlateLength > 3000.0) PlateLength = 0.0;
		EnablePulseCounter = 0;
	}
/* mit Einlaufstart den Pulszähler enablen*/
	if(InputSensorEdgeR.Q)
	{
		EnablePulseCounter = 1;
	}
	if (EnablePulseCounter)
	{
		PlateLengthCounter.CU = RPMCheck;
	}
/*zur Sicherheit den Pulscounter disablen, wenn keine Platte im Einlaufbereich ist*/
	if ( !InputSensorOnDelay.Q && !InputSensorOffDelay.Q)
		EnablePulseCounter = 0;

/*bei Start einer Platte Zähler resetten, Takte zählen*/
	PlateLengthCounter.RESET = InputSensorEdgeR.Q;

/* ENDE Zählen der Takte, während Platte durchläuft*/
/*-------------------------------------------------------------------------------------------*/
}

_INIT void init(void)
{
	PlateLength = 0;
/*	PeriodTimeCounter = 0;*/
	PeriodTime = 0;
	PeriodTimeRPM.IN = FALSE;
}

_CYCLIC void cyk(void)
{
	LCCounter(&LCCountVar);
	MeasureCurrentSpeed();
	MeasurePlateLength();
}


