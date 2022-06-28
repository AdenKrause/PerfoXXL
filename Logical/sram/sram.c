#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Zugriff auf remanenten Speicher*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung										von		*/
/*	1.00		16.09.02	erste Implementation								HA		*/
/*	1.10		31.03.03	bei Änderung Aufl. und Speed speichern		HA		*/
/*	1.11		20.05.03	floats auf Gültigkeit prüfen				HA		*/
/*																			*/
/****************************************************************************/

#include <bur/plc.h>
#include <bur/plctypes.h>
#include <math.h>
#include "glob_var.h"
#include "egmglob_var.h"
#include <fileio.h>
#include <AsBrStr.h>


#define OVERALLSRAMSIZEWORDS 12500
#define OVERALLSRAMSIZEEBYTES 25000

typedef struct{
BOOL enable;
UDINT len;
UDINT offset;
UDINT	address;
}puttyp;
typedef struct{
BOOL enable;
UDINT len;
UDINT offset;
UDINT	address;
}gettyp;
puttyp put;
gettyp get;

_LOCAL UINT             wStatus;
_LOCAL UDINT            dwDirNum, dwFileNum;
_LOCAL DirInfo_typ      DInfo;
_LOCAL DirCreate_typ      DCreate;
_LOCAL BOOL Created;
_GLOBAL BOOL PreheatParamEnter,DeveloperTankParamEnter;

int i,j;
USINT tmp[OVERALLSRAMSIZEEBYTES];
_GLOBAL USINT *TmpPtr;

int len;
_GLOBAL UDINT	PlatesMade;
_GLOBAL	BOOL	savePlatesMade;
		BOOL	loadPlatesMade;
_GLOBAL	STRING FileIOName[MAXFILENAMELENGTH];
_GLOBAL	STRING	FileType[5];
_GLOBAL	UDINT *FileIOData;
_GLOBAL	UDINT	FileIOLength;
_GLOBAL	BOOL	WriteFileCmd,ReadFileCmd,DeleteFileCmd,WriteFileOK,ReadFileOK,DeleteFileOK,
				FileNotExisting,ReadDirCmd,ReadDirOK,NoDisk;
_GLOBAL	BOOL ParameterChanged;

_LOCAL	BOOL	StartUp,ReadActive,WriteActive,SaveToFile;
_LOCAL	BOOL	ClearSRAM,LoadSRAMFromFile;
_GLOBAL BOOL	PutTmpDataToSRAM,GetTmpDataFromSRAM;
BOOL	DontResetMotorData;
_GLOBAL BOOL MirroredMachine;

_GLOBAL SINT SRAM[OVERALLSRAMSIZEEBYTES]	_VAR_RETAIN;
_LOCAL INT	Test1,Test2;

_LOCAL	STRING	MagicNumber[10];
_GLOBAL	DATE_AND_TIME	OffTime,OnTime;

void SRAMPut(puttyp *param)
{
	memcpy(&SRAM[0]+param->offset*2,(SINT *)(param->address),param->len *2);
}

void SRAMGet(gettyp *param)
{
	memcpy((SINT *)(param->address),&SRAM[0]+param->offset*2,param->len *2);
}

void InitXData(void)
{
	ClearSRAM = 0;
	X_Param.RefSpeed1		= 20;
	X_Param.RefSpeed2		= 10;
	X_Param.RefSpeed3		= 5;
	X_Param.ManSpeed1		= 20;
	X_Param.ManSpeed2		= 50;
	X_Param.ParkSpeed		= 195;
/*Positionen*/
	X_Param.Offset			= 0;		/*Nullpunktkorrektur*/
	X_Param.ParkPosition	= -3.0;
	X_Param.InkrementeProMm	= 2730.666;
	X_Param.MinPosition	= -37.0;
	X_Param.MaxPosition	= 1258.0;
}

