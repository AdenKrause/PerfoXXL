#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Steuern von Visualisierungsfunktionen */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.00		25.09.02	erste Implementation					HA		*/
/*	1.01		28.03.03	bugfix Plattentyp bei Panorama->Absturz			HA		*/
/*							- erweiterte Fehlermeldung: EGM/Stanze -> LED */
/*	*/
/*		*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"
#include "egmglob_var.h"
#include "in_out.h"
#include "standard.h"
#include "asstring.h"
#include "sys_lib.h"
#include <visapi.h>
#include <string.h>
#include <loopcont.h>
#include <astime.h>
#include "auxfunc.h"

#define	MAXFILELIST	32
#define MAXFILES	50
#define MAXFILENAMELENGTH	32

#define ROT_HG		0x0056 /*Hintergrundfarbe*/
#define ROT 			0x002D
#define GRUEN 		0x000A

#define GRAU 			0x003B
#define DUNKELGRAU 	0x0008

#define SCHWARZ 		0x0000
#define DUNKELROT		0x009F
#define GELB	 		0x002E
#define WEISS			0x1010

#define CL_DISABLED		0x0C0F
#define CL_ENABLED		0x000F
#define CL_INPUTENABLED	0x0016
#define CL_INPUTDISABLED	0x0807

/*#define CL_DISABLED_RED		0x0856*/
#define CL_DISABLED_RED		0x0456
#define CL_ENABLED_RED		0x0056
#define CL_INPUTENABLED_RED	0x007B
#define CL_INPUTDISABLED_RED	0x567B

#define MIN_X_OFFSET	(-30)

#define CURVEPIC	(109)

#define		ENABLEDCOLOR	0x000E
#define		DISABLEDCOLOR	0x0A0E

/* 	0 englisch
	1 deutsch
MAXLANGUAGES ist der grösste Index, nicht die Anzahl !
*/
#define MAXLANGUAGES	1



typedef struct
{
	REAL	Up;
	REAL	AboveTrolley;
	REAL	MaxDown;
	REAL	PreRelease;
	REAL	ReleasePlateVert;
	REAL	EnablePaperRemove;
	REAL	PanoramaAdapter;
	REAL	SpeedChangePosition;
	REAL	ReleasePlateHor;
	REAL	TakePlate;
	REAL	Stack0Pos;
	REAL	Stack20Pos;
	REAL	SlowDownPos;
	REAL	SpeedUpSlow;
	REAL	SpeedUpFast;
	REAL	StackDetectSpeed;
	REAL	HorPositionPano;
}Feeder_Param_mm_Typ;

typedef struct
{
	REAL	EnableFeeder;
	REAL	PaperReleaseSingle;
	REAL	PaperReleasePanorama;
	REAL	TrolleyOpenCloseSpeed;
	REAL	PaperRemoveSpeed;
	REAL	PushPos_Horizontal;
	REAL	TrolleyDetectSpeed;
	REAL	TrolleyDetectStartPos;
	REAL	TrolleyDetectEndPos;
	REAL	HorPositionPano;
}PaperRemove_Param_mm_Typ;

typedef struct
{
	REAL	EnableFeeder;
	REAL	TakePlate;
	REAL	ReleasePlate;
	REAL	ToAdjusterSpeed;
	REAL	ToTrolleySpeed;
	REAL	PlateDropPosition;
	REAL	ReleasePlateMax;
}Shuttle_Param_mm_Typ;

typedef struct
{
	REAL	FastSpeed;
	REAL	SlowSpeed;
	REAL	AdjustSpeedFast;
}Adjuster_Param_mm_Typ;

typedef struct
{
	REAL	PlateTakePosition;
	REAL	PlateReleasePosition;
	REAL	PlateReleasePosition2;
	REAL	MaxPlateTakePosition;
	REAL	TurnEnabledPosition;
/*Rotation*/
	REAL	Turn0Position;
	REAL	Turn90Position;
	REAL	Turn180Position;
	REAL	ConveyorBeltSpeed;
	REAL	TurnSpeed;
	REAL	ToAdjusterSpeed;
	REAL	ToConveyorSpeed;
	REAL	AdjusterDeloadSpeed;
}Deloader_Param_mm_Typ;


_GLOBAL SINT IOConfiguration _VAR_RETAIN,NewIOConfig	_VAR_RETAIN;
_LOCAL	UINT	StartButtonColorReleased,StartButtonColorPressed;
_LOCAL	UINT	ActiveColor,StandbyColor,ErrorColor;
_GLOBAL USINT	TrolleyOnDisplay;

_GLOBAL	STRING FileIOName[MAXFILENAMELENGTH];
_GLOBAL	STRING	FileType[5];
_GLOBAL	UDINT *FileIOData;
_GLOBAL	UDINT	FileIOLength;
_GLOBAL	BOOL	WriteFileCmd,ReadFileCmd,DeleteFileCmd,WriteFileOK,ReadFileOK,DeleteFileOK,
				FileNotExisting,ReadDirCmd,ReadDirOK,NoDisk;
		BOOL	IOActive;
_GLOBAL		UINT	OrgBildFile;

_LOCAL	UINT	JetOpen,TrolleyKey;
_LOCAL	UINT	JetOpenInvisible;
_LOCAL	UINT	JetClosedInvisible;
_LOCAL	UINT	JetRedInvisible;
_LOCAL	UINT	BeltSensor2Invisible;

_LOCAL UINT		KPGClosedInvisible,KPGOpenInvisible,KPGRedInvisible;

_LOCAL	UINT	Trolley1Invisible;
_LOCAL	UINT	Trolley2Invisible;
_LOCAL	UINT	TrolleyBigInvisible,PlateStack2Invisible;
_LOCAL	UINT	Plates1,Plates2,TrolleyNr1,TrolleyNr2;

_LOCAL	UINT	StartRedInvisible,StartGreenInvisible;
_LOCAL	BOOL	IOButton,IOExit,load,save,loadall,saveall;
_LOCAL	UINT	HeaderNumber;

_GLOBAL BOOL	RedLight,noRedLight;
_GLOBAL BOOL	BUSY;
_GLOBAL BOOL	ERROR,NEWERROR;
_GLOBAL BOOL	STANDBY;
int i,j;
BOOL	flag1,flag2;
_LOCAL	UINT	RedIconsInvisible;
_LOCAL	UINT	DefaultIconsInvisible;

_GLOBAL	RTCtime_typ	RTCtime;
UINT OrgYear;
_GLOBAL	USINT	FileListShift;
_GLOBAL	STRING	FileListName[MAXFILELIST][50];
_GLOBAL	STRING	FileListStatus[MAXFILELIST][5];
_GLOBAL	STRING	FileListTime[MAXFILELIST][20];
_GLOBAL	STRING	FileListResolution[MAXFILELIST][5];
_LOCAL	USINT	ClearListButton;
_GLOBAL	UDINT	PlatesMadeToday;
_GLOBAL UDINT	PlatesMade;
_GLOBAL	BOOL	savePlatesMade;

TP_10ms_typ		BlinkTimer1,BlinkTimer2;
TON_10ms_typ	PulseTimer1,CalibTimer,ControlVoltageTimer;
_GLOBAL BOOL	Clock_1s;
		BOOL	Pulse_1s;
_LOCAL	USINT	InitTouch,LaserPowerChanged;
		USINT	InitTouchOld;
TON_10ms_typ	InitTouchTimer,StartUpTimer;

UINT  _LOCAL  ready,CalibStep,status;
unsigned long VC_HANDLE;

/*FEEDER PARAM PICS*/
_LOCAL	Feeder_Param_Typ	FeederParamInput;
_LOCAL	Feeder_Param_mm_Typ	FeederParameterMm;
_LOCAL	USINT	FeederInputReady,FeederInputReadyMm,FeederInputOk;

/*PAPERREMOVE PARAM PICS*/
_LOCAL	PaperRemove_Param_Typ	PaperRemoveParamInput;
_LOCAL	PaperRemove_Param_mm_Typ	PaperRemoveParameterMm;
_LOCAL	USINT	PaperRemoveInputReady,PaperRemoveInputReadyMm,PaperRemoveInputOk;

/*SHUTTLE PARAM PICS*/
_LOCAL	USINT	ShuttleInputReady,ShuttleInputReadyMm,ShuttleInputOk;

/*ADJUSTER PARAM PICS*/
_LOCAL	Adjust_Param_Typ		AdjusterParamInput;
_LOCAL	Adjuster_Param_mm_Typ	AdjusterParameterMm;
_LOCAL	USINT	AdjusterInputReady,AdjusterInputReadyMm,AdjusterInputOk;
_LOCAL	UINT	SlowDownEnabledOFF;

/*DELOADER PARAM PICS*/
_LOCAL	Deload_Param_Typ		DeloaderParamInput;
_LOCAL	Deloader_Param_mm_Typ	DeloaderParameterMm;
_LOCAL	USINT	DeloaderInputReady,DeloaderInputReadyMm,DeloaderInputOk;
_LOCAL	BOOL	Turn90,Turn180;
		BOOL	Turn90Old,Turn180Old;

/*GLOBAL PARAM PICS*/
_LOCAL	BOOL	deutsch,englisch,spanisch,franzoesisch,italienisch,slowakisch;
		BOOL	deutschAlt,englischAlt,spanischAlt,franzoesischAlt,italienischAlt,slowakischAlt;
_LOCAL	USINT	GlobalInputReady,GlobalInputReadyMm,GlobalInputOk,GlobalInputCancel;
		USINT	LanguageOld;
_LOCAL	Global_Data_Typ	GlobalParameterInput;
_LOCAL	REAL			TrolleyOffsetMm;
_LOCAL	UINT	PaperRemoveEnabledOFF,TBSimulationOFF,
				ProcessorSimulationOFF,PunchSimulationOFF,AlternatingTrolleysOFF,
				FlexibleFeederOFF,TurnStationOFF,AutomaticTrolleyOpenCloseOFF,
				ShiftedLayingOffOFF,WaitForExposerEnabledOFF,
				IgnoreFeederVacuumOFF,IgnoreShuttleVacuumOFF,StopPinsOFF,CompFlagOFF,ConveyorbeltSimOFF,
				TransportSimOFF,MirroredMachineOFF;

_LOCAL	USINT	TogglePaperRemove,ToggleAlternating,ToggleEGMSim,
				ToggleVCPSim,ToggleTBSim,ToggleAutomaticTrolley,
				ToggleTurnStation,ToggleFlexible,ToggleWaitForExposer,
				ToggleIgnoreFeederVacuum,ToggleIgnoreShuttleVacuum,
				ToggleStopPins,ToggleCompFlag,ToggleConveyorbeltSim,ToggleTransportSim,
				ToggleMirrored;

_LOCAL UINT		SeparateBeltTracksOFF,SeparateBeltSensorsOFF,MirroredInvisible;

_LOCAL	USINT	ToggleSeparateBeltTracks,ToggleSeparateBeltSensors;
/*V1.93 Papiererkennungssensor abschaltbar*/
_LOCAL UINT		PaperDetectionOFF,EnablePlateToPaperbasketOFF;
_LOCAL	USINT	TogglePaperDetection,TogglePlateToPaperbasket;

/*time/date set*/
_LOCAL	RTCtime_typ	RTCSetTime;
_LOCAL	BOOL		SetTimeButton;

_LOCAL	UINT	AbfrageCancelButton,AbfrageOKButton;
_LOCAL	USINT	ExitScreenSaver,ScreenSaverOrgBild;
_LOCAL	STRING	VERSION[5];
_LOCAL	UINT	CounterLineColor1,CounterLineColor2,CounterInputReady;

_LOCAL	UINT	ClearLineColor1,ClearLineColor2,ClearLinePressed;
_GLOBAL USINT	ClearLine;

_LOCAL 	UINT	PanoramaInv,PanoramaButtonColor,PrevPlate,NextPlate,PlateOnScreen,PlateInputOk;
_LOCAL 	UINT	ManualModeInv;
_LOCAL	STRING	PanoramaPlateName[32];
UINT			LastPlateType;
_LOCAL 	STRING	CurrentPlateName[32];
_LOCAL	UINT	AdapterTextColor;
_LOCAL	USINT	LengthInputReady,OffsetInputReady;
_LOCAL	UINT	TrolleyOpenInv;
_GLOBAL	BOOL	ReferenceStart;
/*HA 23.10.03 V1.67 send laserpower to TiffBlaster if a value is entered (even if it didn't change) */
/*therefore SendLP must be GLOBAL*/
_GLOBAL 	BOOL	SendYOffset,SendLP;

/*HA 16.01.04 V1.71 enable/disable deloader functions*/
_LOCAL		UINT	DeloaderEnabledColor,DeloaderDisabledColor,StopPinsEnabledColor,StopPinsInputEnabledColor;
_LOCAL		UINT	DeloaderTurnEnabledColor,DeloaderTurnInputColor;
_LOCAL		UINT	DeloaderEnabledInputColor,DeloaderDisabledInputColor;
_LOCAL		USINT	StopPinsInputEnabled,DeloaderInputEnabled,NoDeloaderInputEnabled,TurnInputEnabled;
_LOCAL		UINT	SeparateBeltTracksEnabledColor,PlateToPaperbasketColor;

/*HA 20.01.04 V1.73 enable/disable flexible feeder functions*/
_LOCAL		UINT	FeederEnabledColor,FeederDisabledColor;
_LOCAL		UINT	FeederEnabledInputColor,FeederDisabledInputColor;
_LOCAL		USINT	FeederInputEnabled,RightStackInputEnabled;

/*HA 22.01.04 V1.74 Make laserpower input blink during setting procedure*/
_LOCAL		UINT	LaserPowerInputColor;
_GLOBAL	BOOL	ConveyorbeltSimulation;

_LOCAL BOOL DelPlateInDropPosition,DelAdjustReady,DelPlateOnConveyorBelt;
_LOCAL BOOL	QuitAllAlarms;
_GLOBAL USINT *TmpPtr;
_GLOBAL BOOL	PutTmpDataToSRAM,GetTmpDataFromSRAM;
BOOL	WaitSRAM;
_LOCAL UINT ScreensaverSec;
_LOCAL UINT	LogoSelection;
_GLOBAL UINT	HideKrause,HideKPG;
_LOCAL UINT	AdressIndex[6];
_LOCAL UINT SaverLogo[16];
UINT SaverStep;
_LOCAL BOOL SaverDisabled;
UINT ColorValueEn,ColorValueDis,ColorValueInEn,ColorValueInDis;

_GLOBAL BOOL GoToMainPic;
_GLOBAL UINT MainPicNumber;
_GLOBAL BOOL MirroredMachine;


/* NEW EASYLOAD*/
_LOCAL	USINT	ParamNext,ParamPrev,InputOK,InputCancel,CurrentPage,CurrentPageOnDispl,
				MaxPages,ChangeUnits;
_LOCAL	USINT UnitMotorLength,UnitMotorSpeed,UnitLength,UnitTemperature;
_LOCAL	UINT	PageInv1,PageInv2,PageInv3,PageInv4,PageInv5,PageInv6;
_LOCAL	USINT MotorOnDisplay,NextMotor;
_LOCAL	USINT	SelMotor1,SelMotor2,SelMotor4,SelMotor5,SelMotor9,SelMotor10;
_LOCAL	USINT	MotActInv,MotErrInv,MotRefInv;
_LOCAL	INT		MotTemp,MaxMotTemp,MotCurr,MaxMotCurr;
_LOCAL	STRING	MotStatus[6];


DINT	ParamOrgBild,OldManSpeed, OldLimitP,OldLimitM;
_LOCAL X_Param_Typ	X_ParamInput,X_Param_Alt;
_GLOBAL	USINT	MotorTemp[MAXMOTORS],MaxMotorTemp[MAXMOTORS];
_GLOBAL	STRING	MotorFaultStatus[MAXMOTORS][5];
_GLOBAL	DINT	MotorCurrent[MAXMOTORS],MaxMotorCurrent[MAXMOTORS];

_LOCAL	STRING	PlateNameList[MAXPLATETYPES][MAXPLATETYPENAMELENGTH];
_LOCAL	INT		SelectedPlateIndex,PlateSelOk;
_LOCAL	Plate_Data_Typ PlateParamInput;
_LOCAL	Trolley_Data_Typ TrolleyParamInput;
_LOCAL	STRING	PlateName[MAXPLATETYPENAMELENGTH];
_LOCAL	INT		ChangeLanguage;

USINT			tmp[32];

/*Für E/A Bild*/
_GLOBAL	UINT	PlateOnConveyorBeltVis,PlateAdjustedVis,TrolleyCodeSensorVis,
				ConveyorBeltSensorVis,EGM1Vis,EGM2Vis,VCPVis,CoverLockVis;
_GLOBAL	UINT	BeltLooseVis;
_GLOBAL	UINT	PaperGripClosedVis;
_GLOBAL	UINT	FeederVacuumVis;
_LOCAL	UINT	LaserPowerAllPlatesEqual;
int OldUnitLength;
_LOCAL  INT     Lock2Stacks;

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/** EGM  ***/
_GLOBAL INT HideSafetyCheck,SafetyCheckOK,SafetyCheckCancel;
_LOCAL BOOL	ZoomPlus,ZoomMinus;
_LOCAL	REAL	ZoomFactor,NormZoomFactor,topval;
BOOL CurvePainted;
int x1,y1,x2,y2;
_GLOBAL BOOL RedrawCurve,PreheatParamEnter,DeveloperTankParamEnter;
_GLOBAL	UINT	SamplingTime,DeveloperSamplingTime;
_GLOBAL	REAL	YMidValue,YMidValueDisplay;
_LOCAL	BOOL	CurveReturn,CurveEnter1,CurveEnter2;
int CurveOrgBild;
_LOCAL	USINT	ButtonActivateControlVoltage;

