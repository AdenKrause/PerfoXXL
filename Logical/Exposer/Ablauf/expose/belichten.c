#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Ablauf belichten */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			19.09.02	erste Implementation					HA		*/
/*	1.10		13.11.02	erweiterte TB1-Komm. (Polygonspeed)		HA		*/
/*	1.11		02.04.03	Belichtung wird im AUTO Modus nicht 	HA		*/
/*							gestartet, wenn keine Verbindung zum TIFFBlaster */
/*							und keine TB Simulation*/
/*	1.12		07.04.03	Ausgabe: Warte auf Daten..." implementiert 	HA		*/
/*																			*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"
#include "asstring.h"
#include "sys_lib.h"
#include "in_out.h"
#include <string.h>

#define	MAXFILELIST	32
#define TCPIPTIMEOUT 150

_LOCAL	USINT	ExposeStep;
_LOCAL TON_10ms_typ ExposeTimer;
USINT			tmp[20];

_GLOBAL	STRING	FileName[100];
_GLOBAL	USINT	FileListShift;
_GLOBAL	STRING	FileListName[MAXFILELIST][50];
_GLOBAL	STRING	FileListStatus[MAXFILELIST][5];
_GLOBAL	STRING	FileListTime[MAXFILELIST][20];
_GLOBAL	STRING	FileListResolution[MAXFILELIST][5];

_GLOBAL BOOL	AdjustFailed;
_GLOBAL BOOL	ExposingStarted;	/*HA 08.07.03 V1.14 Flag to inform plate taking that exposing has started*/

_GLOBAL	RTCtime_typ	RTCtime;

_GLOBAL UDINT	PlatesMade;		/* [0] = Plates made ever,[1] = plates made today*/
_GLOBAL	UDINT	PlatesMadeToday;
_GLOBAL	BOOL	savePlatesMade;

_GLOBAL USINT	ClearLine;

_LOCAL	USINT	Length,PermanentMode;

int	TimeoutCounter,i;
BOOL	StepChanged;
_LOCAL UINT	ExposureProgress;
_GLOBAL 	BOOL	SendYOffset,SendLP,SendOSCommand;
STRING tmpstr[100];
int waitcounter;

_LOCAL    UINT                CheckForDataStep;
_LOCAL    USINT               TestNumberOfPlates,TestPlateType;
static    USINT               OldSecond;
_LOCAL    BOOL                StopCheckingForData;

_INIT void init(void)
{
	StepChanged = 0;
	ExposeStep		= 0;
	ExposeStart	= 0;
	ExposureReady		= 0;
	ExposeTimer.IN	= 0;
	ExposeTimer.PT	= 10; /*just to have any value in there...*/

	Expose_Param.ScansPerMinute			= 96000;
	Expose_Param.Resolution				= 1270;

	PlatesMadeToday = 0;
	ExposingStarted = 0;
	PermanentMode = 0;
	CheckForDataStep = 0;
	StopCheckingForData = 0;
	OldSecond = 0;
	TestPlateType = 1;
}