void InitGlobalData(void)
{
	PlatesMade = 0;

	PlateParameter.XOffset = 0.0;
	PlateParameter.YOffset = 0.0;
	PlateParameter.Length  = 320.0;
	PlateParameter.Width   = 600.0;
	PlateParameter.Thickness = 0.30;
	PlateParameter.Sensitivity = 100.0;

	strcpy(GlobalParameter.MachineName,"LS Performance XXL");
	strcpy(GlobalParameter.MachineNumber,"12345");
	strcpy(GlobalParameter.CustomerName,"eintragen!!");
	GlobalParameter.ScansPerMinute			= 96000;
	GlobalParameter.Resolution				= 1270;
	GlobalParameter.MaxPlateLength			= 1250; /*not in use*/
	GlobalParameter.MinPlateLength			= 250;  /*not in use*/
	GlobalParameter.TrolleyRightOffset		= 73650;/*not in use*/
	GlobalParameter.AlternatingTrolleys		= FALSE;/*not in use*/
	GlobalParameter.XSimulation				= FALSE;
	GlobalParameter.TBSimulation			= FALSE;
	GlobalParameter.ProcessorSimulation		= FALSE;
	GlobalParameter.PunchSimulation			= FALSE;
	GlobalParameter.PaperRemoveEnabled		= TRUE;
	GlobalParameter.UseOldTiffBlaster		= FALSE;/*not in use*/
	GlobalParameter.Dummy					= FALSE;
	GlobalParameter.Language				= 1;
	GlobalParameter.DeloaderTurnStation		= 0;
	GlobalParameter.FlexibleFeeder			= 0;
	GlobalParameter.AutomaticTrolleyOpenClose	= TRUE;
	GlobalParameter.TrolleyLeft				= 0;
	GlobalParameter.TrolleyRight			= 0;
	GlobalParameter.RealLaserPower			= 300;
	GlobalParameter.LaserPower = (GlobalParameter.RealLaserPower+5) / 10;
	GlobalParameter.MirroredMachine			= 0; /* !=42: not mirrored; 42: mirrored*/
	GlobalParameter.SlowModeWaitTime		= 2000; /* 20 sec default */
	GlobalParameter.DisableSlowMode			= 1;

	Expose_Param.ScansPerMinute			= 96000;
	Expose_Param.Resolution				= 1270;

/*TROLLEYS*/
	for(i = 0; i < MAXTROLLEYS; i++)
	{
		strcpy( Trolleys[i].Name,"Trolley");
		Trolleys[i].Number 				= i;	/*bugfix i instead of i+1*/
		Trolleys[i].PlateType 			= 1;
		Trolleys[i].Empty 				= FALSE;
		Trolleys[i].Single 				= FALSE;
		Trolleys[i].Double 				= TRUE;
		Trolleys[i].NoCover		 		= FALSE;
		Trolleys[i].LeftStack 			= TRUE;
		Trolleys[i].RightStack 			= FALSE;
		Trolleys[i].PlatesLeft 			= 0;
		Trolleys[i].PlatesRight 		= 0;
		Trolleys[i].OpenStart 			= 175600;
		Trolleys[i].OpenStop 			= 353100;
		Trolleys[i].CloseStart 			= 353100;
		Trolleys[i].CloseStop 			= 174440;
		Trolleys[i].TakePlateLeft 		= 8300;  /* not in use*/
		Trolleys[i].TakePlateRight 		= 57000; /* not in use*/
		Trolleys[i].TakePaperLeft 		= 335000;
		Trolleys[i].TakePaperRight 		= 335000;
		Trolleys[i].EmptyLeft 			= FALSE;
		Trolleys[i].EmptyRight 			= FALSE;
		Trolleys[i].EmptyPositions[0] 			= 64000;
		Trolleys[i].EmptyPositions[1] 			= 64000;
		Trolleys[i].EmptyPositions[2] 			= 64000;
	}


/*PLATETYPES*/
	for(i = 0; i < MAXPLATETYPES; i++)
	{
		strcpy( PlateTypes[i].Name,"Platte");
		PlateTypes[i].Number 				= i; /*bugfix i instead of i+1*/
		PlateTypes[i].Length 				= 312.00;
		PlateTypes[i].XOffset 				= 0.00;
		PlateTypes[i].Width 				= 600.00;
		PlateTypes[i].YOffset 				= 10.00;
		PlateTypes[i].Thickness 				= 0.32;
		PlateTypes[i].TakePlatePosition 				= 100000;
		PlateTypes[i].ReleasePlatePosition 			= 10000;
		PlateTypes[i].ReleasePlatePosition2 			= 10000;
		PlateTypes[i].TurnMode							= 0;
		PlateTypes[i].SeparateBeltTracks	= 0;
	}

/*FEEDER*/
	FeederParameter.HorizontalPositions.ReleasePlate 				= 500;/* not in use*/
	FeederParameter.HorizontalPositions.TakePlate 				= 1500;/* not in use*/
	FeederParameter.VerticalPositions.Up 							= 2000;
	FeederParameter.VerticalPositions.EnablePaperRemove 		= 22000;
	FeederParameter.VerticalPositions.AboveTrolley 				= 22000;
	FeederParameter.VerticalPositions.MaxDown	 				= 70000;
	FeederParameter.VerticalPositions.PreRelease 					= 2000;
	FeederParameter.VerticalPositions.ReleasePlate 				= 2000;
	FeederParameter.VerticalPositions.SpeedChangePosition 		= 18200;
	FeederParameter.VacuumOnTime								= 100;
	FeederParameter.VacuumOffTime								= 50;
	FeederParameter.SpeedUpSlow									= 700;
	FeederParameter.SpeedUpFast									= 7000;
	FeederParameter.Stack0Pos									= -186000;/* not in use*/
	FeederParameter.Stack20Pos									= -183248;/* not in use*/
	FeederParameter.SlowDownPos									= 1000;
	FeederParameter.StackDetectSpeed							= 500;
	FeederParameter.PanoramaAdapter							= 25000;
	FeederParameter.SuckRepitions								= 0;
	FeederParameter.ShakeRepitions								= 0;
	FeederParameter.ShakeTime									= 30;
	FeederParameter.ShakeWay									= 3000;
	FeederParameter.WaitForExposer								= 0;
	FeederParameter.IgnoreVacuumSwitch							= 0;
	FeederParameter.HorPositionPano							= 2000;
	FeederParameter.MaxTablePosition							= 170.0;
	FeederParameter.MinTablePosDrop							= 1180.0;
	FeederParameter.DropOffset									= 20.0;/* not in use */

/* Paper Remove */
	PaperRemoveParameter.HorizontalPositions.EnableFeeder 			= 126000;
	PaperRemoveParameter.HorizontalPositions.PaperReleaseSingle		= 70000;
	PaperRemoveParameter.HorizontalPositions.PaperReleasePanorama	= 50000;

/*folgende Werte werden im Ablauf errechnet*/
	PaperRemoveParameter.HorizontalPositions.PaperRemoveLeft		= -120000;
	PaperRemoveParameter.HorizontalPositions.PaperRemoveRight		= -196000;
	PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStart	= -196000;
	PaperRemoveParameter.HorizontalPositions.TrolleyCloseRightStop	= -178000;
	PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStart	= -178000;
	PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop	= -196000;
	PaperRemoveParameter.HorizontalPositions.PaletteDetectStartPos	=  120000;
	PaperRemoveParameter.HorizontalPositions.PaletteDetectEndPos	=  130000;
	PaperRemoveParameter.HorizontalPositions.TrolleyOpenRightStop	= -196000;
/* bis hier*/
	PaperRemoveParameter.VerticalPositions.Up						= 300;
	PaperRemoveParameter.VerticalPositions.Down						= 13000;
	PaperRemoveParameter.VerticalPositions.OnTrolley				= 1200;
	PaperRemoveParameter.TrolleyOpenCloseSpeed						= 1000;
	PaperRemoveParameter.PaperRemoveSpeed							= 5000;
	PaperRemoveParameter.TrolleyDetectSpeed							= 500;
	PaperRemoveParameter.TrolleyOpenDownSpeed						= 300;
	PaperRemoveParameter.TrolleyOpenUpSpeed							= 800;
	PaperRemoveParameter.PaperRemoveDownSpeed						= 1000;
	PaperRemoveParameter.PaperRemoveUpSpeed							= 1500;
	PaperRemoveParameter.TrolleyDetectStartPos						= 180000;
	PaperRemoveParameter.TrolleyDetectEndPos						= 280000;
	PaperRemoveParameter.MaxCurrentDown								= 2000;
	PaperRemoveParameter.MaxCurrentUp								= 5000;
	PaperRemoveParameter.GripOnTime									= 100;
	PaperRemoveParameter.GripOffTime								= 80;
	PaperRemoveParameter.TimeoutOnTrolley							= 100;
	PaperRemoveParameter.PaletteDetectSpeed							= 300;
	PaperRemoveParameter.TimeoutUpFromTrolley						= 100;
	PaperRemoveParameter.TimeoutOpenTrolleySmall					= 1000;/* not in use*/
	PaperRemoveParameter.TimeoutOpenTrolleyBig						= 2000;/* not in use*/
	PaperRemoveParameter.PaperGripCycles							= 0;
	PaperRemoveParameter.HorPositionPano							= 300000;
	PaperRemoveParameter.PaperReleasePosVertical					= 10000;


/*Adjuster*/
	AdjusterParameter.PinsUpTime									= 20;
	AdjusterParameter.PinsDownTime									= 20;
	AdjusterParameter.TableVacuumOnDelay							= 10;
	AdjusterParameter.TableVacuumOffDelay							= 20;
	AdjusterParameter.AdjVacuumOnDelay							= 50;
	AdjusterParameter.AdjVacuumOffDelay							= 10;
	AdjusterParameter.BlowairOnDelay								= 10;
	AdjusterParameter.BlowairOffDelay								= 10;
	AdjusterParameter.TimeoutAdjustedPosition						= 100;
	AdjusterParameter.MaxFailedPlates								= 0;
	AdjusterParameter.AddBlowairValveDelay						    = 1;
	AdjusterParameter.MaxAttempts									= 3;
	AdjusterParameter.AdjActiveTime									= 30;
	AdjusterParameter.AdjInactiveTime								= 30;
	AdjusterParameter.AdjustSim										= FALSE;
	AdjusterParameter.FeederAdjustposition							= 6450;
	AdjusterParameter.FeederFixposition							    = 7500;

/*DELOADER*/
	DeloaderParameter.MaxPlateTakePosition							= 100000;
	DeloaderParameter.PlateReleasePosition							= 10000;
	DeloaderParameter.PlateReleasePosition2							= 10000;
	DeloaderParameter.TurnEnabledPosition							= 50000;
	DeloaderParameter.MinTablePositionTake							= 235.0;
	DeloaderParameter.MaxTablePositionRelease						= 500.0;
	DeloaderParameter.Turn0Position									= 15400;
	DeloaderParameter.Turn90Position								= 26100;
	DeloaderParameter.Turn180Position								= 36800;
	DeloaderParameter.ConveyorBeltSpeed								= 300;
	DeloaderParameter.TableDeloadSpeedManual								= 200.0;
	DeloaderParameter.TableDeloadSpeed								= 200.0;
	DeloaderParameter.TurnSpeed										= 2000;
	DeloaderParameter.VacuumOnTime									= 50;
	DeloaderParameter.VacuumOffTime									= 50;
	DeloaderParameter.PlateLengthAddition								= 120;
	DeloaderParameter.ConveyorBeltDelay								= 500;
	DeloaderParameter.EGMStopDelay									= 4500;
	DeloaderParameter.ShiftedLayingOff								= 0;
	DeloaderParameter.PinsUpDelay									= 200;
	DeloaderParameter.PinsDownDelay								= 200;
	DeloaderParameter.EnableStopPins								= 0;
	DeloaderParameter.SeparateSensors								= 0;
	DeloaderParameter.StartTablePosition                           = -6.0;
	DeloaderParameter.EndTablePosition                             = 1258;
	DeloaderParameter.ConvLateralMoveTime                          = 200;
	DeloaderParameter.ConvLateralPulseTime                          = 30;

	for (i=1;i<MAXLASERPOWERSETTINGS;i++)
	{
		LaserPowerSettings[i].Resolution = 0;
		for (j = 1;j<MAXPLATETYPES;j++)
			LaserPowerSettings[i].LaserPower[j] = 0;
	}
}


