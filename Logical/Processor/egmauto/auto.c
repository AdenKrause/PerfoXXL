#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
 Handling of Automatic mode

*************************************************************************************************/

/****	Bedeutung der Ampel Signale

o = AUS, X = AN, b = blinkt

					Grün		Gelb		Rot
Aus					o			o			X
Aufheizen			o			o			X
Betriebsbereit		X			o			o
Verarbeitung		o			X			o
Reinigung			o			b			o
Störung				o			o			b
***/

/*********************************************************************************/
/*	Alarmliste */
/**
x	EGM_AlarmBitField[0]	AlarmNr 1	Notaus
x	EGM_AlarmBitField[1]	AlarmNr 2	kein Antriebstakt
x	EGM_AlarmBitField[2]	AlarmNr 3	EGMGummierung Füllstand zu niedrig
x	EGM_AlarmBitField[3]	AlarmNr 4	Vorspülen Füllstand zu niedrig
x	EGM_AlarmBitField[4]	AlarmNr 5	Nachspülen Füllstand zu niedrig
x	EGM_AlarmBitField[5]	AlarmNr 6	Entwickler Füllstand zu niedrig
x	EGM_AlarmBitField[6]	AlarmNr 7	Entwickler Temperatur zu niedrig
x	EGM_AlarmBitField[7]	AlarmNr 8	Entwickler Temperatur zu hoch
x	EGM_AlarmBitField[8]	AlarmNr 9	Nacherwärmung 1 Temperatur zu niedrig
x	EGM_AlarmBitField[9]	AlarmNr 10	Nacherwärmung 1 Temperatur zu hoch
x	EGM_AlarmBitField[10]	AlarmNr 11	Nacherwärmung 2 Temperatur zu niedrig
x	EGM_AlarmBitField[11]	AlarmNr 12	Nacherwärmung 2 Temperatur zu hoch
x	EGM_AlarmBitField[12]	AlarmNr 13	Plattenstau
	EGM_AlarmBitField[13]	AlarmNr 14	Entwickler zu alt
	EGM_AlarmBitField[14]	AlarmNr 15	E/A Modul nicht gefunden
x	EGM_AlarmBitField[15]	AlarmNr 16	Nacherw. Sensor 1 defekt
x	EGM_AlarmBitField[16]	AlarmNr 17	Nacherw. Sensor 2 defekt
x	EGM_AlarmBitField[17]	AlarmNr 18	Nacherw. Umgeb. Sensor defekt
x	EGM_AlarmBitField[18]	AlarmNr 19	Entwickler Temp. Sensor defekt
x	EGM_AlarmBitField[19]	AlarmNr 20	Nacherw. Heizung 1 defekt
x	EGM_AlarmBitField[20]	AlarmNr 21	Nacherw. Heizung 2 defekt
	EGM_AlarmBitField[21]	AlarmNr 22	Vorspülstation Befüllen fehlgeschlagen
	EGM_AlarmBitField[22]	AlarmNr 23	Nachspülstation Befüllen fehlgeschlagen
	EGM_AlarmBitField[23]	AlarmNr 24	Entwickler Befüllen fehlgeschlagen
x	EGM_AlarmBitField[24]	AlarmNr 25	Nacherwärmung 1 überhitzt
x	EGM_AlarmBitField[25]	AlarmNr 26	Nacherwärmung 2 überhitzt
x	EGM_AlarmBitField[26]	AlarmNr 27	Entwickler überhitzt
x	EGM_AlarmBitField[27]	AlarmNr 28	Antrieb: Geschwindigkeitsabweichung zu groß
x	EGM_AlarmBitField[28]	AlarmNr 29	Temperaturmodul nicht gefunden
**/

#include <stdlib.h>
#include "EGMglob_var.h"
#include "glob_var.h"
#include "auxfunc.h"

#define MM_PER_PULSE (251.0/18.0)
#define NUMBER_OF_TIMING_CYCLES 10

#define LAMPTESTTIME 50 /*  *10 ms*/

_GLOBAL BOOL	Clock_1s;

R_TRIGtyp	RPMSignalEdgeR;

R_TRIGtyp	InputSensorEdgeR,OutputSensorEdgeR;
F_TRIGtyp	InputSensorEdgeF,OutputSensorEdgeF;
TON_10ms_typ	PlateInputTimer,StandbyDelay;
TON_10ms_typ	InputSensorOnDelay,OutputSensorOnDelay,ProcessingTimer;
TOF_10ms_typ	InputSensorOffDelay,OutputSensorOffDelay;


/*Timer für Spülprogramm*/
TON_10ms_typ	CleaningTimer;

BOOL AutoInit,OffInit,WaitForStandby;
_GLOBAL REAL PlateLength; /* wird in lccount berechnet */
_LOCAL	STRING CurrentPlateName[MAXPLATETYPENAME+6];
_LOCAL	INT PlatesInMachine,TimeAtPlateInput;
_LOCAL INT	DontCheckHeating;
REAL tmpval;
int i;
BOOL	 EnablePulseCounter;
_LOCAL BOOL	OnButton,OffButton;
_LOCAL UDINT CleaningTimerDisplay,CleaningTimerOverallDisplay;


/* Variable, die genauso lange true ist, wie der sensor Eingang, aber über  ein- und ausschaltverzögerung entprellt*/
_GLOBAL	BOOL PlateOverSensor;

