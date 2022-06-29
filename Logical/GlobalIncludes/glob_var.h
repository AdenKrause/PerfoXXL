/****************************************************************************/
/*	Globale Definitionen													*/
/*	Änderungsdatum:		13.08.01											*/
/*																			*/
/****************************************************************************/

#ifndef _GLOB_VAR_INCLUDED_
	#define _GLOB_VAR_INCLUDED_ 1

	/* INCLUDEs */
	#include	<bur/plctypes.h>
	#include 	<standard.h>

/* Muss aktiviert sein fuer Ziel-HW PPC2100 */
//#define PPC2100

	/* !!! ACHTUNG!!  Bei Veränderungen in dieser Datei immer ALLES KOMPILIEREN !!!*/


	/* Auswahl der Maschine: Performance, Bluefin oder Kombination*/


	#define MODE_PERFORMANCE	0
	#undef MODE_BLUEFIN
	#undef MODE_BOTH

	/*
	#define MODE_BLUEFIN	1
	#undef MODE_BOTH
	#undef MODE_PERFORMANCE
	*/

	/* Performance und Bluefin mit gemeinsamer Steuerung*/
/*
	#define MODE_BOTH	2
	#undef MODE_BLUEFIN
	#undef MODE_PERFORMANCE
*/

	/* defines */
	#define EIN			1
	#define AUS			0
#ifndef ON
	#define ON		(1)
#endif
#ifndef OFF
	#define OFF		(0)
#endif
#ifndef TRUE
	#define TRUE 		1
#endif
#ifndef FALSE
	#define FALSE 		0
