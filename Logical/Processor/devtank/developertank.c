#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
 Handling of Developertank

 - Heating, cooling, Temperature Measurement
 - filling, circulation, regeneration

*************************************************************************************************/
/*
#define TESTCURVE
*/
/**/
#undef TESTCURVE
/**/

#define OVERHEATTEMP 500	/* 50 °C */
#include "glob_var.h"
#include "egmglob_var.h"
#include "auxfunc.h"
#include <math.h>

extern void Replenish(void);
extern void InitRepl(void);
extern void TopUp(void);
extern void InitTopUp(void);
extern void InitProV(void);
extern void ProV(void);

TON_10ms_typ	MinOnTimeCooling;

TON_10ms_typ DeveloperLevelOKTimer,DeveloperLevelNOTOKTimer,DeveloperLevelErrorTimer,
			 CirculationPumpStartTimer,CircFlowDelay,CirculationPumpOffTimer;

TON_10ms_typ StandbyReplenishmentTimer;
TON_10ms_typ	RefillSafetyTimer,
				ReplSafetyTimer;


R_TRIGtyp	LevelOKR,LevelNOTOKR;

BOOL 	DevOldAuto;

/*um Heizkurve aufzunehmen*/
_LOCAL	BOOL	TriggerLogging,TriggerLoggingOld;
_GLOBAL int	DeveloperValues[350];
_GLOBAL	UINT	DeveloperSamplingTime;
_GLOBAL BOOL RedrawCurve,DeveloperTankParamEnter;
TON_10ms_typ	PulseTimer1;
int ValueCount;


_LOCAL	LCPWM_DMod_typ		LCPWM_DMod_var;
_LOCAL	LCPIDpara_typ			LCPIDpara_var;
_LOCAL	LCPID_typ				LCPID_var;
_LOCAL	LCPIDAutoTune_typ		LCPIDAutoTune_var;
_LOCAL	LCMovAvFilter_typ		LCMovAvFilter_var;

_LOCAL	BOOL 	StartAutoTune;
_LOCAL UINT	TopUpPumpTime;
		UINT	TopUpTmpPumpTime;

_LOCAL	BOOL	ResetOverallValues;
BOOL	DeveloperRefillActive;
BOOL	Overheated;
_LOCAL INT	TempSensorValue;
REAL tmpval;

TON_10ms_typ	PulseTimer1,SamplingTimer1;
BOOL	Pulse_1s;

void DeveloperPID_DefaultParam(void)
{
	DeveloperTankParamEnter = 0;
	DeveloperRefillActive = 0;

	LCPWM_DMod_var.tminpuls			= 1;  /* 100 ms kürzester Puls bzw pause*/
	LCPWM_DMod_var.tperiod			= 20;  /*Periodendauer 2 sec*/
	LCPWM_DMod_var.enable			= 1;
	LCPWM_DMod_var.max_value	= 100;
	LCPWM_DMod_var.min_value		= 0;

/*Sollwertabschwächung (1=AUS)*/
	LCPIDpara_var.Kw						= 1;
/*windup Dämpfung: 0=AUS*/
	LCPIDpara_var.Kfbk						= 0;
	LCPIDpara_var.enable					= 1;
	LCPIDpara_var.Y_max				= 100;
	LCPIDpara_var.Y_min					= -100;
	LCPIDpara_var.dY_max				= 0; /* 0= keine Rampe*/
	/*
	Rückführmodus der Stellgröße
	LCPID_FBK_MODE_INTERN: intern
	LCPID_FBK_MODE_EXTERN: extern für Ablöseregelung
	LCPID_FBK_MODE_EXT_SELECTOR: extern für Selektorregelung. In diesem Modus wird der I-Anteil des momentan nicht selektierten Reglers (Y_fbk ? Y) nach der
	Formel Yi = Y_fbk – Yp – Yd – A dem Eingang Y_fbk nachgeführt.
	*/
	LCPIDpara_var.fbk_mode				= LCPID_FBK_MODE_INTERN;
	/*
	Differenziermodus
	LCPID_D_MODE_X: nur -X (Istwert)
	LCPID_D_MODE_E: e (Regelabweichung, wie bei einem Standard- PID- Regler )
	*/
	LCPIDpara_var.d_mode				= LCPID_D_MODE_X;
	/*
	Rechenmodus:
	LCPID_CALC_MODE_EXACT: Genau (Alle Berechnungen erfolgen in Double Float - keine Rundungsfehler, auf SG3-Zentraleinheiten lange Rechenzeiten).
	LCPID_CALC_MODE_FAST: Schnell (Alle Berechnungen erfolgen in Integer - es kann zu geringfügigen Rundungsfehlern kommen, wird auch auf SG3-Zentraleinheiten sehr schnell gerechnet)
	*/
	LCPIDpara_var.calc_mode			= LCPID_CALC_MODE_EXACT;

/* Reglerbaustein LCPID*/
	LCPID_var.enable					= 1;
/*Stellgröße Handbetrieb */
	LCPID_var.Y_man					= 0;
	/*Aufschaltgröße*/
	LCPID_var.A							= 0;
	/*I-Anteil einfrieren*/
	LCPID_var.hold_I					= 0;

/* Autotune*/
	LCPIDAutoTune_var.enable				= 1;
	LCPIDAutoTune_var.Y_max				= 100;
	LCPIDAutoTune_var.Y_min				= 0;
	LCPIDAutoTune_var.P_manualAdjust		= 0;
	LCPIDAutoTune_var.I_manualAdjust		= 0;
	LCPIDAutoTune_var.D_manualAdjust		= 0;
	StartAutoTune = 0;

}