/* Variablen für Ampelansteuerung, interne Merker, werden an zentraler Stelle auf die Ausgänge kopiert
wegen Lampentest Funktion*/
BOOL	InternalLightGreen;
BOOL	InternalLightRed;
BOOL	InternalLightYellow;
BOOL	FatalError;
_LOCAL BOOL TriggerLampTest;
TON_10ms_typ		HornTimer1,HornTimer2,LampTestTimer;
int LampTestStep,LampTestCycle,LampTestReturnStep;
TP_10ms_typ		BlinkTimer1,BlinkTimer2;

F_TRIGtyp ServiceKeyEdgeF;

R_TRIGtyp AlarmTrigger[EGM_MAXALARMS];
_GLOBAL BOOL	HornActive;

static    TON_10ms_typ        WaitForOutputTimer;
static	BOOL	OutputBlocked;



/**
	Bei allen Platten im FIFO die Position um Betrag X erhöhen
**/
void FIFOChangePlatePosition(REAL X)
{
	int i = 0;
	REAL BackEnd;
	do
	{
		if (EGMPlateFIFO[i].Used)
		{
			EGMPlateFIFO[i].Position += X;
			BackEnd = (EGMPlateFIFO[i].Position - EGMPlateFIFO[i].Length);
			/* Trigger Waiting for output
			 */
			if(  (EGMPlateFIFO[i].Position > ((MACHINELENGTH - 0.35) * 1000))
			  && (EGMPlateFIFO[i].Status != PS_OUTPUTREACHED)
			  )
				EGMPlateFIFO[i].Status = PS_WAITINGFOROUTPUT;

			if(  (BackEnd > ((MACHINELENGTH + 0.15) * 1000))
			  && (EGMPlateFIFO[i].Status == PS_OUTPUTREACHED)
			  )
			{
				OutputBlocked = TRUE;
			}
		}
		i++;
	}
	while (i <FIFOLENGTH);

}


/**
	Alle Platten im FIFO um 1 Platz weiterrücken
**/
void FIFOShift(void)
{
	int i = FIFOLENGTH-2;

	do
	{
		if (EGMPlateFIFO[i].Used)
		{
			EGMPlateFIFO[i+1] = EGMPlateFIFO[i];
		}
		i--;
	}
	while (i>=0);
	EGMPlateFIFO[0].Used = FALSE;
	EGMPlateFIFO[0].Position = 0;
	EGMPlateFIFO[0].Length =  0;
	EGMPlateFIFO[0].Status = PS_NONE;
}

/**
	FIFO mit Platten für die Darstellung bearbeiten
**/
void HandleFIFO()
{
	RPMSignalEdgeR.CLK = RPMCheck;
	R_TRIG(&RPMSignalEdgeR);
	if ((CurrentMode == M_AUTO)
	&& ((CurrentState==S_PROCESSING) || (CurrentState==S_STARTPROCESSING))
	)
	{
/* mit jedem Antriebspuls werden die Platten um einen festen Betrag X weitergefördert*/
		if(RPMSignalEdgeR.Q )
			FIFOChangePlatePosition(MM_PER_PULSE);

/* mit steigender Flanke eine neue Platte im FIFO erzeugen*/
		if(InputSensorEdgeR.Q)
		{
			EGMPlateFIFO[0].Used = TRUE;
			EGMPlateFIFO[0].Position = 0;
			EGMPlateFIFO[0].Length =  0;
			EGMPlateFIFO[0].Status = PS_RUNNING;
		}
/*wenn im FIFO eine Platte am Anfang steht und der Sensor belegt bleibt, wird diese länger*/
		if(PlateOverSensor)
		{
			if(RPMSignalEdgeR.Q )
				EGMPlateFIFO[0].Length += MM_PER_PULSE;
		}
/* mit fallender Flanke die Platte im FIFO weiterschieben*/
		if(InputSensorEdgeF.Q)
		{
			FIFOShift();
			if(EGMPlatesInFIFO < (FIFOLENGTH - 1) )
				EGMPlatesInFIFO++;
		}

/* mit steigender Flanke des Ausgangssensors den Status der letzten Platte im FIFO wechseln */
		if(OutputSensorEdgeR.Q)
		{
			if (EGMPlatesInFIFO > 0 && EGMPlatesInFIFO < FIFOLENGTH)
			{
				if(EGMPlateFIFO[EGMPlatesInFIFO].Status == PS_WAITINGFOROUTPUT)
					EGMPlateFIFO[EGMPlatesInFIFO].Status = PS_OUTPUTREACHED;
			}
		}


/* mit fallender Flanke des Ausgangssensors die letzte Platte im FIFO Löschen*/
		if(OutputSensorEdgeF.Q)
		{
			if(OutputBlocked)
				EGM_AlarmBitField[12] = 0;

			OutputBlocked = FALSE;
			if (EGMPlatesInFIFO > 0 && EGMPlatesInFIFO < FIFOLENGTH)
			{
				EGMPlateFIFO[EGMPlatesInFIFO].Used = FALSE;
				EGMPlateFIFO[EGMPlatesInFIFO].Position = 0;
				EGMPlateFIFO[EGMPlatesInFIFO].Length =  0;
				EGMPlateFIFO[EGMPlatesInFIFO].Status = PS_NONE;
				EGMPlatesInFIFO--;
			}
		}
/* Auslauf blockiert: warten, bis alle Platten raus sind, dann Fehlermeldung*/
		if(OutputBlocked)
		{
			int i;
			REAL BackEnd;
			for(i=FIFOLENGTH;i>0;i--)
			{
				if(EGMPlateFIFO[i].Used)
				{
					BackEnd = (EGMPlateFIFO[i].Position - EGMPlateFIFO[i].Length);
					if(BackEnd > ((MACHINELENGTH - 0.35) * 1000))
					{
						EGMPlateFIFO[i].Used = FALSE;
						EGMPlateFIFO[i].Position = 0;
						EGMPlateFIFO[i].Length =  0;
						EGMPlateFIFO[i].Status = PS_NONE;
						if(EGMPlatesInFIFO > 0)
							EGMPlatesInFIFO--;
						if(PlatesInMachine > 0)
							PlatesInMachine--;
					}
				}
			}
			if(EGMPlatesInFIFO == 0 && !EGM_AlarmBitField[12])
			{
				ShowMessage(104,138,0,CAUTION,OKONLY, FALSE);
				EGM_AlarmBitField[12] = 1;
			}
		}

	}
	else
	if (((CurrentMode == M_AUTO)&& (CurrentState == S_READY))
	|| (PlatesInMachine == 0)
	)
	{
		int i;
		EGMPlatesInFIFO = 0;
		for( i = 0; i < FIFOLENGTH; i++)
		{
			EGMPlateFIFO[i].Used = 0;
			EGMPlateFIFO[i].Position = 0;
			EGMPlateFIFO[i].Length =  0;
		}
	}
}