void InitMotorData(void)
{
	Motors[FEEDER_VERTICAL].Parameter.ControllerIFact						= 20;
	strcpy(Motors[FEEDER_VERTICAL].Parameter.Function,"Feeder vertical");
	Motors[FEEDER_VERTICAL].Parameter.Priority						= 1;
	Motors[FEEDER_VERTICAL].Parameter.IncrementsPerMm				= 140;
	Motors[FEEDER_VERTICAL].Parameter.ReferenceSpeed				= 500;
	Motors[FEEDER_VERTICAL].Parameter.ReferenceDirectionNegative	= 0;
	Motors[FEEDER_VERTICAL].Parameter.ReferencePeekCurrent			= 3000;
	Motors[FEEDER_VERTICAL].Parameter.DefaultPeekCurrent			= 8000;
	Motors[FEEDER_VERTICAL].Parameter.ReferenceContCurrent			= 2500;
	Motors[FEEDER_VERTICAL].Parameter.DefaultContCurrent			= 2000;
	Motors[FEEDER_VERTICAL].Parameter.ReferencePropFact				= 2;
	Motors[FEEDER_VERTICAL].Parameter.DefaultPropFact				= 8;
	Motors[FEEDER_VERTICAL].Parameter.ReferenceOffset				= 2000;
	Motors[FEEDER_VERTICAL].Parameter.ReferenceAcceleration			= 50;
	Motors[FEEDER_VERTICAL].Parameter.DefaultAcceleration			= 300;
	Motors[FEEDER_VERTICAL].Parameter.ManSpeed						= 500;
	Motors[FEEDER_VERTICAL].Parameter.MaximumSpeed					= 10000;
	Motors[FEEDER_VERTICAL].Parameter.MaximumPosition				= 70000;
	Motors[FEEDER_VERTICAL].Parameter.MinimumPosition				= 100;
	Motors[FEEDER_VERTICAL].Parameter.PositionWindow				= 200;

	Motors[FEEDER_HORIZONTAL].Parameter.ControllerIFact						= 20;
	strcpy(Motors[FEEDER_HORIZONTAL].Parameter.Function,"Feeder horizontal");
	Motors[FEEDER_HORIZONTAL].Parameter.Priority					= 1;
	Motors[FEEDER_HORIZONTAL].Parameter.IncrementsPerMm				= 140;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferenceSpeed				= 500;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferenceDirectionNegative	= 1;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferencePeekCurrent		= 3000;
	Motors[FEEDER_HORIZONTAL].Parameter.DefaultPeekCurrent			= 8000;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferenceContCurrent		= 2500;
	Motors[FEEDER_HORIZONTAL].Parameter.DefaultContCurrent			= 6000;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferencePropFact			= 2;
	Motors[FEEDER_HORIZONTAL].Parameter.DefaultPropFact				= 8;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferenceOffset				= 2000;
	Motors[FEEDER_HORIZONTAL].Parameter.ReferenceAcceleration		= 50;
	Motors[FEEDER_HORIZONTAL].Parameter.DefaultAcceleration			= 300;
	Motors[FEEDER_HORIZONTAL].Parameter.ManSpeed					= 500;
	Motors[FEEDER_HORIZONTAL].Parameter.MaximumSpeed				= 10000;
	Motors[FEEDER_HORIZONTAL].Parameter.MaximumPosition				= 100000;
	Motors[FEEDER_HORIZONTAL].Parameter.MinimumPosition				= 100;
	Motors[FEEDER_HORIZONTAL].Parameter.PositionWindow				= 100;

	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ControllerIFact					= 20;
	strcpy(Motors[PAPERREMOVE_HORIZONTAL].Parameter.Function,"Papierentf. horiz.");
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.Priority				= 1;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.IncrementsPerMm		= 140;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferenceSpeed			= 500;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferenceDirectionNegative	= 0;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferencePeekCurrent	= 6000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.DefaultPeekCurrent		= 8000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferenceContCurrent	= 4000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.DefaultContCurrent		= 6000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferencePropFact		= 6;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.DefaultPropFact		= 8;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferenceOffset		= 126000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ReferenceAcceleration	= 50;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.DefaultAcceleration	= 300;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.ManSpeed				= 500;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.MaximumSpeed			= 10000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.MaximumPosition		= 360000;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.MinimumPosition		= 500;
	Motors[PAPERREMOVE_HORIZONTAL].Parameter.PositionWindow			= 1000;

	Motors[PAPERREMOVE_VERTICAL].Parameter.ControllerIFact					= 20;
	strcpy(Motors[PAPERREMOVE_VERTICAL].Parameter.Function,"Papierentf. vertik.");
	Motors[PAPERREMOVE_VERTICAL].Parameter.Priority					= 1;
	Motors[PAPERREMOVE_VERTICAL].Parameter.IncrementsPerMm			= 140;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferenceSpeed			= 200;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferenceDirectionNegative	= 1;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferencePeekCurrent		= 6000;
	Motors[PAPERREMOVE_VERTICAL].Parameter.DefaultPeekCurrent		= 8000;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferenceContCurrent		= 4500;
	Motors[PAPERREMOVE_VERTICAL].Parameter.DefaultContCurrent		= 4000;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferencePropFact		= 2;
	Motors[PAPERREMOVE_VERTICAL].Parameter.DefaultPropFact			= 8;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferenceOffset			= 300;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ReferenceAcceleration	= 50;
	Motors[PAPERREMOVE_VERTICAL].Parameter.DefaultAcceleration		= 300;
	Motors[PAPERREMOVE_VERTICAL].Parameter.ManSpeed					= 500;
	Motors[PAPERREMOVE_VERTICAL].Parameter.MaximumSpeed				= 1000;
	Motors[PAPERREMOVE_VERTICAL].Parameter.MaximumPosition			= 13000;
	Motors[PAPERREMOVE_VERTICAL].Parameter.MinimumPosition			= 100;
	Motors[PAPERREMOVE_VERTICAL].Parameter.PositionWindow			= 30;

	memcpy(&Motors[PAPERREMOVE_VERTICAL2],&Motors[PAPERREMOVE_VERTICAL],sizeof(Motors[PAPERREMOVE_VERTICAL]));
	strcpy(Motors[PAPERREMOVE_VERTICAL].Parameter.Function,"Papierentf. vertik. 2");

	Motors[ADJUSTER].Parameter.ControllerIFact								= 20;
	strcpy(Motors[ADJUSTER].Parameter.Function,"Ausrichtband");
	Motors[ADJUSTER].Parameter.Priority								= 1;
	Motors[ADJUSTER].Parameter.IncrementsPerMm						= 96;
	Motors[ADJUSTER].Parameter.ReferenceSpeed						= 500;
	Motors[ADJUSTER].Parameter.ReferenceDirectionNegative			= 1;
	Motors[ADJUSTER].Parameter.ReferencePeekCurrent					= 3000;
	Motors[ADJUSTER].Parameter.DefaultPeekCurrent					= 8000;
	Motors[ADJUSTER].Parameter.ReferenceContCurrent					= 2500;
	Motors[ADJUSTER].Parameter.DefaultContCurrent					= 6000;
	Motors[ADJUSTER].Parameter.ReferencePropFact					= 2;
	Motors[ADJUSTER].Parameter.DefaultPropFact						= 8;
	Motors[ADJUSTER].Parameter.ReferenceOffset						= 2000;
	Motors[ADJUSTER].Parameter.ReferenceAcceleration				= 50;
	Motors[ADJUSTER].Parameter.DefaultAcceleration					= 200;
	Motors[ADJUSTER].Parameter.ManSpeed								= 500;
	Motors[ADJUSTER].Parameter.MaximumSpeed							= 10000;
	Motors[ADJUSTER].Parameter.MaximumPosition						= 99999999;
	Motors[ADJUSTER].Parameter.MinimumPosition						= -99999999;
	Motors[ADJUSTER].Parameter.PositionWindow						= 100;

	Motors[DELOADER_TURN].Parameter.ControllerIFact					= 2;
	strcpy(Motors[DELOADER_TURN].Parameter.Function,"Entlader Rot.");
	Motors[DELOADER_TURN].Parameter.Priority						= 1;
	Motors[DELOADER_TURN].Parameter.IncrementsPerMm					= 200;
	Motors[DELOADER_TURN].Parameter.ReferenceSpeed					= 250;
	Motors[DELOADER_TURN].Parameter.ReferenceDirectionNegative		= 1;
	Motors[DELOADER_TURN].Parameter.ReferencePeekCurrent			= 3000;
	Motors[DELOADER_TURN].Parameter.DefaultPeekCurrent				= 8000;
	Motors[DELOADER_TURN].Parameter.ReferenceContCurrent			= 2500;
	Motors[DELOADER_TURN].Parameter.DefaultContCurrent				= 3000;
	Motors[DELOADER_TURN].Parameter.ReferencePropFact				= 4;
	Motors[DELOADER_TURN].Parameter.DefaultPropFact					= 8;
	Motors[DELOADER_TURN].Parameter.ReferenceOffset					= 14800;
	Motors[DELOADER_TURN].Parameter.ReferenceAcceleration			= 50;
	Motors[DELOADER_TURN].Parameter.DefaultAcceleration				= 100;
	Motors[DELOADER_TURN].Parameter.ManSpeed						= 100;
	Motors[DELOADER_TURN].Parameter.MaximumSpeed					= 700;
	Motors[DELOADER_TURN].Parameter.MaximumPosition					= 50000;
	Motors[DELOADER_TURN].Parameter.MinimumPosition					= 100;
	Motors[DELOADER_TURN].Parameter.PositionWindow					= 50;

	Motors[DELOADER_HORIZONTAL].Parameter.ControllerIFact					= 20;
	strcpy(Motors[DELOADER_HORIZONTAL].Parameter.Function,"Entlader horiz.");
	Motors[DELOADER_HORIZONTAL].Parameter.Priority					= 1;
	Motors[DELOADER_HORIZONTAL].Parameter.IncrementsPerMm			= 140;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferenceSpeed			= 500;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferenceDirectionNegative	= 1;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferencePeekCurrent		= 3000;
	Motors[DELOADER_HORIZONTAL].Parameter.DefaultPeekCurrent		= 8000;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferenceContCurrent		= 2500;
	Motors[DELOADER_HORIZONTAL].Parameter.DefaultContCurrent		= 4000;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferencePropFact			= 2;
	Motors[DELOADER_HORIZONTAL].Parameter.DefaultPropFact			= 8;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferenceOffset			= 50000;
	Motors[DELOADER_HORIZONTAL].Parameter.ReferenceAcceleration		= 50;
	Motors[DELOADER_HORIZONTAL].Parameter.DefaultAcceleration		= 300;
	Motors[DELOADER_HORIZONTAL].Parameter.ManSpeed					= 500;
	Motors[DELOADER_HORIZONTAL].Parameter.MaximumSpeed				= 5000;
	Motors[DELOADER_HORIZONTAL].Parameter.MaximumPosition			= 110000;
	Motors[DELOADER_HORIZONTAL].Parameter.MinimumPosition			= 100;
	Motors[DELOADER_HORIZONTAL].Parameter.PositionWindow			= 200;

	Motors[CONVEYORBELT].Parameter.ControllerIFact							= 20;
	strcpy(Motors[CONVEYORBELT].Parameter.Function,"Auslaufband");
	Motors[CONVEYORBELT].Parameter.Priority							= 1;
	Motors[CONVEYORBELT].Parameter.IncrementsPerMm					= 100;
	Motors[CONVEYORBELT].Parameter.ReferenceSpeed					= 500;
	Motors[CONVEYORBELT].Parameter.ReferenceDirectionNegative		= 1;
	Motors[CONVEYORBELT].Parameter.ReferencePeekCurrent				= 3000;
	Motors[CONVEYORBELT].Parameter.DefaultPeekCurrent				= 8000;
	Motors[CONVEYORBELT].Parameter.ReferenceContCurrent				= 2500;
	Motors[CONVEYORBELT].Parameter.DefaultContCurrent				= 3000;
	Motors[CONVEYORBELT].Parameter.ReferencePropFact				= 2;
	Motors[CONVEYORBELT].Parameter.DefaultPropFact					= 8;
	Motors[CONVEYORBELT].Parameter.ReferenceOffset					= 500;
	Motors[CONVEYORBELT].Parameter.ReferenceAcceleration			= 50;
	Motors[CONVEYORBELT].Parameter.DefaultAcceleration				= 100;
	Motors[CONVEYORBELT].Parameter.ManSpeed							= 500;
	Motors[CONVEYORBELT].Parameter.MaximumSpeed						= 1000;
	Motors[CONVEYORBELT].Parameter.MaximumPosition					= 99999999;
	Motors[CONVEYORBELT].Parameter.MinimumPosition					= -99999999;
	Motors[CONVEYORBELT].Parameter.PositionWindow					= 50;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void EGMSetDefaults(void)
{
	int i;
	char tmpstr[5];

/* Globals*/
	strcpy(EGMGlobalParam.Version,gVERSION);
	EGMGlobalParam.MachineType =						BLUEFIN_XS;
	MachineType = EGMGlobalParam.MachineType;
	EGMGlobalParam.Unit_Rpm = 							1;
	EGMGlobalParam.Unit_Propulsion = 					1;
	EGMGlobalParam.Unit_Temperature = 					1;
	EGMGlobalParam.Unit_Length = 						0;
	EGMGlobalParam.Unit_Area = 							0;
	EGMGlobalParam.Unit_Volume = 						0;
	EGMGlobalParam.MainMotorFactor = 					3.0/32767.0;
	EGMGlobalParam.MaxPlateSizeX = 						1050;
	EGMGlobalParam.MaxPlateSizeY = 						650;
	EGMGlobalParam.BrushFactor =                        230.0/32767.0;
	EGMGlobalParam.UseGumFlowControl =					0;
	EGMGlobalParam.UseDevCircFlowControl =				0;
	EGMGlobalParam.UsePrewashFlowControl = 				0;
	EGMGlobalParam.UseRinseFlowControl = 				0;
	EGMGlobalParam.UseTopUpFlowControl = 				0;
	EGMGlobalParam.UseReplFlowControl = 				0;
	EGMGlobalParam.PlateDirection = 					0;	/* 0=right to left, 1=left to right */
	EGMGlobalParam.DeveloperPulsesPerLiter =			1200;
	EGMGlobalParam.ReplenisherPulsesPerLiter =			1200;
	EGMGlobalParam.GumPulsesPerLiter =					1200;
	EGMGlobalParam.ScreensaverTime =					300; /* in sec -> 5 min*/

/* Main Motor*/
	EGMMainMotor.Auto = 0;
	EGMMainMotor.Manual = 1;
	EGMMainMotor.Start = 0;
	EGMMainMotor.Stop = 0;
	EGMMainMotor.Enable = 0;
	EGMMainMotor.RatedRpm = 0;
	EGMMainMotor.AutoRatedRpm = 0;
	EGMMainMotor.RealRpm = 0;

/* brush Motor*/
	EGMBrushMotor.Auto = 0;
	EGMBrushMotor.Manual = 1;
	EGMBrushMotor.Start = 0;
	EGMBrushMotor.Stop = 0;
	EGMBrushMotor.Enable = 0;
	EGMBrushMotor.RatedRpm = 0;
	EGMBrushMotor.AutoRatedRpm = 0;
	EGMBrushMotor.RealRpm = 0;

/* Automatik*/
	EGMAutoParam.Speed =					19338;
	EGMAutoParam.BrushSpeed =				25700;
	EGMAutoParam.PreheatTemp = 				1600;
	EGMAutoParam.PreheatTemp2 = 			1600;
	EGMAutoParam.DeveloperTemp = 			230;
	EGMAutoParam.StandbyDelay =				3000;
	EGMAutoParam.StandbySpeed =			8595;
	EGMAutoParam.MaxPlateSizeX =			1050;
	EGMAutoParam.MaxPlateSizeY =			650;
	EGMAutoParam.StandbyBrushSpeed =		5507;
	EGMAutoParam.WaitGumBackTime = 		120;
	EGMAutoParam.GumRinsingTime = 			120;
	EGMAutoParam.WaitGumRinseTime = 		50;

/* Preheat*/
	for (i=0;i<4;i++)
	{
		EGMPreheatParam.RatedTemp[i] = 				1600;
		EGMPreheatParam.RatedTempCorr[i] =			1600;
		EGMPreheatParam.ReglerParam[i].Kp =			3.8;
		EGMPreheatParam.ReglerParam[i].Tn =			48000.0;
		EGMPreheatParam.ReglerParam[i].Tv =			12000.0;
		EGMPreheatParam.ReglerParam[i].Tf =			1200.0;
		EGMPreheatParam.ReglerParam[i].Kw =			1.0;
		EGMPreheatParam.ReglerParam[i].Kfbk =		1.0;
		EGMPreheatParam.ReglerParam[i].d_mode =	1;
		EGMPreheatParam.ReglerParam[i].tminpuls =	10;
		EGMPreheatParam.ReglerParam[i].tperiod =		200;
		EGMPreheatParam.ReglerParam[i].SetpointTracking =	1;

		EGMPreheatParam.EndSetpoint[i] =				1600;
		EGMPreheatParam.StartOverheat[i] =			750;
		EGMPreheatParam.StartAmbient[i] =			300;
		EGMPreheatParam.EndAmbient[i] =				940;
	}

	EGMPreheatParam.ToleratedDevPos =			30;
	EGMPreheatParam.ToleratedDevNeg =			30;
	EGMPreheatParam.MaxOKTemp =				1630;
	EGMPreheatParam.MinOKTemp =				1570;
	EGMPreheatParam.SetPointCorrTMin=			30;
	EGMPreheatParam.SetPointCorrMax=			500;

/* Entwickler*/
	EGMDeveloperTankParam.RatedTemp =				230;
	EGMDeveloperTankParam.TempOffset =				-17;
	EGMDeveloperTankParam.ToleratedDevPos =			10;
	EGMDeveloperTankParam.ToleratedDevNeg =			10;
	EGMDeveloperTankParam.CoolingOnY =					-15;
	EGMDeveloperTankParam.MaxOKTemp =				240;
	EGMDeveloperTankParam.MinOKTemp =				220;
	EGMDeveloperTankParam.MinOnTimeCooling =			700;

	EGMDeveloperTankParam.StartRefillDelay =				0;
	EGMDeveloperTankParam.StopRefillDelay =				0;

	EGMDeveloperTankParam.ReglerParam.Kp =			10.0;
	EGMDeveloperTankParam.ReglerParam.Tn =			0.0;
	EGMDeveloperTankParam.ReglerParam.Tv =			400.0;
	EGMDeveloperTankParam.ReglerParam.Tf =				100.0;
	EGMDeveloperTankParam.ReglerParam.Kw =			1.0;
	EGMDeveloperTankParam.ReglerParam.Kfbk =			1.0;
	EGMDeveloperTankParam.ReglerParam.d_mode =		1;
	EGMDeveloperTankParam.ReglerParam.tminpuls =		1;
	EGMDeveloperTankParam.ReglerParam.tperiod =		20;
	EGMDeveloperTankParam.ReglerParam.SetpointTracking =	0;

	EGMDeveloperTankParam.ReplenishingMode =			1;
	EGMDeveloperTankParam.EnableStandbyReplenishing =	1;
	EGMDeveloperTankParam.EnableOffReplenishing =		0;
	EGMDeveloperTankParam.TopUpMode =						1;
	EGMDeveloperTankParam.ReplenishmentPerSqm =		100.0;
	EGMDeveloperTankParam.ReplenishmentPerPlate =		20.0;
	EGMDeveloperTankParam.StandbyReplenishment =		50.0;
	EGMDeveloperTankParam.OffReplenishment =			20.0;
	EGMDeveloperTankParam.StandbyReplenishmentIntervall = 90;
	EGMDeveloperTankParam.PumpMlPerSec =				30.0;
	EGMDeveloperTankParam.MinPumpOnTime =			300;
	EGMDeveloperTankParam.MinPumpOnTimeStandby =	300;

	EGMDeveloperTankParam.TopUpMode =				1;
	EGMDeveloperTankParam.TopUpPerSqm =				50.0;
	EGMDeveloperTankParam.TopUpPerPlate =			20.0;
	EGMDeveloperTankParam.TopUpPumpMlPerSec =		30.0;
	EGMDeveloperTankParam.TopUpMinPumpOnTime =		300;

/* PreWash*/
	EGMPrewashParam.ReplenishingMode =				1;
	EGMPrewashParam.ReplenishmentPerSqm =			1000.0;
	EGMPrewashParam.ReplenishmentPerPlate = 			300.0;
	EGMPrewashParam.ReplenishmentIntervall = 			0;
	EGMPrewashParam.PumpMlPerSec =					212.0;
	EGMPrewashParam.MinPumpOnTime =					200;

/* Rinse*/
	EGMRinseParam.ReplenishingMode =							1;
	EGMRinseParam.ReplenishmentPerSqm =						500.0;
	EGMRinseParam.ReplenishmentPerPlate = 						150.0;
	EGMRinseParam.ReplenishmentIntervall = 						0;
	EGMRinseParam.PumpMlPerSec =								212.0;
	EGMRinseParam.MinPumpOnTime =							200;

/* Plate types*/
	for (i=0;i<MAXPLATETYPES;i++)
	{
		brsitoa(i,(UDINT )&tmpstr[0]);
		strcpy(EGMPlateTypes[i].Name,"Plate ");
		strcat(EGMPlateTypes[i].Name,tmpstr);
		strcpy(EGMPlateTypes[i].ManufacturerName,"Company");

		EGMPlateTypes[i].SizeX	= 					312.0;
		EGMPlateTypes[i].SizeY	= 					512.0;
		EGMPlateTypes[i].Area	= 					0.16;
		EGMPlateTypes[i].RegenerationPerSqrM	= 	0.0;
		EGMPlateTypes[i].RegenerationPerPlate	= 	0.0;
		EGMPlateTypes[i].Type	= 					i;
		EGMPlateTypes[i].DipTime	= 				0.0;
	}

	OverallPlateCounter =					0;
	OverallSqmCounter =					0;
	OverallReplenisherCounter =			0;
	ReplenisherCounter =					0;
	OverallReplenisherPumpTime =			0;
	DeveloperChangeDate =				OnTime;
	ReplenisherUsedSinceDevChange =		0;
	SqmSinceDevChg =						0;


	strcpy(MagicNumber,"Herbert");
}


_INIT void init(void)
{

Created = 0;
                        DInfo.enable      = 1;
                        DInfo.pDevice   = (UDINT) "ROOT";
                        DInfo.pPath     = (UDINT) "USER";

                        /* Call FUB */
                        DirInfo(&DInfo);

                        /* Get FUB output information */
                        wStatus         = DInfo.status;
                        dwDirNum        = DInfo.dirnum;
                        dwFileNum       = DInfo.filenum;
		if (wStatus ==fiERR_DIR_NOT_EXIST)
		{
			DCreate.enable = 1;
			DCreate.pDevice   = (UDINT) "ROOT";
			DCreate.pName = (UDINT) "USER";
			DirCreate(&DCreate);
			wStatus = DCreate.status;
			if (wStatus == 0)
				Created = 1;

		}


		DontResetMotorData = 0;
		PutTmpDataToSRAM = 0;
		GetTmpDataFromSRAM = 0;
	    saveParameterData = 0;
	    saveXData = 0;
	    saveMotorData = 0;
		savePlatesMade = 0;

	    put.enable=1;
	    get.enable=1;
	    loadParameterData = 1;
		loadPlatesMade = 1;
		loadMotorData = 1;
		StartUp = 1;
		ReadActive = 0;
		WriteActive=0;
		SaveToFile = 0;
		TmpPtr = &tmp[0];
/*V1.95 make sure, all numbers are correct*/
		for(i = 0; i < MAXTROLLEYS; i++)
		{
			Trolleys[i].Number 	= i;
		}
		for(i = 0; i < MAXPLATETYPES; i++)
		{
			PlateTypes[i].Number = i;
		}
}

_CYCLIC void zykl ()
{
	Test1 = sizeof(DeloaderParameter);
	Test2= sizeof (Motors);

	if(!loadMotorData && !loadParameterData && !loadXData && StartUp)
	{
		/*Machine number valid? (not "12345" AND defaults ok?*/
#ifdef MODE_BLUEFIN
	/*MagicNumber OK-> Daten im SRAM gültig-> Datei schreiben*/
		if( !strcmp(MagicNumber,"Herbert") )
#else
		if( memcmp(GlobalParameter.MachineNumber,"12345",5) && !strcmp(Motors[MAXMOTORS].Parameter.Function,"Krause-Biagosch") )
#endif
		{
		/*save sram to file*/
			SaveToFile = 1;
		}
		else
		{
		/*try to load sram from file*/
			FileIOData = (UDINT *) &tmp[0];
			FileIOLength = sizeof(tmp);
			strcpy(FileType,"SAV");
			strcpy(FileIOName,"SRAM");

			if(!ReadActive)
			{
				ReadActive = 1;
				ReadFileCmd = 1;
				ReadFileOK = 0;
				FileNotExisting = 0;
			}
			else
			{
/*wait for reading ready*/
				if(ReadFileOK)
				{
					ReadActive = 0;
					ReadFileOK = 0;
					StartUp = 0;
					/*save to SRAM*/
				    put.offset=0;
					put.len = OVERALLSRAMSIZEWORDS; /*WORDS!!*/
					put.address=(UDINT) &tmp[0];
					SRAMPut (&put);
					loadMotorData = 1;
					loadParameterData = 1;
					loadXData = 1;
					loadPlatesMade = 1;
				}
/*file not existing*/
				if(FileNotExisting)
				{
					Test1 = 123;
/*no defaults: set defaults*/
					if(strcmp(Motors[MAXMOTORS].Parameter.Function,"Krause-Biagosch") )
					{
						InitMotorData();
						InitXData();
						InitGlobalData();
						strcpy(Motors[MAXMOTORS].Parameter.Function,"Krause-Biagosch");
						saveMotorData = 1;
						saveParameterData = 1;
						saveXData = 1;
						savePlatesMade = 1;
					}
/*no file: set defaults*/
					EGMSetDefaults();
					ReadActive = 0;
					ReadFileOK = 0;
					FileNotExisting = 0;
					StartUp = 0;

				}
			}
		}
	}


	if (saveXData)
	{
	    put.offset=0;
	    put.len=((sizeof (X_Param))/2)+1;
	    put.address=(UDINT) &X_Param;
	    SRAMPut (&put);
		saveXData = 0;
		SaveToFile = 1;
	}


	if (saveMotorData)
	{
	    put.offset=300;
	    put.len=((sizeof (Motors))/2)+1;
	    put.address=(UDINT) &Motors[0];
	    SRAMPut (&put);
	    saveMotorData = 0;
		SaveToFile = 1;
	}


	if (saveParameterData)
	{
/* Laserleistungstabelle*/
	    put.offset=3800;
	    put.len=((sizeof (LaserPowerSettings))/2)+1;
	    put.address=(UDINT) &LaserPowerSettings[0];
	    SRAMPut (&put);

	    put.offset=4000;
	    put.len=((sizeof (FeederParameter))/2)+1;
	    put.address=(UDINT) &FeederParameter;
	    SRAMPut (&put);

	    put.offset=4200;
	    put.len=((sizeof (PaperRemoveParameter))/2)+1;
	    put.address=(UDINT) &PaperRemoveParameter;
	    SRAMPut (&put);

	    put.offset=4600;
	    put.len=((sizeof (AdjusterParameter))/2)+1;
	    put.address=(UDINT) &AdjusterParameter;
	    SRAMPut (&put);

	    put.offset=4800;
	    put.len=((sizeof (GlobalParameter))/2)+1;
	    put.address=(UDINT) &GlobalParameter;
	    SRAMPut (&put);

	    put.offset=5000;
	    put.len=((sizeof (PlateParameter))/2)+1;
	    put.address=(UDINT) &PlateParameter;
	    SRAMPut (&put);

	    put.offset=5200;
	    put.len=((sizeof (DeloaderParameter))/2)+1;
	    put.address=(UDINT) &DeloaderParameter;
	    SRAMPut (&put);

	    put.offset=5400;
	    put.len=((sizeof (Trolleys))/2)+1;
	    put.address=(UDINT) &Trolleys[0];
	    SRAMPut (&put);

/*sizeof(Trolleys) = 800 + 200 spare*/

	    put.offset=6400;
	    put.len=((sizeof (PlateTypes))/2)+1;
	    put.address=(UDINT) &PlateTypes[0];
	    SRAMPut (&put);
/*sizeof(PlateTypes) = 840 */

	    put.offset=8980;
	    put.len=((sizeof (gVERSION))/2)+1;
	    put.address=(UDINT) &gVERSION[0];
	    SRAMPut (&put);

/************************************************/
/****** EGM Daten **********/
/************************************************/
	    put.offset=10010;
	    put.len=((sizeof (EGMGlobalParam))/2)+1;
	    put.address= (UDINT)&EGMGlobalParam;
	    SRAMPut (&put);

	    put.offset=10600;
	    put.len=((sizeof (EGMAutoParam))/2)+1;
	    put.address=(UDINT)&EGMAutoParam;
	    SRAMPut (&put);

	    put.offset=10800;
	    put.len=((sizeof (EGMPreheatParam))/2)+1;
	    put.address=(UDINT) &EGMPreheatParam;
	    SRAMPut (&put);

	    put.offset=11000;
	    put.len=((sizeof (EGMDeveloperTankParam))/2)+1;
	    put.address=(UDINT) &EGMDeveloperTankParam;
	    SRAMPut (&put);

	    put.offset=11200;
	    put.len=((sizeof (EGMPlateTypes))/2)+1;
	    put.address=(UDINT) &EGMPlateTypes[0];
	    SRAMPut (&put);

	    put.offset=11700;
	    put.len=((sizeof (EGMPrewashParam))/2)+1;
	    put.address=(UDINT) &EGMPrewashParam;
	    SRAMPut (&put);

	    put.offset=11800;
	    put.len=((sizeof (EGMRinseParam))/2)+1;
	    put.address=(UDINT) &EGMRinseParam;
	    SRAMPut (&put);

	    put.offset=11890;
	    put.len=((sizeof (MagicNumber))/2)+1;
	    put.address=(UDINT) &MagicNumber[0];
	    SRAMPut (&put);

	    saveParameterData = 0;
		SaveToFile = 1;
	}

	if(savePlatesMade)
	{
/* 1.47
 * Nach jeder Belichtung werden jetzt auch die Trolleydaten gespeichert,
 * damit der Platten-Füllstand nach Aus- und Einschalten korrekt angezeigt wird
 */
	    put.offset=5400;
	    put.len=((sizeof (Trolleys))/2)+1;
	    put.address=(UDINT) &Trolleys[0];
	    SRAMPut (&put);

	    put.offset=9000;
	    put.len=(sizeof (UDINT))/2;
	    put.address=(UDINT) &PlatesMade;
	    SRAMPut (&put);
	    savePlatesMade = 0;
		SaveToFile = 1;
	}

	if( SaveToFile  )
	{
		/*save sram to file*/
		if(!WriteActive)
		{
			if ( !DeleteFileCmd && !ReadFileCmd && !WriteFileCmd && !WriteFileOK && (wBildNr != FILEPIC) && (wBildNr != MESSAGEPIC))
			{
				WriteActive = 1;
				get.offset=0;
				get.len = OVERALLSRAMSIZEWORDS; /*WORDS!!*/
				get.address=(UDINT) &tmp[0];
				SRAMGet (&get);

				FileIOData = (UDINT *) &tmp[0];
				FileIOLength = sizeof(tmp);
				strcpy(FileType,"SAV");
				strcpy(FileIOName,"SRAM");
				WriteFileCmd = 1;
				WriteFileOK = 0;
			}
			/*WriteFileCmd is true? just wait...*/
		}
		else
		{
		/*wait for writing ready*/
			if(WriteFileOK)
			{
				WriteActive = 0;
				SaveToFile = 0;
				StartUp = 0;
				WriteFileOK = 0;
				ParameterChanged = 1;
			}
		}
	}



/*******************************************************/
/* LOAD*/
/*******************************************************/

	if(loadXData)
	{
		len = sizeof (X_Param);
	    get.offset=0;
		get.len = (len / 2) +1;
		get.address=(UDINT) &tmp[0];
		SRAMGet (&get);
		memcpy(&X_Param,tmp,len);
	    loadXData = 0;
	}

	if(loadMotorData)
	{
		Motor_Typ BackupMotors[MAXMOTORS+1];
		len = sizeof (Motors);
		if (DontResetMotorData)
			memcpy(&BackupMotors[0],&Motors[0],len);

	    get.offset=300;
	    get.len=(len /2 ) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&Motors[0],tmp,len);
	    loadMotorData = 0;
	    if (DontResetMotorData)
	    {
			for (i = 0;i<=MAXMOTORS;i++)
			{
				Motors[i].Error = BackupMotors[i].Error;
				Motors[i].ReferenceOk = BackupMotors[i].ReferenceOk;
				Motors[i].Moving = BackupMotors[i].Moving;
				Motors[i].StartRef = BackupMotors[i].StartRef;
				Motors[i].ReferenceStep = BackupMotors[i].ReferenceStep;
				Motors[i].timeout = BackupMotors[i].timeout;
				Motors[i].Position = BackupMotors[i].Position;
				Motors[i].Position_mm = BackupMotors[i].Position_mm;
				Motors[i].LastPosition = BackupMotors[i].LastPosition;
			}
	    }
	    else
	    {
			for (i = 0;i<=MAXMOTORS;i++)
			{
				Motors[i].Error = 0;
				Motors[i].ReferenceOk = 0;
				Motors[i].Moving = 0;
				Motors[i].StartRef = 0;
				Motors[i].ReferenceStep = 0;
				Motors[i].timeout = 0;
				Motors[i].Position = 0;
				Motors[i].Position_mm = 0;
				Motors[i].LastPosition = 0;
			}
		}
		DontResetMotorData = 0;
	}


	if(loadParameterData)
	{
/* Laserleistungstabelle*/
		len = sizeof(LaserPowerSettings);
	    get.offset=3800;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&LaserPowerSettings[0],tmp,len);

		len = sizeof(FeederParameter);
	    get.offset=4000;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&FeederParameter,tmp,len);

		len = sizeof(PaperRemoveParameter);
	    get.offset=4200;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&PaperRemoveParameter,tmp,len);
		 if (PaperRemoveParameter.PaperGripCycles>10)
		 	PaperRemoveParameter.PaperGripCycles = 2;

		len = sizeof(AdjusterParameter);
	    get.offset=4600;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&AdjusterParameter,tmp,len);

		len = sizeof(GlobalParameter);
	    get.offset=4800;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&GlobalParameter,tmp,len);
		Expose_Param.ScansPerMinute = GlobalParameter.ScansPerMinute;
		Expose_Param.Resolution  = GlobalParameter.Resolution;
		MirroredMachine = (GlobalParameter.MirroredMachine == 42);

