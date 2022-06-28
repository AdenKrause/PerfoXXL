#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/**
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : FILE1.C                                                      **
** version  : 1.61                                                         **
** date     : 12.07.2010                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** This task implements file handling routines to save/load parameter      **
** files to CF card or USB device. Communication with the application is   **
** established via command bits. This task also handles the File-picture   **
** on the display.                                                         **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 12.02.2002
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.50
Date        : 04.10.2005
Revised by  : Herbert Aden
Description : using USB device now instead of floppy

Version     : 1.51
Date        : 31.08.2006
Revised by  : Herbert Aden
Description : now both USB ports are supported

Version     : 1.60
Date        : 29.11.2006
Revised by  : Herbert Aden
Description : general code review, safety (replaced strcpy by strncpy and
              increased buffer length)

Version     : 1.61
Date        : 12.07.2010
Revised by  : Herbert Aden
Description : if the file to write to already exists, it can be deleted
              and newly created by an extra global flag

*/

#define _FILE1_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <bur\plctypes.h>
#include <fileio.h>
#include <ctype.h>
#include <string.h>
#include "brsystem.h"
#include "glob_var.h"
#include "egmglob_var.h"
#include "auxfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define WEISS	4096
#define SCHWARZ	0

#define MAXFILES	50

#define MESSAGEPIC 41
#define FILEPIC 42

#define READDIR		71
#define READDATA	73
#define WRITEDATA	72
#define NOFILE		74

#define FILENAMEINFODISPLAYLENGTH 40

#define CFCARD      (0)
#define USB         (1)
#define LISTEMPTY   (-1)

#define FUB_OK   (0)
#define FUB_BUSY (65535)
/****************************************************************************/
/**                                                                        **/
/**                      TYPEDEFS AND STRUCTURES                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      PROTOTYPES OF LOCAL FUNCTIONS                     **/
/**                                                                        **/
/****************************************************************************/
extern void usbcyclic(void);
extern void usbinit(void);
/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                           GLOBAL VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/
_GLOBAL   STRING              FileIOName[MAXFILENAMELENGTH];
_GLOBAL   STRING              FileType[MAXFILETYPELENGTH];
_GLOBAL   void               *FileIOData;
_GLOBAL   UDINT               FileIOLength;
_GLOBAL   BOOL                WriteFileCmd,
                              ReadFileCmd,
                              DeleteFileCmd,
                              WriteFileOK,
                              ReadFileOK,
                              DeleteFileOK,
                              FileNotExisting,
                              ReadDirCmd,
                              ReadDirOK,
                              NoDisk,
                              DeleteFileFirst;
_GLOBAL   UINT                DeviceType;   /* 0->CF, 1->USB*/

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    USINT               byStep,DeleteRetStep;
_LOCAL    UDINT               dwIdent;
_LOCAL    UINT                wStatus;
_LOCAL    FileOpen_typ        FOpen;
_LOCAL    FileClose_typ       FClose;
_LOCAL    FileCreate_typ      FCreate;
_LOCAL    FileRead_typ        FRead;
_LOCAL    FileWrite_typ       FWrite;
_LOCAL    FileDelete_typ      FDelete;
_LOCAL    UDINT               dwDirNum,
                              dwFileNum,
                              dwCounter;
_LOCAL    DirInfo_typ         DInfo;
_LOCAL    DirRead_typ         DRead;
_LOCAL    fiDIR_READ_DATA     ReadData[MAXFILES];
_LOCAL    STRING              FileNames[MAXFILES][MAXFILENAMELENGTH];
_LOCAL    STRING              Device[10];
_LOCAL    BOOL                ReadDirData,
                              ReadDirFloppy,
                              DeleteFile,
                              SelectionEnd;
_LOCAL    SINT                FileListSelected;
_LOCAL    UINT                DeviceDataInvisible,
                              DeviceFloppyInvisible;

_LOCAL    UINT                DiskButtonPressed1,
                              DiskButtonPressed2,
                              FloppyButtonPressed1,
                              FloppyButtonPressed2,
                              DelButtonPressed1,
                              DelButtonPressed2,
                              FloppyInvisible,
                              DeleteInvisible;