int	FindPlateType(UDINT parLength)
{
	int i;
	int Deviation,MinDeviation,PosInTable;
	BOOL	transverse;

	MinDeviation = 32000; /* Startwert: Abweichung riesig*/
	transverse = FALSE;
	for (i=1;i<MAXPLATETYPES;i++)
	{
		Deviation = abs(EGMPlateTypes[i].SizeX - parLength);
		if (Deviation < MinDeviation)
		{
			MinDeviation = Deviation;
			PosInTable = i;
			transverse = FALSE;
		}
/*
		Deviation = abs(EGMPlateTypes[i].SizeY - parLength);
		if (Deviation < MinDeviation)
		{
			MinDeviation = Deviation;
			PosInTable = i;
			transverse = TRUE;
		}
*/
	}
	if (transverse)
		return PosInTable+MAXPLATETYPES;
	else
		return PosInTable;
}


void LampAndHorn(void)
{
	int i,j,k;
/*
 * definiert die Alarme, die eine Meldung auf dem Hauptbild auslösen
 * -1 ist Ende Kennung der Liste!
 * Achtung: Bei Veränderung muss auch in pics.c "case 101" angepasst werden
 *          Alarmnummern um 1 versetzt zu Alarmnummer in Visu, zB AlarmNr 49 zeigt
 *          in Visu Alarmnummer 50 an!!
 */
	int MessageAlarms[]={0,2,12,33,34,35,36,37,38,39,40,-1};

/* ********************************************************************************/
/* Ampel */
/* ********************************************************************************/
	BlinkTimer1.PT = 30;
	BlinkTimer2.PT = 30;
	TON_10ms(&LampTestTimer);
	BlinkTimer1.IN = ! BlinkTimer2.Q;
	TP_10ms(&BlinkTimer1);
	BlinkTimer2.IN = ! BlinkTimer1.Q;
	TP_10ms(&BlinkTimer2);

	switch (LampTestStep)
	{
		case 0:
		{
			int i;
			BOOL flag;
			flag = 0;
			for (i=0;i<EGM_MAXALARMS;i++)
			{
/* EGMPreheat Temperatur zu niedrig nur relevant, wenn der Regler auch an ist*/
				if (i==8 || i==10)
					flag = flag || (EGM_AlarmBitField[i] && EGMPreheat.Enable && !DontCheckHeating);
				else
					flag = flag || EGM_AlarmBitField[i];
			}


/*solange kein Lampentest ist: interne Ampelmerker auf Ausgänge kopieren*/
			if (flag)
			{
				LightRed = BlinkTimer1.Q;
				LightGreen = 0;
				LightYellow = 0;
			}
			else
			{
				LightGreen = InternalLightGreen;
				LightYellow = InternalLightYellow;
				if (FatalError)
					LightRed = InternalLightRed && BlinkTimer1.Q;
				else
					LightRed = InternalLightRed;
			}

/* warten auf Trigger*/
			if (TriggerLampTest)
			{
				LampTestStep = 1;
				LampTestTimer.IN = 0;
				LampTestReturnStep = 0;
				Horn = 1;
			}
			break;
		}
		case 1:
		{
			LightGreen = 1;
			LightRed = 0;
			LightYellow = 0;
			LampTestStep = 100;
			LampTestReturnStep = 5;
			break;
		}
		case 5:
		{
			LightGreen = 0;
			LightRed = 0;
			LightYellow = 1;
			LampTestStep = 100;
			LampTestReturnStep = 10;
			break;
		}
		case 10:
		{
			LightGreen = 0;
			LightRed = 1;
			LightYellow = 0;
			LampTestStep = 100;
			LampTestReturnStep = 1;
			break;
		}
/*Warteschritt als Unterprogramm*/
		case 100:
		{
			if (!TriggerLampTest)
			{
				LampTestTimer.IN = 0;
				LampTestStep = 0;
				break;
			}

			LampTestTimer.IN = 1;
			LampTestTimer.PT = LAMPTESTTIME;
			if (LampTestTimer.Q)
			{
				LampTestTimer.IN = 0;
				LampTestStep = LampTestReturnStep;
			}
			break;
		}
	}

/* ********************************************************************************/
/*
 * Fehler, die einen Benutzereingriff erfordern, lösen die Hupe aus
 * z.B. Kanister leer, Gummierung leer
 * Hupe quittierbar im Hauptbild, kommt wieder, wenn ein Fehler neu
`* auftritt (Flanke)
 */
 	i = j = k = 0;

 	while (MessageAlarms[i] != -1)
 	{
 		j = MessageAlarms[i];
		AlarmTrigger[j].CLK = EGM_AlarmBitField[j];
		R_TRIG(&AlarmTrigger[j]);
/* wenn ein Alarm aus der Liste eine positive Flanke hat, dann Hupe an */
		if( AlarmTrigger[j].Q )
			HornActive = TRUE;
/* Wenn kein Alarm aus der Liste ansteht, wird k in keinem Durchlauf erhöht,
 * dann wird unten die Hupe ausgeschaltet
 */
		if( EGM_AlarmBitField[j] )
			k++;

		i++;
 	}
/* wenn alle Alarme der Liste 0 sind, dann wurde k nicht erhöht, dann Hupe aus; */
/* Hupe kann auch per Knopfdruck vom Bediener zurückgesetzt werden */
	if (k == 0)
		HornActive = FALSE;

	if (  HornActive
	  && !HornTimer2.Q )
		HornTimer1.IN = 1;
	else
		HornTimer1.IN = 0;

	HornTimer1.PT = 1000; /* alle 10 s*/
	TON_10ms(&HornTimer1);

	HornTimer2.PT = 200; /* 2 sec*/
	TON_10ms(&HornTimer2);

/* nach Zeit die Hupe ein */
	if (HornTimer1.Q  )
		HornTimer2.IN = 1;

	if (HornTimer2.Q)
		HornTimer2.IN = 0;
/* Hupe nur ansteuern wenn kein Lampentest*/
	if (LampTestStep == 0)
	{
/*
 * bei fatalem Fehler schneller Hup-Takt
 */
		if (FatalError)
			Horn = BlinkTimer1.Q;
		else
/*
 * bei "normalem" Fehler Hupe alle 10 sec für 2 sec an
 * wird oben getriggert, indem HornTimer1.IN gesetzt wird
 */
			Horn = HornTimer2.IN;
	}
/* ********************************************************************************/
}