_LOCAL INT	InVoltageOk;
_LOCAL INT	InSpeedCheck;
_LOCAL INT	InInputSensor;
_LOCAL INT	InPreWashTank;
_LOCAL INT	InRinseTank;
_LOCAL INT	InGumTank;
_LOCAL INT	InDeveloperTank;
_LOCAL INT	InOutputSensor,InPlateSensor,InServiceKey;

_LOCAL INT	OutReady,OutError;
_LOCAL	USINT	ExitScreenSaver,ScreenSaverOrgBild;

_LOCAL	EGMPreheatParam_Type		InputPreHeatParam;
_LOCAL	INT		PreheatParamExit,EGMPreheatParamStyle1,EGMPreheatParamStyle2;

_LOCAL	EGMAutoParam_Type InputAutoParam;

_LOCAL	INT		AutoParamExit;

_LOCAL	EGMDeveloperTankParam_Type InputDeveloperParam;
_LOCAL	INT		DeveloperParamExit;

_LOCAL	EGMPrewashParam_Type	InputPreWashParam;
_LOCAL	EGMRinseParam_Type	InputRinseParam;
_LOCAL	EGMPlate_Type	InputPlateType;

_LOCAL	EGMGlobalParam_Type	EGMInputGlobalParam;
_LOCAL	INT		EGMGlobalParamExit;

_LOCAL	BOOL	CurveReturn,CurveEnter1,CurveEnter2;
int CurveOrgBild;
_LOCAL INT	HideCleaningOverlay,HideLevels,HideAlarmOverlay,HideMessageOverlay,HideLevelsLC,HideXS,HideLC;
UINT DisplaySamplingTime;
_LOCAL	BOOL	PlateInputReady;
_LOCAL UINT	PlateOnDisplay;
_LOCAL BOOL		ToggleLamp;

/***
 ShutDownTime enthält den letzten Zeitstempel vor dem letzten Ausschalten
 kann nur im INIT ausgewertet werden, weil ShutDownTime ja zyklisch beschrieben wird
 OnTime wird im INIT mit der aktuellen Zeit gesetzt, damit kann man aus der
 Differenz zwischen OnTime und ShutDownTime die OffTime ermitteln, d.h. wie lange die Maschine aus war
***/
_GLOBAL	DATE_AND_TIME	OffTime,OnTime;
_LOCAL DTGetTime_typ DTGetTime_1;
_GLOBAL	UDINT ShutDownTime	_VAR_RETAIN;

_LOCAL	BOOL	DeveloperChangeNow,ResetTodaysData;
_LOCAL	UDINT	DaysSinceDeveloperChange;
/* controlling checkboxes and other visibility issues*/
_LOCAL	UINT	ReplenishingPerPlateInv,ReplenishingPerSqmInv,StandbyReplenishingInv,
					OffReplenishingInv,
					PreWashReplPerPlateInv,PreWashReplPerSqmInv,
					PreWashFreshwaterOnlyInv,
					RinseReplPerPlateInv,RinseReplPerSqmInv,RinseFreshwaterOnlyInv,
					TopUpPerPlateInv,TopUpPerSqmInv,
					GumFCInv,DevCircFCInv,PrewashFCInv,RinseFCInv,TopUpFCInv,ReplFCInv,
					TranspDirInv,PlateDirLRInv,PlateDirRLInv,IOConfigWideInv,EnableWasteTankInv,
                    GumInv,GumCheckInv,GumLevelInv,HideMarksXS,HideMarksLC,
                    PROVKitInv,XSInv,LowChemInv;
_LOCAL	BOOL	ToggleReplenishingPerPlate,ToggleReplenishingPerSqm,ToggleStandbyReplenishing,
					ToggleOffReplenishing,
					TogglePreWashReplPerPlate,TogglePreWashReplPerSqm,
					TogglePreWashFreshwaterOnly,
					ToggleRinseReplPerPlate,ToggleRinseReplPerSqm,ToggleRinseFreshwaterOnly,
					ToggleTopUpPerPlate,TogglePROVKit,ToggleMachineType,
					ToggleGumFC,ToggleDevCircFC,TogglePrewashFC,ToggleRinseFC,ToggleTopUpFC,
					ToggleReplFC,ToggleTranspDir,ToggleIOConfig,ToggleEnableWasteTank,ToggleGumSection;
_LOCAL	UINT	StandbyReplenishingEnabledCol,OffReplenishingEnabledCol,
					ReplenishingPerPlateCol,ReplenishingPerSqmCol,
					PreWashReplPerPlateCol,PreWashReplPerSqmCol,
					PreWashValveParCol,
					RinseReplPerPlateCol,RinseReplPerSqmCol,RinseValveParCol,
					TopUpPerPlateCol,TopUpPerSqmCol;
_LOCAL	UINT	AckButtonInv,ScreenSaverSettingsInv,WideInv;

_GLOBAL int	Values[350],DeveloperValues[350],AmbientValues[350];
int *pValueTable;
_LOCAL BOOL	ZoomPlus,ZoomMinus;
_LOCAL	REAL	ZoomFactor,NormZoomFactor,topval;
BOOL CurvePainted;
int x1,y1,x2,y2;
char ctmp[5000];
UDINT 	LastMinute;
_LOCAL	UINT	Status_Attach;
_LOCAL	UINT	AlarmGroupDisplay,AlarmGroupFilter;
_LOCAL	INT		EmergencyStopInv,ServiceKeyInv;
_LOCAL	INT		StartBluefin,StartPerformance;

_GLOBAL	UINT	DeviceType;

_LOCAL	UINT	EGMBackInv,PerfBackInv,ToggleLampInv;
_LOCAL	BOOL	ReturnFromEmpty;
_GLOBAL	UINT	EmptyReturnPic;
_LOCAL	UINT	StartCountdown;

_GLOBAL	UINT	EGMUserLevelIsNotAdmin,EGMUserLevelIsNotService;

_LOCAL	INT		DummyZero; /* needed for visualisation (dummy scale offset var for drehzahl) */
_LOCAL	INT MaxBrushRPM;
INT MaxBrushRPMOld;
_LOCAL	REAL MaxPropulsionSpeed;
REAL MaxPropulsionSpeedOld;
_LOCAL	BOOL	AckDeveloperFlow;
_LOCAL	INT		MessageNo1,MessageNo2,MessageNo3,MessageSymbolNo;
REAL	TopUpPumpMlPerSec_Old,PumpMlPerSec_Old;

/*
 * für Sensor-Kalibrierung
 */
_LOCAL UDINT MlMeasured,CalibMessage,TextSnippetNr,CalibPumpCmd,CalibCounter,CalibOrgMl;
_LOCAL BOOL  CalibRepl,CalibDev,CalibGum,SensorCalibReady,SensorCalibCancel,SensorCalibOKButton;

TouchAction  touch,oldtouch;
int TouchPulseCnt;
TON_10ms_typ ScreensaverTimer;

/* Auto-Abschalten der Pumpen für Bild 145 Testfunktionen */
TON_10ms_typ PrewashPumpTimer,RinsePumpTimer,DeveloperCircPumpTimer,GumPumpTimer;
static	TIME Now;/* for Production stats pic*/
BOOL InitPicReady; /*Hilfsvar: damit NotAus Bild nicht vor Init Bild erscheinen kann*/
_LOCAL  UINT  PaperRemoveUpInv,RefSensorLayerInv,PaletteLoadingInv;

void ParamPicture(void *Input, void *OrgData,int size, int pMaxPages,int StartPage)
{
	int cur_m_start = CurrentPage - StartPage;
	MaxPages = pMaxPages; /* Anzahl der Seiten auf den Bildschirm */

	if( cur_m_start >= 0)
		CurrentPageOnDispl = cur_m_start + 1;
	else
		CurrentPageOnDispl = cur_m_start + pMaxPages + 1;

	if(  xBildInit
	 && ( wBildLast != FILEPIC )
	 && ( wBildLast != MESSAGEPIC )
	 && ( wBildLast != EMPTYPIC )
	 )
	{
		if (wBildLast != CURVEPIC )/*Kurven */
		{
			ParamOrgBild = wBildLast;
			CurrentPage = StartPage;
		}
		ParamNext = 0;
		ParamPrev = 0;
		InputOK = 0;
		InputCancel = 0;
		if (size != 0)
			memcpy(Input,OrgData,size);
	}
	else
	{
		if (ParamNext )
		{
			ParamNext = 0;
			if (CurrentPage < pMaxPages)
				CurrentPage++;
			else
				CurrentPage=1;
		}
		if (ParamPrev )
		{
			ParamPrev = 0;
			if (CurrentPage > 1)
				CurrentPage--;
			else
				CurrentPage=pMaxPages;
		}


/*
 * wenn das Bild über den Haken verlassen wird, Sicherheitsabfrage sichtbar machen
*/
		if (InputOK)
			HideSafetyCheck = 0;

/*
 * Buttons der Sicherheitsabfrage auswerten
*/
		if (SafetyCheckOK)
		{
		/*
		 * SafetyCheckOK wird hier bewußt nicht zurückgesetzt, damit es beim
		 * Aufrufer noch ausgwertet werden kann, es muß dann da zurückgesetzt werden!
		*/
			if (size != 0)
			{
				memcpy(OrgData,Input,size);
				saveParameterData = 1;
			}
			HideSafetyCheck = 1;
			InputOK = 0;
			wBildNeu = ParamOrgBild;
		}

		if (SafetyCheckCancel)
		{
			HideSafetyCheck = 1;
			SafetyCheckCancel = 0;
			SafetyCheckOK = 0;
			InputOK = 0;
			InputCancel = 0;
		}

/*
 * Bild verlassen über escape (Kreuz)
*/
		if	(InputCancel)
		{
			InputOK = 0;
			InputCancel = 0;
			SafetyCheckCancel = 0;
			SafetyCheckOK = 0;
			wBildNeu = ParamOrgBild;
		}

		switch (CurrentPage)
		{
			case 1:
			{
				PageInv1 = 0;
				PageInv2 = 1;
				PageInv3 = 1;
				PageInv4 = 1;
				PageInv5 = 1;
				PageInv6 = 1;
				break;
			}
			case 2:
			{
				PageInv1 = 1;
				PageInv2 = 0;
				PageInv3 = 1;
				PageInv4 = 1;
				PageInv5 = 1;
				PageInv6 = 1;
				break;
			}
			case 3:
			{
				PageInv1 = 1;
				PageInv2 = 1;
				PageInv3 = 0;
				PageInv4 = 1;
				PageInv5 = 1;
				PageInv6 = 1;
				break;
			}
			case 4:
			{
				PageInv1 = 1;
				PageInv2 = 1;
				PageInv3 = 1;
				PageInv4 = 0;
				PageInv5 = 1;
				PageInv6 = 1;
				break;
			}
			case 5:
			{
				PageInv1 = 1;
				PageInv2 = 1;
				PageInv3 = 1;
				PageInv4 = 1;
				PageInv5 = 0;
				PageInv6 = 1;
				break;
			}
			case 6:
			{
				PageInv1 = 1;
				PageInv2 = 1;
				PageInv3 = 1;
				PageInv4 = 1;
				PageInv5 = 1;
				PageInv6 = 0;
				break;
			}
		}
	}
}



void TouchCalib()
{
	if (ready)
	{
		switch (CalibStep)
		{
			case 0:
			{
				if(xBildInit) CalibStep = 1;
				CalibTimer.IN = 0;
				break;
			}
/*wait for timer*/
			case 1:
			{
				if(CalibTimer.Q)
				{
					CalibStep = 2;
					CalibTimer.IN = 0;
					break;
				}
				CalibTimer.IN = 1;
				CalibTimer.PT = 250;
				break;
			}
/*Start calibration*/
			case 2:
			{
                if (!VA_Saccess(1,VC_HANDLE))
                {
					status = VA_StartTouchCal (1,VC_HANDLE);
                    VA_Srelease(1,VC_HANDLE);
				}
				if( !status )
					CalibStep = 3;
				else
				{
					CalibStep = 0;
					wBildNeu = 1;
				}
				break;
			}
/*check, if calibration ready*/
			case 3:
			{
				if (!VA_Saccess(1,VC_HANDLE))
				{
					status = VA_GetCalStatus (1,VC_HANDLE);
					VA_Srelease(1,VC_HANDLE);
				}
/*ready?*/
				if(status==0)
				{
					CalibStep = 0;
#ifdef MODE_BOTH
					wBildNeu = 999;
#else
	#ifdef MODE_PERFORMANCE
					wBildNeu = 1;
	#else
		#ifdef MODE_BLUEFIN
					wBildNeu = 101;
		#endif
	#endif
#endif
				}
				else
				if( status == 65535 ) /*ERROR*/
				{
					CalibStep = 1;/*retry*/
				}
				else
				{
				/*just retry in next cycle*/
				}

				break;
			}
		}
	}
}


void PaintCurve(void)
{
	USINT tmp[20];

				if (ready && !CurvePainted )
				{
					if (!VA_Saccess(1,VC_HANDLE))
			  		{
			  			int j;
						VA_Rect (1,VC_HANDLE,7,29,301,200,15,15);
						for (j=0;j<300;j+=10)
							VA_Line (1,VC_HANDLE,7+j,129,7+j+5,129,0);

						ftoa(YMidValueDisplay,(UDINT)&tmp[0]);
						strcat(tmp," °C");
						VA_Textout (1,VC_HANDLE,4,10,115,0,15,tmp);
/*Zeitachse beschriften*/
						ftoa((DisplaySamplingTime/100.0)*155,(UDINT)&tmp[0]);
						strcat(tmp," s");
						VA_Textout (1,VC_HANDLE,4,150,215,0,15,tmp);
						ftoa((DisplaySamplingTime/100.0)*301,(UDINT)&tmp[0]);
						strcat(tmp," s");
						VA_Textout (1,VC_HANDLE,4,280,215,0,15,tmp);

						x1 = 4;
						y1 = 229;
						x2 = 4;
						y2 = 229;
						NormZoomFactor = 100.0 /(YMidValue);
						topval = YMidValue / ZoomFactor + YMidValue;

						for (j=0;j<301;j++)
						{
							x1 = 7+j;
							x2 = x1+1;
							y1 = y2;
							y2 = 229 - (((*(pValueTable+j))  * ZoomFactor )-
								(YMidValue*ZoomFactor - YMidValue)	)* NormZoomFactor;
							if (y2<29)y2=29;
							if (y2>229)y2=229;
							VA_Line (1,VC_HANDLE,x1,y1,x2,y2,0);
						}
						CurvePainted = 1;
						VA_Srelease(1,VC_HANDLE);
					}
				} /*if ready*/
}


