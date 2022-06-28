/*************************************************************************************************
 Global header file for EGM

 defines, global variables and common includes

*************************************************************************************************/

#include <bur/plc.h>       /* macros for B&R PLC programming */
#include <bur/plctypes.h>
#include <standard.h>      /* prototypes for STANDARD-Library */
#include <sys_lib.h>
#include <asstring.h>
#include <string.h>
#include <loopcont.h>

#ifndef TRUE
	#define  TRUE (1)
#endif
#ifndef FALSE
	#define FALSE (0)
#endif
#define MAXPLATETYPES (10)
#define MAXPLATETYPENAME 20
/*Modes*/
#define M_OFF 0
#define M_AUTO 1
#define M_INITCLEANING 2
#define M_CLEANING 3

/*States*/
#define S_OFF 0
#define S_HEATING 1
#define S_READY 2
#define S_STARTPROCESSING 3
#define S_PROCESSING 4
#define S_WAITFORSTANDBY 5
#define S_CLEANING1 6
#define S_CLEANING2 7
#define S_CLEANING3 8
#define S_ERROR 10

#define CAUTION 0
#define INFO	1
#define REQUEST	2

#define MESSAGEPIC 41
#define FILEPIC 42
#define EMPTYPIC 107

#define	MAXFILELIST	32
#define MAXFILES	50
#define MAXFILENAMELENGTH	128
#define MAXFILETYPELENGTH 5

#define EGMSRAMSIZEWORDS 2500
#define EGMSRAMSIZEBYTES 5000

#define OVERALLSRAMSIZEWORDS 12500
#define OVERALLSRAMSIZEEBYTES 25000

#define PERFSRAMSIZEWORDS 10000
#define PERFSRAMSIZEBYTES 20000

#define OPEN		(1)
#define CLOSE	(0)
#ifndef ON
	#define ON		(1)
#endif
#ifndef OFF
	#define OFF		(0)
#endif

#define REPLPERSQM (1)
#define REPLPERPLATE (2)
#define FRESHWATERONLY (3)

#define FIFOLENGTH		(10)

#define EGM_MAXALARMS	(99)

enum PlateState {PS_NONE=0, PS_RUNNING, PS_WAITINGFOROUTPUT, PS_OUTPUTREACHED};

#define MAX_NUMBER_OF_CONTROLLERS	4

#define BLUEFIN_XS (1)
#define BLUEFIN_LOWCHEM (2)

/***************************** Type Definitions ***********************************/
typedef struct
{
	CTUtyp	EdgeCounter;
	INT		LastCV;
	INT		Frequency;
	INT		FlowCounter;
	INT		OverflowCounter;
	REAL	LitersPerHour;
	REAL	LitersPerMinute;
	DINT	MlFlown;
	BOOL	ResetMlFlown;
	BOOL	Flowing;
	UDINT	PulsesPerLiter;
}FlowCount_type;


typedef struct
{
	STRING Name[MAXPLATETYPENAME];
	STRING ManufacturerName[20];
	UDINT	SizeX;
	UDINT	SizeY;
/* Fläche der Platte (ergibt sich natürlich aus SizeX*SizeY */
	REAL	Area;
/* Regeneratverbrauch pro Quadratmeter */
	REAL	RegenerationPerSqrM;
/* Regeneratverbrauch pro Platte */
	REAL	RegenerationPerPlate;
/* Plattentyp (Silber, Polymer oder was immer */
	USINT	Type;
/*Dip Zeit in s*/
	USINT	DipTime;
/*20 Bytes reserve*/
	UDINT Reserve[5];
} EGMPlate_Type;


/***********************************************************/
/* Developer */
typedef struct
{
	STRING	Name[20];
	UINT		Temperature;				/* 0,1 °C*/
	UINT		Lifetime;					/* Days */
	UINT		MaxArea;					/* m² */
	UINT		TopUp;					/* ml/m² */
	REAL		Replenishment;			/* ml/m² */
	REAL		StandbyReplenishment;	/* ml/h */
	UINT		BrushSpeed;				/* rpm (mit Faktor für Visualisierung!)*/
} EGMDeveloper_Type;