_INIT void init()
{
	OutputBlocked = FALSE;
	FatalError  = 0;
	WaitForStandby = FALSE;
	CurrentMode = M_OFF;
	AutoInit = 1;
	CleaningTimer.IN = 0;
	CleaningTimer.PT = 100;
	SessionPlateCounter = 0;
	InternalLightGreen = 0;
	InternalLightYellow = 0;
	InternalLightRed = 0;
	Horn = 0;
	EGMPlatesInFIFO = 0;
	HornTimer1.IN = 0;
	HornTimer2.IN = 0;
	LampTestStep = 0;
	LampTestCycle = 0;
}

_CYCLIC void cyclic()
{
	EGM_AlarmBitField[14] =    (ModulStatus_PS  != 1)
							&& (ModulStatus_DI  != 1)
							&& (ModulStatus_DO1 != 1)
							&& (ModulStatus_DO2 != 1)
							&& (ModulStatus_DO3 != 1)
							&& (ModulStatus_AO  != 1)
							&& (ModulStatus_AT  != 1);
	if( !EGM_AlarmBitField[14])
	{
		EGM_AlarmBitField[28] = (ModulStatus_AT != 1);
		EGM_AlarmBitField[29] = (ModulStatus_DI != 1);
		EGM_AlarmBitField[30] = (ModulStatus_DO1 != 1) || (ModulStatus_DO2 != 1) || (ModulStatus_DO3 != 1);
		EGM_AlarmBitField[31] = (ModulStatus_AO != 1);
		EGM_AlarmBitField[32] = (ModulStatus_PS != 1);
	}
	else
	{
		EGM_AlarmBitField[28] = 0;
		EGM_AlarmBitField[29] = 0;
		EGM_AlarmBitField[30] = 0;
		EGM_AlarmBitField[31] = 0;
		EGM_AlarmBitField[32] = 0;
	}

	if (InputSensorEdgeR.Q)
		PlateOverSensor = TRUE;
	if (InputSensorEdgeF.Q)
		PlateOverSensor = FALSE;
	HandleFIFO();
	LampAndHorn();


/******************************************************/
/*
 * wenn die erste Platte im FIFO länger als 30 cm den Status WaitForOutput hat
 * dann ist das ein Plattenstau
 * Auswertung erfolgt im status Processing
 */
	WaitForOutputTimer.IN = (EGMPlateFIFO[EGMPlatesInFIFO].Status == PS_WAITINGFOROUTPUT)
	                        && !OutputBlocked;
/*	WaitForOutputTimer.PT = 1000;*/
/* 0,3 m -> 30 cm */
	WaitForOutputTimer.PT = (UDINT)
			(60.0 * 100.0 *
			(0.35 / ((REAL)EGMMainMotor.RatedRpm * EGMGlobalParam.MainMotorFactor)));
	TON_10ms(&WaitForOutputTimer);

/******************************************************/

/* aktuellen Status für die XML-Schnittstelle in Struktur eintragen */
	EGMState.mode = CurrentMode;
	EGMState.state = CurrentState;


/*Flankenerkennung aller Plattensensoren*/
	InputSensorEdgeR.CLK = InputSensorOnDelay.Q;
	InputSensorEdgeF.CLK = InputSensorOffDelay.Q;
	OutputSensorEdgeR.CLK = OutputSensorOnDelay.Q;
	OutputSensorEdgeF.CLK = OutputSensorOffDelay.Q;

	R_TRIG(&InputSensorEdgeR);
	R_TRIG(&OutputSensorEdgeR);
	F_TRIG(&InputSensorEdgeF);
	F_TRIG(&OutputSensorEdgeF);

	ServiceKeyEdgeF.CLK = ServiceKey;
	F_TRIG(&ServiceKeyEdgeF);
/* Timer für Spülprogramm*/
	TON_10ms(&CleaningTimer);
	CleaningTimerDisplay = (CleaningTimer.PT  - CleaningTimer.ET)/100;

/*Zeit messen, von steigender bis fallender Flanke*/
	PlateInputTimer.PT = 60000; /*soll nie erreicht werden*/
	TON_10ms(&PlateInputTimer);
	TON_10ms(&ProcessingTimer);
	TON_10ms(&StandbyDelay);
/*Entprellen je 1 sec*/
	InputSensorOnDelay.PT = 100;
	InputSensorOffDelay.PT = 100;
/*Entprellen je 1 sec*/
	OutputSensorOnDelay.PT = 100;
	OutputSensorOffDelay.PT = 100;
	InputSensorOnDelay.IN = InputSensor;
	InputSensorOffDelay.IN = InputSensor;
	OutputSensorOnDelay.IN = OutputSensor;
	OutputSensorOffDelay.IN = OutputSensor;
	TON_10ms(&InputSensorOnDelay);
	TON_10ms(&OutputSensorOnDelay);
	TOF_10ms(&InputSensorOffDelay);
	TOF_10ms(&OutputSensorOffDelay);
/*bei steigender Flanke starten*/
	if(InputSensorEdgeR.Q && !PlateInputTimer.IN)
	{
		PlateInputTimer.IN = 1;
	}
/*bei fallender Flanke stoppen*/
	if(InputSensorEdgeF.Q && PlateInputTimer.IN)
	{
		PlateInputTimer.IN = 0;
		TimeAtPlateInput = PlateInputTimer.ET;
	}


/* Bei Not Aus die Maschine in Stop setzen*/
	if (!ControlVoltageOk)
	{
		OffInit = 1;
		CurrentMode = M_OFF;
		CurrentState = S_OFF;
		InternalLightGreen = 0;
		InternalLightYellow = 0;
		InternalLightRed = Clock_1s;
	}
	EGM_AlarmBitField[0] = (!ControlVoltageOk);


/*****************************************************************/
/**  Signale an Belichter */
/*****************************************************************/
	if(  (CurrentMode == M_AUTO )
	  &&((CurrentState == S_READY) || (CurrentState == S_PROCESSING))
	  && !FatalError
	  && !PlateOverSensor
	  && (!EGMGum.LevelNotInRange || (!EGMGlobalParam.EnableGumSection && (MachineType==BLUEFIN_LOWCHEM)))
	  && ((EGMGum.PumpCmd == ON) || (!EGMGlobalParam.EnableGumSection && (MachineType==BLUEFIN_LOWCHEM)) )
	  && !OutputBlocked
	  )
	{
		Ready = 1;
		NoError = 1;
	}
	else
	{
		Ready = 0;
		if (FatalError || !ControlVoltageOk)
			NoError = 0;
		else
			NoError = 1;
	}
/*****************************************************************/
/*****************************************************************/

	switch (CurrentMode)
	{
		case M_OFF: /*OFF*/
		{
			FatalError = 0;
			EGM_AlarmBitField[12] = 0;
			OutputBlocked = FALSE;
			WaitForStandby = FALSE;
			PlatesInMachine = 0;
			CurrentState = S_OFF;
			EGMMainMotor.AutoRatedRpm = 0;
			EGMBrushMotor.AutoRatedRpm= 0;
			if (OffInit)
			{
				InternalLightGreen = 0;
				InternalLightYellow = 0;
				InternalLightRed = 1;

				EGMMainMotor.RatedRpm = 0;
				EGMBrushMotor.RatedRpm= 0;
/*Pumpen aus*/
				EGMGum.PumpCmd = OFF;
				if(MachineType == BLUEFIN_XS)
				{
					EGMPrewash.PumpCmd = OFF;
					if (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY)
						EGMPrewash.Valve = CLOSE;

					EGMRinse.PumpCmd = OFF;
					if (EGMRinseParam.ReplenishingMode == FRESHWATERONLY)
						EGMRinse.Valve = CLOSE;
					EGMPrewash.Auto = 0;
					EGMRinse.Auto = 0;
				}
				Drying = OFF;
				EGMDeveloperTank.Enable = 0;
/*EGMPreheat Regelung aus und auf Manuell*/
				EGMPreheat.Enable = 0;
				for(i=0;i<4;i++)
					EGMPreheat.Auto[i] = 0;
				EGMDeveloperTank.Auto = 0;
/* V1.11 Gummierung Spülwasserventil AUS*/
				EGMGum.RinseValve = OFF;
			}

			EGMBrushMotor.Auto = 0;
			EGMMainMotor.Auto = 0;
			OffInit = 0;
			AutoInit =1;
			if (OnButton && ControlVoltageOk)
			{
				OnButton = 0;
				CurrentMode = M_AUTO;
			}
			break;
		}
		case M_AUTO:  /*AUTO*/
		{
			OffInit = 1;


			if (AutoInit)
			{
				InternalLightGreen = 0;
				InternalLightYellow = 0;
				InternalLightRed = 1;

				AutoInit = 0;
				EGMPreheatParam.RatedTemp[0] = EGMAutoParam.PreheatTemp;
				EGMPreheatParam.RatedTemp[1] = EGMAutoParam.PreheatTemp2;
				EGMPreheat.Enable = 1;
				for(i=0;i<4;i++)
					EGMPreheat.Auto[i] = 1;
				EGMDeveloperTankParam.RatedTemp = EGMAutoParam.DeveloperTemp;
				EGMDeveloperTank.Auto = 1;
				EGMBrushMotor.Auto = 1;
				EGMMainMotor.Auto = 1;
				EGMDeveloperTank.Enable = 1;
				if(MachineType == BLUEFIN_XS)
				{
					EGMPrewash.Auto = 1;
					EGMRinse.Auto = 1;
				}
				CurrentState = S_HEATING;

/* wenn Füllstand ok, dann die EGMGummierung-Zirkulationspumpe EIN*/
				if (!EGMGum.LevelNotInRange)
				{
					EGMGum.PumpCmd = ON;
					EGMGum.Auto = ON;
				}
				if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS))
					EGMGum.Valve = ON;
			}

			if (OffButton )
			{
				OffButton = 0;
				if ((CurrentState == S_READY) || (CurrentState == S_HEATING) )
				{
					CurrentMode = M_INITCLEANING;
				}
				else
				if (CurrentState == S_PROCESSING )
				{
					CurrentState = S_WAITFORSTANDBY;
					WaitForStandby = 1;
				}
			}

			switch (CurrentState)
			{
				case S_HEATING:
				{
					BOOL LevelsOK;
					InternalLightYellow = 0;
					InternalLightGreen = 0;
					InternalLightRed = 1;

					EGMMainMotor.AutoRatedRpm = EGMAutoParam.StandbySpeed;
					EGMBrushMotor.AutoRatedRpm = EGMAutoParam.StandbyBrushSpeed;
					if(MachineType == BLUEFIN_XS)
					{
						LevelsOK = (!EGMPrewash.LevelNotInRange || (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY))
		  			            && (!EGMRinse.LevelNotInRange   || (EGMRinseParam.ReplenishingMode == FRESHWATERONLY));
					}
					else
						LevelsOK = TRUE;

					if( ((EGMPreheat.TempInRange && EGMDeveloperTank.TempInRange) || DontCheckHeating)
	  				  &&  (!EGMGum.LevelNotInRange || (!EGMGlobalParam.EnableGumSection && (MachineType==BLUEFIN_LOWCHEM)))
	  				  &&   EGMDeveloperTank.LevelInRange
					  &&   LevelsOK
					  )
						CurrentState = S_READY;

					break;
				}
				case S_READY:
				{
					InternalLightGreen = 1;
					InternalLightRed = 0;
					InternalLightYellow = 0;

/*
 *2.08: bugfix, jetzt auch bei "ready"-> es erfolgt auch wieder
        Freigabe an Jet
 */
/* wenn Füllstand ok, dann die EGMGummierung-Zirkulationspumpe EIN*/
					if (!EGMGum.LevelNotInRange)
					{
						EGMGum.PumpCmd = ON;
						EGMGum.Auto = ON;
					}
					if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS))
						EGMGum.Valve = ON;

					if (InputSensor)
					{
						CurrentState = S_STARTPROCESSING;
						break;
					}
					if(WaitForStandby)
					{
						WaitForStandby = 0;
						CleaningTimer.IN = 0;
						CurrentMode = M_INITCLEANING;
					}
					break;
				}
				case S_STARTPROCESSING:
				{
					EGMMainMotor.AutoRatedRpm = EGMAutoParam.Speed;
					EGMBrushMotor.AutoRatedRpm = EGMAutoParam.BrushSpeed;
/* Platte war 3 s im Einlauf: Verarbeitung läuft -> Schrittwechsel*/
					if (PlateInputTimer.ET > 300) /*3 s*/
					{
						PlatesInMachine = 0;
						CurrentState = S_PROCESSING;
						StandbyDelay.IN = 0;

						if(MachineType == BLUEFIN_XS)
						{
							if (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY)
								EGMPrewash.PumpCmd = ON;
							else
								EGMPrewash.Valve = OPEN;

							if (EGMRinseParam.ReplenishingMode != FRESHWATERONLY)
								EGMRinse.PumpCmd = ON;
							else
								EGMRinse.Valve = OPEN;
						}
/* wenn Serviceschalter nicht umgelegt ist, Trocknung EIN */
						if (ServiceKey)
							Drying = ON;
					}
					else /* innerhalb der ersten 3s:*/
					{
					/*wenn Platte zu kurz im Einlauf: zurück*/
						if (InputSensorEdgeF.Q)
						{
							EGMMainMotor.AutoRatedRpm = EGMAutoParam.StandbySpeed;
							EGMBrushMotor.AutoRatedRpm = EGMAutoParam.StandbyBrushSpeed;
							CurrentState = S_READY;
							break;
						}
						else  /* nicht die negative Flanke vom Einlauf: abbruch, warten*/
							break;
					}
/***
das obligatorische break fehlt hier bewußt, da es vorher im if .. else eingebaut ist.
Wenn ein Start erkannt wird soll NICHT gebreakt werden, sondern direkt in PROCESSING
weitergemacht werden
***/
				}
				case S_WAITFORSTANDBY:
				case S_PROCESSING:
				{
					InternalLightGreen = 0;
					InternalLightYellow = 1;
					InternalLightRed = 0;
/* wenn Füllstände ok sind, dann die Zirkulationspumpen EIN*/
					if (!EGMGum.LevelNotInRange)
					{
						EGMGum.PumpCmd = ON;
						EGMGum.Auto = ON;
					}
					if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS))
					EGMGum.Valve = ON;

					if(MachineType == BLUEFIN_XS)
					{
						if (!EGMPrewash.LevelNotInRange
						&& (!AlarmBitField[35])
						&& (EGMPrewashParam.ReplenishingMode != FRESHWATERONLY))
							EGMPrewash.PumpCmd = ON;

						if (!EGMRinse.LevelNotInRange
						&& (!AlarmBitField[36])
						&& (EGMRinseParam.ReplenishingMode != FRESHWATERONLY))
							EGMRinse.PumpCmd = ON;

	/* Prewash auf "Nur Frischwasser" -> Ventil Auf*/
						if (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY)
							EGMPrewash.Valve = OPEN;

	/* Spülen auf "Nur Frischwasser" -> Ventil Auf*/
						if (EGMRinseParam.ReplenishingMode == FRESHWATERONLY)
							EGMRinse.Valve = OPEN;
					}