/*safety bei update*/
		if ( (GlobalParameter.LaserPower != ((GlobalParameter.RealLaserPower+5) / 10))
			|| (GlobalParameter.RealLaserPower>1000)
		)
			GlobalParameter.RealLaserPower = GlobalParameter.LaserPower * 10;

		if( (GlobalParameter.DisableSlowMode != 0)
		 && (GlobalParameter.DisableSlowMode != 1) )
			GlobalParameter.DisableSlowMode = 1;

		if( GlobalParameter.SlowModeWaitTime > 9900 )
			GlobalParameter.SlowModeWaitTime = 2000;

		len = sizeof(PlateParameter);
	    get.offset=5000;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&PlateParameter,tmp,len);
		if(isnan(PlateParameter.Length)) PlateParameter.Length = 0.0;
		if(isnan(PlateParameter.Width)) PlateParameter.Width = 0.0;
		if(isnan(PlateParameter.XOffset)) PlateParameter.XOffset = 0.0;
		if(isnan(PlateParameter.YOffset)) PlateParameter.YOffset = 0.0;
		if(isnan(PlateParameter.Thickness)) PlateParameter.Thickness = 0.0;
		if(isnan(PlateParameter.Sensitivity)) PlateParameter.Sensitivity = 0.0;
		len = sizeof(DeloaderParameter);
	    get.offset=5200;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&DeloaderParameter,tmp,len);
		if( (DeloaderParameter.TableDeloadSpeed < 10.0) || (DeloaderParameter.TableDeloadSpeed > 1000.0) )
			DeloaderParameter.TableDeloadSpeed = 200.0;
		if( (DeloaderParameter.TableDeloadSpeedManual < 10.0) || (DeloaderParameter.TableDeloadSpeedManual > 1000.0) )
			DeloaderParameter.TableDeloadSpeedManual = 200.0;

		len = sizeof(Trolleys);
	    get.offset=5400;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&Trolleys[0],tmp,len);