typedef struct
{
	STRING Version[10];
	USINT MachineType;
	USINT	Unit_Rpm; /*rpm oder Hz*/
	USINT	Unit_Propulsion; /*m/min oder ft/min*/
	USINT	Unit_Temperature; /*C oder F*/
	USINT	Unit_Length; /*mm oder Inch*/
	USINT	Unit_Area; /*quadratmeter oder squareInch*/
	USINT	Unit_Volume; /* Liter oder Gallone*/
/*Factor Rpm<->PlateSpeed in m/min*/
	REAL	MainMotorFactor;
	REAL	CurrentPlateSpeed; /*m/min*/
	UDINT	CurrentDipTime; /*s*/
	UDINT	CurrentBrushSpeed; /*rpm*/
	UDINT	MaxPlateSizeX;
	UDINT	MaxPlateSizeY;
/* added 18.01.2007*/
	REAL	BrushFactor;
	STRING	MachineNumber[10];
/* Flow control einzeln aktivierbar */
	BOOL	UseGumFlowControl;
	BOOL	UseDevCircFlowControl;
	BOOL	UsePrewashFlowControl;
	BOOL	UseRinseFlowControl;
	BOOL	UseTopUpFlowControl;
	BOOL	UseReplFlowControl;
	BOOL	PlateDirection;	/* 0=right to left, 1=left to right */
	USINT	PROV_KitInstalled;	/* */
	BOOL	CheckIfCoolingDeviceOk;
	UDINT	DeveloperPulsesPerLiter;
	UDINT	ReplenisherPulsesPerLiter;
	UDINT	GumPulsesPerLiter;
	UDINT	ScreensaverTime; /* in sec */
	BOOL	EnableGumSection; /* neu für LowChem: Gummierung ist optional bei LC*/
} EGMGlobalParam_Type;

/* Motors */

typedef struct
{
	BOOL	Enable;	/*Freigabe Ausgang (EIN)*/
	USINT	Auto;
	USINT	Manual;
	USINT	Start;
	USINT	Stop;
	INT	RatedRpm; /*in cm/min wg Genauigkeit*/
	INT		AutoRatedRpm;
	INT		RealRpm;
/*20 Bytes reserve*/
	UDINT Reserve[5];
} EGMMotor_Type;

typedef struct
{
	REAL	Kp;
	REAL	Tn;
	REAL	Tv;
	REAL	Tf;
	REAL	Kw;
	REAL	Kfbk;
	USINT	d_mode;
	UINT	tminpuls;
	UINT	tperiod;
	BOOL	SetpointTracking;
}EGMReglerParam_typ;

typedef struct
{
	UINT	RatedTemp[4];
/*Bereich, der als OK betrachtet wird*/
	UINT	ToleratedDevPos;
	UINT	ToleratedDevNeg;

	UINT	MaxOKTemp; /*ergeben sich automatisch aus ToleratedDev...*/
	UINT	MinOKTemp; /*ergeben sich automatisch aus ToleratedDev...*/

	INT		RatedTempCorr[4];
	EGMReglerParam_typ		ReglerParam[4];
	UINT	SetPointCorrTMin;	/*Sollwertnachf. Zeitkonst in Min*/
	UINT	SetPointCorrMax;		/*Sollwertnachf. Maxwert in °C*/
/*Wertepaare für Korrekturgeraden*/
	UINT	EndSetpoint[5];
	UINT	StartOverheat[5];
	UINT	StartAmbient[5];
	UINT	EndAmbient[5];

	INT		DecPerPlate;
	INT		IncPerMinute;
	INT		MaxInc;
	INT		AmbientThreshold;
} EGMPreheatParam_Type;

typedef struct
{
	BOOL	Enable;
	BOOL	Auto[4];

/*
4 Heizkreise vorgesehen
bisher [0] und [1] verwendet,
[2] nichtz benutzt
[3] für Umgebungstemperatur
*/
	INT RealTemp[4];
/* Umgebungstemp (4 mal vorgesehen als Reserve) */
	INT AmbientTemp[4];
/*4 Heizkreise vorgesehen*/
	INT		HeatingOn[4];

/*Ausgang: Bereit, Temp OK*/
	USINT	TempInRange;
	USINT	TempNotInRange;
/* Schaltausgang Lüfter */
	BOOL	FansOn;

} EGMPreheat_Type;