/* Überwachungstimer:
solange Platten in der Maschine sind, muß mindestens alle 2 min
der Auslaufsensor kommen
*/
					if (PlatesInMachine > 0)
					{
						if (OutputSensorEdgeR.Q)
							ProcessingTimer.IN = 0;
						else
						{
							ProcessingTimer.IN = 1;
							ProcessingTimer.PT = (UDINT)
								(60.0 * 100.0 *
								(MACHINELENGTH / ((REAL)EGMMainMotor.RatedRpm * EGMGlobalParam.MainMotorFactor)));

							/* Geschw > 1,5 m/min: 10 sec Sicherheit*/
							/* Geschw < 1,5 m/min: 20 sec Sicherheit*/
							if ( ((REAL)EGMMainMotor.RatedRpm * EGMGlobalParam.MainMotorFactor)<1.5 )
								ProcessingTimer.PT += 2000;
							else
								ProcessingTimer.PT += 1000;
						}
					}
					else /* PlatesInMachine is 0*/
					{
						ProcessingTimer.IN = 0;
						if( OutputSensorEdgeR.Q )
							PlatesInMachine = 1;
					}

/* Überwachungstimer schlägt an
 * -> ERROR
 */
					if( /* ProcessingTimer.Q
					 || */ WaitForOutputTimer.Q
					  )
					{
						CurrentState = S_ERROR;
						FatalError = 1;
						ProcessingTimer.IN = 0;
						ShowMessage(104,103,0,CAUTION,OKONLY, FALSE);

						EGM_AlarmBitField[12] = 1;
					}