/*SAFETY*/
		for(i=0;i<MAXTROLLEYS;i++)
		{
			Trolleys[i].Name[MAXTROLLEYNAMELENGTH-1] = 0;
			Trolleys[i].Number 	= i;
			if(GlobalParameter.FlexibleFeeder == FALSE)
			{
				Trolleys[i].Double 	= 1;
				Trolleys[i].Single 	= 0;
				Trolleys[i].RightStack = 0;
			}

		}

		len = sizeof(PlateTypes);
	    get.offset=6400;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&PlateTypes[0],tmp,len);
/*SAFETY*/
		for(i=0;i<MAXPLATETYPES;i++)
		{
			PlateTypes[i].Number = i;
			PlateTypes[i].Name[MAXPLATETYPENAMELENGTH-1] = 0;
			if(isnan(PlateTypes[i].Length)) PlateTypes[i].Length = 0.0;
			if(isnan(PlateTypes[i].Width)) PlateTypes[i].Width = 0.0;
			if(isnan(PlateTypes[i].XOffset)) PlateTypes[i].XOffset = 0.0;
			if(isnan(PlateTypes[i].YOffset)) PlateTypes[i].YOffset = 0.0;
			if(isnan(PlateTypes[i].Thickness)) PlateTypes[i].Thickness = 0.0;
			if(isnan(PlateTypes[i].Sensitivity)) PlateTypes[i].Sensitivity = 0.0;
			if (PlateTypes[i].SeparateBeltTracks > 1) PlateTypes[i].SeparateBeltTracks = 0;
		}