typedef struct
{
	UDINT	RatedTemp;

/*Bereich, der als OK betrachtet wird*/
	UINT	ToleratedDevPos;
	UINT	ToleratedDevNeg;

/*Hysterese f Heizung*/
/**/
	UINT	SwitchDevHeatingOn;
	UINT	SwitchDevHeatingOff;
	UINT	SwitchDevCoolingOn;
	UINT	SwitchDevCoolingOff;

	UINT	HeatingOnTemp; /*ergeben sich automatisch aus Switchdev...*/
	UINT	HeatingOffTemp; /*ergeben sich automatisch aus Switchdev...*/
/*Kühlung Einschaltpunkt*/
	INT		CoolingOnY;
/*Kühlung Ausschaltpunkt*/
	UINT	CoolingOffTemp;

	UINT	MaxOKTemp; /*ergeben sich automatisch aus ToleratedDev...*/
	UINT	MinOKTemp; /*ergeben sich automatisch aus ToleratedDev...*/

/*Wartezeiten, um stabile Zustände abzuwarten (im Prinzip der I-Anteil)*/
	UINT	HeatingOnDelay;
	UINT	HeatingOffDelay;
	UINT	CoolingOnDelay;
	UINT	CoolingOffDelay;
/*min/max Ein/Ausschaltzeiten*/
	UINT	MaxOnTimeHeating;
	UINT	MinOnTimeHeating;
	UINT	MaxOffTimeHeating;
	UINT	MinOffTimeHeating;
	UINT	MaxOnTimeCooling;
	UINT	MinOnTimeCooling;
	UINT	MaxOffTimeCooling;
	UINT	MinOffTimeCooling;

	UDINT	StartRefillDelay; /*s*/
	UDINT	StopRefillDelay; /*s*/
	EGMReglerParam_typ		ReglerParam;
/* Replenishing*/
	UINT	DeveloperType; /*Nummer des Entwicklers aus Liste*/
	STRING	DeveloperName[20];
	UINT		ReplenishingMode;	/* per m², per plate */
	BOOL		EnableStandbyReplenishing;
	BOOL		EnableOffReplenishing;
	UINT		TopUpMode;	/* per m², per plate */
	REAL		ReplenishmentPerSqm;	/* ml/m² */
	REAL		ReplenishmentPerPlate;	/* ml/pl */
	REAL		StandbyReplenishment;	/* ml/h */
	REAL		OffReplenishment;	/* ml/h */
	UINT		StandbyReplenishmentIntervall;	/* min */
	REAL		PumpMlPerSec;			/* ml/s */
	UINT		MinPumpOnTime;			/* 10 ms */
	UINT		MinPumpOnTimeStandby;			/* 10 ms */
	INT			TempOffset;
	REAL		TopUpPerSqm;	/* ml/m² */
	REAL		TopUpPerPlate;	/* ml/pl */
	REAL		TopUpPumpMlPerSec;			/* ml/s */
	UINT		TopUpMinPumpOnTime;			/* 10 ms */
} EGMDeveloperTankParam_Type;


typedef struct
{
	BOOL	Enable;
	BOOL	Auto;
	INT		RealTemp;
/*in/out*/
	BOOL	TankFull;
	BOOL	TankEmpty;
	BOOL	RegenerationPump;
	BOOL	CirculationPump;
	BOOL	Refill;
	BOOL	HeatingOn;
	BOOL	CoolingOn;

/*Ausgang: Bereit, Temp OK*/
	USINT	TempInRange;
	USINT	TempNotInRange;

	USINT	LevelInRange;
	USINT	LevelNotInRange;
} EGMDeveloperTank_Type;


typedef struct
{
	UINT	Speed;
	UINT	BrushSpeed;
	UINT	PreheatTemp;
	UINT	DeveloperTemp;
	UINT	StandbyDelay;
	UINT	StandbySpeed;
	REAL	MaxPlateSizeX;
	REAL	MaxPlateSizeY;
	UINT	StandbyBrushSpeed;
	UINT	PreheatTemp2;
/*die nächsten 3 Zeiten in sec*/
	UINT	WaitGumBackTime;
	UINT	GumRinsingTime;
	UINT	WaitGumRinseTime;
/* PRO V Erweiterungen */
	UINT	PROV_PumpOnTime;
	UINT	PROV_PumpOffTime;
} EGMAutoParam_Type;

typedef struct
{
/*IN / OUT*/
	BOOL	TankFull;
	BOOL	Pump;
	BOOL	Valve;
	BOOL	HeatingOn;
	USINT		LevelNotInRange;
	INT		RealTemperature;
	BOOL	Auto;
	BOOL	PumpCmd;
} EGMPrewash_Type;

typedef struct
{
	UINT		ReplenishingMode;	/* per m², per plate, freshwater only */
	REAL		ReplenishmentPerSqm;	/* ml/m² */
	REAL		ReplenishmentPerPlate;	/* ml/pl */
	UINT		ReplenishmentIntervall;	/* min */
	REAL		PumpMlPerSec;			/* ml/s */
	UINT		MinPumpOnTime;			/* 10 ms */

} EGMPrewashParam_Type;