_CYCLIC void cyclic(void)
{
UINT RequiredLaserPower;

	SequenceSteps[4] = ExposeStep;

	TON_10ms(&ExposeTimer);

/*	if(START)*/
/*HA 28.05.03 V1.13 added "|| ClearLine" */
	if( (AdjustReady) && AUTO
		&& (GlobalParameter.TBSimulation || TCPConnected || ClearLine))
		 ExposeStart=1;

/*safety*/
	if( !ExposeStart )
	{
		 PermanentMode = 0;
		 ExposeStep = 0;
		 ExposingStarted = 0;
/*		 strcpy(FileName,"----");*/
		 ExposureProgress = 0;
	}

	switch (ExposeStep)
	{
		case 0:
		{
			if(!ExposeStart) break;


			ExposeStep = 2;
			break;
		}

		case 2:
		{
			REAL AccWay = 3.0;
			if (DisableTable || AdjustStart) break; /*SAFETY*/

/* for LSJet 350: if speed is > than the usual 97000 then double acceleration/deceleration way*/
			if(Expose_Param.ScansPerMinute < 99000)
				AccWay = 3.0;
			else
				AccWay = 6.0;

			Expose_Param.PEGStartPosition = PlateParameter.XOffset+PlateParameter.Length;
			if ( Expose_Param.PEGStartPosition > X_Param.MaxPosition ) Expose_Param.PEGStartPosition = X_Param.MaxPosition;
			Expose_Param.PEGEndPosition = PlateParameter.XOffset;
			if ( Expose_Param.PEGEndPosition < X_Param.MinPosition ) Expose_Param.PEGEndPosition = X_Param.MinPosition;
			Expose_Param.StartPosition = PlateParameter.XOffset+PlateParameter.Length + AccWay;
			if ( Expose_Param.StartPosition > X_Param.MaxPosition ) Expose_Param.StartPosition = X_Param.MaxPosition;
			Expose_Param.EndPosition = PlateParameter.XOffset - AccWay;
			if ( Expose_Param.EndPosition < X_Param.MinPosition ) Expose_Param.EndPosition = X_Param.MinPosition;

			if ( FU.cmd_Target == 0 )
			{
				FU.TargetPosition_mm = Expose_Param.StartPosition;
				FU.cmd_Target = 1;
				ExposeStep = 3;
			}
			break;
		}
		case 3:
		{
/*und warten*/
			if ( abs(FU.aktuellePosition_mm - FU.TargetPosition_mm )< 2 )
			{
/*Tisch ist in Position -> OK, weiter*/
				ExposeStep = 5;
			}
			break;
		}


		case 5:
		{
/* Communication with TiffBlaster*/
			if(GlobalParameter.TBSimulation || ClearLine)
			{
				strcpy(FileName,"KrauseTestImage.TIF");
				ExposeStep = 20;
				StepChanged = 1;
				break;
			}
/*Warte auf Daten...*/
			if (DataReady)
			{
				ExposeStep = 15;
				StepChanged = 1;
				StopCheckingForData = 1;
			}

			break;
		}


/*V2.15 automatische Laserleistungseinstellung bei Aufl Wechsel*/
		case 15:
		{
			if( AdjustFailed  )
			{
				ExposeStep	= 0;
				ExposeStart	= 0;
				break;
			}

/*
if plate is longer than 600 mm then wait for adjusting to finish,
to make sure, the plate cannot be under the beam during power setting
*/
			if(!AdjustReady && AUTO && (PlateParameter.Length>600))
				break;

/*Range check for safety*/
			if(Expose_Param.Resolution < 300 || Expose_Param.Resolution > 4000)
			{
				Expose_Param.Resolution=1270;
				GlobalParameter.Resolution = 1270;
			}

/*check, which laserpower to use*/
			RequiredLaserPower = GlobalParameter.RealLaserPower;
/*gehe durch die Liste, bis sie entweder ganz abgearbeitet ist oder die passende Aufl gefunden wurde*/
			for (i=1; i<MAXLASERPOWERSETTINGS ; i++)
			{
				if ( Expose_Param.Resolution == LaserPowerSettings[i].Resolution )
				{
/*if-construct for safety*/
					if ((PlateParameter.Number>0)&& (PlateParameter.Number<MAXPLATETYPES))
						RequiredLaserPower = LaserPowerSettings[i].LaserPower[PlateParameter.Number];
					else
						RequiredLaserPower = LaserPowerSettings[i].LaserPower[1];
					break;
				}
			}

/*if the power is different from currently used power, then send new power*/
			if (RequiredLaserPower != GlobalParameter.RealLaserPower)
			{
				GlobalParameter.RealLaserPower = RequiredLaserPower;
				SendLP = 1;
			}

/*go ahead if Power setting is ready or not started*/
			if ( !SendLP )
			{
				ExposeStep = 16;
/* HA V1.36 08.05.06 send Open Shutter command to TB*/
				SendOSCommand = 1;
				StepChanged = 1;
			}
			break;
		}


/* HA V2.10  08.05.06 send Open Shutter command to TB*/
		case 16:
		{
			waitcounter = 0;
			if (SendOSCommand == 0)
				ExposeStep = 18;
			break;
		}

/* wait shortly*/
		case 18:
		{
			waitcounter++;
			if (waitcounter >= 120)/*120 cycles = 0,6 sec*/
			{
				ExposeStep = 20;
				waitcounter = 0;
				StepChanged = 1;
			}
			break;
		}

		case 20:
		{
			if(StepChanged)
			{
				if(strlen(FileName)<50)
					strcpy(&FileListName[MAXFILELIST-1][0],FileName);
				else
				{
					memcpy(&FileListName[MAXFILELIST-1][0],FileName,48);
					FileListName[MAXFILELIST-1][49]=0;
				}
				itoa(Expose_Param.Resolution,(UDINT)&FileListResolution[MAXFILELIST-1][0]);
				strcpy(&FileListStatus[MAXFILELIST-1][0],"ACT");
				itoa(RTCtime.hour,(UDINT)&tmp[0]);
				if(RTCtime.hour<=9)
					strcpy(&FileListTime[MAXFILELIST-1][0],"0");
				else
					strcpy(&FileListTime[MAXFILELIST-1][0],"");
				strcat(&FileListTime[MAXFILELIST-1][0],&tmp[0]);
				strcat(&FileListTime[MAXFILELIST-1][0],":");
				itoa(RTCtime.minute,(UDINT)&tmp[0]);
				if(RTCtime.minute<=9)
					strcat(&FileListTime[MAXFILELIST-1][0],"0");
				strcat(&FileListTime[MAXFILELIST-1][0],&tmp[0]);
				strcat(&FileListTime[MAXFILELIST-1][0],":");
				itoa(RTCtime.second,(UDINT)&tmp[0]);
				if(RTCtime.second<=9)
					strcat(&FileListTime[MAXFILELIST-1][0],"0");
				strcat(&FileListTime[MAXFILELIST-1][0],&tmp[0]);
				StepChanged = 0;
				FileListShift = 1;
			}

			if(Expose_Param.Resolution < 300 || Expose_Param.Resolution > 4000)
			{
				Expose_Param.Resolution=1270;
				GlobalParameter.Resolution = 1270;
			}


			if(Expose_Param.ScansPerMinute<1000 || Expose_Param.ScansPerMinute>150000)
				Expose_Param.ScansPerMinute = 76200;

/*calculate X-speed*/
			Expose_Param.ExposeSpeed = (25.4/60.0) * ((REAL)Expose_Param.ScansPerMinute/(REAL)Expose_Param.Resolution);

			if(Expose_Param.ExposeSpeed<1 || Expose_Param.ExposeSpeed>100)
				Expose_Param.ExposeSpeed = 25.4;

/*wait for adjusting to finish*/
			if( AdjustReady || !AUTO)
			{
				FU.cmd_Expose = 1;
/*HA 08.07.03 V1.14 Flag to inform plate taking that exposing has started*/
				ExposingStarted = 1;
				ExposeStep = 25;
				StepChanged = 1;
			}


/*HA 17.02.04 V1.74B Adjusting failed and MaxFailedPlates is 0: stop */
			if( AdjustFailed && (AdjusterParameter.MaxFailedPlates == 0) )
			{
				ExposeStep	= 0;
				ExposeStart	= 0;
				break;
			}


			if( AdjustFailed && AUTO )
			{
				strcpy(&FileListStatus[0][0],"ERR");
				itoa(RTCtime.hour,(UDINT)&tmp[0]);
				if(RTCtime.hour<=9)
					strcpy(&FileListTime[0][0],"0");
				else
					strcpy(&FileListTime[0][0],"");
				strcat(&FileListTime[0][0],&tmp[0]);
				strcat(&FileListTime[0][0],":");
				itoa(RTCtime.minute,(UDINT)&tmp[0]);
				if(RTCtime.minute<=9)
					strcat(&FileListTime[0][0],"0");
				strcat(&FileListTime[0][0],&tmp[0]);
				strcat(&FileListTime[0][0],":");
				itoa(RTCtime.second,(UDINT)&tmp[0]);
				if(RTCtime.second<=9)
					strcat(&FileListTime[0][0],"0");
				strcat(&FileListTime[0][0],&tmp[0]);

				ExposureReady = 1;
				ExposeStep = 26;
			}
			break;
		}

		case 25:
		{
			if ((FU.aktuellePosition_mm <= Expose_Param.PEGStartPosition) && (PlateParameter.Length > 1))
			{
				ExposureProgress = ((Expose_Param.PEGStartPosition - FU.aktuellePosition_mm) *100.0)
									 / PlateParameter.Length;
			}
			else
				ExposureProgress = 0;
/*Wait for Exposure ready*/
			if(FU.cmd_Expose) break;
			if(!ExposureReady)
			{
				strcpy(&FileListStatus[0][0],"RDY");
				itoa(RTCtime.hour,(UDINT)&tmp[0]);
				if(RTCtime.hour<=9)
					strcpy(&FileListTime[0][0],"0");
				else
					strcpy(&FileListTime[0][0],"");
				strcat(&FileListTime[0][0],&tmp[0]);
				strcat(&FileListTime[0][0],":");
				itoa(RTCtime.minute,(UDINT)&tmp[0]);
				if(RTCtime.minute<=9)
					strcat(&FileListTime[0][0],"0");
				strcat(&FileListTime[0][0],&tmp[0]);
				strcat(&FileListTime[0][0],":");
				itoa(RTCtime.second,(UDINT)&tmp[0]);
				if(RTCtime.second<=9)
					strcat(&FileListTime[0][0],"0");
				strcat(&FileListTime[0][0],&tmp[0]);
				StepChanged = 0;
				PlatesMade++;
				PlatesMadeToday++;
				savePlatesMade=1;
			}
			ExposureReady = 1;
			ExposeStep = 26;
			ExposureProgress = 0;

			StopCheckingForData = 0;
			DataReady = 0;
			break;
		}
		case 26:
		{

/*feeder fährt runter: Tisch darf nicht fahren*/
			if (DisableTable)
				break;

/****************************************************************************/

/*wait for deloading ready, then go on*/
			if(DeloadReady || !AUTO)
			{
				DeloadReady = 0;
				ExposureReady = 0;
				ExposeStep = 30;
			}

			break;
		}
		case 30:
		{
			if ( PermanentMode && !AUTO )
				ExposeStep = 5;
			else
				ExposeStep = 35;
			break;
		}
		case 35:
		{
			ExposeStep = 0;
			ExposeStart = 0;
			break;
		}
	} /*switch*/


/****************************************************************************/
/* V2.50 asynchronous checking for data*/
/****************************************************************************/
	if( (ReferenceStart && (ExposeStep == 0))
	 || (ClearLine == TRUE)
	)
	{
		DataReady = 0;
		StopCheckingForData = 0;
		CheckForDataStep = 0;
	}

	if(GlobalParameter.TBSimulation)
	{
/* V2.55 Durchlaufsimulation*/
		DataReady = 1;
		TestNumberOfPlates = 3;

		if (TestNumberOfPlates==0)
			TestNumberOfPlates = 1;
		if (DataReady)
		{
			PlateToDo.PlateType = TestPlateType;
		}

	}

	switch (CheckForDataStep)
	{
		case 0:
		{
/*	check for data every second */
			if( (RTCtime.second != OldSecond)
			&& !StopCheckingForData
			&& !(GlobalParameter.TBSimulation || !TCPConnected)
			&& !ClearLine
			&& !SendLP
			&& !SendOSCommand
			&& !SendYOffset
			&& !ReferenceStart
			  )
				CheckForDataStep = 5;
			break;
		}
		case 5:
		{
/* Communication with TiffBlaster*/
			OldSecond = RTCtime.second;
			if(TCPSendCmd == 0)
			{
				strcpy(TCPCmd,"15");
				TCPSendCmd = 1;
				CheckForDataStep = 6;
			}
			TimeoutCounter = 0;
			break;
		}
		case 6:
		{

			if( !TCPRcvFlag)
			{
				TimeoutCounter++;
				if(TimeoutCounter>TCPIPTIMEOUT)
				{
					TimeoutCounter = 0;
					if(GlobalParameter.TBSimulation || !TCPConnected)
					{
						CheckForDataStep = 0;
						break;
					}

					CheckForDataStep = 0;
					DataReady = 0;
					switch (GlobalParameter.Language)
					{
						case 0:
						{
							strcpy(FileName,"Warte auf TIFF Blaster...");
							break;
						}
						case 1:
						{
							strcpy(FileName,"Waiting for TIFF Blaster...");
							break;
						}
					}
				}
				break;
			}

/*****       TCP answer received   *************/
/*IP0 : wait*/
			TCPRcvFlag = 0;
			if( TCPAnswer[2]=='0')
			{
				CheckForDataStep = 7;
				TimeoutCounter = 0;
				DataReady = 0;
				switch (GlobalParameter.Language)
				{
					case 1: /*deutsch*/
					{
						strcpy(FileName,"Warte auf Daten...");
						break;
					}
					default: /*english*/
					{
						strcpy(FileName,"Waiting for data...");
					}
				}
				break;
			}
			if(TCPAnswer[2]=='1')
			{
/* Aufbau des Strings:

Byte 0..2 	Fix: IP1
Byte 3..6 	Auflösung, fix 4stellig
Byte 7..12	Scans Pro Minute, fix 6stellig
Byte 13		Plattentyp
Byte 14		Anzahl Platten
Byte 15..End	Dateiname
BSP:	IP1127009600032C:\FileName.TIF
*/

				Length = strlen(TCPAnswer)-15;
				if(Length < 50 && Length>0 )
				{
			/*extract resolution*/
					memcpy(	tmp,&TCPAnswer[3],4 );
					tmp[4] = 0;
					Expose_Param.Resolution = atoi((UDINT) &tmp);
			/*extract Scans per min */
					memcpy(	tmp,&TCPAnswer[7],6 );
					tmp[6] = 0;
					Expose_Param.ScansPerMinute = atoi((UDINT) &tmp);
			/*extract Platetype */
					memcpy(	tmp,&TCPAnswer[13],1 );
					tmp[1] = 0;
			/*extract Number of plates */
					memcpy(	tmp,&TCPAnswer[14],1 );
					tmp[1] = 0;
			/*extract Filename */
					memcpy(	FileName,&TCPAnswer[15],Length );
					FileName[Length]=0;
				}
				CheckForDataStep = 0;
				DataReady = 1;
				StopCheckingForData = 1;
			}
			break;
		}

/*Warteschritt, um 14 etwas langsamer zu schicken*/
		case 7:
		{
/*Timeoutcounter hier als wartecounter missbraucht*/
			TimeoutCounter++;
			if(TimeoutCounter>20) /* 200 ms*/
			{
				TimeoutCounter = 0;
				CheckForDataStep = 5;
			}
			break;
		}
	}


}