_LOCAL    UINT                HideWait,
                              MessageNumber,
                              Progress;
_LOCAL    STRING              FileNameDisplay[FILENAMEINFODISPLAYLENGTH];
_LOCAL    TARGETInfo_typ      TARGETInfo_01;
_LOCAL    STRING              OSVersionString[10];
_LOCAL    char                USBDeviceStr[32];
/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    BOOL                FirstTime,
                              ReadDataActive,
                              DeleteActive;
static    STRING              TmpFileName[MAXFILENAMELENGTH+MAXFILETYPELENGTH];
static    int                 FileListCounter;
static    BOOL                AbfrageAktiv;
static    UINT                CycleCounter;
static    UINT                FloppyInvOld;


/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/
/****************************************************************************/
/*     INIT FUNCTION                                                        */
/****************************************************************************/
_INIT void Init(void)
{
	/* Initialize file enable bits*/
	FOpen.enable 	= TRUE;
	FClose.enable 	= TRUE;
	FCreate.enable 	= TRUE;
	FRead.enable 	= TRUE;
	FWrite.enable 	= TRUE;
	FDelete.enable 	= TRUE;
	WriteFileOK     = FALSE;
	ReadFileOK      = FALSE;
	WriteFileCmd    = FALSE;
	ReadFileCmd     = FALSE;
	FileNotExisting = FALSE;
	ReadDirCmd      = FALSE;
	ReadDirOK       = FALSE;
	DeviceFloppyInvisible = TRUE;
	DeviceDataInvisible   = TRUE;
	FirstTime             = TRUE;
	HideWait              = TRUE;
	DeleteFile            = FALSE;
	DeleteActive          = FALSE;
	NoDisk                = FALSE;
	DelButtonPressed1     = WEISS;
	DelButtonPressed2     = SCHWARZ;
	DiskButtonPressed1    = WEISS;
	DiskButtonPressed2    = SCHWARZ;
	FloppyButtonPressed1  = WEISS;
	FloppyButtonPressed2  = SCHWARZ;
	byStep 		    = 0;
	strcpy(Device,"DATA");
	strcpy(FileIOName,"");
	DeleteFileFirst = FALSE;

	TARGETInfo_01.enable = 1;                         /* Funktion wird nur ausgeführt wenn ENABLE <> 0 ist. */
	TARGETInfo_01.pOSVersion = (UDINT) &OSVersionString;  /* Zeiger auf einen String mit vorgegebener Länge, in den die Betriebssystemversion geschrieben wird */
	do
		TARGETInfo(&TARGETInfo_01);
	while (TARGETInfo_01.status == ERR_FUB_BUSY);

	if(OSVersionString[2] >= '4')
		usbinit();
	else
		strcpy(USBDeviceStr,"USBStick");
}