typedef struct
{
	UINT		ReplenishingMode;	/* per m², per plate */
	REAL		ReplenishmentPerSqm;	/* ml/m² */
	REAL		ReplenishmentPerPlate;	/* ml/pl */
	UINT		ReplenishmentIntervall;	/* min */
	REAL		PumpMlPerSec;			/* ml/s */
	UINT		MinPumpOnTime;			/* 10 ms */

} EGMRinseParam_Type;

typedef struct
{
/*IN / OUT*/
	BOOL	TankFull;
	BOOL	Pump;
	BOOL	Valve;
	USINT		LevelNotInRange;
	BOOL	Auto;
	BOOL	PumpCmd;

} EGMRinse_Type;

typedef struct
{
/*IN / OUT*/
	BOOL	TankFull;
	BOOL	Pump;
	BOOL	Valve;			/* Umschalten: 0->Ablauf in Abfluß, 1->Ablauf in Gummierungsbehälter*/
	BOOL	RinseValve;	/* Spülwasser */
	USINT		LevelNotInRange;
	BOOL	Auto;
	BOOL	PumpCmd;
} EGMGum_Type;


typedef struct
{
	BOOL	Used;
	UINT	Type;
	REAL	Position;
	REAL	Length;
	USINT	Status;
}EGMPlate_FIFO_Type;


typedef struct
{
	UINT	mode;
	UINT	state;
	UINT	ErrorNumber;
	STRING	ErrorText[64];
}EGMState_struct;

typedef struct
{
	REAL	SetPoint;
	REAL	LimitLow;
	REAL	LimitHigh;
	REAL	HeatP;
	REAL	HeatI;
	REAL	HeatD;
	REAL	CoolP;
	REAL	CoolI;
	REAL	CoolD;

}Cooler_Values;

typedef struct
{
	USINT	RawDataB1;
	USINT	RawDataB2;
	USINT	RawDataB3;
	USINT	RawDataB4;
	USINT	RawDataB5;

	BOOL	RTD1OpenFault;
	BOOL	RTD1ShortedFault;
	BOOL	RTD1Open;
	BOOL	RTD1Shorted;
	BOOL	RTD2OpenFault;
	BOOL	RTD2ShortedFault;
	BOOL	RTD2Open;
	BOOL	RTD2Shorted;
	BOOL	RTD2OpenWarn;
	BOOL	RTD2ShortedWarn;
	BOOL	RTD3OpenFault;
	BOOL	RTD3ShortedFault;
	BOOL	RTD3Open;
	BOOL	RTD3Shorted;
	BOOL	RefrigHighTemp;
	BOOL	HTCFault;
	BOOL	HighFixedTempFault;
	BOOL	LowFixedTempFault;
	BOOL	HighTempFault;
	BOOL	LowTempFault;
	BOOL	LowLevelFault;
	BOOL	HighTempWarn;
	BOOL	LowTempWarn;
	BOOL	LowLevelWarn;
	BOOL	BuzzerOn;
	BOOL	AlarmMuted;
	BOOL	UnitFaulted;
	BOOL	UnitStopping;
	BOOL	UnitOn;
	BOOL	PumpOn;
	BOOL	CompressorOn;
	BOOL	HeaterOn;
	BOOL	RTD2Controlling;
	BOOL	HeatLEDFlashing;
	BOOL	CoolLEDFlashing;
	BOOL	CoolLEDOn;

	BOOL	Cmd_Start;
	BOOL	Cmd_Stop;

	Cooler_Values CurrentValues;
	Cooler_Values NewValues;

	REAL	IntTemp;
	REAL	ExtTemp;

} Cooler_struct;

/***************************** Variable Definitions ***********************************/

_GLOBAL STRING  EGMgVERSION[9];

_GLOBAL USINT UserLevel;
_GLOBAL USINT Language;
_GLOBAL USINT CurrentMode;
_GLOBAL USINT CurrentState;

_GLOBAL	EGMMotor_Type	EGMBrushMotor;
_GLOBAL	EGMMotor_Type	EGMMainMotor;
_GLOBAL	EGMAutoParam_Type		EGMAutoParam;

_GLOBAL	EGMDeveloperTank_Type	EGMDeveloperTank;
_GLOBAL	EGMPreheat_Type	EGMPreheat;
_GLOBAL	EGMPrewash_Type	EGMPrewash;
_GLOBAL	EGMRinse_Type	EGMRinse;
_GLOBAL	EGMGum_Type	EGMGum;

_GLOBAL	EGMPreheatParam_Type		EGMPreheatParam;
_GLOBAL	EGMGlobalParam_Type		EGMGlobalParam;
_GLOBAL	EGMDeveloperTankParam_Type		EGMDeveloperTankParam;
_GLOBAL	EGMPrewashParam_Type	EGMPrewashParam;
_GLOBAL	EGMRinseParam_Type	EGMRinseParam;