_INIT void init()
{
/*sinnvolle defaultwerte*/
	EGMDeveloperTankParam.ToleratedDevPos = 50;
	EGMDeveloperTankParam.ToleratedDevNeg = 50;
	DeveloperSamplingTime = 3000;
	TriggerLogging = 0;
	ValueCount = 0;
	DeveloperPID_DefaultParam();
	StandbyReplenishmentTimer.IN = 0;
	StandbyReplenishmentTimer.PT = 360000; /* 1 h */
	ResetOverallValues = 0;
	ReplenisherCounter = 0;
	LCMovAvFilter_var.enable 	= TRUE;
	LCMovAvFilter_var.x			= 0;
	LCMovAvFilter_var.base		= 128;
	LCMovAvFilter(&LCMovAvFilter_var);
	PulseTimer1.IN = 0;
	PulseTimer1.PT = 100;
	DeveloperLevelOKTimer.IN = 0;
	DeveloperLevelNOTOKTimer.IN = 0;
	DeveloperLevelErrorTimer.IN = 0;
	InitRepl();
	InitTopUp();
	InitProV();
}


_CYCLIC void cyclic()
{
	PulseTimer1.IN = 1;
	if(PulseTimer1.Q)
		PulseTimer1.IN = 0;
	TON_10ms(&PulseTimer1);
	Pulse_1s = PulseTimer1.Q;

/* Korrekturwert für Entwicklertemperatur verwenden*/
	EGMDeveloperTank.RealTemp = TempSensorValue + EGMDeveloperTankParam.TempOffset;
/*  Pulstimer mit 30 sec Takt*/
	SamplingTimer1.IN = 1;
	SamplingTimer1.PT = DeveloperSamplingTime;
	if(SamplingTimer1.Q)
		SamplingTimer1.IN = 0;
	TON_10ms(&SamplingTimer1);

	if (SamplingTimer1.Q)
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
					DeveloperValues[j] = 0;
#else
/*TEST ONLY*/
				{
					DeveloperValues[j] = EGMDeveloperTankParam.RatedTemp + j/10;
				}
#endif
			}
#ifndef TESTCURVE
			DeveloperValues[ValueCount] = EGMDeveloperTank.RealTemp;
#endif

			ValueCount++;
			if(ValueCount>301)
			{
				ValueCount = 0;
				TriggerLogging = 0;
			}
			RedrawCurve = 1;
		}
	}


	if (DeveloperTankParamEnter)
	{
		DeveloperTankParamEnter = 0;
		LCPIDpara_var.enter = 1;
		LCPIDpara_var.Kp = EGMDeveloperTankParam.ReglerParam.Kp;
		LCPIDpara_var.Tn = EGMDeveloperTankParam.ReglerParam.Tn;
		LCPIDpara_var.Tv = EGMDeveloperTankParam.ReglerParam.Tv;
		LCPIDpara_var.Tf = EGMDeveloperTankParam.ReglerParam.Tf;
		LCPIDpara_var.Kw = EGMDeveloperTankParam.ReglerParam.Kw;
		LCPIDpara_var.Kfbk = EGMDeveloperTankParam.ReglerParam.Kfbk;
/*
		LCPWM_DMod_var.tminpuls = EGMDeveloperTankParam.ReglerParam.tminpuls;
		LCPWM_DMod_var.tperiod = EGMDeveloperTankParam.ReglerParam.tperiod;
*/
		LCPWM_DMod_var.tminpuls			= 1;  /* 100 ms kürzester Puls bzw pause*/
		LCPWM_DMod_var.tperiod			= 20;  /*Periodendauer 2 sec*/
		if ( ( EGMDeveloperTankParam.ReglerParam.d_mode != LCPID_D_MODE_X )&&
			( EGMDeveloperTankParam.ReglerParam.d_mode != LCPID_D_MODE_E ))
		{
			LCPIDpara_var.d_mode = LCPID_D_MODE_X;
			EGMDeveloperTankParam.ReglerParam.d_mode = LCPID_D_MODE_X;
		}
		else
			LCPIDpara_var.d_mode = EGMDeveloperTankParam.ReglerParam.d_mode;

	}
	else
		LCPIDpara_var.enter = 0;