/****************************************************************************/
/*     CYCLIC FUNCTION                                                      */
/****************************************************************************/
_CYCLIC void Cyclic(void)
{
	if(OSVersionString[2] >= '4')
		usbcyclic();
	else
	{ 
	/* bei statischer Zuordnung immer die Möglichkeit, USB anzuwählen */
		FloppyInvisible = FALSE;
	}
	if ( STREQ(Device,"DATA") )
		DeviceType = CFCARD;
	else
		DeviceType = USB;

	if(UserLevel < 2)
	{
		DeleteInvisible = TRUE;
		DeleteFile = FALSE;
	}
	else
	{
//		FloppyInvisible = FALSE;
		DeleteInvisible = FALSE;
	}

	if(FileListSelected >= FileListCounter )FileListSelected = FileListCounter-1;
	if(SelectionEnd)
	{
		if(FileListSelected < 0)
			FileListSelected = FileListCounter-1;

		if(FileListSelected >= FileListCounter || FileListSelected < 0)
			FileListSelected = 0;

		strncpy(FileIOName,FileNames[FileListSelected],MAXFILENAMELENGTH-1);
		SelectionEnd = FALSE;
	}

	if(DeleteFile && !DeleteInvisible && FileListSelected != LISTEMPTY)
	{
		DelButtonPressed1 = SCHWARZ;
		DelButtonPressed2 = WEISS;

		if( !AbfrageAktiv)
		{
			ShowMessage(101,1,0,REQUEST,OKCANCEL,FALSE);
			AbfrageAktiv = TRUE;
		}
		else
		{
			if(AbfrageOK)
			{
				AbfrageOK = FALSE;
				DeleteActive = TRUE;
				DeleteFile = FALSE;
				strncpy(FileIOName,FileNames[FileListSelected],MAXFILENAMELENGTH-1);
				if(strlen(FileIOName) > 0 && strlen(FileIOName) < MAXFILENAMELENGTH)
					DeleteFileCmd = TRUE;
				else
				{
					DeleteFile = FALSE;
					DelButtonPressed1 = WEISS;
					DelButtonPressed2 = SCHWARZ;
				}
				AbfrageAktiv = FALSE;
			}
			if(AbfrageCancel)
			{
				AbfrageCancel = FALSE;
				DeleteFile = FALSE;
				DelButtonPressed1 = WEISS;
				DelButtonPressed2 = SCHWARZ;
				AbfrageAktiv = FALSE;
			}
		}
	}

	if( DeleteActive == TRUE )
	{
		if(DeleteFileOK)
		{
			DeleteFileOK = FALSE;
			DeleteActive = FALSE;
			ReadDirCmd = TRUE;
			ReadDirOK = FALSE;
			DelButtonPressed1 = WEISS;
			DelButtonPressed2 = SCHWARZ;
			FileListSelected = 0;
			strncpy(FileIOName,FileNames[FileListSelected],MAXFILENAMELENGTH-1);
		}
	}

	if(FirstTime && wBildNr == FILEPIC)
	{
		DeleteFile = FALSE;
		DeleteActive = FALSE;
		ReadDirData = TRUE;
		FirstTime = FALSE;
		FileListSelected = 0;
		strncpy(FileIOName,FileNames[FileListSelected],MAXFILENAMELENGTH-1);
		DelButtonPressed1 = WEISS;
		DelButtonPressed2 = SCHWARZ;
	}
	if(wBildAktuell != FILEPIC && wBildAktuell != MESSAGEPIC)
		FirstTime = TRUE;

	/* wenn USB Stick gezogen wird, das interne Verzeichnis auslesen */	
	if(FloppyInvisible && !FloppyInvOld)
		ReadDirData = TRUE;
	FloppyInvOld = FloppyInvisible;

	if((ReadDirData || (ReadDirFloppy && !FloppyInvisible)) && !ReadDataActive)
	{
		ReadDirCmd = TRUE;
		ReadDirOK = FALSE;
		ReadDataActive = TRUE;
	}

	if(ReadDataActive)
	{
		if( ReadDirOK )
		{
			ReadDirOK = FALSE;
			ReadDataActive = FALSE;
			DiskButtonPressed1 = WEISS;
			DiskButtonPressed2 = SCHWARZ;
			FloppyButtonPressed1 = WEISS;
			FloppyButtonPressed2 = SCHWARZ;
			FileListSelected = 0;
			strncpy(FileIOName,FileNames[FileListSelected],MAXFILENAMELENGTH-1);
		}
		if(NoDisk)
		{
			ReadDataActive = FALSE;
			ReadDirData = TRUE;
			strcpy(FileNames[0],"NO DISK!");
			NoDisk = FALSE;
			FileListSelected = 0;
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
				HideWait = TRUE;
				break;
			}
/*safety: if both commands: read is preferred*/
			if(WriteFileCmd && ReadFileCmd)
				WriteFileCmd = FALSE;
			if( DeleteFileCmd )
			{
				byStep = 40;
				DeleteRetStep = 0;
				break;
			}
			if(ReadDirCmd)
			{
				byStep = 100;
				ReadDirOK = FALSE;
				HideWait = FALSE;
				MessageNumber = READDIR;
				break;
			}

			FileNotExisting = FALSE;
			WriteFileOK = FALSE;
			ReadFileOK = FALSE;
			DeleteFileOK = FALSE;
/*wenn SRAM Daten automatisch gespeichert werden, dann immer auf CF !*/
			if (STREQ(FileType,"SAV"))
				strcpy(Device,"DATA");
	/* Initialize file open structrue */
			FOpen.pDevice 	= (UDINT) Device;

			strncpy(TmpFileName,FileIOName,MAXFILENAMELENGTH-MAXFILETYPELENGTH-1);
			strcat(TmpFileName,".");
			strncat(TmpFileName,FileType,MAXFILETYPELENGTH);
			FOpen.pFile 	= (UDINT) &TmpFileName[0]; 	/* File name */
			FOpen.mode 	= FILE_RW; 		/* Read and write access */
			/* Call FUB */
			FileOpen(&FOpen);

			/* Get FUB output information */
			dwIdent = FOpen.ident;
			wStatus = FOpen.status;

			if (wStatus == FUB_BUSY)
			{
		/*wait */
				break;
			}

			if (wStatus == fiERR_FILE_NOT_FOUND )
			{
				if(ReadFileCmd)
				{
					ReadFileCmd = FALSE;
					FileNotExisting = TRUE;
					break;
				}
				if(!WriteFileCmd)
				{
				/*VOID should never happen !*/
					break;
				}
				else
				{
/*File does not exist->create it*/
					byStep = 5;
					HideWait = FALSE;
					MessageNumber = WRITEDATA;
					break;
				}
			}
			else
			if (wStatus != FUB_OK) /*any other error -> Error message*/
			{
				byStep = 50;
				WriteFileCmd = FALSE;
				break;
			}

/* no error, go ahead*/
			if( WriteFileCmd )
			{
				if(DeleteFileFirst)
					byStep = 2;
				else
					byStep = 10;

				DeleteFileFirst = FALSE;
				HideWait = FALSE;
				MessageNumber = WRITEDATA;
			}
			if( ReadFileCmd )
			{
				byStep = 20;
				HideWait = FALSE;
				MessageNumber = READDATA;
			}

			FClose.ident 	= dwIdent;
			FRead.ident 	= dwIdent;
			FWrite.ident 	= dwIdent;
			break;
		}

		case 2: /**** delete file ****/
		{
			byStep = 40;
			DeleteRetStep = 5;
			break;
		}

		case 5: /**** create file ****/
		{
			/* Initialize file create structure */
			FCreate.enable 	= TRUE;
			FCreate.pDevice = (UDINT) Device;
			strncpy(TmpFileName,FileIOName,MAXFILENAMELENGTH-MAXFILETYPELENGTH-1);
			strcat(TmpFileName,".");
			strncat(TmpFileName,FileType,MAXFILETYPELENGTH);
			FCreate.pFile 	= (UDINT) &TmpFileName[0]; 	/* File name */

		/* Call FUB */
			FileCreate(&FCreate);

		/* Get output information of FUB */
			dwIdent = FCreate.ident;
			wStatus = FCreate.status;

			if (wStatus == FUB_BUSY)
			{
		/*wait */
				break;
			}

			/* any error-> error message */
			if (wStatus != FUB_OK)
			{
				byStep = 50;
				WriteFileCmd = FALSE;
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
			FWrite.len 	    = FileIOLength;

			/* Call FUB */
			FileWrite(&FWrite);

			/* Get status */
			wStatus = FWrite.status;

			if (wStatus == FUB_BUSY)
			{
				break;
			}

			/* any error -> error message */
			if (wStatus != FUB_OK)
			{
				byStep 		= 50;
				WriteFileCmd = FALSE;
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
			FRead.len 	    = FileIOLength;

			/* Call FUB */
			FileRead(&FRead);

			/* Get status */
			wStatus = FRead.status;

			if (wStatus == FUB_BUSY)
			{
				break;
			}

			/* any error -> error message*/
			if (wStatus != FUB_OK)
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

			if (wStatus == FUB_BUSY)
			{
				break;
			}

			/* any error -> error message */
			if (wStatus != FUB_OK)
			{
				byStep = 50;
			}
			else
			{
				byStep = 0;	/* Set new step value */
				HideWait = TRUE;
				if(WriteFileCmd)
				{
					WriteFileCmd = FALSE;
					WriteFileOK = TRUE;
				}
				if(ReadFileCmd)
				{
					ReadFileCmd = FALSE;
					ReadFileOK = TRUE;
				}
			}

			break;
		}

		case 40: /**** Delete file ****/
		{
			/* Initialize file delete structure */
			FDelete.pDevice = (UDINT) Device;
			strncpy(TmpFileName,FileIOName,MAXFILENAMELENGTH-MAXFILETYPELENGTH-1);
			strcat(TmpFileName,".");
			strncat(TmpFileName,FileType,MAXFILETYPELENGTH);
			FDelete.pName 	= (UDINT) &TmpFileName[0]; 	/* File name */

			/* Call FUB */
			FileDelete(&FDelete);

			/* Get status */
			wStatus = FDelete.status;

			/* Verify status */
			if (wStatus == FUB_BUSY)
			{
				break;
			}

			/* Verify status */
			if (wStatus == fiERR_FILE_NOT_FOUND)
			{
				FileNotExisting = TRUE;
			}
			else
			if(wStatus == FUB_OK)
			{
				DeleteFileOK = TRUE;
			}
			byStep = DeleteRetStep;	/* jump back to returnstep  */
			DeleteFileCmd = FALSE;
			break;
		}


		case 50: /* ERROR */
		{
			HideWait = TRUE;
			ShowMessage(0,70,0,CAUTION,OKONLY,FALSE);
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
				DeviceFloppyInvisible = TRUE;
				DeviceDataInvisible = FALSE;
				DiskButtonPressed1 = SCHWARZ;
				DiskButtonPressed2 = WEISS;
			}
			if(ReadDirFloppy && !FloppyInvisible)
			{
				strcpy(Device,USBDeviceStr);
				DeviceFloppyInvisible = FALSE;
				DeviceDataInvisible = TRUE;
				FloppyButtonPressed1 = SCHWARZ;
				FloppyButtonPressed2 = WEISS;
			}

	/* Initialize info structure */
			DInfo.enable 	= TRUE;
			DInfo.pDevice 	= (UDINT) Device;
			DInfo.pPath 	= NULL;

	/* Call FUB */
			DirInfo(&DInfo);

	/* Get FUB output information */
			wStatus 	= DInfo.status;
			dwDirNum 	= DInfo.dirnum;
			dwFileNum 	= DInfo.filenum;

			if (wStatus == FUB_BUSY)
			{
				/*busy: just wait*/
				break;
			}

	/* Verify status */
//			if (wStatus == fiERR_FILE_DEVICE)
			if( (wStatus == fiERR_FILE_DEVICE) || (wStatus == fiERR_DEVICE_MANAGER) || (wStatus == fiERR_FILE_DEVICE ))
			{
			/* 1st USB port fails: try 2nd*/
				if (ReadDirFloppy)
				{
					if(OSVersionString[2] >= '4')
					{
						/* USB not available */ 
						HideWait = TRUE;
						NoDisk = TRUE;
						ReadDirCmd = FALSE;
						ReadDirOK = FALSE;
						byStep = 0;
						ReadDirFloppy = FALSE;
						ReadDirData = FALSE;
					}
					else
					{
						byStep = 102;
					}
				}
				else
				{
					NoDisk = TRUE;
					ReadDirCmd = FALSE;
					ReadDirOK = FALSE;
					byStep = 0;
					ReadDirFloppy = FALSE;
					ReadDirData = FALSE;
					HideWait = TRUE;
				}
			}
			else
			if (wStatus != FUB_OK)
			{
				ReadDirCmd = FALSE;
				ReadDirOK = FALSE;
				ReadDirFloppy = FALSE;
				ReadDirData = FALSE;
				HideWait = TRUE;
				byStep = 50;
			}
			else
			{
				NoDisk = FALSE;

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
			strcpy(Device,"USB2");

	/* Initialize info structure */
			DInfo.enable 	= TRUE;
			DInfo.pDevice 	= (UDINT) Device;
			DInfo.pPath 	= NULL;

	/* Call FUB */
			DirInfo(&DInfo);

	/* Get FUB output information */
			wStatus 	= DInfo.status;
			dwDirNum 	= DInfo.dirnum;
			dwFileNum 	= DInfo.filenum;

			if (wStatus == FUB_BUSY)
			{
				/*busy: just wait*/
				break;
			}

	/* Verify status */
			if (wStatus == fiERR_FILE_DEVICE)
			{
				HideWait = TRUE;
				NoDisk = TRUE;
				ReadDirCmd = FALSE;
				ReadDirOK = FALSE;
				byStep = 0;
				ReadDirFloppy = FALSE;
				ReadDirData = FALSE;

				ShowMessage(0,69,0,CAUTION,OKONLY,TRUE);
			}
			else
			if (wStatus != FUB_OK)
			{
				HideWait = TRUE;
				ReadDirCmd = FALSE;
				ReadDirOK = FALSE;
				ReadDirFloppy = FALSE;
				ReadDirData = FALSE;
				byStep = 50;
			}
			else
			{
				NoDisk = FALSE;

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
		/* read all filenames in a loop */
			while (dwCounter < dwFileNum)
			{
			/* Initialize read directory structure */
				DRead.enable 	= TRUE;
				DRead.pDevice 	= (UDINT) Device;
				DRead.pPath 	= NULL;
				DRead.entry 	= dwCounter;
				DRead.option 	= FILE_FILE;
				DRead.pData 	= (UDINT) &ReadData[dwCounter];
				DRead.data_len 	= sizeof (ReadData[0]);

			/* Call FUB */
				DirRead(&DRead);

			/* Get status */
				wStatus = DRead.status;

				if (wStatus == FUB_BUSY)
				{
					/*busy: just wait*/
					break;
				}

			/* Verify status */
				if (wStatus != FUB_OK)
				{
					ReadDirCmd = FALSE;
					ReadDirOK = FALSE;
					HideWait = TRUE;
					byStep = 50;
					break;
				}
				else
				{
					int i = strlen((char *) ReadData[dwCounter].Filename);
					STRING TypeFound[MAXFILETYPELENGTH];
					char *p;

/* for info display */
					if (i < FILENAMEINFODISPLAYLENGTH-1)
						strncpy(FileNameDisplay,(char *)ReadData[dwCounter].Filename,FILENAMEINFODISPLAYLENGTH-1);
					else
					{
						strncpy(FileNameDisplay,(char *)ReadData[dwCounter].Filename,
						         FILENAMEINFODISPLAYLENGTH-1-strlen("...") );
						FileNameDisplay[FILENAMEINFODISPLAYLENGTH-1-strlen("...")] = 0;
						strcat(FileNameDisplay,"...");
					}
/* separate the filetype and compare it to the wanted filetype*/
					while(i-- > 1)
					{
						ReadData[dwCounter].Filename[i] = toupper(ReadData[dwCounter].Filename[i]);
						if( ReadData[dwCounter].Filename[i] == '.' )
						{
							ReadData[dwCounter].Filename[i] = 0;
							p = (char *) &ReadData[dwCounter].Filename[i+1];
								strncpy(TypeFound,p,MAXFILETYPELENGTH-1);
							if( STREQN(TypeFound,FileType,MAXFILETYPELENGTH-1)  )
								strncpy(FileNames[FileListCounter++],(char *)ReadData[dwCounter].Filename,MAXFILENAMELENGTH);
							i=0;
						}
					}
					dwCounter ++;	/* Increase counter variable */
					Progress = (dwCounter*100)/dwFileNum;
				}
			}

			if (wStatus == FUB_BUSY)
			{
				/*busy: just wait*/
				break;
			}
/* keine Datei gefunden*/
			if (FileListCounter == 0)
			{
				MessageNumber = NOFILE; /* no file found */
				strcpy(FileIOName,"");
				byStep = 106;
				CycleCounter = 0;
			}
			else
			{
				byStep = 0;
				HideWait = TRUE;
			}
			ReadDirCmd = FALSE;
			ReadDirOK = TRUE;
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
				HideWait = TRUE;
			}
			break;
		}
	}
}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