void MainPic(void)
{

/*HA 22.01.04 V1.74 Make laserpower input blink during setting procedure*/
	if (SendLP)
	{
		if(Clock_1s)
			LaserPowerInputColor = DUNKELGRAU;
		else
			LaserPowerInputColor = GRAU;
	}
	else
		LaserPowerInputColor = GRAU;

/*HA 25.06.03 V1.10 make sure to save the Laserpower, when it's entered*/
	if( LaserPowerChanged )
	{
		LaserPowerChanged = 0;
		saveParameterData = 1;
		GlobalParameter.LaserPower = (GlobalParameter.RealLaserPower+5) / 10;
/*HA 23.10.03 V1.67 send laserpower to TiffBlaster if a value is entered (even if it didn't change) */
		if(TCPConnected)
			SendLP = 1;
	}


	if( PlateType == 0 || PlateType>=MAXPLATETYPES)
	{
		PlateType = 1;
		strcpy(PanoramaPlateName,PlateTypes[PlateType].Name);
	}


	if(ManualMode)
	{
		if(PrevPlate)
		{
			PrevPlate = 0;
			if(PlateType>0)
				PlateType--;
			if(PlateType<=0)
				PlateType = MAXPLATETYPES-1;
		}

		if(NextPlate)
		{
			NextPlate = 0;
			PlateType++;
			if(PlateType>=MAXPLATETYPES)
				PlateType = 1;
		}


		if(PlateType >0 && PlateType <= MAXPLATETYPES)
			strcpy(CurrentPlateName,PlateTypes[PlateType].Name);
		else
			strcpy(CurrentPlateName,"----");
	}

/*****************************************************/
/* PanoramaAdapter is obsolete !*/
/*****************************************************/
/*
	if(PanoramaAdapter)
	{
		if(PrevPlate)
		{
			PrevPlate = 0;
			if(PlateType>0)
				PlateType--;
			if(PlateType<=0)
				PlateType = MAXPLATETYPES-1;
		}

		if(NextPlate)
		{
			NextPlate = 0;
			PlateType++;
			if(PlateType>=MAXPLATETYPES)
				PlateType = 1;
		}


		if(PlateType >0 && PlateType <= MAXPLATETYPES)
			strcpy(CurrentPlateName,PlateTypes[PlateType].Name);
		else
			strcpy(CurrentPlateName,"----");
	}
	else
*/
/*	{*/
		AlarmBitField[25] = 0;
		if(GlobalParameter.TrolleyLeft != 0)
			strcpy(CurrentPlateName,PlateTypes[Trolleys[GlobalParameter.TrolleyLeft].PlateType].Name);
		else
			if(GlobalParameter.TrolleyRight != 0)
				strcpy(CurrentPlateName,PlateTypes[Trolleys[GlobalParameter.TrolleyRight].PlateType].Name);
			else
				strcpy(CurrentPlateName,"----");
/*	}*/

	if(CounterInputReady)
	{
		CounterInputReady = 0;
		if(PlatesToDo > 0)
			CounterOn = 1;
	}



	if(CounterOn)
	{
		CounterLineColor1 = SCHWARZ;
		CounterLineColor2 = WEISS;
	}
	else
	{
		CounterLineColor2 = SCHWARZ;
		CounterLineColor1 = WEISS;
	}



	if(ClearLine)
	{
		if( !PlateAtFeeder.present && !AdjustReady && !ExposeStart)
			ClearLine = 0;
		else
		{
			if( !START )
				AUTO = 1;
		}
	}

/*colors and bitmaps*/

	if (HideKPG) /*Krause*/
	{
		KPGClosedInvisible = 1;
		KPGOpenInvisible = 1;

		if(RedLight)
		{
			JetRedInvisible = 0;
			JetOpenInvisible = 1;
			JetClosedInvisible = 1;
			Trolley1Invisible = 1;
			Trolley2Invisible = 1;
			TrolleyBigInvisible = 1;
			StartRedInvisible = 0;
			StartGreenInvisible = 1;
			JetOpen = 0;
			RedIconsInvisible = 0;
			DefaultIconsInvisible = 1;
			TrolleyOpenInv = 1;
		}
		else
		{
			RedIconsInvisible = 1;
			DefaultIconsInvisible = 0;
			JetRedInvisible = 1;
			StartRedInvisible = 1;
			StartGreenInvisible = 0;
		}

		if(GlobalParameter.TrolleyLeft != 0)
		{
			if( Trolleys[GlobalParameter.TrolleyLeft].Single)
			{
				Trolley1Invisible = 0;
				TrolleyBigInvisible = 1;
				Plates1 = Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft;
				TrolleyNr1 = GlobalParameter.TrolleyLeft;
			}
			else /*Doublesize trolley*/
			{
				Trolley1Invisible = 1;
				TrolleyBigInvisible = 0;
				Plates1 = Trolleys[GlobalParameter.TrolleyLeft].PlatesLeft;
				if(Trolleys[GlobalParameter.TrolleyLeft].RightStack)
				{
					PlateStack2Invisible = 0;
					Plates2 = Trolleys[GlobalParameter.TrolleyLeft].PlatesRight;
				}
				else
				{
					PlateStack2Invisible = 1;
				}

				TrolleyNr1 = GlobalParameter.TrolleyLeft;
			}
		}
		else
		{
			Trolley1Invisible = 1;
			TrolleyBigInvisible = 1;
		}

		if( GlobalParameter.TrolleyLeft != 0 || GlobalParameter.TrolleyRight != 0 )
		{
			TrolleyOpenInv = !TrolleyOpen;
		}
		else
			TrolleyOpenInv = 1;

		if(GlobalParameter.TrolleyRight != 0)
		{
			Trolley2Invisible = 0;
			Plates2 = Trolleys[GlobalParameter.TrolleyRight].PlatesLeft;
			TrolleyNr2 = GlobalParameter.TrolleyRight;
			PlateStack2Invisible = 0;
		}
		else
		{
			Trolley2Invisible = 1;
/*no trolley at all*/
			if(GlobalParameter.TrolleyLeft == 0 )
				PlateStack2Invisible = 1;
			else
/*left trolley only one stack*/
				if(!Trolleys[GlobalParameter.TrolleyLeft].RightStack)
					PlateStack2Invisible = 1;

		}
	}
	else /*KPG*/
	{
		if(TrolleyKey)
		{
			JetOpen = 1;
		}
		TrolleyKey = 0;
		if (JetOpen)
		{
			KPGClosedInvisible = 1;
			KPGOpenInvisible = 0;
		}
		else
		{
			KPGClosedInvisible = 0;
			KPGOpenInvisible = 1;
		}
		JetRedInvisible = 1;
		JetOpenInvisible = 1;
		JetClosedInvisible = 1;
		Trolley1Invisible = 1;
		Trolley2Invisible = 1;
		TrolleyBigInvisible = 1;
		PlateStack2Invisible = 1;

		if(RedLight)
		{
			StartRedInvisible = 0;
			StartGreenInvisible = 1;
			RedIconsInvisible = 0;
			DefaultIconsInvisible = 1;
		}
		else
		{
			RedIconsInvisible = 1;
			DefaultIconsInvisible = 0;
			StartRedInvisible = 1;
			StartGreenInvisible = 0;
		}

	}

/* active/standby/error LEDs*/
	if(BUSY)
	{
			ActiveColor = SCHWARZ;
	}
	else
	{
			ActiveColor = WEISS;
	}

	if(STANDBY)
	{
			StandbyColor = SCHWARZ;
	}
	else
	{
			StandbyColor = WEISS;
	}

	if(ERROR)
	{
			if(NEWERROR) /*New eror: blink*/
			{
				if(Clock_1s)
					ErrorColor = SCHWARZ;
				else
					ErrorColor = WEISS;
			}
			else
				ErrorColor = SCHWARZ;
	}
	else /*No error*/
	{
			ErrorColor = WEISS;
	}

/*Start/Stop Button*/
	if(RedLight)
	{
		StartButtonColorPressed = DUNKELROT;
		StartButtonColorReleased = ROT;
	}
	else
	{
		StartButtonColorPressed = ROT;
		StartButtonColorReleased = GRUEN;
	}
}

void ClearList(void)
{
	int i;
	for(i=0;i<32;i++)
	{
		strcpy(FileListName[i]," ");
		strcpy(FileListStatus[i]," ");
		strcpy(FileListTime[i]," ");
		strcpy(FileListResolution[i]," ");
	}
}



void SetTurnButtons(void)
{
				switch (PlateTypes[0].TurnMode)
				{
					case 0:
					{
						Turn90 = 0;
						Turn180 = 0;
						Turn90Old = 0;
						Turn180Old = 0;
						break;
					}
					case 1:
					{
						Turn90 = 1;
						Turn180 = 0;
						Turn90Old = 1;
						Turn180Old = 0;
						break;
					}
					case 2:
					{
						Turn90 = 0;
						Turn180 = 1;
						Turn90Old = 0;
						Turn180Old = 1;
						break;
					}
					default:
					{
						Turn90 = 0;
						Turn180 = 0;
						Turn90Old = 0;
						Turn180Old = 0;
						PlateTypes[0].TurnMode = 0;
						break;
					}
				}

}




void SwitchStopPinsEnabled(BOOL Enable)
{
				if (Enable)
				{
					StopPinsEnabledColor = ColorValueEn;
					StopPinsInputEnabledColor = ColorValueInEn;
					if (UserLevel >= 2)
						StopPinsInputEnabled = 1;
					else
						StopPinsInputEnabled = 0;
				}
				else
				{
					StopPinsEnabledColor = ColorValueDis;
					StopPinsInputEnabledColor = ColorValueInDis;
					StopPinsInputEnabled = 0;
				}
}


BOOL AlarmMessage(INT AlarmNo, INT Message1, INT Message2, INT Message3,
					INT Symbol,BOOL EnableAck)
{
	BOOL retval;

	if(EnableAck)
		AckButtonInv = FALSE;
	else
		AckButtonInv = TRUE;

	if( EGM_AlarmBitField[AlarmNo] == TRUE )
	{
		MessageNo1 = Message1;
		MessageNo2 = Message2;
		MessageNo3 = Message3;
		MessageSymbolNo = Symbol;
		HideMessageOverlay = FALSE;
		HideAlarmOverlay = TRUE;
		HideCleaningOverlay = TRUE;
		HideLevels = TRUE;
		HideLevelsLC = TRUE;
		if( AckDeveloperFlow )
		{
			AckDeveloperFlow = FALSE;
			if(EnableAck)
				EGM_AlarmBitField[AlarmNo] = FALSE;
		}
		retval = TRUE;
	}
	else
		retval = FALSE;

	return retval;

}


void InitPic()
{
	if(xBildInit)
	{
		InitTouch = 0;
		InitTouchOld = 0;
		StartUpTimer.IN = 0;
		InitTouchTimer.IN = 0;
		return;
	}
	StartCountdown = (StartUpTimer.PT - StartUpTimer.ET) / 100 +1;
	InitTouchTimer.IN = InitTouch;
/*	if(InitTouchTimer.Q)*/
	if(InitTouch)
	{
		wBildNeu = 102;
		InitTouch = 0;
		InitTouchOld = 0;
		StartUpTimer.IN = 0;
		return;
	}
	if(/* (!InitTouch && InitTouchOld) || */StartUpTimer.Q)
	{
#ifdef MODE_BOTH
					wBildNeu = 999;
#else
	#ifdef MODE_PERFORMANCE
					wBildNeu = 26;
	#else
		#ifdef MODE_BLUEFIN
					if(	(ModulStatus_PS  != 1)
					||  (ModulStatus_DI  != 1)
					||  (ModulStatus_DO1 != 1)
					||  (ModulStatus_DO2 != 1)
					||  (ModulStatus_DO3 != 1)
					||  (ModulStatus_AT  != 1)
					||  (ModulStatus_AO  != 1)
					)
					{
/*
 * Fkt. ShowMessage geht hier nicht, da wir das Rücksprungbild festlegen wollen auf 101
*/
						if(wBildAktuell != MESSAGEPIC )
							OrgBild = 101;
						wBildNeu = MESSAGEPIC;
						AbfrageOK = 0;
						AbfrageCancel = 0;
						IgnoreButtons = 1;
						OK_CancelButtonInv = 1;
						OK_ButtonInv = 0;
						AbfrageText1 = 105;
						AbfrageText2 = 106;
						AbfrageText3 = 107;
						AbfrageIcon = CAUTION;
					}
					else
						wBildNeu = 101;
		#endif
	#endif
#endif
		InitTouch = 0;
		InitTouchOld = 0;
		StartUpTimer.IN = 0;
		return;
	}
/*	StartUpTimer.IN = !InitTouch;*/
	StartUpTimer.IN = 1;
	InitTouchOld = InitTouch;
	return;
}


void HandleTopUpSettings(BOOL PageInvFlag)
{
			if (InputDeveloperParam.TopUpMode == REPLPERPLATE)
			{
				TopUpPerPlateCol = ENABLEDCOLOR;
				TopUpPerSqmCol = DISABLEDCOLOR;
			}
			else
			{
				TopUpPerPlateCol = DISABLEDCOLOR;
				TopUpPerSqmCol = ENABLEDCOLOR;
			}
			TopUpPerPlateInv = PageInvFlag || (InputDeveloperParam.TopUpMode != REPLPERPLATE);
			TopUpPerSqmInv = PageInvFlag || (InputDeveloperParam.TopUpMode != REPLPERSQM);
			TopUpFCInv = EGMGlobalParam.UseTopUpFlowControl;
			if(ToggleTopUpPerPlate)
			{
				ToggleTopUpPerPlate = 0;
				if (InputDeveloperParam.TopUpMode == REPLPERPLATE)
					InputDeveloperParam.TopUpMode = REPLPERSQM;
				else
					InputDeveloperParam.TopUpMode = REPLPERPLATE;
			}

/* wenn sich TopUpPumpMlPerSec ändert (aus der Developertank task, weil
 * durch flow control ein neuer Wert ermittelt wurde), dann muss der Wert
 * auf dem Schirm angepasst werden
 */
			if(EGMDeveloperTankParam.TopUpPumpMlPerSec != TopUpPumpMlPerSec_Old)
			{
				TopUpPumpMlPerSec_Old = EGMDeveloperTankParam.TopUpPumpMlPerSec;
				InputDeveloperParam.TopUpPumpMlPerSec = EGMDeveloperTankParam.TopUpPumpMlPerSec;
			}
}

void HandleReplenishingSettings(BOOL PageInvFlag)
{
			if (InputDeveloperParam.EnableStandbyReplenishing)
				StandbyReplenishingEnabledCol = ENABLEDCOLOR;
			else
				StandbyReplenishingEnabledCol = DISABLEDCOLOR;

			if (InputDeveloperParam.EnableOffReplenishing)
				OffReplenishingEnabledCol = ENABLEDCOLOR;
			else
				OffReplenishingEnabledCol = DISABLEDCOLOR;

			if (InputDeveloperParam.ReplenishingMode == REPLPERPLATE)
			{
				ReplenishingPerPlateCol = ENABLEDCOLOR;
				ReplenishingPerSqmCol = DISABLEDCOLOR;
			}
			else
			{
				ReplenishingPerPlateCol = DISABLEDCOLOR;
				ReplenishingPerSqmCol = ENABLEDCOLOR;
			}


			ReplenishingPerPlateInv = PageInvFlag || (InputDeveloperParam.ReplenishingMode != REPLPERPLATE);
			ReplenishingPerSqmInv = PageInvFlag || (InputDeveloperParam.ReplenishingMode != REPLPERSQM);
			StandbyReplenishingInv = PageInvFlag ||  (!InputDeveloperParam.EnableStandbyReplenishing);
			OffReplenishingInv = PageInvFlag ||  (!InputDeveloperParam.EnableOffReplenishing);
			/*
			 * diese Flags funktionieren genau andersherum als die anderen, weil
			 * damit die entsprechenden Teile abgedeckt werden
			 */
			ReplFCInv = EGMGlobalParam.UseReplFlowControl;

			if (ToggleStandbyReplenishing)
			{
				ToggleStandbyReplenishing = 0;
				InputDeveloperParam.EnableStandbyReplenishing  = !InputDeveloperParam.EnableStandbyReplenishing;
			}
			if (ToggleOffReplenishing)
			{
				ToggleOffReplenishing = 0;
				InputDeveloperParam.EnableOffReplenishing  = !InputDeveloperParam.EnableOffReplenishing;
			}

			if(ToggleReplenishingPerPlate)
			{
				ToggleReplenishingPerPlate = 0;
				if (InputDeveloperParam.ReplenishingMode == REPLPERPLATE)
					InputDeveloperParam.ReplenishingMode = REPLPERSQM;
				else
					InputDeveloperParam.ReplenishingMode = REPLPERPLATE;
			}

/* wenn sich PumpMlPerSec ändert (aus der Developertank task, weil
 * durch flow control ein neuer Wert ermittelt wurde), dann muss der Wert
 * auf dem Schirm angepasst werden
 */
			if(EGMDeveloperTankParam.PumpMlPerSec != PumpMlPerSec_Old)
			{
				PumpMlPerSec_Old = EGMDeveloperTankParam.PumpMlPerSec;
				InputDeveloperParam.PumpMlPerSec = EGMDeveloperTankParam.PumpMlPerSec;
			}

}