/*wenn eine Platte den Ein- oder Auslaufsensor betätigt, dann  Trocknung EIN*/
					if ((InputSensor || OutputSensor) && ServiceKey)
						Drying = ON;
/* wenn Service Schalter umgelegt ist, wird die Trocknung immer Ausgeschaltet */
					if (ServiceKeyEdgeF.Q)
						Drying = OFF;


/*weitere Platten mitkriegen*/
					if (InputSensorEdgeF.Q)
					{
						int TypeNr;
						StandbyDelay.IN = 0;
						PlatesInMachine++;
						TypeNr = FindPlateType(PlateLength);
						if (TypeNr >= MAXPLATETYPES)
						{
							TypeNr -= MAXPLATETYPES;
							strcpy(CurrentPlateName,EGMPlateTypes[TypeNr].Name);
							strcat (CurrentPlateName," (q)");
						}
						else
							strcpy(CurrentPlateName,EGMPlateTypes[TypeNr].Name);

						OverallSqmCounter += EGMPlateTypes[TypeNr].Area;
						SessionSqmCounter += EGMPlateTypes[TypeNr].Area;
						ReplenishmentSqmCounter += EGMPlateTypes[TypeNr].Area;
						TopUpSqmCounter += EGMPlateTypes[TypeNr].Area;
						PrewashReplenishmentSqmCounter += EGMPlateTypes[TypeNr].Area;
						RinseReplenishmentSqmCounter += EGMPlateTypes[TypeNr].Area;
						SqmSinceDevChg += EGMPlateTypes[TypeNr].Area;

						ReplenishmentPlateCounter++;
						TopUpPlateCounter++;
						PrewashReplenishmentPlateCounter++;
						RinseReplenishmentPlateCounter++;
					}