/***************************************************************/
/***************************************************************/
/***************************************************************/
/***************************************************************/
/* Parameter ungültig? dann Standard setzen*/
	if ( ( LCPIDpara_var.d_mode != LCPID_D_MODE_X )&&
		( LCPIDpara_var.d_mode != LCPID_D_MODE_E ))
	{
		LCPIDpara_var.d_mode	= LCPID_D_MODE_X;
		EGMDeveloperTankParam.ReglerParam.d_mode = LCPID_D_MODE_X;
	}


/*********************************************************************************************/
/* Alarme */
		if (ModulStatus_AT == 1)
		{
			/*Sensor defekt*/
			EGM_AlarmBitField[18] =	 (EGMDeveloperTank.RealTemp > 32000 ) || (EGMDeveloperTank.RealTemp < -500 );
			/*Temp. nicht im Range*/
			EGM_AlarmBitField[6] = (EGMDeveloperTank.RealTemp < (INT)(EGMDeveloperTankParam.RatedTemp - EGMDeveloperTankParam.ToleratedDevNeg))
									&& !EGM_AlarmBitField[18];
			EGM_AlarmBitField[7] =	(EGMDeveloperTank.RealTemp > (INT)(EGMDeveloperTankParam.RatedTemp + EGMDeveloperTankParam.ToleratedDevPos))
									&& !EGM_AlarmBitField[18];
			/* Überhitzt*/
			EGM_AlarmBitField[26] = (EGMDeveloperTank.RealTemp > OVERHEATTEMP) && !EGM_AlarmBitField[18];
		}
		else /* Modulsttatus nicht OK? Werte nicht gültig, also keine Fehlermeldung */
		{
			EGM_AlarmBitField[18] = 0;
			EGM_AlarmBitField[6]  = 0;
			EGM_AlarmBitField[7]  = 0;
			EGM_AlarmBitField[26] = 0;
		}
/*********************************************************************************************/
/* PWM Ansteuerung des Ausgangs */
/* VAR_INPUT (analogous) */

/**
wenn alle Randbedingunge erfüllt sind, dann die Stellgröße auf den PWM Baustein geben
**/
	if (EGMDeveloperTank.LevelInRange && (EGMDeveloperTank.CoolingOn == OFF)
	&& EGMDeveloperTank.Enable
	&& !Overheated)
	{
/* solange wir mehr als 0,3 °C unterhalb sollwert sind: vollgas (Aufheizphase)
danach Regleraktivieren*/
		if(EGMDeveloperTank.RealTemp < (EGMDeveloperTankParam.RatedTemp -3))
			LCPWM_DMod_var.x 				= 100;
		else
			LCPWM_DMod_var.x 				= LCPID_var.Y;

	}
	else
		LCPWM_DMod_var.x 				= 0;


	LCPWM_DMod_var.basetime		= LCCountVar.ms100cnt;

/*  PWM Ansteuerung des Ausgangs */
	LCPWM_DMod(&LCPWM_DMod_var);

/*******************************************************************************/
/*******************************************************************************/
/* Thermo Kühl-/Heizgerät als Stellglied*/
/*******************************************************************************/
/*******************************************************************************/
/*
 * Variante Thermo Kühl/Heiz Gerät
 * Sollwert wird gemappt auf Temperaturbereich des Gerätes
 * -100% bis +100% wird zu (z.B.) 2°C bis 80°C
 * f(x) = (39/100)x + SW
 *
 * Änderung: Stellgröße beschränkt auf 10 bis 45 °C, mehr wird vom Kühlgerät
 * sowieso nicht erreicht (als Istwert)
 * f(x) = (35/200)x + SW
 */