_INIT void Init(void)
{
/*

 VERSION INFO
 Version history:
  1.00 first version  based on Performance V2.64
  1.01 - default parameters adjusted
       - removed LSJAPAN define, PerfoXXL always has 2 Paper removal vertical motors
  1.02 - no blowair during deloading of broadsheet
  1.03 - removed blowair change from 1.02
  	   - if adjusting error occurs table blowair and adjuster vacuum are now switched off
  	   - "waiting for data" is displayed instead of "Warte auf Daten": Language numbers
  	     were mixed up
  	   - adjusting is now monitored correctly for both plates, adjust error displays, which
  	     plate caused the failure (if both in use)
  1.04 - deloading can now be started if ExposeStart is false even if AUTO is still true
  	   - safety: referencing table is now done after feeder vertical ref is ready
  	   - in final step of ref seq beamOFF is set, to make sure, that the beam
  	     is definitely off after restart of machine
  	   - laserpower table: min value for resolution changed from 300 to 0
  	   - Feeder down waits now, until paper remove is in papre release position, if paper remove
  	     is running and at least one paper grip is closed
	   - enabled Emergency stop handling
  1.05 - changed time for lifting pins up from 0,1 to 0,2 s
  	   - if pins were down longer than 2 min, the time is set to 1 sec
  1.06 - set default plate config to BOTH
  	   - if pins were down longer than 2 min, the time is set to 5 sec
  1.07 - implemented Paper detection
  	   - STANDBY jetzt nur noch, wenn X-Achse Ref ok, activ, auto und Ablauf meldet
  1.08 - implemented semiauto exposure for double broadsheet
  	   - Faulhaber Comm: answer on get current cmd is now also evaluated for
  	     correct characters
  	   - removed Variables PlateTypeToDo and NumberOfPlatesToDo, were not used;
  	     set PlateToDo.PlateType with preselected plate type if Tiffblaster Simulation is active
  	   - default plateconfig BOTH from 1.06 did not work -> fixed
  1.09 - state change from busy to standby now only, if no plates on table and conveyor belt
       - the same applies to coverlock release
       - Plate transport simulation did not work due to the fact that the paper remove
         does not remove the paper and the feeder paper detect sensors detect paper
         and give a paper detected error
         -> fixed, paper sensors are now ignored in that case
  1.10 - bugfix: in udp_nl ManualMode was not checked, so jobs were not taken
         because the platetype in trolley was used instead of manual platetype
       - station PlateToDo was not cleared after manual loading
  1.11 - new parameters for deloading: table speed during deloading for automatic and
         for manual mode independently
  1.12 - Änderungen gegen "Duplicate filename" aus Perfo 2.70 nachgezogen
         (StopCheckingForData in belichten.c)
	   - Übergabe der Quadratmeter und Plattenanzahl (1 oder 2) an Bluefin via
	     CANbus (ima Comm in ifbluefin)-> Datenobjekt do_vcp erweitert
	     und in entladen.c die Variablen befüllt

Lücke für evtl Änderungen an bestehenden Maschinen in USA

  1.20
       - Krupp Druck Version: kein UL, Hauben- und Klappenschalter direkt verdrahtet
         (wie im standard Perfo)
         -> Hauben- und Türschalter ans Ende der Eingänge
         -> Türschalter auf E/A Bild nachgerüstet
         -> Notaus-Bild wird nicht mehr angesprungen
       - Vakuum Systeme in auxfunc.c angepasst: nur für PANORAMA config
         wird zusätzlich der PlateType ausgewertet und die entsprechenden
         Systeme angesteuert:
			Plattentyp	Feeder	Ausrichttisch
				1		1+2		1
				2		1+3		1+2

       - Entlade Ablauf: Platte wird jetzt 250 statt 50mm vom Tisch gezogen mit 3000 rpm
       - Papiergreifer Ausgang negiert, ist jetzt wie beim Standard Perfo
         ->Warum war es bei den US Maschinen anders?
  1.21
       - Handanlage jetzt ohne jede Abfrage
       - Papiergreifer waren bei Durchlaufsimulation immer fest auf 0,
         jetzt wie beim Standard-Performance und Jet2 unabhängig von Simulationsmodus
       - Handanlage nur im Admin Mode verfügbar
  1.22
       - Anbindung an Bluefin: Sqm und plate counter Übertragung aus USA Version rausgenommen
         damit Anbindung an Standard Bluefin Version funktioniert
       - Entladeablauf:Plattenweg nochmal verlängert auf 350mm
  1.23
       - AdjustStart wird jetzt in Plattenaufnahme Schritt 70 unbedingt gestartet (wenn AUTO),
         war vorher verriegelt mit !ExposeStart und !DeloadStart
         Diese Abfrage ist jetzt im null-Schritt des Ausrichtens untergebracht

         Da Aufnahme Schritt 70 nur einen Zyklus lang ansteht konnte es sonst passieren, dass
         das Ausrichten gar nicht getriggert wurde
	   - Fehler noch nicht behoben: Sicherheitsabfrage im null-Schritt des Ausrichtens entfernt
  1.24
       - beim Entladen wird jetzt erst Blasluft eingeschaltet und nach 1/2 Sekunde dann der Tisch gefahren

  1.25 151113 Maschine 29110 Manila Bulletin
       - Feeder Saugsysteme sind bei diesem XXL anders, entsprechend angepasst: bei Pano, Plattentyp 2 werden
         Systeme 2 und 4 zusätzlich geschaltet, also alle 4
  1.26 151216 Maschine 29110 Manila Bulletin
       - kompiliert mit integrierter Bf Steuerung
       - Ausrichten: wenn PlateAtFeeder.present nicht gesetzt ist (zB Platte händisch auf den Tisch gelegt),
         dann wird PlateAtFeeder.PlateConfig nicht verwendet, um das Ausrichten zu prüfen, sondern nur fix LEFT
         gecheckt
  1.27 160112 Maschine 29110 Manila Bulletin
       - diese Maschine kann nur einbahnig (plateconfig ist immer LEFT)
         -> dadurch ging prefetch nicht, das war nur für BOTH vorgesehen
         -> entsprechende Abfrage in plattenaufnahme.c auskommentiert
		 !! Bei Maschinen mit zweibahniger belichtung wieder einkommentieren !!

*************************************************************************

  1.40 160122 upgrade auf AS4.1 und AR A3.10
		- BatteryStatus hier einmal auf 0 gesetzt, damit es nicht wegoptimiert wird
		- Mail Funktion entfernt und GlobalParam SMTP Variablen in Reserve umbenannt 
		- Palettenloading Vorbereitung
			-> Eingang f Papierentf oben?
			-> Eingangsvariable In_PaperRemoveUp angelegt
			-> Trolley member Panorama umbenannt in NoCover
			-> GlobalData erweitert um EnablePaletteLoading (statt reserve5)
			-> diese Option auf global param Bild aktivierbar gemacht
			-> Motor 5 (PAPERREMOVE_VERTICAL) Referenzfahrt mit Sensor für "oben" (wenn EnablePaletteLoading)
			-> Variablen angelegt in PaperRemoveParam Struktur
			-> Papier Entf. Parameter erweitern um Eintauchtiefe bei Papierablage 
			-> Fehlermeldungstexte angelegt
			-> Trolley Bild um Checkbox NoCover erweitert
			-> Eingang auf Motorbild visualisiert
			-> Trolley Erkennung erweitert um Palettenerkennung
			-> Abläufe Trolley auf/zu angepasst

		- bugfix: in AutoParam für Bluefin waren die Häkchen für Replenishing nicht richtig gesteuert	
		- Nach Trolley schließen wird die Pap.Entf. hor. jetzt nur noch 100 Inkr zurückgefahren zum Entlasten 
	      (vorher fuhr sie auf OpenStartPos)

*/
	strcpy(VERSION,"1.40");

/* damit in der Versionsnummer deutlich wird, ob es standard oder "wide 1250" ist */
#ifdef MODE_BLUEFIN
	if(IOConfiguration == 2)
		strcat(VERSION,"W"); /* w for wide */
#endif

	strcpy(gVERSION,VERSION);
	MainPicNumber = 1;
	BatteryStatus = 0;
	InitPicReady = 0;
	GoToMainPic = 0;
	saveall = 0;
	loadall = 0;
	WaitSRAM = 0;
	IOActive = 0;
	QuitAllAlarms = 0;
	PlateTransportSim = 0;
	DelPlateInDropPosition=0;
	DelAdjustReady=0;
	DelPlateOnConveyorBelt=0;
	ConveyorbeltSimulation = 0;
	CounterInputReady = 0;
	ExitScreenSaver = 0;
	ClearList();
	BlinkTimer1.PT = 100;
	BlinkTimer2.PT = 100;
	PulseTimer1.PT = 100;
	InitTouchTimer.PT = 200;
	StartUpTimer.PT = 300;
	PlateType = 0;
	ClearLine = 0;
	LaserPowerChanged = 0;
	SeparateBeltTracksEnabledColor = CL_DISABLED;
	PlateToPaperbasketColor = CL_DISABLED;
	for (i=0;i<16;i++)
		SaverLogo[i] = 1;
	SaverStep = 0;
	SaverDisabled = 0;
	UnitTemperature = 1;
	HideSafetyCheck = 1;
	SafetyCheckOK = 0;
	EGMGlobalParam.Unit_Rpm	= 1;
	EGMGlobalParam.Unit_Propulsion = 1;
	DTGetTime_1.enable = 1;
	DTGetTime(&DTGetTime_1);
	OnTime = DTGetTime_1.DT1;
	if (OnTime > ShutDownTime) /*sollte der Fall sein, außer bei erst IBN*/
		OffTime = OnTime - ShutDownTime;
	else
		OffTime = 0;

	LastMinute = 0;
	DummyZero = 0;
}