/*auslaufende Platten mitkriegen*/
					if (OutputSensorEdgeF.Q )
					{
						PlatesInMachine--;
						OverallPlateCounter++;
						SessionPlateCounter++;
					}

					if (PlatesInMachine < 0) PlatesInMachine = 0;

					if ((PlatesInMachine == 0)	&& !InputSensor && !OutputSensor  )
					{
						StandbyDelay.IN = 1;
						StandbyDelay.PT = EGMAutoParam.StandbyDelay;
					}
					else
						StandbyDelay.IN = 0;

					if (StandbyDelay.Q)
					{
						StandbyDelay.IN = 0;
						EGMMainMotor.AutoRatedRpm = EGMAutoParam.StandbySpeed;
						EGMBrushMotor.AutoRatedRpm = EGMAutoParam.StandbyBrushSpeed;
						if(MachineType == BLUEFIN_XS)
						{
							if (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY)
								EGMPrewash.Valve = CLOSE;
							if (EGMRinseParam.ReplenishingMode == FRESHWATERONLY)
								EGMRinse.Valve = CLOSE;

							EGMPrewash.PumpCmd = OFF;
							EGMRinse.PumpCmd = OFF;
						}
						Drying = OFF;
						CurrentState = S_READY;
					}

					break;
				}
				case S_ERROR:
				{
					InternalLightGreen = 0;
					InternalLightYellow = 0;
					InternalLightRed = 1;

					EGMMainMotor.RatedRpm = 0;
					EGMBrushMotor.RatedRpm= 0;
					EGMMainMotor.AutoRatedRpm = 0;
					EGMBrushMotor.AutoRatedRpm= 0;
	/*Pumpen aus*/
					EGMGum.PumpCmd = OFF;
					EGMGum.Auto = OFF;
					if(MachineType == BLUEFIN_XS)
					{
						EGMPrewash.PumpCmd = OFF;
						if (EGMPrewashParam.ReplenishingMode == FRESHWATERONLY)
							EGMPrewash.Valve = CLOSE;
						if (EGMRinseParam.ReplenishingMode == FRESHWATERONLY)
							EGMRinse.Valve = CLOSE;
						EGMRinse.PumpCmd = OFF;
					}
					Drying = OFF;
					EGMDeveloperTank.Enable = 0;
	/*EGMPreheat Regelung aus und auf Manuell*/
					EGMPreheat.Enable = 0;
					for(i=0;i<4;i++)
						EGMPreheat.Auto[i] = 0;
					EGMDeveloperTank.Auto = 0;
					if (AbfrageOK || AbfrageCancel)
					{
						FatalError = 0;
						CurrentState = S_OFF;
						CurrentMode = M_OFF;
						OffInit = 1;
						EGM_AlarmBitField[12] = 0;
					}
					break;
				}