#endif
	#define OK  		1
	#define NOT_OK 		0
	#define ACTIVE		1
	#define INACTIVE	0

	#define NOPLATE     99
	#define ALL	     	98
	#define	LEFT		0
	#define	RIGHT		1
	#define BOTH		2
	#define PANORAMA	3
	#define BROADSHEET	4
	#define PINS_DOWN	5

	#define	MAXALARMS	50

	#define MONOGON_MAX_SPEEDS 16
	#define WRITEDATALENGTH 20
	#define READDATALENGTH 60
	#define MAXMOTORS 20
	#define MAXTROLLEYS 10
	#define MAXPLATETYPES (10)
	#define MAXPLATETYPENAMELENGTH	32
	#define MAXTROLLEYNAMELENGTH	32
	#define MAXEXPOSEFILENAMELENGTH	128
	#define  JOBLISTSIZE			10
	#define  NO_JOB_RESERVED		(-1)
	#define  JOBIDLENGTH			(15)

	#define FEEDER_VERTICAL			1
	#define FEEDER_HORIZONTAL 		2
	#define SHUTTLE 				3
	#define PAPERREMOVE_HORIZONTAL 	4
	#define PAPERREMOVE_VERTICAL 	5
	#define ADJUSTER				6
	#define DELOADER_TURN			7
	#define DELOADER_HORIZONTAL		8
	#define CONVEYORBELT			9
	#define PAPERREMOVE_VERTICAL2 10

	#define CAUTION 0
	#define INFO	1
	#define REQUEST	2

	#define TABLELENGTH	1075
	#define MAXLASERPOWERSETTINGS 10
	#define TCPIPTIMEOUT 150

	#define MAXNUMBEROFTIFFBLASTERS (2)

	#define STREQ(a,b)    (strcmp ((a),(b))     == 0)
	#define STREQN(a,b,n) (strncmp((a),(b),(n)) == 0)

	#define S(a)	((SINT) (a))	     /* shortcut for cast to SINT (signed char)*/
	#define US(a)	((USINT) (a))	     /* shortcut for cast to USINT (unsigned char)*/
	#define I(a)	((INT) (a))	     /* shortcut for cast to INT (short int)*/
	#define UI(a)	((UINT) (a))	 /* shortcut for cast to UINT (unsigned short int)*/
	#define L(a)	((DINT) (a))	 /* shortcut for cast to DINT (signed long)*/
	#define UL(a)	((UDINT) (a))	 /* shortcut for cast to UDINT (unsigned long)*/

	#define SCREENSAVERPIC 5
	#define MESSAGEPIC 41
	#define FILEPIC 42

	#define MAXLASERPOWERSETTINGS 10

	#define NO_DELOADER               ((USINT) 0)
	#define DELOADER_ONLY             ((USINT) 1)
	#define DELOADER_AND_TURNSTATION  ((USINT) 2)

	#define NO_ROTATION	              ((USINT) 0)
	#define ROTATE_90                 ((USINT) 1)
	#define ROTATE_180                ((USINT) 2)

	/*--------------TYPES------------------------------------------------*/
	/* Datenstruktur Can Objekte*/
	typedef struct{
		unsigned char	InitRes[8];
		unsigned char	InitReq[8];
		unsigned char	StatRes[8];
		unsigned char	SteuReq[8];
		unsigned char	ParaRes[8];
		unsigned char	ParaReq[8];

		unsigned char	InitResEv;
		unsigned char	InitReqEv;
		unsigned char	StatResEv;
		unsigned char	SteuReqEv;
		unsigned char	ParaReqEv;
		unsigned char	ParaResEv;
	}CAN_OBJEKT_FUtyp;

	/* Datenstruktur für LUST CDD*/
	typedef struct
	{
	  unsigned char		SetPara;
	  unsigned char		GetPara;
	  unsigned short	IndexPara;
	  unsigned short	ParaStep;
	  unsigned short	IdxIntern;
	  unsigned char		Status;
	  char				DrvError[10];
	  unsigned short	timeoutCounter;
	  float				aktuellePosition_mm;/* aktuelle Position in mm*/
	  DINT				TargetPosition; 	/* Zielposition in Inkrementen*/
	  float				TargetPosition_mm; 	/*Eingegebene Zielposition (mm)*/
	  BOOL 				CANError;			/*Timeout oder CAN Komm Fehler*/
	/* vom Antrieb */
	  BOOL				Error;				/* allgemeiner Fehler */
	  BOOL				CANStatus;	 		/* 0=System Stop, 1=System Start */
	  BOOL				SollwertErreicht;
	  BOOL				Activ;				/* Endstufe aktiv */
	  BOOL				Rot0;				/* Drehzahl 0 */
	  BOOL				CReady;				/* Gerät betriebsbereit */
	  BOOL				ENPO;				/* ENPO Eingang */
	  BOOL				OSD00;				/* Ausgang OSD00 */
	  BOOL				OSD01;				/* Ausgang OSD01 */
	  BOOL				RefOk;				/* Referenzpunkt definiert */
	  BOOL				Auto;				/* Automatikbetrieb aktiv */
	  BOOL				Ablauf;				/* Ablaufprogramm aktiv */
	  BOOL				ZustandsMerker[8];	/* M80 bis M87 aus PosMod  */
	  BOOL				BefehlsMerker[8];	/* M90 bis M97 an PosMod  */
	  DINT				aktuellePosition;	/* aktuelle Position in Inkrementen*/
	/* Kommandos von Applikation */
	  BOOL				cmd_Start;			/* Regler Freigabe*/
	  BOOL				cmd_Auto;			/* Automatik/Handbetrieb*/
	  BOOL				cmd_tipp_plus;
	  BOOL				cmd_tipp_minus;
	  BOOL				cmd_tipp_schnell;
	  BOOL				cmd_Ref;
	  BOOL				cmd_Target;
	  BOOL				cmd_Park;
	  BOOL				cmd_Expose;
	/*interne Stati*/
	  BOOL				state_tipp_plus;
	  BOOL				state_tipp_minus;
	  BOOL				state_tipp_schnell;
	} FU_Typ;

	/* Datenstruktur für Parametrierung LUST CDD */
	typedef struct
	{
		UDINT ParaNr;
		UDINT Wert;
		USINT Index;
		BOOL Error;
		BOOL Ready;
	} FU_Para_Typ;

	/* Datenstruktur für Parameter X-Achse */
	typedef struct
	{
	/*Geschwindigkeiten*/
		UINT RefSpeed1;
		UINT RefSpeed2;
		UINT RefSpeed3;
		UINT ManSpeed1;
		UINT ManSpeed2;
		UINT ParkSpeed;
		REAL	MaxPosition;	/* Verfahrweg Ende plus*/
		REAL	MinPosition;	/* Verfahrweg Ende minus*/
	/*Positionen*/
		REAL Offset;		/*Nullpunktkorrektur*/
		REAL ParkPosition;
		REAL InkrementeProMm;
	} X_Param_Typ;				/* Länge: 24 Byte*/


	/* datenstruktur für Belichtungsparameter */
	typedef struct
	{
	/*Geschwindigkeiten*/
		REAL ExposeSpeed;		/* in mm/s */
		UINT MoveToStartSpeed; /* not yet used! controller uses ParkSpeed*/
	/*Positionen*/
		REAL StartPosition;		/* in mm */
		REAL PEGStartPosition;	/* in mm */
		REAL EndPosition;		/* in mm */
		REAL PEGEndPosition;	/* in mm */
		UINT Resolution;
		UDINT ScansPerMinute;
	} Expose_Param_Typ;


	/********************************************************************/
	/* Types for Faulhaber Motors	*/
	/********************************************************************/
	typedef struct
	{
		USINT	ControllerIFact;
		char 	Function[20];
		USINT	Priority;
		REAL	IncrementsPerMm;
		UDINT 	ReferenceSpeed;
		BOOL	ReferenceDirectionNegative;
		UDINT	ReferencePeekCurrent;
		UDINT	DefaultPeekCurrent;
		UDINT	ReferenceContCurrent;
		UDINT	DefaultContCurrent;
		UDINT	ReferencePropFact;
		UDINT	DefaultPropFact;
		UDINT	ReferenceOffset;
		UDINT	ReferenceAcceleration;
		UDINT	DefaultAcceleration;
		UDINT	ManSpeed;
		UDINT	MaximumSpeed;
		DINT	MaximumPosition;
		DINT	MinimumPosition;
		UDINT	PositionWindow;
	} Motor_Param_Typ;

	typedef struct
	{
		DINT 	Position;
		DINT 	LastPosition;
		REAL	Position_mm;
		BOOL	ReferenceOk;
		BOOL	Error;
		BOOL	Moving;
		BOOL	MovingPlus;
		BOOL	MovingMinus;
		USINT	ReferenceStep;
		USINT	timeout;
		BOOL	StartRef;
		Motor_Param_Typ	Parameter;
	}Motor_Typ;


	/* Datenstrukturen für Feeder */
	typedef struct
	{
		DINT		Up;
		DINT		AboveTrolley;
	/*V1.95 geänderte Funktion dieses Parameters: tiefste Position!*/
		DINT		MaxDown;
		DINT		PreRelease;
		DINT		ReleasePlate;
		DINT		EnablePaperRemove;
		DINT		SpeedChangePosition;  /* relative to platestack*/
	}Feeder_Vert_Pos_Typ;

	typedef struct
	{
		DINT		ReleasePlate;
		DINT		TakePlate;
	}Feeder_Horiz_Pos_Typ;

	typedef struct
	{
		UDINT		VacuumOnTime;		/* in 10ms */
		UDINT		VacuumOffTime;		/* in 10ms */
		UDINT		SpeedUpSlow;
		UDINT		SpeedUpFast;
		Feeder_Vert_Pos_Typ		VerticalPositions;
		Feeder_Horiz_Pos_Typ	HorizontalPositions;
		DINT		Stack0Pos;
		DINT		Stack20Pos;
		DINT		SlowDownPos;
		UDINT		StackDetectSpeed;
		DINT		PanoramaAdapter;
		USINT		SuckRepitions;
		USINT		ShakeRepitions;
		USINT		ShakeTime; /*in 10ms*/
		UDINT		ShakeWay;
		BOOL		WaitForExposer;		/*waiting on exposer start (avoid suction marks) on/off */
		BOOL		IgnoreVacuumSwitch;	/*don't check vacuum switch*/
		DINT		HorPositionPano;		/*HA 01.10.03 V1.64 new Param for panoadapter*/
	/*V1.95 new array for trolley empty pos*/
		DINT		TrolleyEmptyPositions[MAXTROLLEYS*2]; /*Trick, da 2-dim array nicht geht
														   * wird im Performance nicht gebraucht
														   * hier sind die Leer-Positionen direkt
														   * in den Trolley-Parametern abgelegt,
														   * das ging beim Jet nicht, weil die Kompatibilität
														   ' der Parameter gewahrt bleiben musste (Trolley-Array!)*/
		REAL		MaxTablePosition; /*Enable Position f Feeder runter*/
		REAL		MinTablePosDrop; /*Position des Tisches so, dass Bandrollen mittig unter Saugern sind*/
		REAL		DropOffset;		/* In welchem Abstand soll die Platte an die Stifte gelegt werden? in mm*/
	}Feeder_Param_Typ;

	/* Datenstrukturen für Papierentfernung */
	typedef struct
	{
	/*horizontal*/
		DINT		EnableFeeder;
		DINT		PaperReleaseSingle;
		DINT		PaperReleasePanorama;
		DINT		PaperRemoveLeft;
		DINT		PaperRemoveRight;
		DINT		TrolleyCloseRightStart;		/* Startpos for closing trolley right */
		DINT		TrolleyCloseRightStop;		/* Stoppos for closing trolley right */
		DINT		TrolleyOpenRightStart;		/* Startpos for opening trolley right */
		DINT		TrolleyOpenRightStop;		/* Stoppos for opening trolley right */

		DINT		PaletteDetectStartPos;			/* Startpos for palette detection */
		DINT		PaletteDetectEndPos;  			/* Endpos for palette detection */
		DINT		Reserve1;
		DINT		Reserve2;

	}PaperRemove_Horiz_Pos_Typ;



	typedef struct
	{
	/*vertical*/
		DINT		Up;
		DINT		Down;
		DINT		OnTrolley;
	}PaperRemove_Vert_Pos_Typ;

	typedef struct
	{
	/*speeds*/
		UDINT		TrolleyOpenCloseSpeed;
		UDINT		PaperRemoveSpeed;
	/*times*/
		UDINT		GripOnTime;			/* in 10ms */
		UDINT		GripOffTime;		/* in 10ms */
		PaperRemove_Horiz_Pos_Typ		HorizontalPositions;
		PaperRemove_Vert_Pos_Typ		VerticalPositions;
		UDINT		MaxCurrentDown;
		UDINT		MaxCurrentUp;
		UDINT		PushPos_Horizontal;
		UDINT		PushPos_Vertical;
		UDINT		TrolleyOpenDownSpeed;
		UDINT		TrolleyOpenUpSpeed;
		UDINT		PaperRemoveDownSpeed;
		UDINT		PaperRemoveUpSpeed;
		UDINT		TimeoutOnTrolley;		/* in 10ms */
		UDINT		PaletteDetectSpeed;		/* */
		UDINT		TimeoutUpFromTrolley;	/* in 10ms */
		UDINT		TimeoutOpenTrolleySmall;/* in 10ms */
		UDINT		TimeoutOpenTrolleyBig;	/* in 10ms */
		UDINT		TrolleyDetectSpeed;
		DINT		TrolleyDetectStartPos;
		DINT		TrolleyDetectEndPos;
		USINT		PaperGripCycles;
		DINT		HorPositionPano;		/*HA 01.10.03 V1.64 new Param for panoadapter*/
		DINT		PaperReleasePosVertical;
	}PaperRemove_Param_Typ;

	/*Datenstrukturen für Shuttle*/
	typedef struct
	{
		DINT		EnableFeeder;
		DINT		TakePlate;
		DINT		ReleasePlateMax;
	}Shuttle_Pos_Typ;

	typedef struct
	{
		UDINT		VacuumOnTime;		/* in 10ms */
		UDINT		VacuumOffTime;		/* in 10ms */
		Shuttle_Pos_Typ			Positions;
		REAL DropOffset;
		UDINT		ToAdjusterSpeed;
		UDINT		ToTrolleySpeed;
		REAL		MaxTablePositionDrop;
		BOOL		IgnoreVacuumSwitch;	/*don't check vacuum switch*/
		BOOL		EnablePaperDetection; /*V1.93 Papiersensor schaltbar*/
		DINT		PlateDropPositions[MAXPLATETYPES];
		DINT		ReleasePlatePositions[MAXPLATETYPES]; /*V1.95 Release Posiitons plate-dependant*/
		BOOL		EnablePlateToPaperbasket; /*V2.00 Papiersensor Aktion schaltbar*/
	}Shuttle_Param_Typ;


	/*struct for adjuster Parameter*/
	typedef struct
	{
		UDINT	PinsUpTime;
		UDINT	PinsDownTime;
		UDINT	AdjActiveTime;
		UDINT	AdjInactiveTime;
		UDINT	AdjVacuumOnDelay;
		UDINT	AdjVacuumOffDelay;
		UDINT	TableVacuumOnDelay;
		UDINT	TableVacuumOffDelay;
		UDINT	BlowairOnDelay;
		UDINT	BlowairOffDelay;
		UDINT	TimeoutAdjustedPosition;
		USINT	MaxFailedPlates;
		USINT	MaxAttempts;
		UDINT	AddBlowairValveDelay;
		INT		AdjustSim;
		DINT	FeederAdjustposition; /* vertical position for adjusting */
		DINT	FeederFixposition; /* vertical position for fixing the plate */
	}Adjust_Param_Typ;


	/* struct to hold plate data*/
	typedef struct
	{
		UINT	Number;
		STRING	Name[MAXPLATETYPENAMELENGTH];
		UINT	SeparateBeltTracks;	/*V1.92 separate tracks possible on conveyorbelt*/
		USINT	TurnMode;	/*turn mode for deloader*/
		REAL	XOffset;
		REAL	YOffset;
		REAL	Length;
		REAL	Width;
		REAL	Thickness;
		REAL	Sensitivity;
	/*HA 23.06.03 V1.10*/
	/*Positions for Deloader*/
		UDINT	TakePlatePosition;
		UDINT	ReleasePlatePosition; /* in Performance used for Feeder-horizontal-release-position*/
		UDINT	ReleasePlatePosition2;
		BOOL	Reserve3;
		BOOL	Reserve1;
		INT		Reserve2;
		UDINT	dummy[1];		/*for future enhancements*/
	}Plate_Data_Typ;


	/*struct for global machine data*/
	typedef struct
	{
		STRING	MachineName[32];
		STRING	MachineNumber[6];
		STRING	CustomerName[32];

		REAL	MaxPlateLength;
		REAL	MinPlateLength;
		UDINT	TrolleyRightOffset;
		USINT	TrolleyLeft;
		USINT	TrolleyRight;
		BOOL	AlternatingTrolleys;

		BOOL	XSimulation;
		BOOL	TBSimulation;
		BOOL	ProcessorSimulation;
		BOOL	PunchSimulation;
		BOOL	PaperRemoveEnabled;
		UINT	Language;
		BOOL	Dummy;
		BOOL	FlexibleFeeder;				/*Feeder horizontal present? 1=Yes*/
		BOOL	AutomaticTrolleyOpenClose;
		USINT	reserve1;
		USINT	reserve2;
		USINT	reserve3;
		USINT	reserve4;
		USINT	EnablePaletteLoading;
		USINT	LaserPower;
		UINT 	Resolution;
		UDINT 	ScansPerMinute;
		BOOL	UseOldTiffBlaster;
		USINT	DeloaderTurnStation;		/*deloader turn and positioning station present? 1=hor only, 2=hor+turn*/
	/*neue Var für Laserpower mit Nachkommastelle*/
		UINT	RealLaserPower;
		USINT	MirroredMachine;
		UINT	DisableSlowMode;
		UDINT	SlowModeWaitTime;
	}Global_Data_Typ;


	/*Trolley Data*/
	typedef struct
	{
		STRING 		Name[MAXTROLLEYNAMELENGTH];
		USINT		Number;
		USINT		PlateType;
		BOOL		Empty;				/*Empty flag for single trolley*/
		BOOL		Single;
		BOOL		Double;
		BOOL		NoCover;			/*No cover -> Palette*/
		BOOL		LeftStack;			/*only for double trolley */
		BOOL		RightStack;			/*only for double trolley */
		UINT		PlatesLeft;			/*current Number of plates*/
		UINT		PlatesRight;		/*current Number of plates*/
	/*Positions for OPen / Close*/
		DINT		OpenStart;
		DINT		OpenStop;
		DINT		CloseStart;
		DINT		CloseStop;
	/*Positions for 2-stack trolley*/
		DINT		TakePlateLeft;
		DINT		TakePlateRight;
		DINT		TakePaperLeft;
		DINT		TakePaperRight;
	/* data for Single trolley*/
	/*Positions are stored in "left" data for 2 stack trolley*/

		BOOL		EmptyLeft;		/*double trolley: left stack empty*/
		BOOL		EmptyRight;		/*double trolley: right stack empty*/
		DINT		EmptyPositions[3]; /*Leer Positionen*/
	}Trolley_Data_Typ;


	/*struct for deloader Parameter*/
	typedef struct
	{
		UDINT	PlateTakePosition;
		UDINT	PlateReleasePosition;
		REAL	MinTablePositionTake;
		REAL	MaxTablePositionRelease;
		UDINT	MaxPlateTakePosition;
		UDINT	VacuumOnTime;
		UDINT	VacuumOffTime;
	/*HA 27.01.04 V1.75 Plate length factor for deloading w/out Turnstation*/
		UDINT	PlateLengthAddition; 	/*in mm*/
	/*hor. position to move to for turning (middle)*/
		UDINT	TurnEnabledPosition;
	/*Turn positions in Incr.*/
		UDINT	Turn0Position;
		UDINT	Turn90Position;
		UDINT	Turn180Position;
		DINT	ConveyorBeltSpeed;
		UDINT	TurnSpeed;
		REAL	TableDeloadSpeed;
		REAL	TableDeloadSpeedManual;
		USINT	TurnMode;			/* 0 = no turn, 1 = 90 degrees, 2 = 180 degrees*/
		UDINT	ConveyorBeltDelay;
		REAL	StartTablePosition;	/*Startpos for deloading w/out turnstation*/
		REAL	EndTablePosition;	/*Endpos for deloading w/out turnstation*/
		UDINT	EGMStopDelay;
	/*new for shifted laying off*/
		BOOL	ShiftedLayingOff;
		UDINT	PlateReleasePosition2; /**/
		BOOL	EnableStopPins;
		UDINT	PinsUpDelay;
		UDINT	PinsDownDelay;
		BOOL	SeparateSensors; /*V1.93*/
		UDINT	ConvLateralPulseTime;
		UDINT	ConvLateralMoveTime;
	}Deload_Param_Typ;


	typedef struct
	{
		USINT	TrolleyNumber;
		USINT	PlateType;
		BOOL	TwoStacks;
		USINT	CurrentStack; /*LEFT,RIGHT,DRAWER*/
		USINT	PlateStackInTrolley; /*LEFT,RIGHT,DRAWER*/
		DINT	FeederHorizontalPos;
		DINT	LastStackPos;
		DINT	FeederVerticalPos;
		DINT	PaperRemovePos;
		USINT	LastUsedStack;
	}CurrentDataTyp;


	typedef struct
	{
		UINT	Resolution;
		UINT	LaserPower[MAXPLATETYPES];
	}LaserPowerSettings_Type;

	typedef struct
	{
		BOOL	present; /* a plate is present at that position */
		USINT	PlateType;
		USINT	PlateConfig;
		STRING	FileName[MAXEXPOSEFILENAMELENGTH];
		UINT	ErrorCode;  /* e.g. Paper detected*/
		STRING	ID[JOBIDLENGTH];
		STRING	IDOnDispl[10]; /* weil die ganze ID nicht auf den Schirm passt */
		INT		Status;
		USINT	NextPlateType;
	}PlateInSystem_Type;

	/********************************************************************/
	/* Globale Variablen 												*/
	/********************************************************************/

	/* Struktur als Schnittstelle zur Applikation */
	_GLOBAL	FU_Typ				FU;
	_GLOBAL	X_Param_Typ			X_Param;
	_GLOBAL	Expose_Param_Typ	Expose_Param;

	/*global machine Data*/
	_GLOBAL	Global_Data_Typ	GlobalParameter;

	/*plate data*/
	_GLOBAL	Plate_Data_Typ	PlateTypes[MAXPLATETYPES];
	_GLOBAL	Plate_Data_Typ	PlateParameter;


	_GLOBAL UINT wBildAktuell;
	_GLOBAL UINT wBildNeu;
	_GLOBAL UINT wBildAlt;
	_GLOBAL UINT wBildNr;
	_GLOBAL UINT wBildLast;
	_GLOBAL BOOL xBildExit;
	_GLOBAL BOOL xBildInit;

	_GLOBAL UINT  ACHSE_PIC;
	_GLOBAL UINT  ACHSE_PARAM_PIC;

	_GLOBAL BOOL saveXData;
	_GLOBAL BOOL loadXData;
	_GLOBAL BOOL saveMotorData;
	_GLOBAL BOOL loadMotorData;
	_GLOBAL BOOL saveParameterData;
	_GLOBAL BOOL loadParameterData;

	_GLOBAL BOOL ParkPositionChanged;

	_GLOBAL USINT SendMotor;
	_GLOBAL Motor_Typ Motors[MAXMOTORS+1];
	_GLOBAL USINT MotorsConnected;

	_GLOBAL	Trolley_Data_Typ	Trolleys[MAXTROLLEYS];


	/* Strukturen für Funktionseinheiten */
	_GLOBAL	Feeder_Param_Typ		FeederParameter;
	_GLOBAL	PaperRemove_Param_Typ	PaperRemoveParameter;
	_GLOBAL Adjust_Param_Typ		AdjusterParameter;
	_GLOBAL Deload_Param_Typ		DeloaderParameter;
	_GLOBAL	BOOL	OpenTrolleyStart;
	_GLOBAL	BOOL	CloseTrolleyStart;
	_GLOBAL	BOOL	ReferenceStart;
	_GLOBAL	BOOL	PaperRemoveStart;
	_GLOBAL	BOOL	PlateTakingStart;
	_GLOBAL	BOOL	AdjustStart;
	_GLOBAL	BOOL	DeloadStart;
	_GLOBAL	BOOL	ExposeStart;
	_GLOBAL	BOOL	DeloadStart;
	_GLOBAL BOOL	PaperRemoveReady;
	_GLOBAL BOOL	AdjustReady;
	_GLOBAL BOOL	ExposureReady;
	_GLOBAL	BOOL	DeloadReady;

	/* variables for trolley management*/
	_GLOBAL	USINT	CurrentStack;

	/*Sequence controll variabs*/

	_GLOBAL	BOOL	TCPConnected;
	_GLOBAL	USINT	TCPSendCmd;
	_GLOBAL	char	TCPCmd[20];
	_GLOBAL	char	TCPAnswer[100];
	_GLOBAL	BOOL	TCPRcvFlag;

	_GLOBAL	BOOL	START,AUTO;

	_GLOBAL BOOL	TrolleyEmpty;

	_GLOBAL	BOOL	TrolleyOpen;
	_GLOBAL	BOOL	AlarmBitField[MAXALARMS];
	_GLOBAL	BOOL	AlarmQuitBitField[MAXALARMS];

	_GLOBAL	UINT	AbfrageText1,AbfrageText2,AbfrageText3,AbfrageOK,AbfrageCancel,AbfrageIcon;
	_GLOBAL	UINT	OrgBild,IgnoreButtons,OK_CancelButtonInv,OK_ButtonInv;

	_GLOBAL	USINT	UserLevel;
	_GLOBAL	STRING	FileName[100];
	_GLOBAL	USINT	CounterOn;
	_GLOBAL	UDINT	PlatesToDo;

	_GLOBAL	UINT	PlateType;
	_GLOBAL	BOOL	PanoramaAdapter;

	/*zur Info per TCPIP*/
	_GLOBAL	USINT	SequenceSteps[16];
	_GLOBAL BOOL	PlateTransportSim;
	_GLOBAL STRING  gVERSION[9];
	_GLOBAL DINT AbsMotorPos[MAXMOTORS];
	_GLOBAL REAL AbsMotorPosMm[MAXMOTORS];
	_GLOBAL	CurrentDataTyp	CurrentData;
	_GLOBAL BOOL	StopPlateTaking;
	_GLOBAL BOOL	DisablePaperRemove;
	_GLOBAL	BOOL	StackDetect[2];
	_GLOBAL	BOOL	AlternatingTrolleys;
	_GLOBAL	BOOL	DisableTable;
	_GLOBAL	LaserPowerSettings_Type LaserPowerSettings[MAXLASERPOWERSETTINGS];
	_GLOBAL	BOOL	StatusEGM1;
	_GLOBAL	BOOL	StatusEGM2;
	_GLOBAL	BOOL	SlowMode;
	_GLOBAL USINT	UnexposedPlatesInSystem;
	_GLOBAL BOOL	DataReady;
	_GLOBAL BOOL	ResetStartupSeq;
	_GLOBAL BOOL	DontCloseTrolley;

	_GLOBAL BOOL	BUSY;
	_GLOBAL BOOL	ERROR;
	_GLOBAL BOOL	STANDBY;
	_GLOBAL	BOOL	InEGM1;
	_GLOBAL	BOOL	InEGM2;
	_GLOBAL	BOOL	VCP;
	_GLOBAL UDINT 	PlatesPerHour;
	_GLOBAL	BOOL	BeamON,BeamOFF,TBShutdown;
	_GLOBAL	BOOL 	ParameterChanged;
	_GLOBAL USINT	CANNodeNumber;
	_GLOBAL USINT		gMotorOnDisplay,gSendManSpeeds;

	_GLOBAL PlateInSystem_Type	PlateToDo;
	_GLOBAL PlateInSystem_Type	PlateAtFeeder;
	_GLOBAL PlateInSystem_Type 	PlateAtAdjustedPosition;
	_GLOBAL PlateInSystem_Type	PlateAtDeloader;
	_GLOBAL PlateInSystem_Type	PlateOnConveyorBelt;
	_GLOBAL	Trolley_Data_Typ *pTrolley;
	_GLOBAL	BOOL ManualLoadingStart;
	_GLOBAL	BOOL ManualMode;
	_GLOBAL BOOL	PanelIsTFT;
#endif