/************************************************/
/****** EGM Daten **********/
/************************************************/
	 	len = sizeof(EGMGlobalParam);
	    get.offset=10010;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMGlobalParam,tmp,len);
		strcpy(EGMGlobalParam.Version,gVERSION);
		if( (EGMGlobalParam.MachineType == BLUEFIN_XS)
		 || (EGMGlobalParam.MachineType == BLUEFIN_LOWCHEM) )
			MachineType = EGMGlobalParam.MachineType;
		else
			EGMGlobalParam.MachineType = MachineType;

	 	len = sizeof(EGMAutoParam);
	    get.offset=10600;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMAutoParam,tmp,len);

	 	len = sizeof(EGMPreheatParam);
	    get.offset=10800;
	    get.len=(len / 2) +1;
	    get.address=(UDINT) &tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMPreheatParam,tmp,len);
		PreheatParamEnter = 1;

	 	len = sizeof(EGMDeveloperTankParam);
	    get.offset=11000;
	    get.len=(len / 2) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMDeveloperTankParam,tmp,len);
		if(isnan(EGMDeveloperTankParam.PumpMlPerSec))
			EGMDeveloperTankParam.PumpMlPerSec = 20.0;
		if(isnan(EGMDeveloperTankParam.TopUpPumpMlPerSec))
			EGMDeveloperTankParam.TopUpPumpMlPerSec = 20.0;
		DeveloperTankParamEnter = 1;

	 	len = sizeof(EGMPlateTypes);
	    get.offset=11200;
	    get.len=(len / 2) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMPlateTypes[0],tmp,len);

	 	len = sizeof(EGMPrewashParam);
	    get.offset=11700;
	    get.len=(len / 2) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMPrewashParam,tmp,len);

	 	len = sizeof(EGMRinseParam);
	    get.offset=11800;
	    get.len=(len / 2) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&EGMRinseParam,tmp,len);

	 	len = sizeof(MagicNumber);
	    get.offset=11890;
	    get.len=(len / 2) +1;
	    get.address=(UDINT)&tmp[0];
	    SRAMGet (&get);
		memcpy(&MagicNumber[0],tmp,len);