_CYCLIC void Cyclic(void)
{

	if( (EGMGlobalParam.Unit_Propulsion == 0) || (EGMGlobalParam.Unit_Propulsion > 2) )
		EGMGlobalParam.Unit_Propulsion = 1;

	PulseTimer1.IN = 1;
	if(PulseTimer1.Q)
		PulseTimer1.IN = 0;
	TON_10ms(&PulseTimer1);
	Pulse_1s = PulseTimer1.Q;

#ifdef MODE_BLUEFIN
	ScreensaverTimer.PT = EGMGlobalParam.ScreensaverTime * 100;
	TON_10ms(&ScreensaverTimer);

	if( (ScreensaverTimer.Q)
	 && (wBildAktuell != FILEPIC)
	 && (wBildAktuell != MESSAGEPIC)
	 && (wBildAktuell != EMPTYPIC
	 && (CalibStep == 0))
	 )
	{
		wBildNeu = EMPTYPIC;
		ScreensaverTimer.IN = 0;
	}

	if (ready && Pulse_1s && (EGMGlobalParam.ScreensaverTime > 0))
	{
		TouchPulseCnt++;
		if(TouchPulseCnt >= 2)
		{
			TouchPulseCnt = 0;
			if (!VA_Saccess(1,VC_HANDLE))
	  		{
				ScreensaverTimer.IN = 1;
				VA_GetTouchAction (1,VC_HANDLE, 1, &touch);
				VA_Srelease(1,VC_HANDLE);
				if(	(touch.status == 1 && oldtouch.status != 1)
				 || (touch.x != oldtouch.x)
				 || (touch.y != oldtouch.y)
				 )
					ScreensaverTimer.IN = 0;

				oldtouch = touch;
			}
		}
	} /*if ready*/
	ScreenSaverSettingsInv = 0;
#else
	ScreenSaverSettingsInv = 1;
	EGMGlobalParam.ScreensaverTime = 0;
#endif

#ifdef MODE_BOTH
	EGMBackInv = 0;
	PerfBackInv = 0;
	ToggleLampInv = 1;
#else
	EGMBackInv = 1;
	PerfBackInv = 1;
	ToggleLampInv = 0;
#endif

	if (IOConfiguration == 2)
		WideInv = 0;
	else
		WideInv = 1;

/*einmal pro minute ShutDownTime beschreiben*/
	if (RTCtime.minute != LastMinute)
	{
		DTGetTime_1.enable = 1;
		DTGetTime(&DTGetTime_1);
		ShutDownTime = DTGetTime_1.DT1;
		LastMinute = RTCtime.minute;
	}

/*Bildwechsel erfassen*/
	if (wBildAktuell != wBildAlt)
	{
		xBildExit = TRUE;
		wBildLast = wBildAlt;
		wBildAlt = wBildAktuell;
	}
	else
	if (wBildAktuell != wBildNr)
	{
		xBildExit = FALSE;
		xBildInit = TRUE;
		wBildLast = wBildNr;
		wBildNr = wBildAktuell;
	}
	else
		xBildInit = FALSE;

/* Userlevel Variablen für Sichtbarkeitssteuerung setzen */
	EGMUserLevelIsNotAdmin = (UserLevel < 2);
	EGMUserLevelIsNotService = (UserLevel < 1);
/* Einheit der Bürstendrehzahl muß RPM sein, also hier zyklisch festfeschrieben */
	if(EGMGlobalParam.Unit_Rpm	!= 1)
		EGMGlobalParam.Unit_Rpm	= 1;


/*Anzahl gemachter Platten zu groß? -> auf 0 setzen*/
	if (PlatesMade>99999999) PlatesMade=0;

/*safety*/
	if (GlobalParameter.DeloaderTurnStation>2)
	{
		if (GlobalParameter.Dummy)
			GlobalParameter.DeloaderTurnStation = 2;
		else
			GlobalParameter.DeloaderTurnStation = 0;
	}

 	if (!ready)
    {
        VC_HANDLE = VA_Setup(1 , "visual");
        if (VC_HANDLE)
    	        ready = 1;
		CalibStep = 0;
    }

/* HA 16.01.04 V1.71 colors for switchable devices enable/disable*/
/*Deloader*/
	if(RedLight)
	{
		ColorValueEn = CL_ENABLED_RED;
		ColorValueDis = CL_DISABLED_RED;
		ColorValueInEn = CL_INPUTENABLED_RED;
		ColorValueInDis = CL_INPUTDISABLED_RED;
	}
	else
	{
		ColorValueEn = CL_ENABLED;
		ColorValueDis = CL_DISABLED;
		ColorValueInEn = CL_INPUTENABLED;
		ColorValueInDis = CL_INPUTDISABLED;
	}

	if (GlobalParameter.DeloaderTurnStation)
	{
		DeloaderEnabledColor = ColorValueEn;
		DeloaderDisabledColor = ColorValueDis;
		DeloaderEnabledInputColor = ColorValueInEn;
		DeloaderDisabledInputColor = ColorValueInDis;
		if (UserLevel >= 2)
		{
			DeloaderInputEnabled = 1;
			NoDeloaderInputEnabled = 0;
		}
		else
		{
			DeloaderInputEnabled = 0;
			NoDeloaderInputEnabled = 0;
		}
	}
	else
	{
		DeloaderEnabledColor = ColorValueDis;
		DeloaderDisabledColor = ColorValueEn;
		DeloaderEnabledInputColor = ColorValueInDis;
		DeloaderDisabledInputColor = ColorValueInEn;
		if (UserLevel >= 2)
		{
			DeloaderInputEnabled = 0;
			NoDeloaderInputEnabled = 1;
		}
		else
		{
			DeloaderInputEnabled = 0;
			NoDeloaderInputEnabled = 0;
		}
	}

	if (GlobalParameter.DeloaderTurnStation==2)
	{
		DeloaderTurnEnabledColor = ColorValueEn;
		DeloaderTurnInputColor = ColorValueInEn;
		TurnInputEnabled = 1;
	}
	else
	{
		DeloaderTurnEnabledColor = ColorValueDis;
		DeloaderTurnInputColor = ColorValueInDis;
		TurnInputEnabled = 0;
	}

/*flexible feeder */
	if (GlobalParameter.FlexibleFeeder)
	{
		FeederEnabledColor = ColorValueEn;
		FeederEnabledInputColor = ColorValueInEn;
		if (UserLevel >= 2)
			FeederInputEnabled = 1;
		else
			FeederInputEnabled = 0;
	}
	else
	{
		GlobalParameter.AlternatingTrolleys = 0;
		FeederEnabledColor = ColorValueDis;
		FeederEnabledInputColor = ColorValueInDis;
		FeederInputEnabled = 0;
		for(i=0;i<MAXTROLLEYS;i++)
		{
			Trolleys[i].Double 	= 1;
			Trolleys[i].RightStack = 0;
		}

	}
/*1.93 Farben/Sichtbarkeit für 2. Auslaufsensor*/
	BeltSensor2Invisible = !DeloaderParameter.SeparateSensors;
	if (DeloaderParameter.SeparateSensors && GlobalParameter.DeloaderTurnStation)
		SeparateBeltTracksEnabledColor = ColorValueEn;
	else
		SeparateBeltTracksEnabledColor = ColorValueDis;

/* Blinker*/
	BlinkTimer1.IN = ! BlinkTimer2.Q;
	TP_10ms(&BlinkTimer1);
	BlinkTimer2.IN = ! BlinkTimer1.Q;
	TP_10ms(&BlinkTimer2);
	Clock_1s = BlinkTimer1.Q;


/*check for BUSY/ERROR/STANDBY*/
	BUSY = PaperRemoveStart
		||	PlateTakingStart
		||	AdjustStart
		||	DeloadStart
		||	ExposeStart
		|| 	OpenTrolleyStart
		|| 	CloseTrolleyStart
		||	START
		||  PlateOnConveyorBelt.present
		||  PlateAtAdjustedPosition.present
		||  PlateAtDeloader.present
		||  ManualLoadingStart;

	flag1 = 0;
	flag2 = 0;
	for(i=1;i<=MotorsConnected;i++)
	{
		if(Motors[i].Error)
			flag1 = 1;
		AlarmBitField[i] = Motors[i].Error;

		if(i != 6 && i != 9 && i != 3)
			if( !Motors[i].ReferenceOk) flag2 = 1;
	}
	AlarmBitField[10] = FU.CANError;
	AlarmBitField[11] = FU.Error;
	AlarmBitField[12] = !TCPConnected && !GlobalParameter.TBSimulation;

	if(flag1 ||  (FU.Error || FU.CANError)
		|| AlarmBitField[22]
		|| AlarmBitField[23]
		|| AlarmBitField[26]
		|| AlarmBitField[28]
		|| AlarmBitField[29]
		|| AlarmBitField[30]
/*HA 25.08.04 V1.83 new ultrasonic Paper detection sensor at shuttle */
/*HA 19.11.04 V1.93 merged 1.83 and 1.92 */
		|| AlarmBitField[33] )
		ERROR = 1;
	else
		ERROR = 0;

/*check for new errors*/
	flag1 = 0;
	for(i=0;i<MAXALARMS;i++)
	{
		if (i != 34) /*34:Papier erkannt (nur protokollierung, kein Alarm)*/
		{
			if(AlarmBitField[i] && AlarmQuitBitField[i])
				flag1 = 1;
		}
	}
	NEWERROR = flag1;

	if( !BUSY && !ERROR &&	!flag2 && FU.RefOk && FU.Auto && FU.Ablauf && FU.Activ )
		STANDBY = 1;
	else
		STANDBY = 0;

/*read real time clock*/

	RTC_gettime(&RTCtime);
	OrgYear = RTCtime.year;
	RTCtime.year -= 2000;
/*belichtete Datei in Liste eintragen (wird in belichten.c getriggert*/
	if(FileListShift != 0)
	{
		if(FileListShift == 1)
		{
			for(i=MAXFILELIST-3;i>=0;i--)
			{
				strcpy(&FileListName[i+1][0],&FileListName[i][0]);
				strcpy(&FileListStatus[i+1][0],&FileListStatus[i][0]);
				strcpy(&FileListTime[i+1][0],&FileListTime[i][0]);
				strcpy(&FileListResolution[i+1][0],&FileListResolution[i][0]);
			}
			strcpy(&FileListName[0][0],&FileListName[MAXFILELIST-1][0]);
			strcpy(&FileListStatus[0][0],&FileListStatus[MAXFILELIST-1][0]);
			strcpy(&FileListTime[0][0],&FileListTime[MAXFILELIST-1][0]);
			strcpy(&FileListResolution[0][0],&FileListResolution[MAXFILELIST-1][0]);
		}
		FileListShift = 0;
	}

	TON_10ms(&StartUpTimer);
	TON_10ms(&InitTouchTimer);
	TON_10ms(&CalibTimer);
	TON_10ms(&ControlVoltageTimer);

/*make text panoramaadapter blink*/
	if(PanoramaAdapter && (wBildAktuell == MainPicNumber) )
	{
		if(Clock_1s)
			AdapterTextColor = GRAU;
		else
			AdapterTextColor = ROT;
	}

/*	PanoramaInv = !PanoramaAdapter;*/
	if(UserLevel < 2)
	{
		ManualModeInv = TRUE;
		ManualMode = FALSE;
	}
	else
		ManualModeInv = FALSE;

	PanoramaInv = !ManualMode || ManualModeInv;


/*HA 14.10.03 V1.66 added emergency stop*/
/*
	if (!CoverLockOK)
	{
		if (wBildNr != 73 && (InitPicReady == 1) )
			wBildNeu = 73;

		AlarmBitField[30]=1;
		START = 0;
		AUTO = 0;
		PaperRemoveStart = 0;
		PlateTakingStart = 0;
		AdjustStart = 0;
		DeloadStart = 0;
		ExposeStart = 0;
		CloseTrolleyStart = 0;
		OpenTrolleyStart = 0;
		ReferenceStart = 0;

		Out_CoverLockOn = 0;

		FU.cmd_Start = 0;
		FU.cmd_Auto = 0;

		for(i=0;i<MotorsConnected;i++)
		{
			Motors[i].ReferenceOk = 0;
			Motors[i].StartRef = 0;
			Motors[i].ReferenceStep = 0;
		}
	}
	else
		AlarmBitField[30]=0;

*/

/***** Schlüsselschalter *********/
	ServiceKeyInv = ServiceKey;
/* ********************NOTAUS BLUEFIN *********************************************/
	EmergencyStopInv = ControlVoltageOk &&  ActivateControlVoltage;

	if (ButtonActivateControlVoltage )
	{
		ControlVoltageTimer.IN = 1;
		ControlVoltageTimer.PT = 100; /* 1 Sec.*/
		ActivateControlVoltage = 1;
		ButtonActivateControlVoltage = 0;
	}

	if (ControlVoltageOk)
		ControlVoltageTimer.IN = 0;
	else
	{
		if(	!ControlVoltageTimer.IN )
			ActivateControlVoltage = 0;
	}

/*nach 1/2 sec nicht eingeschaltet*/
	if (ControlVoltageTimer.Q && !ControlVoltageOk)
	{
		ControlVoltageTimer.IN = 0;
		ActivateControlVoltage = 0;
	}

	if(MachineType == BLUEFIN_XS)
	{
		HideXS = FALSE;
		HideLC = TRUE;
	}
	else
	{
		HideXS = TRUE;
		HideLC = FALSE;
	}

	PaletteLoadingInv = !GlobalParameter.EnablePaletteLoading || PageInv1;
	
	switch (wBildNr)
	{
		case 0: /*Init Krause Version*/
		{
			InitPic();
			HideKPG = 1;
			HideKrause = 0;
			KPGClosedInvisible = 1;
			KPGOpenInvisible = 1;
			JetRedInvisible = 1;
			JetOpenInvisible = 1;
			for (i=0;i<6;i++)
				AdressIndex[i] = i;
			InitPicReady = 1;
			break;
		}
		case 1: /*Main*/
		{

			MainPic();
			if (ToggleLamp)
			{
				ToggleLamp = 0;
				if (ready)
				{
					wBildNeu = EMPTYPIC;

				} /*if ready*/
			}
			break;
		}

		case 3: /*FEEDER PARAM*/
		{
			ParamPicture((void *)&FeederParamInput,(void *)&FeederParameter, sizeof(FeederParameter),4,1);
			SafetyCheckOK = 0;
			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitMotorSpeed < 2)
					UnitMotorSpeed++;
				else
					UnitMotorSpeed = 0;

				if (UnitLength < 1)
					UnitLength++;
				else
					UnitLength = 0;
			}

			break;
		}
		case 4: /*PAPERREMOVE PARAM*/
		{
			if(GlobalParameter.EnablePaletteLoading)
				ParamPicture((void *)&PaperRemoveParamInput,(void *)&PaperRemoveParameter, sizeof(PaperRemoveParameter),5,1);
			else
				ParamPicture((void *)&PaperRemoveParamInput,(void *)&PaperRemoveParameter, sizeof(PaperRemoveParameter),4,1);
			SafetyCheckOK = 0;

			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitMotorSpeed < 2)
					UnitMotorSpeed++;
				else
					UnitMotorSpeed = 0;
			}

			break;
		}
		case 5: /*ADJUSTER PARAM*/
		{
			ParamPicture((void *)&AdjusterParamInput,(void *)&AdjusterParameter, sizeof(AdjusterParameter),2,1);
			SafetyCheckOK = 0;

			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitMotorSpeed < 2)
					UnitMotorSpeed++;
				else
					UnitMotorSpeed = 0;
			}

			break;
		}

		case 6: /*DELOADER PARAM*/
		{
			ParamPicture((void *)&DeloaderParamInput,(void *)&DeloaderParameter, sizeof(DeloaderParameter),2,1);
			SafetyCheckOK = 0;

			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitMotorSpeed < 2)
					UnitMotorSpeed++;
				else
					UnitMotorSpeed = 0;

				if (UnitLength < 1)
					UnitLength++;
				else
					UnitLength = 0;
			}

			break;
		}


		case 7: /*PARAM SELECT*/
		{
			if (ChangeLanguage)
			{
				ChangeLanguage = 0;
				if (GlobalParameter.Language < MAXLANGUAGES)
					GlobalParameter.Language++;
				else
					GlobalParameter.Language =  0;
			}

			if(save || load)
			{
				if (save) saveall = 1;
				if (load) loadall = 1;
				FileIOData = (UDINT *) TmpPtr;
				FileIOLength = PERFSRAMSIZEBYTES;
				strcpy(FileType,"SA1");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}


		case 8: /*MOTOR PARAM*/
		{
			gMotorOnDisplay = MotorOnDisplay;
			MotRefInv = (!(Motors[MotorOnDisplay].ReferenceOk))|| PageInv3;
			MotActInv = (!(Motors[MotorOnDisplay].Moving))|| PageInv3;
			MotErrInv =( !(Motors[MotorOnDisplay].Error))|| PageInv3;

			if(GlobalParameter.EnablePaletteLoading)
			{
				RefSensorLayerInv = (MotorOnDisplay != PAPERREMOVE_VERTICAL);
				PaperRemoveUpInv = !In_PaperRemoveUp;
			}
			else
			{
				RefSensorLayerInv = TRUE;
				PaperRemoveUpInv = TRUE;
			}
			
/* update the current position*/
			if ( (MotorOnDisplay == ADJUSTER) || (MotorOnDisplay == CONVEYORBELT) )
			{
				Motors[0].Position = Motors[MotorOnDisplay].Position;
				Motors[0].Position_mm = Motors[MotorOnDisplay].Position_mm;
			}
			else
			{
				Motors[0].Position = AbsMotorPos[MotorOnDisplay];
				Motors[0].Position_mm = AbsMotorPosMm[MotorOnDisplay];
			}

			Motors[0].ReferenceStep = Motors[MotorOnDisplay].ReferenceStep;
/* update the Temp/current*/
			MotTemp = MotorTemp[MotorOnDisplay];
			MaxMotTemp = MaxMotorTemp[MotorOnDisplay];
			MotCurr = MotorCurrent[MotorOnDisplay];
			MaxMotCurr = MaxMotorCurrent[MotorOnDisplay];
			memcpy(&MotStatus[0],&MotorFaultStatus[MotorOnDisplay][0],4);
			MotStatus[4] = 0;

/* cont current cannot be greater than peek current */
			if(Motors[0].Parameter.ReferencePeekCurrent < Motors[0].Parameter.ReferenceContCurrent)
				Motors[0].Parameter.ReferenceContCurrent = Motors[0].Parameter.ReferencePeekCurrent;
			if(Motors[0].Parameter.DefaultPeekCurrent < Motors[0].Parameter.DefaultContCurrent)
				Motors[0].Parameter.DefaultContCurrent = Motors[0].Parameter.DefaultPeekCurrent;
/*copy the command bits*/
			if ( Motors[0].StartRef )
			{
				Motors[MotorOnDisplay].StartRef = 1;
				Motors[0].StartRef = 0;
			}

			ParamPicture((void *)&Motors[0],(void *)&Motors[MotorOnDisplay], sizeof(Motors[0]),4,3);
			if (SafetyCheckOK)
				saveMotorData = 1;
			SafetyCheckOK = 0;

			if (Motors[0].Parameter.ManSpeed != OldManSpeed)
			{
				OldManSpeed = Motors[0].Parameter.ManSpeed;
				Motors[MotorOnDisplay].Parameter.ManSpeed = Motors[0].Parameter.ManSpeed;
			}
			if (Motors[0].Parameter.MaximumPosition != OldLimitP)
			{
				OldLimitP = Motors[0].Parameter.MaximumPosition;
				Motors[MotorOnDisplay].Parameter.MaximumPosition  = Motors[0].Parameter.MaximumPosition ;
			}
			if (Motors[0].Parameter.MinimumPosition != OldLimitM)
			{
				OldLimitM = Motors[0].Parameter.MinimumPosition;
				Motors[MotorOnDisplay].Parameter.MinimumPosition  = Motors[0].Parameter.MinimumPosition ;
			}

			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitTemperature < 2)
					UnitTemperature++;
				else
					UnitTemperature = 1;

				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitMotorSpeed < 2)
					UnitMotorSpeed++;
				else
					UnitMotorSpeed = 0;

				if (UnitLength < 1)
					UnitLength++;
				else
					UnitLength = 0;

			}

			break;
		}

		case 9: /*MOTOR SELECT*/
		{
			if (SelMotor1)
			{
				SelMotor1 = 0;
				MotorOnDisplay = FEEDER_VERTICAL;
				wBildNeu = 8;
			}
			if (SelMotor2)
			{
				SelMotor2 = 0;
				MotorOnDisplay = FEEDER_HORIZONTAL;
				wBildNeu = 8;
			}
			if (SelMotor4)
			{
				SelMotor4 = 0;
				MotorOnDisplay = PAPERREMOVE_HORIZONTAL;
				wBildNeu = 8;
			}
			if (SelMotor5)
			{
				SelMotor5 = 0;
				MotorOnDisplay = PAPERREMOVE_VERTICAL;
				wBildNeu = 8;
			}
			if (SelMotor9)
			{
				SelMotor9 = 0;
				MotorOnDisplay = CONVEYORBELT;
				wBildNeu = 8;
			}
			if (SelMotor10)
			{
				SelMotor10 = 0;
				MotorOnDisplay = PAPERREMOVE_VERTICAL2;
				wBildNeu = 8;
			}
			break;
		}

		case 11: /*X-AXIS PARAM*/
		{
			if (xBildInit)
			{
				X_Param_Alt = X_Param;
				gSendManSpeeds = 0;
			}

			if (InputOK)
			{
				if (	(X_ParamInput.ManSpeed1 != X_Param_Alt.ManSpeed1) ||
					(X_ParamInput.ManSpeed2 != X_Param_Alt.ManSpeed2) )
				{
					X_Param_Alt.ManSpeed1 = X_ParamInput.ManSpeed1;
					X_Param_Alt.ManSpeed2 = X_ParamInput.ManSpeed2;
					gSendManSpeeds = 1;
				}/*if X_param.ManSpeed1 != manSpeed1_alt*/

				if (X_ParamInput.ParkPosition != X_Param_Alt.ParkPosition)
					ParkPositionChanged = 1;
			}

			ParamPicture((void *)&X_ParamInput,(void *)&X_Param, sizeof(X_ParamInput),3,3);
			if (SafetyCheckOK)
				saveXData = 1;

			SafetyCheckOK = 0;

			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitLength < 1)
					UnitLength++;
				else
					UnitLength = 0;
			}
			break;
		}


		case 12: /*GLOBAL PARAM*/
		{
			if (UserLevel >= 2)
				ParamPicture((void *)&GlobalParameterInput,(void *)&GlobalParameter, sizeof(GlobalParameter),3,1);
			else
				ParamPicture((void *)&GlobalParameterInput,(void *)&GlobalParameter, sizeof(GlobalParameter),2,1);

			SafetyCheckOK = 0;
			if (xBildInit)
			{
				UnitMotorLength = 1;
				UnitLength = 1;
			}
			if (ChangeUnits)
			{
				ChangeUnits = 0;
				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitLength < 2)
					UnitLength++;
				else
					UnitLength = 0;
			}

			if(xBildExit && (GlobalParameter.FlexibleFeeder == FALSE))
			{
				for(i=1;i<MAXTROLLEYS;i++)
					Trolleys[i].RightStack = FALSE;
			}


			break;
		}

		case 13: /*PLATE SELECT*/
		{

			if (xBildInit)
			{
				SelectedPlateIndex = 1;
				PlateSelOk = 0;
				for (i=1;i<MAXPLATETYPES;i++)
				{
					itoa(i,(UDINT) &tmp[0]);
					memcpy(PlateNameList[i],&tmp[0],1);
					PlateNameList[i][1] = ' ';
					memcpy(&PlateNameList[i][2],PlateTypes[i].Name,MAXPLATETYPENAMELENGTH-1);
				}
			}
			if (PlateSelOk)
			{
				PlateSelOk = 0;
				wBildNeu = 14;
			}

			break;
		}
		case 14: /*PLATE PARAM*/
		{
			if(save || load)
			{
				FileIOData = (UDINT *) &PlateParamInput;
				FileIOLength = sizeof(PlateParamInput);
				strcpy(FileType,"PLT");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			ParamPicture((void *)&PlateParamInput,
						 (void *)&PlateTypes[SelectedPlateIndex],
						  sizeof(PlateParamInput),1,1);

			if (SafetyCheckOK)
			{
				EGMPlateTypes[SelectedPlateIndex].SizeX = PlateTypes[SelectedPlateIndex].Length;
				EGMPlateTypes[SelectedPlateIndex].SizeY = PlateTypes[SelectedPlateIndex].Width;
				InputPlateType.Area = (EGMPlateTypes[SelectedPlateIndex].SizeX /1000.0)*(EGMPlateTypes[SelectedPlateIndex].SizeY /1000.0);
				EGMPlateTypes[SelectedPlateIndex].Area = InputPlateType.Area;

				memcpy(&EGMPlateTypes[SelectedPlateIndex].Name,&PlateTypes[SelectedPlateIndex].Name,
						sizeof(EGMPlateTypes[SelectedPlateIndex].Name));
				EGMPlateTypes[SelectedPlateIndex].Name[19] = 0; /*safety!*/
			}

			SafetyCheckOK = 0;
			if (xBildInit)
			{
				UnitLength = 0;
				UnitMotorLength = 0;
			}
			if (ChangeUnits)
			{
				ChangeUnits = 0;

				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;

				if (UnitLength < 1)
					UnitLength++;
				else
					UnitLength = 0;
			}

			break;
		}

		case 15: /*INPUT*/
		{
			ParamPicture((void *)NULL,(void *)NULL, 0,4,1);

/*			FeederVacuumVis = !FeederVacuumOK;*/
/*			PlateAdjustedVis = !PlateAdjusted;*/
			PlateOnConveyorBeltVis = !(PlateOnConveyorBelt.present);
			BeltLooseVis = !BeltLoose;
			TrolleyCodeSensorVis = !TrolleyCodeSensor;
			ConveyorBeltSensorVis = !In_ConveyorBeltSensor[0];
			EGM1Vis = !StatusEGM1;
			EGM2Vis = !StatusEGM2;
			VCPVis = !VCP;
			CoverLockVis = !(CoverLockOK && DoorsOK);
			PaperGripClosedVis = !Out_PaperGripOn[LEFT] && !Out_PaperGripOn[RIGHT];
			PaperRemoveUpInv = !GlobalParameter.EnablePaletteLoading || !In_PaperRemoveUp;
			break;
		}

		case 20: /*TROLLEY PARAM*/
		{
			if(save || load)
			{
				FileIOData = (UDINT *) &TrolleyParamInput;
				FileIOLength = sizeof(TrolleyParamInput);
				strcpy(FileType,"TRO");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}

			ParamPicture((void *)&TrolleyParamInput,(void *)&Trolleys[SelectedPlateIndex], sizeof(TrolleyParamInput),2,1);
			SafetyCheckOK = 0;
			if (xBildInit && wBildLast != FILEPIC)
			{
				UnitLength = 0;
				strcpy(PlateName,PlateTypes[TrolleyParamInput.PlateType].Name);
			}

			TrolleyParamInput.Single = !TrolleyParamInput.Double;
			Lock2Stacks =  TrolleyParamInput.Single
			           || !GlobalParameter.FlexibleFeeder;
			if(TrolleyParamInput.Single)
				TrolleyParamInput.RightStack = FALSE;

			if (NextPlate)
			{
				NextPlate = 0;
				if (TrolleyParamInput.PlateType < (MAXPLATETYPES-1))
					TrolleyParamInput.PlateType++;
				else
					TrolleyParamInput.PlateType = 1;
				strcpy(PlateName,PlateTypes[TrolleyParamInput.PlateType].Name);
			}
			if (PrevPlate)
			{
				PrevPlate = 0;
				if (TrolleyParamInput.PlateType > 1)
					TrolleyParamInput.PlateType--;
				else
					TrolleyParamInput.PlateType = MAXPLATETYPES-1;
				strcpy(PlateName,PlateTypes[TrolleyParamInput.PlateType].Name);

			}

			if (ChangeUnits)
			{
				ChangeUnits = 0;

				if (UnitMotorLength < 2)
					UnitMotorLength++;
				else
					UnitMotorLength = 0;
			}

			if(GlobalParameter.FlexibleFeeder && TrolleyParamInput.RightStack)
				RightStackInputEnabled = TRUE;
			else
				RightStackInputEnabled = FALSE;

			break;
		}

		case 21: /*TROLLEY SELECT*/
		{

			if (xBildInit)
			{
				SelectedPlateIndex = 1;
				PlateSelOk = 0;
				for (i=1;i<MAXTROLLEYS;i++)
				{
					itoa(i,(UDINT) &tmp[0]);
					memcpy(PlateNameList[i],&tmp[0],1);
					PlateNameList[i][1] = ' ';
					memcpy(&PlateNameList[i][2],Trolleys[i].Name,MAXTROLLEYNAMELENGTH-1);
				}
			}
			if (PlateSelOk)
			{
				PlateSelOk = 0;
				wBildNeu = 20;
			}

			break;
		}

		case 22: /*Performance Alarmliste aktuell*/
		{
			AlarmGroupDisplay = 1;
			AlarmGroupFilter = 3;
			break;
		}
		case 23: /*Performance Alarmliste historisch*/
		{
			AlarmGroupDisplay = 1;
			AlarmGroupFilter = 3;
			break;
		}


		case 25: /*SEMIAUTO*/
		{
			if (xBildInit)
			{
				OldUnitLength = UnitLength;
				UnitLength = 0;
			}
			if (xBildExit)
				 UnitLength = OldUnitLength;
			break;
		}

		case 55: /*Screensaver*/
		{
			if(xBildInit)
			{
				ScreenSaverOrgBild = wBildLast;
				/*erstmal alle löschen*/
				for (i=0;i<16;i++)
					SaverLogo[i] = 1;
				SaverStep = 0;
				if (SaverDisabled)
					ExitScreenSaver = 1;
			}

			if(ExitScreenSaver && !xBildInit)
			{
				ExitScreenSaver = 0;
				wBildNeu = ScreenSaverOrgBild;
			}
/*jede Sekunde den Schoner ändern*/
			if (Pulse_1s)
			{
				UINT TmpVal = SaverStep;
				if (SaverStep<5)
					SaverStep++;
				else
					SaverStep = 0;

				if (HideKrause)	/*KPG*/
				{
					SaverLogo[TmpVal] = 1;
					SaverLogo[SaverStep] = 0;
				}
				else	/*Krause*/
				{
					SaverLogo[TmpVal+10] = 1;
					SaverLogo[SaverStep+10] = 0;
				}
			}
			break;
		}


		case 27: /*Statistik*/
		{
/*coming back from request picture*/
			if(xBildInit && wBildLast == MESSAGEPIC && AbfrageOK && UserLevel >= 2)
			{
				PlatesMade = 0;
				savePlatesMade = 1;
			}
			AbfrageOK = 0;
			AbfrageCancel = 0;

			if(ClearListButton)
			{
				ClearListButton = 0;
				ClearList();
				PlatesMadeToday = 0;
				if(UserLevel >= 2)
				{
					if(wBildAktuell != MESSAGEPIC )
						OrgBild = wBildAktuell;
					wBildNeu = MESSAGEPIC;
					AbfrageOK = 0;
					AbfrageCancel = 0;
					IgnoreButtons = 0;
					OK_CancelButtonInv = 0;
					OK_ButtonInv = 1;
					AbfrageText1 = 0;
					AbfrageText2 = 39;
					AbfrageText3 = 0;
					AbfrageIcon = REQUEST;
				}
			}
			break;
		}

		case 29: /*LASERPOWER Auflösungsabh.*/
		{
			ParamPicture(NULL,NULL, 0,2,1);
			if (SafetyCheckOK)
			{
				saveParameterData = TRUE;
			}
			SafetyCheckOK = 0;
			if (LaserPowerAllPlatesEqual)
			{
				LaserPowerAllPlatesEqual = 0;
				for (i=1;i<MAXLASERPOWERSETTINGS;i++)
					for (j = 2;j<MAXPLATETYPES;j++)
						LaserPowerSettings[i].LaserPower[j] = LaserPowerSettings[i].LaserPower[1];
			}
			break;
		}


		case 18: /*Service*/
		{
			if(xBildInit)
				LastPlateType = 0;

			if(!PanoramaAdapter)
			{
				PlateType = 0;
/*HA 04.04.03 */
				AlarmBitField[25] = 0;
			}
			else
			{
				if(PrevPlate)
				{
					PrevPlate = 0;
					if(PlateType>0)
						PlateType--;
					if(PlateType<=0)
						PlateType = MAXPLATETYPES-1;
				}

				if(NextPlate)
				{
					NextPlate = 0;
					PlateType++;
					if(PlateType>=MAXPLATETYPES)
						PlateType = 1;
				}
			}

			if(LastPlateType != PlateType)
			{
				LastPlateType = PlateType;
				if(PlateType != 0)
					strcpy(PanoramaPlateName,PlateTypes[PlateType].Name);
				else
					strcpy(PanoramaPlateName,"----");
			}

			break;
		}

/* Abfrage / Hinweis Bild*/
		case MESSAGEPIC:
		{
			if( IgnoreButtons && (AbfrageCancelButton || AbfrageOKButton) )
			{
				AbfrageCancelButton = 0;
				AbfrageOKButton = 0;
				if(OrgBild == SCREENSAVERPIC)
				{
					if(ScreenSaverOrgBild!=MESSAGEPIC)
						wBildNeu = ScreenSaverOrgBild;
					else
						wBildNeu = MainPicNumber;
				}
				else
					wBildNeu = OrgBild;

				break;
			}

			if(AbfrageCancelButton)
			{
				AbfrageCancelButton = 0;
				if(OrgBild == SCREENSAVERPIC)
				{
					if(ScreenSaverOrgBild!=MESSAGEPIC)
						wBildNeu = ScreenSaverOrgBild;
					else
						wBildNeu = MainPicNumber;
				}
				else
					wBildNeu = OrgBild;
				AbfrageCancel = 1;
			}
			if(AbfrageOKButton)
			{
				AbfrageOKButton = 0;
				if(OrgBild == SCREENSAVERPIC)
				{
					if(ScreenSaverOrgBild!=MESSAGEPIC)
						wBildNeu = ScreenSaverOrgBild;
					else
						wBildNeu = MainPicNumber;
				}
				else
					wBildNeu = OrgBild;
				AbfrageOK = 1;
			}
			break;
		}

		case FILEPIC:
		{
			if(IOExit)
			{
				load = 0;
				save = 0;
				loadall = 0;
				saveall = 0;
				IOExit = 0;
				IOButton = 0;
				IOActive = 0;
				wBildNeu = OrgBildFile;
			}


			if(save)
			{
				if (saveall && !WaitSRAM)
				{
					GetTmpDataFromSRAM = 1;
					WaitSRAM = 1;
				}
				if (WaitSRAM && !GetTmpDataFromSRAM)
				{
					WaitSRAM = 0;
					saveall = 0;
				}

				if (DeviceType == 0) 	/* CF */
					HeaderNumber = 0;
				else					/* USB */
					HeaderNumber = 3;

				if(IOButton && !NoDisk && !DeleteFileCmd)
				{
					if( !IOActive )
					{
						if ( !WriteFileCmd && !WriteFileOK )
						{
							if(strlen(FileIOName)>0 && strlen(FileIOName)<MAXFILENAMELENGTH )
							{
/* ungültige Zeichen im Dateinamen finden und ersetzen*/
								char *cptr=NULL;
								do
								{
									cptr = strchr(FileIOName,' ');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,'?');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,'*');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,'\\');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,'/');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,',');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,':');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								do
								{
									cptr = strchr(FileIOName,';');
									if (cptr != NULL)
										*cptr = '_';
								}
								while (cptr!=NULL);

								IOActive = 1;
								WriteFileCmd = 1;
								WriteFileOK = 0;
								ReadDirOK = 0;
							}
							else
							{
								save = 0;
								IOButton = 0;
							}
						} /* if ( !WriteFileCmd ) */
					}
					else
					{
						if(ReadDirOK && WriteFileOK)
						{
							save = 0;
							IOButton = 0;
							ReadDirOK = 0;
							WriteFileOK = 0;
							IOActive = 0;
							wBildNeu = OrgBildFile;
							break;
						}
						if(WriteFileOK)
						{
							ReadDirCmd = 1;
							ReadDirOK = 0;
							break;
						}
					}
				}
			}

			if(load)
			{
				if (DeviceType == 0) 	/* CF */
					HeaderNumber = 1;
				else					/* USB */
					HeaderNumber = 4;

				if(IOButton)
				{
					if(FileNotExisting)
					{
						FileNotExisting = 0;
						IOButton = 0;
						IOActive = 0;
						ReadFileCmd = 0;
						ReadFileOK = 0;
						break;
					}

					if( !IOActive )
					{
						if ( !WriteFileCmd && !WriteFileOK && !ReadFileCmd)
						{
							if(strlen(FileIOName)>0 && strlen(FileIOName)<MAXFILENAMELENGTH )
							{
								IOActive = 1;
								ReadFileCmd = 1;
								ReadFileOK = 0;
							}
							else
							{
								load = 0;
								IOButton = 0;
							}
						} /*if ( !WriteFileCmd && !WriteFileOK )*/
					}
					else
					{
						if(ReadFileOK )
						{
							if (loadall && !WaitSRAM)
							{
								PutTmpDataToSRAM = 1;
								WaitSRAM = 1;
							}
							if (WaitSRAM && !PutTmpDataToSRAM)
							{
								WaitSRAM = 0;
								loadall = 0;

								load = 0;
								IOButton = 0;
								ReadFileOK = 0;
								IOActive = 0;
								wBildNeu = OrgBildFile;
								break;
							}
/*V1.95 bugfix*/
							if (!loadall)
							{
								load = 0;
								IOButton = 0;
								ReadFileOK = 0;
								IOActive = 0;
								wBildNeu = OrgBildFile;
								break;
							}
						}
					} /*else*/
				}
			}
			break;
		}


