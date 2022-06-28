#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************************/
/*	LCCount aufrufen (Zeitbasis f�r Regler)*/
/**/
/******************************************************************************************/

#include "EGMglob_var.h"
#include <loopcont.h>

#define MM_PER_PULSE (251.0/17.0)

R_TRIGtyp	RPMSignalEdgeR;
/*timer f�r die Messung der Periodendauer des Antriebs-Taktes*/
REAL	tmpval,tmpval2,PeriodTime; /* in sec */


CTUtyp	PlateLengthCounter;

/*timer f�r die Messung der Periodendauer des Antriebs-Taktes*/
TON_typ	PeriodTimeRPM;
/*timer f�r die Messung der Verz�gerung Plattenvorderkante->n�chster Takt*/
TON_10ms_typ	PlateStartToRPMDelay;
/*timer f�r die Messung der Verz�gerung Plattenhinterkante->n�chster Takt*/
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
/* 2.02: Messung per Timer statt Zyklusz�hler */
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
/* START Geschwindigkeit und Plattenl�nge errechnen*/
/* Periodendauer ist gegen 0: Antrieb l�uft nicht*/
/* Abfrage wichtig, da durch PeriodTime geteilt wird !!*/
	if (PeriodTime < 0.05)/* k�rzer als 50 ms : ung�ltig, w�re 12 m/min*/
	{
		EGMGlobalParam.CurrentPlateSpeed = 0;
	}
	else
	{
/* 251mm/U / 17Takte/U =14.76 mm/Takt steht in MM_PER_PULSE */
/*PeriodTime*0.01 weil Timerwert in 10ms */
/* *0.06 weil Speed in m/min, Ergebnis vorher ist in mm/s*/
/* gilt so nur, wenn die Speed konstant ist, um schwankungen zu erfassen m�sste man integrieren*/

	 	EGMGlobalParam.CurrentPlateSpeed = (MM_PER_PULSE / (PeriodTime))*0.06;
		tmpval = EGMGlobalParam.CurrentPlateSpeed / EGMGlobalParam.MainMotorFactor;
		if (tmpval <= 32767.0)
			EGMMainMotor.RealRpm = (INT)tmpval;
		else
			EGMMainMotor.RealRpm = 32767;

	}
/* ENDE Geschwindigkeit und Plattenl�nge errechnen*/
/*-------------------------------------------------------------------------------------------*/
}


void MeasurePlateLength(void)
{

/**********************************************************************************/
/* Plattenl�nge ermitteln*/

/*
Strategie f�r erh�hte Genauigkeit:
	- kontinuierlich Periodendauer des Taktsignals vom Antrieb messen (jede 2. Periode)
	- Zeit vom Erkennen des Plattenanfangs bis zum n�chsten Takt messen
	- Zeit vom Erkennen des Plattenendes bis zum n�chsten Takt messen
	- aus den gez�hlten Takten und den gemessenen Zeiten die Plattenl�nge
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
/* START Messung Verz�gerung Plattenvorderkante->n�chster Takt*/
/*mit steigender Flanke des Sensors Zeitmessung starten*/
	PlateStartToRPMDelay.PT = 60000;
	if (InputSensorEdgeR.Q && !PlateStartToRPMDelay.IN)
		PlateStartToRPMDelay.IN = 1;
/*bei der n�chsten steigenden Flanke des Taktes die Zeit nehmen*/
/* || PlateStartToRPMDelay.Q nur f�r den Fall, dass der Antrieb steht o.�.*/
	if (PlateStartToRPMDelay.IN && (RPMSignalEdgeR.Q || PlateStartToRPMDelay.Q ))
	{
		PlateStartToRPMDelay.IN = 0;
		StartDelay = PlateStartToRPMDelay.ET;
	}
	TON_10ms(&PlateStartToRPMDelay);
/* ENDE Messung Verz�gerung Plattenvorderkante->n�chster Takt*/
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/* START Messung Verz�gerung Plattenhinterkante->n�chster Takt*/
/*mit steigender Flanke des Sensors Zeitmessung starten*/
	PlateEndToRPMDelay.PT = 60000;
	if (InputSensorEdgeF.Q && !PlateEndToRPMDelay.IN)
		PlateEndToRPMDelay.IN = 1;
/*bei der n�chsten steigenden Flanke des Taktes die Zeit nehmen*/
/* || PlateEndToRPMDelay.Q nur f�r den Fall, dass der Antrieb steht o.�.*/
	if (PlateEndToRPMDelay.IN && (RPMSignalEdgeR.Q || PlateEndToRPMDelay.Q ))
	{
		PlateEndToRPMDelay.IN = 0;
		EndDelay = PlateEndToRPMDelay.ET;
	}
	TON_10ms(&PlateEndToRPMDelay);
/* ENDE Messung Verz�gerung Plattenhinterkante->n�chster Takt*/
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/
/* START Z�hlen der Takte, w�hrend Platte durchl�uft*/
	CTU(&PlateLengthCounter);

/*bei fallender Flanke den Z�hler auswerten*/
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
/* mit Einlaufstart den Pulsz�hler enablen*/
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

/*bei Start einer Platte Z�hler resetten, Takte z�hlen*/
	PlateLengthCounter.RESET = InputSensorEdgeR.Q;

/* ENDE Z�hlen der Takte, w�hrend Platte durchl�uft*/
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