/*
 * Filterung der Stellgröße
 */
	LCMovAvFilter_var.enable 	= TRUE;
	LCMovAvFilter_var.x			= LCPID_var.Y;

	LCMovAvFilter(&LCMovAvFilter_var);

 	tmpval = (35.0/200.0)*LCMovAvFilter_var.y + (EGMDeveloperTankParam.RatedTemp / 10.0);
	if ((LCPID_var.Y >= 100) || (tmpval > 45.0))
		Cooler.NewValues.SetPoint = 45.0;
	else
	if ((LCPID_var.Y <= -100) || (tmpval < 10.0))
		Cooler.NewValues.SetPoint = 10.0;
	else
		Cooler.NewValues.SetPoint = tmpval;




/*******************************************************************************/
/*******************************************************************************/

/* Ausgang nur ansteuern wenn Füllstand OK und Kühlung aus, sowie
Regler EIN und keine Überhitzung,
 sonst Heizung AUS*/
	EGMDeveloperTank.HeatingOn = LCPWM_DMod_var.y;

	if ((LCPWM_DMod_var.x < EGMDeveloperTankParam.CoolingOnY)
	&& EGMDeveloperTank.Enable)
		EGMDeveloperTank.CoolingOn = ON;
	else
	if (MinOnTimeCooling.Q)
		EGMDeveloperTank.CoolingOn = OFF;

	MinOnTimeCooling.PT = EGMDeveloperTankParam.MinOnTimeCooling;
	MinOnTimeCooling.IN = EGMDeveloperTank.CoolingOn;
	TON_10ms(&MinOnTimeCooling);

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/* Parametrierbaustein*/

	LCPIDpara(&LCPIDpara_var);
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
		LCPIDAutoTune_var.request				= LCPIDAUTOTUNE_REQU_AUTOTUNE;
	else
		LCPIDAutoTune_var.request				= LCPIDAUTOTUNE_REQU_OFF;

	LCPIDAutoTune_var.basetime				= LCCountVar.ms100cnt;

/*	LCPIDAutoTune(&LCPIDAutoTune_var);*/


/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/* Reglerbaustein LCPID*/
	if (StartAutoTune)
		LCPID_var.ident						= LCPIDAutoTune_var.ident;
	else
		LCPID_var.ident						= LCPIDpara_var.ident;
/*Sollwert*/
	LCPID_var.W							= EGMDeveloperTankParam.RatedTemp;

/*Istwert*/
	LCPID_var.X							= EGMDeveloperTank.RealTemp;

/*rückgeführte Stellgröße*/
	LCPID_var.Y_fbk					= LCPID_var.Y;
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
	if (EGMDeveloperTank.Auto)
		LCPID_var.out_mode				= LCPID_OUT_MODE_AUTO;
	else
		LCPID_var.out_mode				= LCPID_OUT_MODE_MAN;

	LCPID_var.basetime					= LCCountVar.ms100cnt;
	LCPID(&LCPID_var);


	if(StartAutoTune)
	{
		LCPIDpara_var.Kp				= LCPIDAutoTune_var.processPar.Kp;
		LCPIDpara_var.Tn				= LCPIDAutoTune_var.processPar.Tn;
		LCPIDpara_var.Tv				= LCPIDAutoTune_var.processPar.Tv;
		LCPIDpara_var.Tf				= LCPIDAutoTune_var.processPar.Tv/10;
	}


/***************************************************************/
/***************************************************************/
/***************************************************************/
/***************************************************************/

	EGMDeveloperTankParam.MaxOKTemp = EGMDeveloperTankParam.RatedTemp + EGMDeveloperTankParam.ToleratedDevPos;
	EGMDeveloperTankParam.MinOKTemp = EGMDeveloperTankParam.RatedTemp - EGMDeveloperTankParam.ToleratedDevNeg;

/*Measurement*/

	Overheated = EGMDeveloperTank.RealTemp > OVERHEATTEMP;

	if(	(EGMDeveloperTank.RealTemp <= EGMDeveloperTankParam.MaxOKTemp) &&
		(EGMDeveloperTank.RealTemp >= EGMDeveloperTankParam.MinOKTemp)
		)
		EGMDeveloperTank.TempInRange = TRUE;
	else
		EGMDeveloperTank.TempInRange = FALSE;

	EGMDeveloperTank.TempNotInRange = !EGMDeveloperTank.TempInRange;

