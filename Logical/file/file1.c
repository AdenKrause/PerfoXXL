#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Datei handling für Speichern/Laden von Einstellungen */
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.00		12.12.02	erste Implementation					HA		*/
/*	1.50		04.10.05	Statt Floppy jetzt Zugriff auf USB Stick*/
/*		*/
/*																			*/
/****************************************************************************/

/***** Header files *****/
#include <bur\plctypes.h>
#include <fileio.h>
#include <string.h>
#include "glob_var.h"
#include "auxfunc.h"

#define WEISS	4096
#define SCHWARZ	0

#define MAXFILES	50
#define MAXFILENAMELENGTH	32

#define READDIR		71
#define READDATA	73
#define WRITEDATA	72
#define NOFILE		74

/***** Variable declaration *****/
_LOCAL USINT byStep, byErrorLevel;
_LOCAL UDINT dwIdent;
_LOCAL UINT wStatus;
_LOCAL FileOpen_typ FOpen;
_LOCAL FileClose_typ FClose;
_LOCAL FileCreate_typ FCreate;
_LOCAL FileRead_typ FRead;
_LOCAL FileWrite_typ FWrite;
_LOCAL FileDelete_typ FDelete;

_GLOBAL	STRING FileIOName[MAXFILENAMELENGTH];
_GLOBAL	STRING	FileType[5];
_GLOBAL	UDINT *FileIOData;
_GLOBAL	UDINT	FileIOLength;
_GLOBAL	BOOL	WriteFileCmd,ReadFileCmd,DeleteFileCmd,WriteFileOK,ReadFileOK,DeleteFileOK,
				FileNotExisting,ReadDirCmd,ReadDirOK,NoDisk;


/***** Variable declaration *****/
_LOCAL UDINT 		dwDirNum, dwFileNum, dwCounter;
_LOCAL DirInfo_typ 	DInfo;
_LOCAL DirRead_typ 	DRead;
_LOCAL fiDIR_READ_DATA 	ReadData[MAXFILES];
_LOCAL	STRING	FileNames[MAXFILES][MAXFILENAMELENGTH];
_LOCAL	STRING Device[10];
_LOCAL BOOL	ReadDirData,ReadDirFloppy,DeleteFile,SelectionEnd;
_LOCAL	SINT	FileListPointer;
_LOCAL	UINT	DeviceDataInvisible,DeviceFloppyInvisible;

		BOOL	FirstTime,ReadDataActive,DeleteActive;
_LOCAL	UINT	DiskButtonPressed1,DiskButtonPressed2,FloppyButtonPressed1,FloppyButtonPressed2,
				DelButtonPressed1,DelButtonPressed2,FloppyInvisible,DeleteInvisible;
STRING	TmpFileName[MAXFILENAMELENGTH];
int FileListCounter;
BOOL	AbfrageAktiv;
_LOCAL UINT	HideWait,MessageNumber,Progress;
_LOCAL STRING FileNameDisplay[30];
_GLOBAL	UINT	DeviceType;
UINT	CycleCounter;

/***** Init part *****/
_INIT void Init(void)
{
	NoDisk = 0;
	/* Initialize variables */
	byStep 		= 0;
	byErrorLevel	= 0;

	/* Initialize file enable bits*/
	FOpen.enable 	= 1;
	FClose.enable 	= 1;
	FCreate.enable 	= 1;
	FRead.enable 	= 1;
	FWrite.enable 	= 1;
	FDelete.enable 	= 1;
	WriteFileOK = 0;
	ReadFileOK = 0;
	WriteFileCmd = 0;
	ReadFileCmd = 0;
	FileNotExisting = 0;
	ReadDirCmd = 0;
	ReadDirOK = 0;
	DeviceFloppyInvisible = 1;
	DeviceDataInvisible = 1;
	FirstTime = 1;
	DeleteFile = 0;
	DeleteActive = 0;
	strcpy(Device,"DATA");
	DelButtonPressed1 = WEISS;
	DelButtonPressed2 = SCHWARZ;
	DiskButtonPressed1 = WEISS;
	DiskButtonPressed2 = SCHWARZ;
	FloppyButtonPressed1 = WEISS;
	FloppyButtonPressed2 = SCHWARZ;
	HideWait = 1;
}