/*****************************************************************************************************/
/*****************************************************************************************************/
/*****************************************************************************************************/
			}
			break;
		}
/*****************************************************************************************************/
		case M_INITCLEANING:
		{
			if((!EGMGlobalParam.EnableGumSection && (MachineType==BLUEFIN_LOWCHEM)))
			{
				FatalError = 0;
				CurrentState = S_OFF;
				CurrentMode = M_OFF;
				OffInit = 1;
				break;
			}

/* Funktionen für OFF schonmal vorweg nehmen: Heizungen aus, Bürsten aus */
			EGMBrushMotor.AutoRatedRpm= 0;
			EGMBrushMotor.RatedRpm= 0;
			EGMBrushMotor.Auto = 0;
			EGMDeveloperTank.Enable = 0;
/*EGMPreheat Regelung aus und auf Manuell*/
			EGMPreheat.Enable = 0;
			for(i=0;i<4;i++)
				EGMPreheat.Auto[i] = 0;
/*Entwicklertank Automatik aus*/
			EGMDeveloperTank.Auto = 0;

/* zum Start der Reinigung EGMGummierungspumpe aus, Ablaufventil auf Rückfluß in Behälter*/
			EGMGum.PumpCmd = OFF;
			EGMGum.Auto = OFF;
			EGMGum.Valve = ON;
/* Walzen auf Normalspeed*/
			EGMMainMotor.AutoRatedRpm = EGMAutoParam.Speed;
			CleaningTimer.IN = 1;
			CleaningTimer.PT = EGMAutoParam.WaitGumBackTime*100;
			CurrentMode = M_CLEANING;
			CurrentState = S_CLEANING1;
			CleaningTimerOverallDisplay = EGMAutoParam.WaitGumBackTime + EGMAutoParam.GumRinsingTime +
												EGMAutoParam.WaitGumRinseTime;
			break;
		}
		case M_CLEANING:
		{
			InternalLightYellow = Clock_1s;
			InternalLightRed = 0;
			InternalLightGreen = 0;

			CleaningTimerDisplay = (CleaningTimer.PT  - CleaningTimer.ET)/100;
/*Abbruch während des Reinigungszyklus*/
			if (OffButton)
			{
				OffButton = 0;
				EGMGum.PumpCmd = OFF;
				EGMGum.Auto = OFF;
/**/
				if (CurrentState == S_CLEANING1)
					EGMGum.Valve = ON;
				else
					EGMGum.Valve = OFF;
				EGMGum.RinseValve = OFF;
				CurrentMode = M_OFF;
				CurrentState = S_OFF;
				OffInit = 1;
				CleaningTimer.IN = 0;
				break;
			}

			switch (CurrentState)
			{
				case  S_CLEANING1:	/*warten, daß Gummierung zurückläuft*/
				{
					CleaningTimerOverallDisplay =  CleaningTimerDisplay + EGMAutoParam.GumRinsingTime +
												EGMAutoParam.WaitGumRinseTime;
					if (CleaningTimer.Q)
					{
						CleaningTimer.IN = 0;
						CurrentState = S_CLEANING2;
					}
					break;
				}
				case  S_CLEANING2:	/*Spülwasser an, Auslaß in Abfluß*/
				{
					CleaningTimerOverallDisplay =  CleaningTimerDisplay +
												EGMAutoParam.WaitGumRinseTime;
					if (EGMGum.RinseValve == ON)
					{
						CleaningTimer.IN = 1;
						CleaningTimer.PT = EGMAutoParam.GumRinsingTime*100;
					}
/* Spülwasser ein, Ablaufventil auf Ablauf in Abfluß*/
					EGMGum.RinseValve = ON;
					EGMGum.Valve = OFF;

					if (CleaningTimer.Q)
					{
/* Spülwasser aus, und warten, damit Spülwasser komplett in Abfluß fliessen kann*/
						EGMGum.RinseValve = OFF;
						CleaningTimer.IN = 0;
						CurrentState = S_CLEANING3;
					}
					break;
				}
				case  S_CLEANING3:	/*warten, daß Spülwasser komplett abgelaufen ist*/
				{
					CleaningTimerOverallDisplay = CleaningTimerDisplay;
					CleaningTimer.PT = EGMAutoParam.WaitGumRinseTime*100; /* 2 min*/
					CleaningTimer.IN = 1;

					if (CleaningTimer.Q)
					{
						CleaningTimer.IN = 0;
						CurrentState = S_OFF;
						CurrentMode = M_OFF;
						OffInit = 1;
						CleaningTimerDisplay = 0;
						CleaningTimerOverallDisplay = 0;
					}
					break;
				}

			}
			break;
		}
	}
/*in jedem Zyklus die Buttonmerker löschen*/
	OffButton=0;
	OnButton=0;
}