/*Level*/
	DeveloperLevelOKTimer.IN = EGMDeveloperTank.TankFull;
	DeveloperLevelOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&DeveloperLevelOKTimer);

	DeveloperLevelNOTOKTimer.IN = !EGMDeveloperTank.TankFull;
	DeveloperLevelNOTOKTimer.PT = 500; /*5 sec*/
	TON_10ms(&DeveloperLevelNOTOKTimer);

	LevelOKR.CLK = DeveloperLevelOKTimer.Q;
	LevelNOTOKR.CLK = DeveloperLevelNOTOKTimer.Q;
	R_TRIG(&LevelOKR);
	R_TRIG(&LevelNOTOKR);
	if(LevelOKR.Q)
		EGMDeveloperTank.LevelNotInRange = FALSE;
	if(LevelNOTOKR.Q)
		EGMDeveloperTank.LevelNotInRange = TRUE;

	EGMDeveloperTank.LevelInRange = !EGMDeveloperTank.LevelNotInRange;
/* Fehlermeldung verzögern, erst wird automatisch befüllt */
	DeveloperLevelErrorTimer.IN = EGMDeveloperTank.LevelNotInRange;
	DeveloperLevelErrorTimer.PT = 2000; /* 20 s */
	TON_10ms(&DeveloperLevelErrorTimer);
	EGM_AlarmBitField[5] = (ModulStatus_DI == 1) && DeveloperLevelErrorTimer.Q;

/*auto-refill and circulation*/
	CirculationPumpOffTimer.IN = EGMDeveloperTank.LevelNotInRange;
	CirculationPumpOffTimer.PT = 2000; /*20 sec */
	TON_10ms(&CirculationPumpOffTimer);

	if(	EGMGlobalParam.UseDevCircFlowControl )
	{
		CirculationPumpStartTimer.IN = EGMDeveloperTank.CirculationPump;
		CirculationPumpStartTimer.PT = 1000; /* 10 sec */
		TON_10ms(&CirculationPumpStartTimer);
		CircFlowDelay.IN = !DevCircFlowInput;
		CircFlowDelay.PT = 300; /* 3 sec */
		TON_10ms(&CircFlowDelay);
		if( CirculationPumpStartTimer.Q && CircFlowDelay.Q )
		{
			EGM_AlarmBitField[34] = TRUE;
			EGMDeveloperTank.CirculationPump = OFF;
		}
	}
	else
		EGM_AlarmBitField[34] = FALSE;


	if (EGMDeveloperTank.Auto )
	{
		if ( CirculationPumpOffTimer.Q || EGM_AlarmBitField[34] )
			EGMDeveloperTank.CirculationPump = OFF;
		else
		if (EGMDeveloperTank.LevelInRange)
			EGMDeveloperTank.CirculationPump = ON;

		if (EGMDeveloperTank.LevelNotInRange && !EGM_AlarmBitField[38])
		{
			EGMDeveloperTank.Refill = ON;
			DeveloperRefillActive = TRUE;
		}
		else
		if(DeveloperRefillActive)
		{
			EGMDeveloperTank.Refill = OFF;
			DeveloperRefillActive = FALSE;
		}

		/*
		* Switch Cooler On
		*/
		if( !Cooler.UnitOn && !DevOldAuto )
			Cooler.Cmd_Start = TRUE;
	}

/* AUTO mode off: stop refill pump, stop circulation*/
	if ((DevOldAuto && !EGMDeveloperTank.Auto) || !ControlVoltageOk)
	{
		EGMDeveloperTank.Refill = OFF;
		DeveloperRefillActive = FALSE;
		EGMDeveloperTank.CirculationPump = OFF;
		if( Cooler.UnitOn )
			Cooler.Cmd_Stop = TRUE;
	}
	DevOldAuto = EGMDeveloperTank.Auto;


/******************************************************************************/
/******************************************************************************/
/* REPLENISHING developer*/

	if(EGMGlobalParam.PROV_KitInstalled)
		ProV();
	else
	{
		Replenish();
		TopUp();
	}

/* Saftey: pump off, after 30 sec, if Tank is full */
	RefillSafetyTimer.IN = EGMDeveloperTank.Refill && EGMDeveloperTank.TankFull;
	RefillSafetyTimer.PT = 3000;
	if(RefillSafetyTimer.Q)
		EGMDeveloperTank.Refill = OFF;

	ReplSafetyTimer.IN = EGMDeveloperTank.RegenerationPump && EGMDeveloperTank.TankFull;
	ReplSafetyTimer.PT = 3000;
	if(ReplSafetyTimer.Q)
		EGMDeveloperTank.RegenerationPump = OFF;

}