/***** Cyclic part *****/
_CYCLIC void Cyclic(void)
{
	if (!strcmp(Device,"DATA"))
		DeviceType = 0; /* CF Card*/
	else
		DeviceType = 1; /* USB */

	if(UserLevel < 2)
	{
		DeleteInvisible = 1;
		DeleteFile = 0;
	}
	else
	{
		FloppyInvisible = 0;
		DeleteInvisible = 0;
	}

	if(FileListPointer >= FileListCounter )FileListPointer = FileListCounter-1;
	if(SelectionEnd)
	{
		if(FileListPointer < 0)
			FileListPointer = FileListCounter-1;

		if(FileListPointer >= FileListCounter || FileListPointer < 0)
			FileListPointer = 0;

		strcpy(FileIOName,FileNames[FileListPointer]);
		SelectionEnd = 0;
	}

	if(DeleteFile && !DeleteInvisible)
	{
		DelButtonPressed1 = SCHWARZ;
		DelButtonPressed2 = WEISS;

		if( !AbfrageAktiv)
		{
			ShowMessage(101,1,0,REQUEST,OKCANCEL, FALSE);
/*
			if(wBildAktuell != MESSAGEPIC )
				OrgBild = wBildAktuell;
			wBildNeu = MESSAGEPIC;
			AbfrageOK = 0;
			AbfrageCancel = 0;
			IgnoreButtons = 0;
			OK_CancelButtonInv = 0;
			OK_ButtonInv = 1;
			AbfrageText1 = 102;
			AbfrageText2 = 1;
			AbfrageText3 = 0;
			AbfrageIcon = REQUEST;*/
			AbfrageAktiv = 1;
		}
		else
		{
			if(AbfrageOK)
			{
				AbfrageOK = 0;
				DeleteActive = 1;
				DeleteFile = 0;
				strcpy(FileIOName,FileNames[FileListPointer]);
				if(strlen(FileIOName) > 0 && strlen(FileIOName) < MAXFILENAMELENGTH)
					DeleteFileCmd = 1;
				else
				{
					DeleteFile = 0;
					DelButtonPressed1 = WEISS;
					DelButtonPressed2 = SCHWARZ;
				}
				AbfrageAktiv = 0;
			}
			if(AbfrageCancel)
			{
				AbfrageCancel = 0;
				DeleteFile = 0;
				DelButtonPressed1 = WEISS;
				DelButtonPressed2 = SCHWARZ;
				AbfrageAktiv = 0;
			}

		}

	}

	if( DeleteActive == 1 )
	{
		if(DeleteFileOK)
		{
			DeleteFileOK = 0;
			DeleteActive = 0;
			ReadDirCmd = 1;
			ReadDirOK = 0;
			DelButtonPressed1 = WEISS;
			DelButtonPressed2 = SCHWARZ;
			FileListPointer = 0;
			strcpy(FileIOName,FileNames[FileListPointer]);
		}
	}

	if(FirstTime && wBildNr == FILEPIC)
	{
		DeleteFile = 0;
		DeleteActive = 0;
		ReadDirData = 1;
		FirstTime = 0;
		FileListPointer = 0;
		strcpy(FileIOName,FileNames[FileListPointer]);
		DelButtonPressed1 = WEISS;
		DelButtonPressed2 = SCHWARZ;
	}
	if(wBildAktuell != FILEPIC && wBildAktuell != MESSAGEPIC)
		FirstTime = 1;

	if((ReadDirData || (ReadDirFloppy && !FloppyInvisible)) && !ReadDataActive)
	{
		ReadDirCmd = 1;
		ReadDirOK = 0;
		ReadDataActive = 1;
	}

	if(ReadDataActive)
	{
		if( ReadDirOK )
		{
			ReadDirOK = 0;
			ReadDataActive = 0;
			DiskButtonPressed1 = WEISS;
			DiskButtonPressed2 = SCHWARZ;
			FloppyButtonPressed1 = WEISS;
			FloppyButtonPressed2 = SCHWARZ;
			FileListPointer = 0;
			strcpy(FileIOName,FileNames[FileListPointer]);
		}
		if(NoDisk)
		{
			ReadDataActive = 0;
			ReadDirData = 1;
			strcpy(FileNames[0],"NO DISK!");
			NoDisk = 0;
			FileListPointer = 0;
			DiskButtonPressed1 = WEISS;
			DiskButtonPressed2 = SCHWARZ;
			FloppyButtonPressed1 = WEISS;
			FloppyButtonPressed2 = SCHWARZ;
		}
	}

	switch (byStep)
	{

		case 0:
		{

			if(!WriteFileCmd && !ReadFileCmd &&  !DeleteFileCmd && !ReadDirCmd)
			{
				HideWait = 1;
				break;
			}
/*safety: if both commands: read is preferred*/
			if(WriteFileCmd && ReadFileCmd)
				WriteFileCmd = 0;
			if( DeleteFileCmd )
			{
				byStep = 40;
				break;
			}
			if(ReadDirCmd)
			{
				byStep = 100;
				ReadDirOK = 0;
				HideWait = 0;
				MessageNumber = READDIR;
				break;
			}

			FileNotExisting = 0;
			WriteFileOK = 0;
			ReadFileOK = 0;
			DeleteFileOK = 0;
/*wenn SRAM Daten automatisch gespeichert werden, dann immer auf CF !*/
			if (!strcmp(FileType,"SAV"))
				strcpy(Device,"DATA");
	/* Initialize file open structrue */
			FOpen.pDevice 	= (UDINT) Device;

			strcpy(TmpFileName,FileIOName);
			strcat(TmpFileName,".");
			strcat(TmpFileName,FileType);
			FOpen.pFile 	= (UDINT) &TmpFileName[0]; 	/* File name */
			FOpen.mode 	= FILE_RW; 		/* Read and write access */
			/* Call FUB */
			FileOpen(&FOpen);

			/* Get FUB output information */
			dwIdent = FOpen.ident;
			wStatus = FOpen.status;

			if (wStatus == 65535)
			{
		/*wait */
				break;
			}

			if (wStatus == fiERR_FILE_NOT_FOUND )
			{
				if(ReadFileCmd)
				{
					ReadFileCmd = 0;
					FileNotExisting = 1;
					break;
				}
				if(!WriteFileCmd)
				{
					break;
				}
				else
				{
/*File does not exist->create it*/
					byStep = 5;
					HideWait = 0;
					MessageNumber = WRITEDATA;
					break;
				}
			}
			else if (wStatus != 0)
			{
				byStep = 50;
				WriteFileCmd = 0;
			}

/* no error, go ahead*/
			if( WriteFileCmd )
			{
				byStep = 10;
				HideWait = 0;
				MessageNumber = WRITEDATA;
			}
			if( ReadFileCmd )
			{
				byStep = 20;
				HideWait = 0;
				MessageNumber = READDATA;
			}

			FClose.ident 	= dwIdent;
			FRead.ident 	= dwIdent;
			FWrite.ident 	= dwIdent;

			break;
		}

		case 5: /**** create file ****/
		{
			/* Initialize file create structure */
			FCreate.enable 	= 1;
			FCreate.pDevice = (UDINT) Device;
			strcpy(TmpFileName,FileIOName);
			strcat(TmpFileName,".");
			strcat(TmpFileName,FileType);
			FCreate.pFile 	= (UDINT) &TmpFileName[0]; 	/* File name */

		/* Call FUB */
			FileCreate(&FCreate);

		/* Get output information of FUB */
			dwIdent = FCreate.ident;
			wStatus = FCreate.status;

			if (wStatus == 65535)
			{
		/*wait */
				break;
			}

			/* any error-> error message */
			if (wStatus != 0)
			{
				byStep = 50;
				WriteFileCmd = 0;
				break;
			}

			FClose.ident 	= dwIdent;
			FRead.ident 	= dwIdent;
			FWrite.ident 	= dwIdent;
			byStep = 10;
			break;
		}

		case 10: /**** Write data to file ****/
		{
			/* Initialize file write structure */
			FWrite.offset 	= 0;
			FWrite.pSrc 	= (UDINT) FileIOData;
			FWrite.len 	= FileIOLength;

			/* Call FUB */
			FileWrite(&FWrite);

			/* Get status */
			wStatus = FWrite.status;

			if (wStatus == 65535)
			{
				break;
			}

			/* any error -> error message */
			if (wStatus != 0)
			{
				byStep 		= 50;
				WriteFileCmd = 0;
			}
			else
			{
				byStep = 30; 	/* Set new step value : close*/
			}

			break;
		}

		case 20: /**** Read data from file ****/
		{
			/* Initialize file read structure */
			FRead.offset 	= 0;
			FRead.pDest 	= (UDINT) FileIOData;
			FRead.len 	= FileIOLength;

			/* Call FUB */
			FileRead(&FRead);

			/* Get status */
			wStatus = FRead.status;

			if (wStatus == 65535)
			{
				break;
			}

			/* any error -> error message*/
			if (wStatus != 0)
			{
				byStep = 50;
			}
			else
			{
				byStep = 30; 	/* Set new step value : close*/
			}

			break;
		}


		case 30: /**** Close file ****/
		{
			/* Initialize file close structure */
			FClose.ident 	= dwIdent;

			/* Call FUB */
			FileClose(&FClose);

			/* Get status */
			wStatus = FClose.status;

			if (wStatus == 65535)
			{
				break;
			}

			/* any error -> error message */
			if (wStatus != 0)
			{
				byStep = 50;
			}
			else
			{
				byStep = 0;	/* Set new step value */
				HideWait = 1;
				if(WriteFileCmd)
				{
					WriteFileCmd = 0;
					WriteFileOK = 1;
				}
				if(ReadFileCmd)
				{
					ReadFileCmd = 0;
					ReadFileOK = 1;
				}
			}

			break;
		}

		case 40: /**** Delete file ****/
		{
			/* Initialize file delete structure */
			FDelete.pDevice = (UDINT) Device;
			strcpy(TmpFileName,FileIOName);
			strcat(TmpFileName,".");
			strcat(TmpFileName,FileType);
			FDelete.pName 	= (UDINT) &TmpFileName[0]; 	/* File name */

			/* Call FUB */
			FileDelete(&FDelete);

			/* Get status */
			wStatus = FDelete.status;

			/* Verify status */
			if (wStatus == 65535)
			{
				break;
			}

			/* Verify status */
			if (wStatus == 26205)
			{
				FileNotExisting = 1;
			}
			else
			if(wStatus != 0)
				byErrorLevel = 6; 	/* Set error level for FileDelete */
			else
			{
				DeleteFileOK = 1;
			}
			byStep = 0; 						/* Reset step variable */
			DeleteFileCmd = 0;
			break;
		}


		case 50: /* ERROR */
		{
			HideWait = 1;
			ShowMessage(0,70,0,CAUTION,OKONLY, FALSE);
/*
			if(wBildAktuell != MESSAGEPIC )
				OrgBild = wBildAktuell;
			wBildNeu = MESSAGEPIC;
			AbfrageOK = 0;
			AbfrageCancel = 0;
			IgnoreButtons = 0;
			OK_CancelButtonInv = 1;
			OK_ButtonInv = 0;
			AbfrageText1 = 0;
			AbfrageText2 = 70;
			AbfrageText3 = 0;
			AbfrageIcon = CAUTION;*/
			byStep = 51;
			break;
		}
		case 51: /* wait for user reaction*/
		{
			if(AbfrageOK || AbfrageCancel)
				byStep = 0;

			break;
		}



/*********************************************/
/*Directory information*/
/*********************************************/
		case 100: /* read DIR Info */
		{
			int i;

			for(i=0;i<MAXFILES;i++)
				FileNames[i][0] = 0;
			if(ReadDirData)
			{
				strcpy(Device,"DATA");
				DeviceFloppyInvisible = 1;
				DeviceDataInvisible = 0;
				DiskButtonPressed1 = SCHWARZ;
				DiskButtonPressed2 = WEISS;
			}
			if(ReadDirFloppy && !FloppyInvisible)
			{
				strcpy(Device,"USBStick");
				DeviceFloppyInvisible = 0;
				DeviceDataInvisible = 1;
				FloppyButtonPressed1 = SCHWARZ;
				FloppyButtonPressed2 = WEISS;
			}

	/* Initialize info structure */
			DInfo.enable 	= 1;
			DInfo.pDevice 	= (UDINT) Device;
			DInfo.pPath 	= 0;

	/* Call FUB */
			DirInfo(&DInfo);

	/* Get FUB output information */
			wStatus 	= DInfo.status;
			dwDirNum 	= DInfo.dirnum;
			dwFileNum 	= DInfo.filenum;

			if (wStatus == 65535)
			{
				/*busy: just wait*/
				break;
			}

	/* Verify status */
			if (wStatus == fiERR_FILE_DEVICE)
			{
/* V4.02 if 1st USB device fails, check 2nd*/
				if (ReadDirFloppy)
				{
					byStep = 102;
				}
				else
				{
					NoDisk = 1;
					ReadDirCmd = 0;
					ReadDirOK = 0;
					byStep = 0;
					ReadDirFloppy = 0;
					ReadDirData = 0;
					HideWait = 1;
				}
			}
			else
			if (wStatus != 0)
			{
				ReadDirCmd = 0;
				ReadDirOK = 0;
				ReadDirFloppy = 0;
				ReadDirData = 0;
				HideWait = 1;
				byStep = 50;
			}
			else
			{
				NoDisk = 0;

	/* Verify number of found files */
				if (dwFileNum > MAXFILES)
				{
					dwFileNum = MAXFILES; 	/* Set variable to maximum value */
				}
				byStep = 105;
				dwCounter = 0;
				FileListCounter = 0;
			}
			break;
		}

		case 102: /* read DIR Info 2nd USB Port*/
		{
			strcpy(Device,"USBStick2");

	/* Initialize info structure */
			DInfo.enable 	= 1;
			DInfo.pDevice 	= (UDINT) Device;
			DInfo.pPath 	= 0;

	/* Call FUB */
			DirInfo(&DInfo);

	/* Get FUB output information */
			wStatus 	= DInfo.status;
			dwDirNum 	= DInfo.dirnum;
			dwFileNum 	= DInfo.filenum;

			if (wStatus == 65535)
			{
				/*busy: just wait*/
				break;
			}

	/* Verify status */
			if (wStatus == fiERR_FILE_DEVICE)
			{
				HideWait = 1;
				NoDisk = 1;
				ReadDirCmd = 0;
				ReadDirOK = 0;
				byStep = 0;
				ReadDirFloppy = 0;
				ReadDirData = 0;

				ShowMessage(0,69,0,CAUTION,OKONLY, TRUE);

/*				if(wBildAktuell != MESSAGEPIC )
					OrgBild = wBildAktuell;
				wBildNeu = MESSAGEPIC;
				AbfrageOK = 0;
				AbfrageCancel = 0;
				IgnoreButtons = 1;
				OK_CancelButtonInv = 1;
				OK_ButtonInv = 0;
				AbfrageText1 = 0;
				AbfrageText2 = 69;
				AbfrageText3 = 0;
				AbfrageIcon = CAUTION;*/

			}
			else
			if (wStatus != 0)
			{
				HideWait = 1;
				ReadDirCmd = 0;
				ReadDirOK = 0;
				ReadDirFloppy = 0;
				ReadDirData = 0;
				byStep = 50;
			}
			else
			{
				NoDisk = 0;

	/* Verify number of found files */
				if (dwFileNum > MAXFILES)
				{
					dwFileNum = MAXFILES; 	/* Set variable to maximum value */
				}
				byStep = 105;
				dwCounter = 0;
				FileListCounter = 0;
			}
			break;
		}

		case 105: /* read DIR Info */
		{
			ReadDirFloppy = 0;
			ReadDirData = 0;

		/* Verify counter variable */
			while (dwCounter < dwFileNum)
			{
			/* Initialize read directory structure */
				DRead.enable 	= 1;
				DRead.pDevice 	= (UDINT) Device;
				DRead.pPath 	= 0;
				DRead.entry 	= dwCounter;
				DRead.option 	= FILE_FILE;
				DRead.pData 	= (UDINT) &ReadData[dwCounter];
				DRead.data_len 	= sizeof (ReadData[0]);

			/* Call FUB */
				DirRead(&DRead);

			/* Get status */
				wStatus = DRead.status;

				if (wStatus == 65535)
				{
					/*busy: just wait*/
					break;
				}

			/* Verify status */
				if (wStatus != 0)
				{
					ReadDirCmd = 0;
					ReadDirOK = 0;
					HideWait = 1;
					byStep = 50;
					break;
				}
				else
				{
					int i = strlen(ReadData[dwCounter].Filename);
					STRING TypeFound[5];
					USINT *p;

/* für info display */
					if (i<=28)
						strcpy(FileNameDisplay,ReadData[dwCounter].Filename);
					else
					{
						strncpy(FileNameDisplay,ReadData[dwCounter].Filename,25);
						FileNameDisplay[25]=0;
						strcat(FileNameDisplay,"...");
					}

					strcpy(TmpFileName,ReadData[dwCounter].Filename);

					while(i-->1)
					{
						if( TmpFileName[i] == '.' )
						{
							TmpFileName[i] = 0;
							p = &TmpFileName[i+1];
								strcpy(TypeFound,p);
							if( !strcmp(TypeFound,FileType) )
								strcpy(FileNames[FileListCounter++],TmpFileName);
							i=0;
						}
					}
					dwCounter ++;	/* Increase counter variable */
					Progress = (dwCounter*100)/dwFileNum;
				}
			}

			if (wStatus == 65535)
			{
				/*busy: just wait*/
				break;
			}
/* keine Datei gefunden*/
			if (!FileListCounter)
			{
				MessageNumber = NOFILE; /* no file found */
				byStep = 106;
				CycleCounter = 0;
			}
			else
			{
				byStep = 0;
				HideWait = 1;
			}
			ReadDirCmd = 0;
			ReadDirOK = 1;
			Progress = 0;
			strcpy(FileNameDisplay,"");
			break;
		}
		case 106: /* wait, to let user read the message*/
		{
			CycleCounter++;
			if(CycleCounter >= 15)	/* 3 sec */
			{
				CycleCounter = 0;
				byStep = 0;
				HideWait = 1;
			}
			break;
		}
	}
}