/* GENERAL INPUTS*/
_GLOBAL	BOOL	ControlVoltageOk;
_GLOBAL	BOOL	RPMCheck;
_GLOBAL 	BOOL	InputSensor;
_GLOBAL	BOOL	OutputSensor;

_GLOBAL BOOL		GumFlowInput;
_GLOBAL BOOL		ReplFlowInput;
_GLOBAL BOOL		TopUpFlowInput;
_GLOBAL BOOL		DevCircFlowInput;
_GLOBAL BOOL		PrewashFlowInput;
_GLOBAL BOOL		RinseFlowInput;
_GLOBAL	BOOL		ServiceKey;
_GLOBAL	BOOL		PlateSensor;
_GLOBAL	BOOL		VCP_OK;

/* GENERAL OUTPUTS*/
_GLOBAL	BOOL	Ready;
_GLOBAL	BOOL	NoError;
_GLOBAL	BOOL	ActivateControlVoltage;
_GLOBAL	BOOL	Drying;
_GLOBAL	BOOL	LightGreen;
_GLOBAL	BOOL	LightYellow;
_GLOBAL	BOOL	LightRed;
_GLOBAL	BOOL	Horn;


/*Zeitbasiswerte für Regelung*/
_GLOBAL	LCCounter_typ LCCountVar;

/*Plattentypen*/
_GLOBAL	EGMPlate_Type EGMPlateTypes[MAXPLATETYPES];

/* Var für Darstellung der Platten auf dem Schirm*/
_GLOBAL	EGMPlate_FIFO_Type	EGMPlateFIFO[FIFOLENGTH];
_GLOBAL	UINT	EGMPlatesInFIFO;

/* Var for replenishment managament. used in Developertank task*/
_GLOBAL UINT	ReplenishmentPlateCounter,PrewashReplenishmentPlateCounter,
					RinseReplenishmentPlateCounter,TopUpPlateCounter;
_GLOBAL REAL	ReplenishmentSqmCounter,PrewashReplenishmentSqmCounter,
					RinseReplenishmentSqmCounter,TopUpSqmCounter;

_GLOBAL UDINT	OverallPlateCounter _VAR_RETAIN,OverallReplenisherCounter _VAR_RETAIN,ReplenisherCounter	_VAR_RETAIN,
OverallReplenisherPumpTime _VAR_RETAIN;
_GLOBAL REAL		OverallSqmCounter	_VAR_RETAIN;

_GLOBAL UINT	SessionPlateCounter;
_GLOBAL REAL	SessionSqmCounter;
_GLOBAL REAL	SqmSinceDevChg	_VAR_RETAIN;
_GLOBAL UDINT	ReplenisherUsedSinceDevChg	_VAR_RETAIN;
_GLOBAL	UDINT	DeveloperChangeDate _VAR_RETAIN,ReplenisherUsedSinceDevChange	_VAR_RETAIN;

_GLOBAL	BOOL	EGM_AlarmBitField[EGM_MAXALARMS],EGM_AlarmQuitBitField[EGM_MAXALARMS];
_GLOBAL	UINT	CPUTemp,AmbientTemp;
_GLOBAL	USINT	BatteryStatus;
_GLOBAL	EGMState_struct	EGMState;
_GLOBAL	Cooler_struct Cooler;
_GLOBAL	BOOL	ModulStatus_DO1,ModulStatus_DO2,ModulStatus_DO3,
ModulStatus_DI,ModulStatus_DI2,ModulStatus_AT,
ModulStatus_AO,ModulStatus_PS,ModulStatus_PS1,ModulStatus_PS2;
_GLOBAL USINT	ModulStatus_AT1;
_GLOBAL	FlowCount_type	GumFC, ReplFC, TopUpFC;
_GLOBAL BOOL	HeatingON[MAX_NUMBER_OF_CONTROLLERS];
/* E/A für ProV Test Japan */
_GLOBAL	BOOL In_DevCanister_Full;
_GLOBAL	BOOL In_DevCanister_Empty;
_GLOBAL	BOOL Out_DevCanister_Circulation;
_GLOBAL	BOOL Out_DevCanister_Replenish;
/*
Maschinenlänge in m
	Standard Bluefin 2.80
	XS Bluefin		 2.70
	LowChem Bluefin  2.05
 */
_GLOBAL REAL MACHINELENGTH;
_GLOBAL USINT MachineType;