/*
		if (strcmp(MagicNumber,"Herbert"))
			EGMSetDefaults();
*/

	    loadParameterData = 0;
	}

	if(loadPlatesMade)
	{
	    get.offset=9000;
	    get.len=(sizeof (UDINT))/2;
	    get.address=(UDINT) &PlatesMade;
	    SRAMGet (&get);
		loadPlatesMade = 0;
	}


	if(ClearSRAM)
	{
		for(i=0;i<OVERALLSRAMSIZEEBYTES;i++)
			tmp[i] = 255;
	    put.offset=0;
	    put.len=OVERALLSRAMSIZEWORDS;
	    put.address=(UDINT) &tmp[0];
	    SRAMPut (&put);
		ClearSRAM = 0;
	}


/* HA V1.50 give possibility to load SRAM from file */
/* to be used for copying data from one machine to another */
/*not used yet!*/
	if(LoadSRAMFromFile)
	{
		/*try to load sram from file*/
			FileIOData = (UDINT *) &tmp[0];
			FileIOLength = sizeof(tmp);
			strcpy(FileType,"SAV");
			strcpy(FileIOName,"SRAM");

			if(!ReadActive)
			{
				if ( !DeleteFileCmd && !ReadFileCmd && !WriteFileCmd && !ReadFileOK && (wBildNr != 42) && (wBildNr != MESSAGEPIC))
				{
					ReadActive = 1;
					ReadFileCmd = 1;
					ReadFileOK = 0;
					FileNotExisting = 0;
				}
			}
			else
			{
/*wait for reading ready*/
				if(ReadFileOK)
				{
					LoadSRAMFromFile = 0;
					ReadActive = 0;
					ReadFileOK = 0;
					/*save to SRAM*/
				    put.offset=0;
					put.len = OVERALLSRAMSIZEWORDS; /*WORDS!!*/
					put.address=(UDINT) &tmp[0];
					SRAMPut (&put);
					loadMotorData = 1;
					loadParameterData = 1;
					loadXData = 1;
					loadPlatesMade = 1;
				}

/*file not existing -> nothing happens*/
				if(FileNotExisting)
				{
					LoadSRAMFromFile = 0;
					ReadActive = 0;
					ReadFileOK = 0;
					FileNotExisting = 0;
				}
			}

	}


	if ( PutTmpDataToSRAM )
	{
		/*save to SRAM*/
	    put.offset=0;
		put.len = OVERALLSRAMSIZEWORDS; /*WORDS!!*/
		put.address=(UDINT) &tmp[0];
		SRAMPut (&put);
		loadMotorData = 1;
		DontResetMotorData = 1;
		loadParameterData = 1;
		loadXData = 1;
/*V2.00 gemachte Platten soll nicht mit geladen werden*/
/*		loadPlatesMade = 1;*/
		PutTmpDataToSRAM = 0;
		SaveToFile = 1;
	}

	if ( GetTmpDataFromSRAM )
	{
		get.offset=0;
		get.len = OVERALLSRAMSIZEWORDS; /*WORDS!!*/
		get.address=(UDINT) &tmp[0];
		SRAMGet (&get);
		GetTmpDataFromSRAM = 0;
	}

	if(GlobalParameter.Resolution != Expose_Param.Resolution)
	{
		GlobalParameter.Resolution = Expose_Param.Resolution;
		saveParameterData = 1;
	}
	if(GlobalParameter.ScansPerMinute != Expose_Param.ScansPerMinute)
	{
		GlobalParameter.ScansPerMinute = Expose_Param.ScansPerMinute;
		saveParameterData = 1;
	}

}