/* HA 19.11.07 V1.43 emergency stop*/
		case 73:
		{
			if (CoverLockOK)
				wBildNeu = 26;

			break;
		}

/*******************************************************************************/
/*******************************************************************************/
/** Bluefin	EGM Bilder	**/
/*******************************************************************************/
/*******************************************************************************/
		case 100: /*Init Pic*/
		{
			InitPic();
			break;
		}

		case 101: /*main*/
		{
			if(EGMGlobalParam.EnableGumSection && (MachineType == BLUEFIN_LOWCHEM))
				GumInv = FALSE;
			else
				GumInv = TRUE;


			if (ToggleLamp)
			{
				ToggleLamp = 0;
				if (ready)
				{
					wBildNeu = EMPTYPIC;
				} /*if ready*/
			}

			if ((CurrentMode ==  M_INITCLEANING)  || (CurrentMode ==  M_CLEANING) )
			{
				HideMessageOverlay = TRUE;
				HideAlarmOverlay = TRUE;
				HideCleaningOverlay = FALSE;
				HideLevels = TRUE;
				HideLevelsLC = TRUE;
			}
			else
			{
			/* Störungsmeldungen auf Hauptschirm
			 * keine Einrückung, weil sonst unübersichtlich
			 */
			/*
			 * definiert die Alarme, die eine Meldung auf dem Hauptbild auslösen
			 * -1 ist Ende Kennung der Liste!
			 * Achtung: Bei Veränderung muss auch in auto.c "LampAndHorn" angepasst werden
			 *          Alarmnummern um 1 versetzt zu Alarmnummer in Visu, zB AlarmNr 49 zeigt
			 *          in Visu Alarmnummer 50 an!!
			 *
			 * Der Vorletzte Parameter von AlarmMessage bezeichnet das angezeigte
			 * Symbol:
			 * 0 = Fehler allgemein
			 * 1 = Tank leer, bitte auffüllen
			 * 2 = Kanister leer
			 * 3 = kein Durchfluß
			 */

				if(MachineType == BLUEFIN_XS)
				{
					if( !AlarmMessage(0,113,114,115,0,FALSE) ) /* NOTAUS */


					if( !AlarmMessage(34,116,117,0,3,TRUE) ) /* Entwickler Zirkulation */
					if( !AlarmMessage(33,118,119,0,3,TRUE) ) /* Gummierung Zirkulation */
					if( !AlarmMessage(35,120,119,0,3,TRUE) ) /* Vorspülen Zirkulation */
					if( !AlarmMessage(36,121,119,0,3,TRUE) ) /* Nachspülen Zirkulation */
					if( !AlarmMessage(37,108,112,0,2,TRUE) ) /* Regeneratkanister */
					if( !AlarmMessage(38,109,112,0,2,TRUE) ) /* Entwicklerkanister */
					if( !AlarmMessage(2,110,111,0,1,FALSE) ) /* Gummierung Füllstand */
					{ /* None of the above errors:*/
						HideMessageOverlay = TRUE;
						HideAlarmOverlay = TRUE;
						HideCleaningOverlay = TRUE;
						HideLevels = FALSE;
						HideLevelsLC = TRUE;
						GumLevelInv = EGMGum.LevelNotInRange || GumInv || (MachineType == BLUEFIN_XS);
					}
				}
				else /* LowChem has some different texts*/
				{
					if( !AlarmMessage(0,113,114,115,0,FALSE) ) /* NOTAUS */
					if( !AlarmMessage(34,135,117,0,3,TRUE) ) /* Entwickler Zirkulation */
					if( !AlarmMessage(33,118,119,0,3,TRUE) ) /* Gummierung Zirkulation */
					if( !AlarmMessage(35,120,119,0,3,TRUE) ) /* Vorspülen Zirkulation */
					if( !AlarmMessage(36,121,119,0,3,TRUE) ) /* Nachspülen Zirkulation */
					if( !AlarmMessage(37,108,112,0,2,TRUE) ) /* Regeneratkanister */
					if( !AlarmMessage(38,134,112,0,2,TRUE) ) /* Entwicklerkanister */
					if( !AlarmMessage(2,110,111,0,1,FALSE) ) /* Gummierung Füllstand */
					{ /* None of the above errors:*/
						HideMessageOverlay = TRUE;
						HideAlarmOverlay = TRUE;
						HideCleaningOverlay = TRUE;
						HideLevels = TRUE;
						HideLevelsLC = FALSE;
						GumLevelInv = EGMGum.LevelNotInRange || GumInv || (MachineType == BLUEFIN_XS);
					}
				}
			}

			break;
		}


		case 102: /*Calibration*/
		{
			TouchCalib();
			break;
		}

		case 104:
		{
			/*set time/date*/
			if(xBildInit)
			{
				RTCSetTime.year = OrgYear;
				RTCSetTime.month = RTCtime.month;
				RTCSetTime.day = RTCtime.day;
				RTCSetTime.hour = RTCtime.hour;
				RTCSetTime.minute = RTCtime.minute;
				RTCSetTime.second = RTCtime.second;
				RTCSetTime.millisec = RTCtime.millisec;
				RTCSetTime.microsec = RTCtime.microsec;
			}
			if(SetTimeButton)
			{
				SetTimeButton = 0;
				RTC_settime(&RTCSetTime);
			}

			if(save || load)
			{
				if (save) saveall = 1;
				if (load) loadall = 1;
				FileIOData = (UDINT *) (TmpPtr + PERFSRAMSIZEBYTES);
				FileIOLength = EGMSRAMSIZEBYTES;
				strcpy(FileType,"EGM");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}

			break;
		}


		case 106: /*Auto-Parameter*/
		{
			if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS) )
			{
				ParamPicture(&InputAutoParam,&EGMAutoParam,sizeof(EGMAutoParam),4,1);
				ParamPicture(&InputDeveloperParam,&EGMDeveloperTankParam,sizeof(EGMDeveloperTankParam),4,1);
			}
			else
			{
				ParamPicture(&InputAutoParam,&EGMAutoParam,sizeof(EGMAutoParam),3,1);
				ParamPicture(&InputDeveloperParam,&EGMDeveloperTankParam,sizeof(EGMDeveloperTankParam),3,1);
			}

			if (SafetyCheckOK)
			{
				EGMDeveloperTankParam.RatedTemp = EGMAutoParam.DeveloperTemp;
				EGMPreheatParam.RatedTemp[0] = EGMAutoParam.PreheatTemp;
				EGMPreheatParam.RatedTemp[1] = EGMAutoParam.PreheatTemp2;
			}
			SafetyCheckOK = 0;

			PROVKitInv = !(EGMGlobalParam.PROV_KitInstalled);
			HandleReplenishingSettings(PageInv3);

			if(save || load)
			{
				FileIOData = (UDINT *) &InputAutoParam;
				FileIOLength = sizeof(InputAutoParam);
				strcpy(FileType,"AUT");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}
		case EMPTYPIC: /*Lampe aus (leeres Bild)*/
		{
			if (xBildInit)
			{
				if (ready)
				{
					if (!VA_Saccess(1,VC_HANDLE))
			  		{
						VA_SetBacklight(1,VC_HANDLE,0);
						VA_Srelease(1,VC_HANDLE);
					}
				} /*if ready*/
				EmptyReturnPic = wBildLast;
			}
			if (ReturnFromEmpty)
			{
				ReturnFromEmpty = 0;
				wBildNeu = EmptyReturnPic;
			}
			break;
		}


		case 108: /*Preheat-Parameter*/
		{
			if (UserLevel >= 2)
				ParamPicture(&InputPreHeatParam,&EGMPreheatParam,sizeof(EGMPreheatParam),5,1);
			else
				ParamPicture(&InputPreHeatParam,&EGMPreheatParam,sizeof(EGMPreheatParam),2,1);
			if (SafetyCheckOK)
				PreheatParamEnter = 1;

			if (CurveEnter1 ||  CurveEnter2)
			{
				wBildNeu = CURVEPIC;
			}

			SafetyCheckOK = 0;
			if(save || load)
			{
				FileIOData = (UDINT *) &InputPreHeatParam;
				FileIOLength = sizeof(InputPreHeatParam);
				strcpy(FileType,"PHT");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}

		case 109: /*Kurven darstellen*/
		{
			if (xBildInit )
			{
				if( (wBildLast != FILEPIC)
				 && (wBildLast != MESSAGEPIC )
				 && (wBildLast != EMPTYPIC )
				 )
					CurveOrgBild = wBildLast;

/* Sicherheitshalber den Datenpointer schonmal sinnvoll initialisieren*/
				pValueTable = &Values[0];
				switch (wBildLast)
				{
					case 108:	/*Preheat*/
					{
						YMidValueDisplay = EGMPreheatParam.RatedTemp[0]/10.0;
						YMidValue			= EGMPreheatParam.RatedTemp[0];
						DisplaySamplingTime = SamplingTime;
						if (CurveEnter1)
						{
							pValueTable = &Values[0];
							YMidValueDisplay = EGMPreheatParam.RatedTemp[0]/10.0;
							YMidValue			= EGMPreheatParam.RatedTemp[0];
						}
						else
						if (CurveEnter2)
						{
							pValueTable = &AmbientValues[0];
							YMidValueDisplay = EGMPreheat.AmbientTemp[0]/10.0;
							YMidValue			= EGMPreheat.AmbientTemp[0];
						}
						break;
					}
					case 110:	/*Developer*/
					{
						YMidValueDisplay = EGMDeveloperTankParam.RatedTemp/10.0;
						YMidValue			= EGMDeveloperTankParam.RatedTemp;
						DisplaySamplingTime = DeveloperSamplingTime;
						pValueTable = &DeveloperValues[0];
						break;
					}
				}
			}
			if (xBildInit || RedrawCurve)
			{
				CurvePainted = 0;
				RedrawCurve = 0;
				break;
			}
			if (ZoomPlus)
			{
				ZoomPlus = 0;
				CurvePainted = 0;
			}
			if (ZoomFactor<1) ZoomFactor = 1;

			if (pValueTable != NULL)
				PaintCurve();

			if (CurveReturn)
			{
				wBildNeu = CurveOrgBild;
				CurveEnter1 = 0;
				CurveEnter2 = 0;
				CurveReturn = 0;
			}

			if(save )
			{
				int a;
				char tmp[20];
				strcpy(ctmp,"");
				if (CurveEnter1)
				{
					strcpy(ctmp,"Kp = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Kp, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\nTn = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Tn, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\nTv = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Tv, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\nTf = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Tf, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\nKw = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Kw, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\nKfbk = ");
					ftoa(EGMPreheatParam.ReglerParam[0].Kfbk, (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					if (EGMPreheatParam.ReglerParam[0].d_mode ==LCPID_D_MODE_E)
						strcat(ctmp,"\nD-Mode = e ");
					else
						strcat(ctmp,"\nD-Mode = X ");
				}
				strcat(ctmp,"\n\n ");
				for(a=0;a<301;a++)
				{
					itoa((UINT)((DisplaySamplingTime/100.0)*a), (UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,";");
					itoa(*(pValueTable+a),(UDINT )&tmp[0]);
					strcat(ctmp,tmp);
					strcat(ctmp,"\n");
				}
				FileIOData = (UDINT *) &ctmp[0];
				FileIOLength = strlen(ctmp);
				strcpy(FileType,"CSV");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}

			break;
		}

		case 110: /*Developer-Parameter*/
		{
			if(xBildInit)
			{
				PumpMlPerSec_Old = EGMDeveloperTankParam.PumpMlPerSec;
				TopUpPumpMlPerSec_Old = EGMDeveloperTankParam.TopUpPumpMlPerSec;
			}
			if (UserLevel >= 2)
				ParamPicture(&InputDeveloperParam,&EGMDeveloperTankParam,sizeof(EGMDeveloperTankParam),6,1);
			else
				ParamPicture(&InputDeveloperParam,&EGMDeveloperTankParam,sizeof(EGMDeveloperTankParam),4,1);

			if (SafetyCheckOK)
				DeveloperTankParamEnter = 1;

			SafetyCheckOK = 0;

			HandleReplenishingSettings(PageInv2);
			HandleTopUpSettings(PageInv4);

			if(save || load)
			{
				FileIOData = (UDINT *) &InputDeveloperParam;
				FileIOLength = sizeof(InputDeveloperParam);
				strcpy(FileType,"DEV");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}

		case 111: /*IO*/
		{
			if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS))
				ParamPicture(0,0,0,4,1);
			else
				ParamPicture(0,0,0,3,1);
			SafetyCheckOK = 0;

			HideMarksXS = HideXS || PageInv1;
			HideMarksLC = HideLC || PageInv1;

			InVoltageOk = !ControlVoltageOk;
			InSpeedCheck = !RPMCheck;
			InInputSensor = !InputSensor;
			InPreWashTank = !EGMPrewash.TankFull;
			InRinseTank = !EGMRinse.TankFull;
			if(MachineType == BLUEFIN_LOWCHEM)
				InGumTank = !EGMGum.TankFull || GumInv;
			else
				InGumTank = !EGMGum.TankFull;

			InDeveloperTank = !EGMDeveloperTank.TankFull;
			InOutputSensor = !OutputSensor;
			InPlateSensor = !PlateSensor;
			InServiceKey = !ServiceKey;

			OutReady = !Ready;
			OutError = !NoError;
			break;
		}

		case 112: /*Plate-Parameter*/
		{
			PageInv1 = 0;
			if(  xBildInit
			 && (wBildLast != FILEPIC)
			 && (wBildLast != MESSAGEPIC )
			 && (wBildLast != EMPTYPIC )
			 )
			{
				if (wBildLast != CURVEPIC )/*Kurven*/
				{
					ParamOrgBild = wBildLast;
					PlateOnDisplay = 1;
					memcpy(&InputPlateType,&EGMPlateTypes[PlateOnDisplay],sizeof(InputPlateType));
				}
			}

			if (InputOK)
			{
				InputOK = 0;
				memcpy(&EGMPlateTypes[PlateOnDisplay],&InputPlateType,sizeof(InputPlateType));
				wBildNeu = ParamOrgBild;
				saveParameterData = 1;
				break;
			}

			if (PrevPlate)
			{
				PrevPlate = 0;
				memcpy(&EGMPlateTypes[PlateOnDisplay],&InputPlateType,sizeof(InputPlateType));
				if (PlateOnDisplay > 1)
					PlateOnDisplay--;
				else
					PlateOnDisplay = MAXPLATETYPES-1;
				memcpy(&InputPlateType,&EGMPlateTypes[PlateOnDisplay],sizeof(InputPlateType));
			}
			if (NextPlate)
			{
				NextPlate = 0;
				memcpy(&EGMPlateTypes[PlateOnDisplay],&InputPlateType,sizeof(InputPlateType));
				if (PlateOnDisplay < MAXPLATETYPES-1)
					PlateOnDisplay++;
				else
					PlateOnDisplay = 1;
				memcpy(&InputPlateType,&EGMPlateTypes[PlateOnDisplay],sizeof(InputPlateType));
			}
			if (PlateInputReady)
			{
				PlateInputReady = 0;
				InputPlateType.Area = (InputPlateType.SizeX /1000.0)*(InputPlateType.SizeY /1000.0);
				EGMPlateTypes[PlateOnDisplay].Area = InputPlateType.Area;
			}

			if(save || load)
			{
				FileIOData = (UDINT *) &InputPlateType;
				FileIOLength = sizeof(InputPlateType);
				strcpy(FileType,"PLT");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}

			break;
		}

		case 114: /*Language-Parameter*/
		{
			if (xBildExit)
			{
			}
			break;
		}

		case 115: /*Statisitik*/
		{
			if(xBildInit || DeveloperChangeNow)
			{
				DTGetTime_1.enable = 1;
				DTGetTime(&DTGetTime_1);
				Now = DTGetTime_1.DT1;
			}

			if (DeveloperChangeNow)
			{
				HideSafetyCheck = 0;
				if (SafetyCheckOK)
				{
					DeveloperChangeDate = Now;
					SqmSinceDevChg = 0;
					ReplenisherUsedSinceDevChg = 0;
					HideSafetyCheck = 1;
					DeveloperChangeNow = 0;
				}
				if (SafetyCheckCancel)
				{
					HideSafetyCheck = 1;
					DeveloperChangeNow = 0;
				}

			}

			if (ResetTodaysData)
			{
				HideSafetyCheck = 0;
				if (SafetyCheckOK)
				{
					SessionPlateCounter = 0;
					SessionSqmCounter = 0;
					ReplenisherCounter = 0;
					HideSafetyCheck = 1;
					ResetTodaysData = 0;
				}
				if (SafetyCheckCancel)
				{
					HideSafetyCheck = 1;
					ResetTodaysData = 0;
				}

			}

			SafetyCheckOK = 0;
			SafetyCheckCancel = 0;
			DaysSinceDeveloperChange	= (Now - DeveloperChangeDate)/ 86400;
			break;
		}

		case 116: /*Visualisierung*/
		{
			InInputSensor = !InputSensor;
			InOutputSensor = !OutputSensor;
			InPlateSensor = !PlateSensor;

/* rechts nach links ist standard, also 0 */
			if (EGMGlobalParam.PlateDirection == 0)
			{
				PlateDirLRInv = TRUE;
				PlateDirRLInv = FALSE;
			}
			else
			{
				PlateDirLRInv = FALSE;
				PlateDirRLInv = TRUE;
			}

			if (xBildInit)
			{
			}

			if (ready )
			{
				if (!VA_Saccess(1,VC_HANDLE))
				{
					UDINT	Drawbox_width,Drawbox_height;
					/* Atach to DrawBox control */
					if(EGMGlobalParam.PlateDirection == 0)
						Status_Attach=VA_Attach(1,VC_HANDLE,0,(UDINT)"EGMVisualisierung/Default/DrawBox_1");
					else
						Status_Attach=VA_Attach(1,VC_HANDLE,0,(UDINT)"EGMVisualisierung/Default1/DrawBox_1");

					if(!Status_Attach)
					{
						UINT i;
						UINT PlatePos,PlateLength; /**/
						/* Get width of DrawBox Control*/
						VA_GetDisplayInfo(1,VC_HANDLE,1,(UDINT)&Drawbox_width);
						/* Get hight of DrawBox Control */
						VA_GetDisplayInfo(1,VC_HANDLE,2,(UDINT)&Drawbox_height);
/* Drawbox löschen*/
						VA_Rect (1,VC_HANDLE,0,0,Drawbox_width,Drawbox_height,14,14);

/* in Schleife alle Platten im FIFO darstellen*/
						for (i = 0; (i <= EGMPlatesInFIFO) && (i < FIFOLENGTH); i++)
						{
							if (EGMPlateFIFO[i].Used)
							{
								PlateLength = (Drawbox_width * EGMPlateFIFO[i].Length) / (MACHINELENGTH * 1000);
/* Position ist die VORDERKANTE der Platte !*/
								PlatePos = (Drawbox_width * EGMPlateFIFO[i].Position) / (MACHINELENGTH * 1000);

								/* rechts nach links*/
								if(EGMGlobalParam.PlateDirection == 0)
									x1 = Drawbox_width - (PlatePos );
								else
								/* links nach rechts*/
									x1 = PlatePos - PlateLength;

								y1 = 20;
								x2 = PlateLength ;					 /*ist eigentlich die Länge*/
								y2 = Drawbox_height - 40;			/*ist eigentlich die Höhe*/

								VA_Rect (1,VC_HANDLE,x1,y1,x2,y2,8,0);
/*									VA_DrawBitmap (1,VC_HANDLE,10,x1+5,y1+10);*/
							}
							/* safety, if EGMPlatesInFIFO is changed in another task... */
							if(EGMPlatesInFIFO >= FIFOLENGTH)
								break;
						}
						/*Detach DrawBox Control*/
						VA_Detach(1,VC_HANDLE);

					}/* if (!Status_Attach)*/
					VA_Srelease(1,VC_HANDLE);
				}/*(!VA_Saccess()*/
			}/*if (ready)*/


			break;
		}


		case 118: /*Prewash-Parameter*/
		{
			ParamPicture(&InputPreWashParam,&EGMPrewashParam,sizeof(EGMPrewashParam),1,1);
			SafetyCheckOK = 0;

			if (InputPreWashParam.ReplenishingMode == FRESHWATERONLY)
			{
				PreWashReplPerPlateCol = DISABLEDCOLOR;
				PreWashReplPerSqmCol = DISABLEDCOLOR;
				PreWashValveParCol = DISABLEDCOLOR;
			}
			else
			if (InputPreWashParam.ReplenishingMode == REPLPERPLATE)
			{
				PreWashReplPerPlateCol = ENABLEDCOLOR;
				PreWashReplPerSqmCol = DISABLEDCOLOR;
				PreWashValveParCol = ENABLEDCOLOR;
			}
			else
			{
				PreWashReplPerPlateCol = DISABLEDCOLOR;
				PreWashReplPerSqmCol = ENABLEDCOLOR;
				PreWashValveParCol = ENABLEDCOLOR;
			}

			PreWashReplPerPlateInv = PageInv1 || (InputPreWashParam.ReplenishingMode != REPLPERPLATE);
			PreWashReplPerSqmInv = PageInv1 || (InputPreWashParam.ReplenishingMode != REPLPERSQM);
			PreWashFreshwaterOnlyInv = PageInv1 || (InputPreWashParam.ReplenishingMode != FRESHWATERONLY);

			if(TogglePreWashReplPerPlate)
			{
				TogglePreWashReplPerPlate = 0;
				if (InputPreWashParam.ReplenishingMode == REPLPERPLATE)
					InputPreWashParam.ReplenishingMode = REPLPERSQM;
				else
				if (InputPreWashParam.ReplenishingMode == REPLPERSQM)
					InputPreWashParam.ReplenishingMode = REPLPERPLATE;
			}

			if(TogglePreWashFreshwaterOnly)
			{
				TogglePreWashFreshwaterOnly = 0;
				if (InputPreWashParam.ReplenishingMode == FRESHWATERONLY)
					InputPreWashParam.ReplenishingMode = REPLPERSQM;
				else
					InputPreWashParam.ReplenishingMode = FRESHWATERONLY;
			}

			if(save || load)
			{
				FileIOData = (UDINT *) &InputPreWashParam;
				FileIOLength = sizeof(InputPreWashParam);
				strcpy(FileType,"PWS");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}

		case 119: /*Rinse-Parameter*/
		{
			ParamPicture(&InputRinseParam,&EGMRinseParam,sizeof(EGMRinseParam),1,1);
			SafetyCheckOK = 0;

			if (InputRinseParam.ReplenishingMode == FRESHWATERONLY)
			{
				RinseReplPerPlateCol = DISABLEDCOLOR;
				RinseReplPerSqmCol = DISABLEDCOLOR;
				RinseValveParCol = DISABLEDCOLOR;
			}
			else
			if (InputRinseParam.ReplenishingMode == REPLPERPLATE)
			{
				RinseReplPerPlateCol = ENABLEDCOLOR;
				RinseReplPerSqmCol = DISABLEDCOLOR;
				RinseValveParCol = ENABLEDCOLOR;
			}
			else
			{
				RinseReplPerPlateCol = DISABLEDCOLOR;
				RinseReplPerSqmCol = ENABLEDCOLOR;
				RinseValveParCol = ENABLEDCOLOR;
			}

			RinseReplPerPlateInv = PageInv1 || (InputRinseParam.ReplenishingMode != REPLPERPLATE);
			RinseReplPerSqmInv = PageInv1 || (InputRinseParam.ReplenishingMode != REPLPERSQM);
			RinseFreshwaterOnlyInv = PageInv1 || (InputRinseParam.ReplenishingMode != FRESHWATERONLY);

			if(ToggleRinseReplPerPlate)
			{
				ToggleRinseReplPerPlate = 0;
				if (InputRinseParam.ReplenishingMode == REPLPERPLATE)
					InputRinseParam.ReplenishingMode = REPLPERSQM;
				else
					InputRinseParam.ReplenishingMode = REPLPERPLATE;
			}

			if(ToggleRinseFreshwaterOnly)
			{
				ToggleRinseFreshwaterOnly = 0;
				if (InputRinseParam.ReplenishingMode == FRESHWATERONLY)
					InputRinseParam.ReplenishingMode = REPLPERSQM;
				else
					InputRinseParam.ReplenishingMode = FRESHWATERONLY;
			}

			if(save || load)
			{
				FileIOData = (UDINT *) &InputRinseParam;
				FileIOLength = sizeof(InputRinseParam);
				strcpy(FileType,"RNS");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}


		case 120: /*EGM Alarmliste aktuell*/
		{
			if(MachineType == BLUEFIN_XS)
				AlarmGroupDisplay = 2;
			else
				AlarmGroupDisplay = 3;

			AlarmGroupFilter = 3;
			break;
		}
		case 122: /*EGM Alarmliste historisch*/
		{
			if(MachineType == BLUEFIN_XS)
				AlarmGroupDisplay = 2;
			else
				AlarmGroupDisplay = 3;
			AlarmGroupFilter = 3;
			break;
		}

		case 123: /*Kühlgerät-Parameter und debugging*/
		{
			ParamPicture(0,0,0,2,1);
			SafetyCheckOK = 0;
			break;
		}


		case 124: /* basic setup */
		{
			ParamPicture(&EGMInputGlobalParam,&EGMGlobalParam,sizeof(EGMGlobalParam),3,1);
			if(SafetyCheckOK)
				MachineType = EGMInputGlobalParam.MachineType;

			SafetyCheckOK = 0;

/* page 1*/
			if(EGMGlobalParam.EnableGumSection && (MachineType == BLUEFIN_LOWCHEM))
				GumInv = FALSE;
			else
				GumInv = TRUE;
			GumCheckInv = PageInv1 || !EGMInputGlobalParam.EnableGumSection
			                       || (MachineType != BLUEFIN_LOWCHEM);
			TranspDirInv = PageInv1 || (!EGMInputGlobalParam.PlateDirection);
/* page 2 */
			HideMarksXS = HideXS || PageInv2;
			HideMarksLC = HideLC || PageInv2;

			GumFCInv =     (!EGMInputGlobalParam.UseGumFlowControl)
						|| (!EGMInputGlobalParam.EnableGumSection && (MachineType == BLUEFIN_LOWCHEM));
			DevCircFCInv = (!EGMInputGlobalParam.UseDevCircFlowControl);
			PrewashFCInv = (!EGMInputGlobalParam.UsePrewashFlowControl);
			RinseFCInv = (!EGMInputGlobalParam.UseRinseFlowControl);
			TopUpFCInv = (!EGMInputGlobalParam.UseTopUpFlowControl);
			ReplFCInv = (!EGMInputGlobalParam.UseReplFlowControl);
			XSInv = !(EGMInputGlobalParam.MachineType == BLUEFIN_XS);
			LowChemInv = !(EGMInputGlobalParam.MachineType == BLUEFIN_LOWCHEM);
			PROVKitInv = !(EGMInputGlobalParam.PROV_KitInstalled);

			IOConfigWideInv =   PageInv2
							|| (IOConfiguration != 2)
							||  EGMUserLevelIsNotAdmin;

			if(ToggleGumSection)
			{
				ToggleGumSection = 0;
				EGMInputGlobalParam.EnableGumSection = !EGMInputGlobalParam.EnableGumSection;
			}
			if(ToggleGumFC)
			{
				ToggleGumFC = 0;
				EGMInputGlobalParam.UseGumFlowControl = !EGMInputGlobalParam.UseGumFlowControl;
			}
			if(ToggleDevCircFC)
			{
				ToggleDevCircFC = 0;
				EGMInputGlobalParam.UseDevCircFlowControl = !EGMInputGlobalParam.UseDevCircFlowControl;
			}
			if(TogglePrewashFC)
			{
				TogglePrewashFC = 0;
				EGMInputGlobalParam.UsePrewashFlowControl = !EGMInputGlobalParam.UsePrewashFlowControl;
			}
			if(ToggleRinseFC)
			{
				ToggleRinseFC = 0;
				EGMInputGlobalParam.UseRinseFlowControl = !EGMInputGlobalParam.UseRinseFlowControl;
			}
			if(ToggleTopUpFC)
			{
				ToggleTopUpFC = 0;
				EGMInputGlobalParam.UseTopUpFlowControl = !EGMInputGlobalParam.UseTopUpFlowControl;
			}
			if(ToggleReplFC)
			{
				ToggleReplFC = 0;
				EGMInputGlobalParam.UseReplFlowControl = !EGMInputGlobalParam.UseReplFlowControl;
			}

			if(ToggleTranspDir)
			{
				ToggleTranspDir = 0;
				EGMInputGlobalParam.PlateDirection = !EGMInputGlobalParam.PlateDirection;
			}

			if(TogglePROVKit)
			{
				TogglePROVKit = 0;
				if (UserLevel >= 2)
				{
					if(MachineType == BLUEFIN_XS)
						EGMInputGlobalParam.PROV_KitInstalled = !EGMInputGlobalParam.PROV_KitInstalled;
				}
			}

			if(ToggleMachineType)
			{
				ToggleMachineType = 0;
				if (UserLevel >= 2)
				{
					if(EGMInputGlobalParam.MachineType == BLUEFIN_XS)
					{
						EGMInputGlobalParam.MachineType = BLUEFIN_LOWCHEM;
						EGMInputGlobalParam.PROV_KitInstalled = FALSE;
					}
					else
/*					if(EGMInputGlobalParam.MachineType == BLUEFIN_LOWCHEM)*/
						EGMInputGlobalParam.MachineType = BLUEFIN_XS;
				}
			}

			if(ToggleIOConfig)
			{
				if(AbfrageOK)
				{
					ToggleIOConfig = 0;
					AbfrageOK = 0;
					if(IOConfiguration != 2)
						NewIOConfig = 2;
					else
						NewIOConfig = 1;

					break;
				}
				if(AbfrageCancel)
				{
					ToggleIOConfig = 0;
					AbfrageCancel = 0;
				}
				ShowMessage(125,126,127,CAUTION,OKCANCEL, FALSE);
			}

			if(save || load)
			{
				FileIOData = (UDINT *) &EGMInputGlobalParam;
				FileIOLength = sizeof(EGMInputGlobalParam);
				strcpy(FileType,"GLB");
				wBildNeu = FILEPIC;
				OrgBildFile = wBildNr;
				break;
			}
			break;
		}



		case 142: /* flow control debug*/
		{
void SwitchPump(BOOL *Pump,UDINT Ml,UDINT CV)
{
	CalibOrgMl = Ml;

	if(*Pump && (CV > 10))
		CalibCounter = CV;

	if( CalibPumpCmd && !(*Pump))
		(*Pump) = ON;
	else
	if(!CalibPumpCmd && (*Pump))
		(*Pump) = OFF;
}

void PrepCalib(USINT Nr,BOOL *ResetVar,BOOL PumpState)
{
	TextSnippetNr = Nr;
	(*ResetVar) = TRUE;
	PageInv1 = 1;
	PageInv2 = 0;
	PageInv3 = 1;
	CalibPumpCmd = PumpState;
}

void CalibCalc(UDINT Ml,UDINT *PPerLiter,BOOL *ResetVar)
{
	if(   (Ml >= 100)
	 &&   (CalibCounter > 0 )
	 )
	{
		*PPerLiter = (1000.0 * CalibCounter) / ((REAL)Ml);
		*ResetVar = TRUE;
		CalibMessage = 14;
	}
	else
		CalibMessage = 15;
}

			/* Störungsmeldungen auf Hauptschirm
			 * keine Einrückung, weil sonst unübersichtlich
			 */
			/*
			 * definiert die Alarme, die eine Meldung auf dem Hauptbild auslösen
			 * -1 ist Ende Kennung der Liste!
			 *
			 * Der Vorletzte Parameter von AlarmMessage bezeichnet das angezeigte
			 * Symbol:
			 * 0 = Fehler allgemein
			 * 1 = Tank leer, bitte auffüllen
			 * 2 = Kanister leer
			 */

			if(MachineType == BLUEFIN_XS)
			{
				if( !AlarmMessage(34,116,117,0,3,TRUE) ) /* Entwickler Zirkulation */
				if( !AlarmMessage(33,118,119,0,3,TRUE) ) /* Gummierung Zirkulation */
				if( !AlarmMessage(35,120,119,0,3,TRUE) ) /* Vorspülen Zirkulation */
				if( !AlarmMessage(36,121,119,0,3,TRUE) ) /* Nachspülen Zirkulation */
				if( !AlarmMessage(37,108,112,0,2,TRUE) ) /* Regeneratkanister */
				if( !AlarmMessage(38,109,112,0,2,TRUE) ) /* Entwicklerkanister */
				{ /* None of the above errors:*/
					HideMessageOverlay = TRUE;
					HideAlarmOverlay = TRUE;
					HideCleaningOverlay = TRUE;
					HideLevels = FALSE;
					HideLevelsLC = TRUE;
				}
			}
			else
			{
				if( !AlarmMessage(34,135,117,0,3,TRUE) ) /* Entwickler Zirkulation */
				if( !AlarmMessage(33,118,119,0,3,TRUE) ) /* Gummierung Zirkulation */
				if( !AlarmMessage(35,120,119,0,3,TRUE) ) /* Vorspülen Zirkulation */
				if( !AlarmMessage(36,121,119,0,3,TRUE) ) /* Nachspülen Zirkulation */
				if( !AlarmMessage(37,108,112,0,2,TRUE) ) /* Regeneratkanister */
				if( !AlarmMessage(38,109,112,0,2,TRUE) ) /* Entwicklerkanister */
				{ /* None of the above errors:*/
					HideMessageOverlay = TRUE;
					HideAlarmOverlay = TRUE;
					HideCleaningOverlay = TRUE;
					HideLevels = TRUE;
					HideLevelsLC = FALSE;
				}
			}

			if(xBildInit)
			{
				TextSnippetNr = 0;
				CalibRepl = 0;
				CalibDev = 0;
				CalibGum = 0;
				SensorCalibCancel = 0;
				SensorCalibReady = 0;
				SensorCalibOKButton = 0;
				CalibCounter = 0;

				PageInv1 = 0;
				PageInv2 = 1;
				PageInv3 = 1;
			}

			if(CalibRepl)
			{
				CalibRepl = 0;
				PrepCalib(0,&ReplFC.ResetMlFlown,EGMDeveloperTank.RegenerationPump);
			}
			if(CalibDev)
			{
				CalibDev = 0;
				PrepCalib(1,&TopUpFC.ResetMlFlown,EGMDeveloperTank.Refill);
			}
			if(CalibGum)
			{
				CalibGum = 0;
				PrepCalib(2,&GumFC.ResetMlFlown,EGMGum.PumpCmd);
			}

			if( PageInv1 )
			{

				switch (TextSnippetNr)
				{
					case 0:
					{
						SwitchPump(&EGMDeveloperTank.RegenerationPump,ReplFC.MlFlown,ReplFC.EdgeCounter.CV);
						break;
					}
					case 1:
					{
						SwitchPump(&EGMDeveloperTank.Refill,TopUpFC.MlFlown,TopUpFC.EdgeCounter.CV);
						break;
					}
					case 2:
					{
						SwitchPump(&EGMGum.PumpCmd,GumFC.MlFlown,GumFC.EdgeCounter.CV);
						break;
					}
				}

				if(SensorCalibCancel)
				{
					SensorCalibCancel = 0;
					SensorCalibReady = 0;
					EGMDeveloperTank.RegenerationPump = OFF;
					EGMDeveloperTank.Refill = OFF;
					EGMGum.PumpCmd = OFF;
					CalibPumpCmd = OFF;
					CalibMessage = 16;
					PageInv1 = 1;
					PageInv2 = 1;
					PageInv3 = 0;
					CalibCounter = 0;
				}

				if(SensorCalibOKButton)
				{
					SensorCalibOKButton = 0;
					SensorCalibCancel = 0;
					SensorCalibReady = 0;
					PageInv1 = 0;
					PageInv2 = 1;
					PageInv3 = 1;
					CalibCounter = 0;
				}

				if(SensorCalibReady)
				{
					SensorCalibCancel = 0;
					SensorCalibReady = 0;
					PageInv1 = 1;
					PageInv2 = 1;
					PageInv3 = 0;

					switch( TextSnippetNr )
					{
						case 0:
						{
							CalibCalc(MlMeasured,&EGMGlobalParam.ReplenisherPulsesPerLiter,&ReplFC.ResetMlFlown);
							break;
						}
						case 1:
						{
							CalibCalc(MlMeasured,&EGMGlobalParam.DeveloperPulsesPerLiter,&TopUpFC.ResetMlFlown);
							break;
						}
						case 2:
						{
							CalibCalc(MlMeasured,&EGMGlobalParam.GumPulsesPerLiter,&GumFC.ResetMlFlown);
							break;
						}
					}
					CalibCounter = 0;
				}
			}

			if(xBildExit && (PageInv1==0) )
			{
				saveParameterData = TRUE;
			}


			break;
		}


		case 143: /*Brush settings */
		{
			if(xBildInit)
				MaxBrushRPMOld = MaxBrushRPM = (32767.0 * EGMGlobalParam.BrushFactor)+0.5;

			if( (MaxBrushRPM > 655) || (MaxBrushRPM < 50) )
				MaxBrushRPM = 230;

			if(xBildExit && (MaxBrushRPM != MaxBrushRPMOld) )
			{
				EGMGlobalParam.BrushFactor = (REAL)MaxBrushRPM / 32767.0;
				saveParameterData = 1;
			}
			break;
		}

		case 144: /*Main motor settings */
		{
			if(xBildInit)
				MaxPropulsionSpeedOld = MaxPropulsionSpeed = (32767.0 * EGMGlobalParam.MainMotorFactor);

			if( (MaxPropulsionSpeed > 5.0) || (MaxPropulsionSpeed < 0.5) )
				MaxPropulsionSpeed = 3.0;

			if(xBildExit && (MaxPropulsionSpeed != MaxPropulsionSpeedOld) )
			{
				EGMGlobalParam.MainMotorFactor = MaxPropulsionSpeed / 32767.0;
				saveParameterData = 1;
			}
			break;
		}

		case 145: /*Test Funktionen*/
		{
			if(xBildInit || xBildExit)
			{
				PrewashPumpTimer.IN = OFF;
				RinsePumpTimer.IN = OFF;
				DeveloperCircPumpTimer.IN = OFF;
				GumPumpTimer.IN = OFF;

				PrewashPumpTimer.PT = 2000;
				RinsePumpTimer.PT = 2000;
				DeveloperCircPumpTimer.PT = 2000;
				GumPumpTimer.PT = 2000;
			}

			if(EGMGlobalParam.EnableGumSection || (MachineType == BLUEFIN_XS))
				GumInv = FALSE;
			else
				GumInv = TRUE;
			PrewashPumpTimer.IN = EGMPrewash.PumpCmd;
			RinsePumpTimer.IN = EGMRinse.PumpCmd;
			DeveloperCircPumpTimer.IN = EGMDeveloperTank.CirculationPump;
			GumPumpTimer.IN = EGMGum.PumpCmd;

			TON_10ms(&PrewashPumpTimer);
			TON_10ms(&RinsePumpTimer);
			TON_10ms(&DeveloperCircPumpTimer);
			TON_10ms(&GumPumpTimer);

			if(PrewashPumpTimer.Q || xBildExit)
				EGMPrewash.PumpCmd = OFF;
			if(RinsePumpTimer.Q || xBildExit)
				EGMRinse.PumpCmd = OFF;
			if(DeveloperCircPumpTimer.Q || xBildExit)
				EGMDeveloperTank.CirculationPump = OFF;
			if(GumPumpTimer.Q || xBildExit)
				EGMGum.PumpCmd = OFF;

			if(xBildExit)
			{
				EGMMainMotor.Enable = FALSE;
				EGMBrushMotor.Enable = FALSE;
				EGMMainMotor.RatedRpm = 0;
				EGMBrushMotor.RatedRpm= 0;
			}

			ParamPicture(0,0,0,2,1);

			SafetyCheckOK = 0;
			break;
		}

		case 999: /*Maschine auswählen*/
		{
			if (StartBluefin)
			{
				StartBluefin = 0;
				wBildNeu = 101;
				break;
			}

			if (StartPerformance)
			{
				StartPerformance = 0;
/* wenn Maschine Referenziert hat, dann ins Hauptmenu, sonst ins Referenzbild*/
				if	( !FU.Error && !FU.CANError && FU.ENPO && FU.RefOk
					&& Motors[FEEDER_VERTICAL].ReferenceOk
					&& Motors[PAPERREMOVE_VERTICAL].ReferenceOk
					&& Motors[PAPERREMOVE_HORIZONTAL].ReferenceOk
					&& (Motors[FEEDER_HORIZONTAL].ReferenceOk || !GlobalParameter.FlexibleFeeder)
					)
				{
					wBildNeu = 1;
				}
				else
				{
					wBildNeu = 26;
				}
			}
			break;
		}
	} /*switch*/


	if (ready && QuitAllAlarms)
	{
		if (!VA_Saccess(1,VC_HANDLE))
  		{
  			QuitAllAlarms = 0;
			VA_QuitAlarms(1, VC_HANDLE, 65535);
   			VA_Srelease(1,VC_HANDLE);
		}
	}

}



