#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/**
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : SERIELL.C                                                    **
** version  : 1.12                                                         **
** date     : 15.06.2010                                                   **
**                                                                         **
*****************************************************************************
**                                                                         **
** Abstract                                                                **
** ========                                                                **
** Implementation of serial communication with Faulhaber motor units       **
** - cyclic polling of current position                                    **
** - send commands to a specific motors                                    **
** - request data from a specific motor                                    **
**                                                                         **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 05.09.2002
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.10
Date        : 30.09.2002
Revised by  : Herbert Aden
Description : If a timeout occurs, the Position request is repeated up to 5 times

Version     : 1.11
Date        : 17.10.2006
Revised by  : Herbert Aden
Description : - Code review, style improvement
              - only number of received chars is copied from receive-buffer now
              - RequestData is now local to this module

Version     : 1.12
Date        : 15.06.2010
Revised by  : Herbert Aden
Description : - generate errormessages from FaultStatus (overtemp., current limiting,
                under- or overvoltage)

Version     : 1.13
Date        : 26.04.2011
Revised by  : Herbert Aden
Description : - errormessage delayed to make sure the faultmotor is correctly set
                before the message shows up (avoid error: Motor 0 blahblah)


TODO: Error handling, if system calls fail
*/
#define _SERIELL_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <bur/plctypes.h>
#include <string.h>
#include <AsString.h>
#include <dvframe.h>
#include <standard.h>
#include <math.h>

#include "glob_var.h"
#include "sys_lib.h"
#include "motorfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define BROADCAST 127	/* also in motorstop.c !! */
#define DATA_RECEIVED	0
#define STATUS_OK	0

/****************************************************************************/
/**                                                                        **/
/**                           GLOBAL VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/
/*@-exportlocal@*/
/* stop lint from complainig, these are used in another task and therefore GLOBAL*/
_GLOBAL	RTCtime_typ		RTCtime;
_GLOBAL	USINT			MotorTemp[MAXMOTORS],MaxMotorTemp[MAXMOTORS];
_GLOBAL	STRING			MotorFaultStatus[MAXMOTORS][5];
_GLOBAL	DINT			MotorCurrent[MAXMOTORS],MaxMotorCurrent[MAXMOTORS];
/*@+exportlocal@*/


/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
/* NONE */
_LOCAL	BOOL			GetMotorCurrent;			/* command to get the motor current*/
_LOCAL	UINT            FaultMotor;                 /* for Alarmmessage */
_LOCAL	DINT BufLng;
_LOCAL	STRING DebugBuf[64];

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static	UINT 			Counter;
static	UINT 			SendWaitCounter;
static	UINT			GetTempStep;
static	UINT			GetTempMotor;
static	USINT			lastSecond;
static	UINT			SecondCounter;
static	BOOL			xopenOK;					/* Opening the interface was successful*/
static	INT 			i;
static 	USINT 			RequestData[READDATALENGTH]; 			/* array for requested Data */
static 	char 			StringDevice[30], StringMode[30]; 		/* initialize strings for FRM_xopen */
static 	UINT 			timeout;
static	UINT 			Motor;						/* index variable for polling loop*/
static	BOOL 			StopPolling;				/* stop polling loop to send a command*/
static	UINT 			PollingStep;
static	UINT 			RequestStep;
static 	UINT			ResendCounter;
static	BOOL			GetMotorCurrentMem;			/* to detect changing of command*/
static	INT				Error;						/* Error Number for debugging*/
static	INT				ReceivedChar;
static	char		    tmp[20];
static	FRM_xopen_typ 	FrameXOpenStruct;
static	FRM_close_typ 	FrameCloseStruct;
static	XOPENCONFIG 	XOpenConfigStruct;
static	FRM_gbuf_typ 	FrameGetBufferStruct;
static	FRM_rbuf_typ 	FrameReleaseBufferStruct;
static	FRM_robuf_typ 	FrameReleaseOutputBufferStruct;
static	FRM_write_typ 	FrameWriteStruct;
static 	FRM_read_typ 	FrameReadStruct;
static  TON_10ms_typ    Alarm36Delay,Alarm37Delay,Alarm38Delay,Alarm39Delay;

/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/


/**
 * Return the next motor-number in the row, skipping those that are not
 * used in differently equipped machines
 *
 * Input Parameter: Current motor-number
 *
 * Returns        : Next motor-number
 *
 */
static UINT	NextMotor(UINT lMotor)
{
	lMotor++;
	if(lMotor == FEEDER_HORIZONTAL && !GlobalParameter.FlexibleFeeder)
		lMotor++;
	if(lMotor == ADJUSTER)
		lMotor++;
	if(lMotor == SHUTTLE)
		lMotor++;
	if(lMotor == DELOADER_TURN && GlobalParameter.DeloaderTurnStation != DELOADER_AND_TURNSTATION)
		lMotor++;
	if(lMotor == DELOADER_HORIZONTAL && GlobalParameter.DeloaderTurnStation == NO_DELOADER)
		lMotor++;

	if(lMotor > MotorsConnected)
		lMotor = 1;

	return lMotor;
}


/**
 * Auxiliary function to handle timeout while reading from serial interface
 *
 * Input Parameter: ----
 *
 * Returns        : ----
 *
 */
static void HandleTimeout(int returnstep)
{
	Error = 5; 						/* set error level for FRM_read */
	timeout++;
	if (timeout == 60) /* 60*cyclic (->300ms) time no answer */
	{
		if(ResendCounter == 5) /*5 times tried? -> error*/
		{
			ResendCounter = 0;
			Motors[Motor].Error = TRUE;
			Motors[Motor].Moving = FALSE;
			Motors[Motor].MovingPlus = FALSE;
			Motors[Motor].MovingMinus = FALSE;
			Motors[Motor].ReferenceOk = FALSE;
			Motor = NextMotor(Motor);
			if(Motor == 1)
				Counter++;

			PollingStep = 0;
		}
		else
		{
			ResendCounter++;
			PollingStep = /*1*/returnstep;
		}
	}
}



/**
 * Auxiliary function to handle error while writing to serial interface
 *
 * Input Parameter: ----
 *
 * Returns        : ----
 *
 */
static void HandleWriteError(void)
{
	/* initialize release output buffer structure */
	FrameReleaseOutputBufferStruct.buffer = FrameGetBufferStruct.buffer;
	FrameReleaseOutputBufferStruct.buflng = FrameGetBufferStruct.buflng;
	FRM_robuf(&FrameReleaseOutputBufferStruct); 	/* release output buffer */

	Error = 3; 					/* set error level for FRM_write */

	if (FrameReleaseOutputBufferStruct.status != 0) 		/* check status */
	{
		Error = 4; 				/* set error level for FRM_robuf */
	}
}


/**
 * Auxiliary function to handle error while writing to serial interface
 *
 * Input Parameter: ----
 *
 * Returns        : ----
 *
 * TODO: Error handling
 */
static void ReleaseBuffer(void)
{
	/* initialize release buffer structure */
	FrameReleaseBufferStruct.buffer = FrameReadStruct.buffer;	/* get adress of read buffer */
	FrameReleaseBufferStruct.buflng = FrameReadStruct.buflng; 	/* get length of read buffer */

	FRM_rbuf(&FrameReleaseBufferStruct); 			/* release read buffer */

	if (FrameReleaseBufferStruct.status != 0) 				/* check status */
	{
		Error = 6; 					/* set error level for FRM_rbuf */
	}
}


/****************************************************************************/
/** Init function, executed once at startup                                **/
/****************************************************************************/
_INIT void InitProgram(void)
{
	Alarm36Delay.IN = FALSE;
	Alarm37Delay.IN = FALSE;
	Alarm38Delay.IN = FALSE;
	Alarm39Delay.IN = FALSE;
	GetMotorCurrent = FALSE;
	GetMotorCurrentMem = FALSE;
	GetTempMotor = 1;
	lastSecond = US(0);
	SecondCounter = 0;
	for(i=0;i<MAXMOTORS;i++)
	{
		MotorTemp[i] = US(0);
		MaxMotorTemp[i] = US(0);
		MotorCurrent[i] = L(0);
		MaxMotorCurrent[i] = L(0);
	}

	/* initialize start values */
	Motor = 1;
/* Motor Nummern sind 1,2,3,6,9
 * um die Zuordnung zwischen Adr. und Fkt. wie beim Jet2 zu haben
 */
	MotorsConnected = 10;
	SendWaitCounter=0;
	Counter = 0;
	StopPolling = FALSE;
	ResendCounter = 0;
	xopenOK = 0;
#ifdef PPC2100
	strcpy(StringDevice, "SL1.IF5"); 							/* interface #1 */
#else
	strcpy(StringDevice, "IF1"); 							/* interface #1 */
#endif
	strcpy(&StringMode[0], "/BD=19200 /PA=N /DB=8 /SB=1"); 		/* RS232 interface, 19200 BAUD, no parity, 8 data bits, 1 stop bit */

	/* initialize config structure */
	XOpenConfigStruct.idle = 6;                           	/* in Zeichenlängen*/
	XOpenConfigStruct.delimc = 1;
	XOpenConfigStruct.delim[0] = (unsigned char)10;				/* LF */
	XOpenConfigStruct.delim[1] = (unsigned char)13;				/* CR */
	XOpenConfigStruct.tx_cnt = 6;
	XOpenConfigStruct.rx_cnt = 6;
	XOpenConfigStruct.tx_len = 64;
	XOpenConfigStruct.rx_len = 64;
	XOpenConfigStruct.argc = 0;
	XOpenConfigStruct.argv = 0;

	/* initialize open structure */
	FrameXOpenStruct.enable = TRUE;
	FrameXOpenStruct.device = (UDINT) StringDevice;
	FrameXOpenStruct.mode = (UDINT) StringMode;
	FrameXOpenStruct.config = (UDINT) &XOpenConfigStruct;

	FRM_xopen(&FrameXOpenStruct); 						/* open an interface */

	if (FrameXOpenStruct.status == 0) 					/* check status */
	{
		xopenOK = TRUE;									/* set error level for FRM_xopen */

		/* initialize get buffer structure */
		FrameGetBufferStruct.enable = TRUE;
		FrameGetBufferStruct.ident = FrameXOpenStruct.ident;				/* get ident */

		/* initialize write structure */
		FrameWriteStruct.enable = TRUE;
		FrameWriteStruct.ident  = FrameXOpenStruct.ident;					/* get ident */

		/* initialize release output buffer structure */
		FrameReleaseOutputBufferStruct.enable = TRUE;
		FrameReleaseOutputBufferStruct.ident = FrameXOpenStruct.ident;		/* get ident */

		/* initialize read structure */
		FrameReadStruct.enable = TRUE;
		FrameReadStruct.ident = FrameXOpenStruct.ident;						/* get ident */

		/* initialize release buffer structure */
		FrameReleaseBufferStruct.enable = TRUE;
		FrameReleaseBufferStruct.ident = FrameXOpenStruct.ident;			/* get ident */

		/* initialize close structure */
		FrameCloseStruct.enable = TRUE;
		FrameCloseStruct.ident = FrameXOpenStruct.ident;					/* get ident */
	}
	else
	{
		xopenOK = FALSE;
	}
}


/****************************************************************************/
/** Cyclic function                                                        **/
/****************************************************************************/
_CYCLIC void CyclicProgram(void)
{
/*HA 24.02.04 V1.76 reset MaxCurrent on activating measurement*/
	if ( GetMotorCurrent && !GetMotorCurrentMem )
		for(i=0;i<MAXMOTORS;i++)
			MaxMotorCurrent[i] = 0;
	GetMotorCurrentMem = GetMotorCurrent;

	if (xopenOK != FALSE)
	{
/******************************************************************/
/* Cyclic polling of current Positions */
/******************************************************************/
		switch (PollingStep)
		{
			case 0:
			{
				if(StopPolling) break;
				/*Safety: priority 0 doesn't make sense*/
				if(Motors[Motor].Parameter.Priority == US(0)) Motors[Motor].Parameter.Priority = US(1);

				if( (Counter % Motors[Motor].Parameter.Priority) == 0  )
				{
					PollingStep = 100;
				}
				else
				{
					Motor = NextMotor(Motor);
					if(Motor == 1)
						Counter++;
				}
				ResendCounter = 0;
				break;
			}

/* clear buffer from any unwanted msg */
			case 100:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == DATA_RECEIVED) 					/* Data received */
				{
					ReleaseBuffer();
				}
				ResendCounter = 0;
				PollingStep = 1;
				break;
			}




/* 1st Step: send the POS command */
			case 1:
			{
				FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

				if (FrameGetBufferStruct.status == 0) 			/* check status */
				{
					size_t length = 4; /* length of "xPOS" */
					char *ptr = (char *)FrameGetBufferStruct.buffer;
				/*fill the buffer */
					(void) itoa(L(Motor),(UDINT) ptr);
					strcat(ptr,"POS\r\n");
					length = strlen(ptr);

					/* initialize write structure */
					FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;
					FrameWriteStruct.buflng = length+1;
					FRM_write(&FrameWriteStruct); 				/* write data to interface */

					if (FrameWriteStruct.status != 0) 			/* check status */
					{
						HandleWriteError();
					}
				}
	/*TODO: Fehler auswerten wenn getBuffer fehlschlägt (8071)*/
				PollingStep++;
				timeout = 0;
				break;
			}
/* wait for the answer of Motor*/
			case 2:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == DATA_RECEIVED) 					/* Data received */
				{
					BufLng = FrameReadStruct.buflng;
					strncpy(DebugBuf,((char *)FrameReadStruct.buffer),sizeof(DebugBuf));
					DebugBuf[BufLng] = '\0';
					if( (((char *)FrameReadStruct.buffer)[0] == '-')
					 || (   (((char *)FrameReadStruct.buffer)[0] >= '0')
					      &&(((char *)FrameReadStruct.buffer)[0] <= '9')
					    )
					  )
					{
						Motors[Motor].LastPosition = Motors[Motor].Position;
						Motors[Motor].Position = atoi( FrameReadStruct.buffer);
	/*@-realcompare@*/
						if(Motors[Motor].Parameter.IncrementsPerMm > 1.0)
							Motors[Motor].Position_mm = Motors[Motor].Position / Motors[Motor].Parameter.IncrementsPerMm;
	/*@+realcompare@*/

						if (Motor == ADJUSTER ||  Motor == CONVEYORBELT)
						{
							AbsMotorPos[Motor] = Motors[Motor].Position;
							AbsMotorPosMm[Motor] = Motors[Motor].Position_mm;
						}
						else
						{
							AbsMotorPos[Motor] = abs(Motors[Motor].Position);
							AbsMotorPosMm[Motor] = fabs(Motors[Motor].Position_mm);
						}


						if( labs(Motors[Motor].Position) > labs(Motors[Motor].LastPosition) )
						{
							Motors[Motor].MovingPlus = TRUE;
							Motors[Motor].MovingMinus = FALSE;
						}
						else
						{
							Motors[Motor].MovingPlus = FALSE;
							Motors[Motor].MovingMinus = TRUE;
						}

						if( labs(Motors[Motor].LastPosition - Motors[Motor].Position) > 100)
							Motors[Motor].Moving = TRUE;
						else
						{
							Motors[Motor].Moving = FALSE;
							Motors[Motor].MovingPlus = FALSE;
							Motors[Motor].MovingMinus = FALSE;
						}

						Motors[Motor].Error = FALSE;
					}
					ReleaseBuffer();
					if ( GetMotorCurrent )
					{
						PollingStep++;
						break;
					}
					else
					{
						Motor = NextMotor(Motor);
						if(Motor == 1)
							Counter++;

						PollingStep = 0;
						ResendCounter = 0;
						break;
					}
				}
				else	/* No data received or any error*/
				{
					HandleTimeout(1);
				}
				break;
			}
/* send the "get current" command*/
			case 3:
			{
					FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

					if (FrameGetBufferStruct.status == 0) 			/* check status */
					{
						size_t length = 4;
						char *ptr = (char *)FrameGetBufferStruct.buffer;
				/*fill the buffer */
						(void) itoa(L(Motor),(UDINT) ptr);
						strcat(ptr,"GRC\r\n"); /*actual current (Get Real Current)*/
						length = strlen(ptr);

					/* initialize write structure */
						FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;		/* get adress of send buffer */
						FrameWriteStruct.buflng = length+1;

						FRM_write(&FrameWriteStruct); 				/* write data to interface */

						if (FrameWriteStruct.status != 0) 			/* check status */
						{
							HandleWriteError();
						}
					}
	/*TODO: Fehler auswerten wenn getBuffer fehlschlägt (8071)*/
					PollingStep++;
					timeout = 0;
					break;
			}

/* wait for the answer of Motor on GRC*/
			case 4:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == DATA_RECEIVED) 					/* Data received */
				{
					if( (((char *)FrameReadStruct.buffer)[0] == '-')
					 || (   (((char *)FrameReadStruct.buffer)[0] >= '0')
					      &&(((char *)FrameReadStruct.buffer)[0] <= '9')
					    )
					  )
					{
						MotorCurrent[Motor] = atoi( FrameReadStruct.buffer);
						if ( MotorCurrent[Motor] > MaxMotorCurrent[Motor] )
							MaxMotorCurrent[Motor] = MotorCurrent[Motor];
					}
					ReleaseBuffer();

					Motor = NextMotor(Motor);
					if(Motor == 1)
						Counter++;

					PollingStep = 0;
					ResendCounter = 0;
					break;
				}
				else	/* No data received or any error*/
				{
					HandleTimeout(3);
				}
				break;
			}

		} /*switch*/




/******************************************************************/
/* requesting of Parameter */
/******************************************************************/
		switch (RequestStep)
		{
			case 0:
			{
				if(SerialCmd.Send == FALSE) break; /*wait for send command*/

				StopPolling = TRUE;
				if(PollingStep == 0)	/*wait for polling to stop*/
					RequestStep++;
				break;
			}
/* 1st Step: send the command */
			case 1:
			{
				FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

				if (FrameGetBufferStruct.status == STATUS_OK)
				{
					size_t length=0; /* length */
					char *ptr = (char *)FrameGetBufferStruct.buffer;
				/*fill the buffer */
					if( SerialCmd.SendMotor != BROADCAST )  /*no BROADCAST, so the address must be first*/
					{
						(void)itoa(L(SerialCmd.SendMotor),(UDINT) ptr);
						strcat(ptr,(char *) SerialCmd.CmdString);
					}
					else
						strcpy(ptr,(char *) SerialCmd.CmdString);

/* build the final commandstring from given parameters*/
					if (SerialCmd.UseParameter)
					{
						if (SerialCmd.UseRefDirection)
						{
				/*
				 * for absolute positioning:
				 * if Reference direction positive then the way to move is in negative direction
				 * therefore labs(SerialCmd.Parameter)
				*/
							if (STREQ(SerialCmd.CmdString,CMD_ABSOLUTE_POS))
							{
								if( (SerialCmd.SendMotor == CONVEYORBELT) || (SerialCmd.SendMotor == ADJUSTER) )
								{
							/*
							 * special case: the belt drives are able to move in a position below 0
							 * so this is allowed here in that special case
							*/
									(void)itoa(SerialCmd.Parameter,(UDINT) &tmp[0]);
								}
								else
								{
									if (Motors[SerialCmd.SendMotor].Parameter.ReferenceDirectionNegative == FALSE)
										strcat(ptr,"-");
									(void)itoa(labs(SerialCmd.Parameter),(UDINT) &tmp[0]);
								}
							}
							else
				/*
				 * for relative positioning:
				 * moving towards reference pos. must be given as negative values
				 * UseRefDirection must be TRUE
				 * the same applies for speed setting (V-command)
				*/

							if ( (STREQ(SerialCmd.CmdString,CMD_RELATIVE_POS))
								||
								(STREQ(SerialCmd.CmdString,CMD_MOVE_AT)))
							{
								if(Motors[SerialCmd.SendMotor].Parameter.ReferenceDirectionNegative)
								{
									if (SerialCmd.Parameter < 0L)
										strcat(ptr,"-");
								}
								else
								{
									if (SerialCmd.Parameter > 0L)
										strcat(ptr,"-");
								}
								(void)itoa(labs(SerialCmd.Parameter),(UDINT) &tmp[0]);
							}
						}
						else /* don't use RefDirection: take Parameter just as it's given */
							(void)itoa(SerialCmd.Parameter,(UDINT) &tmp[0]);

						strcat(ptr,tmp);
					}
					strcat(ptr,"\r\n");
					length = strlen(ptr);

					/* initialize write structure */
					FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;	/* get adress of send buffer */
					FrameWriteStruct.buflng = length+1; 					/* set length of send buffer */

					FRM_write(&FrameWriteStruct); 				/* write data to interface */

					if (FrameWriteStruct.status != 0) 			/* check status */
					{
						HandleWriteError();
					}
				}
				RequestStep++;
				timeout = 0;
				break;
			}
/* wait for the answer of Motor*/
			case 2:
			{
				/* no request: go on*/
				if(SerialCmd.Request == FALSE)
				{
					SendWaitCounter++;
					if(SendWaitCounter > 6)
					{
						SendWaitCounter=0;
						SerialCmd.Request = FALSE;
						SerialCmd.Send = FALSE;
						StopPolling = FALSE;
						RequestStep = 0;
					}
/* HA 13.02.04 V1.76 no request (command only): leave the code immediately */
					break;
				}
/* otherwise go ahead waiting for answer*/

				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == DATA_RECEIVED) 					/* Data received */
				{
					ReceivedChar = (INT) FrameReadStruct.buflng;
					memcpy(RequestData, (USINT*)FrameReadStruct.buffer, (size_t)ReceivedChar); /* copy read data into array */
					Motors[SendMotor].Error = FALSE;

					ReleaseBuffer();

					RequestStep = 0;
					SerialCmd.Request = FALSE;
					SerialCmd.Send = FALSE;
					StopPolling = FALSE;
				}
				else	/* No data received or any error*/
				{
					ReceivedChar = 0;
					Error = 5; 						/* set error level for FRM_read */
					timeout++;
					if (timeout == 60) /* 60*cyclic time no answer */
					{
						RequestStep = 0;
						SerialCmd.Request = FALSE;
						SerialCmd.Send = FALSE;
						StopPolling = FALSE;
						Motors[SerialCmd.SendMotor].Error = TRUE;
						Motors[SerialCmd.SendMotor].Moving = FALSE;
						Motors[SerialCmd.SendMotor].MovingPlus = FALSE;
						Motors[SerialCmd.SendMotor].MovingMinus = FALSE;
					}
				}
				break;
			}
		} /*switch*/

	} /*if (xopenOK == 1)*/


/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
/* Alarm 36 bis 39 5s verzögern, damit der Faultmotor auch sicher eingetragen wurde */
	Alarm36Delay.PT = 500;
	TON_10ms(&Alarm36Delay);
	AlarmBitField[36] = Alarm36Delay.Q;

	Alarm37Delay.PT = 500;
	TON_10ms(&Alarm37Delay);
	AlarmBitField[37] = Alarm37Delay.Q;

	Alarm38Delay.PT = 500;
	TON_10ms(&Alarm38Delay);
	AlarmBitField[38] = Alarm38Delay.Q;

	Alarm39Delay.PT = 500;
	TON_10ms(&Alarm39Delay);
	AlarmBitField[39] = Alarm39Delay.Q;

	switch (GetTempStep)
	{
/* get Temperature every 5 seconds*/
		case 0:
		{
			int CurrentMotor=1,j=0;
/*Fehlermedlungen zurücksetzen, wenn alle Motoren keinen Fehler mehr melden */
			while(CurrentMotor <= MotorsConnected)
			{
				if(CurrentMotor == FEEDER_HORIZONTAL && !GlobalParameter.FlexibleFeeder)
					CurrentMotor++;
				if(CurrentMotor == ADJUSTER)
					CurrentMotor++;
				if(CurrentMotor == SHUTTLE)
					CurrentMotor++;

				if(   (CurrentMotor == DELOADER_TURN)
				   && (GlobalParameter.DeloaderTurnStation != DELOADER_AND_TURNSTATION) )
					CurrentMotor++;

				if(   (CurrentMotor == DELOADER_HORIZONTAL)
				   && (GlobalParameter.DeloaderTurnStation == NO_DELOADER) )
					CurrentMotor++;

				if( !STREQ(MotorFaultStatus[CurrentMotor],"0000"))
					j++;

				CurrentMotor++;
			}

			if( j == 0 )
			{
				Alarm36Delay.IN = FALSE;
				Alarm37Delay.IN = FALSE;
				Alarm38Delay.IN = FALSE;
				Alarm39Delay.IN = FALSE;
				AlarmBitField[36] = FALSE;
				AlarmBitField[37] = FALSE;
				AlarmBitField[38] = FALSE;
				AlarmBitField[39] = FALSE;
			}


			if( RTCtime.second != lastSecond)
			{
				if(SecondCounter >= 5 )
				{
					GetTempStep = 1;
					SecondCounter = 0;
				}
				else
					SecondCounter++;
				lastSecond = RTCtime.second;
				break;
			}
			break;
		}
		case 1:
		{
			if(Motors[GetTempMotor].Error)
			{
				GetTempMotor = NextMotor(GetTempMotor);
				GetTempStep = 0;
				break;
			}

			if( SerialCmd.Send == FALSE )
			{
				if( RequestMotorData(GetTempMotor,CMD_GETTEMP) )
					GetTempStep = 5;
				break;
			}
			break;
		}

		case 5:
		{
			if(SerialCmd.Request == FALSE)
			{
				if(  ReceivedChar != 0
			     && !Motors[SerialCmd.SendMotor].Error )
			    {
					MotorTemp[GetTempMotor] = US(atoi( (UDINT) &RequestData[0]));
					if ( MotorTemp[GetTempMotor] > MaxMotorTemp[GetTempMotor] )
					{
						MaxMotorTemp[GetTempMotor] = MotorTemp[GetTempMotor];
					}
				}
				GetTempStep = 10;
			}
			break;
		}
		case 10:
		{
			if ( RequestMotorData(GetTempMotor,CMD_GETFAULTSTAT) )
				GetTempStep = 15;

			break;
		}
		case 15:
		{
			if( SerialCmd.Request == FALSE )
			{
				if(  ReceivedChar != 0
			     && !Motors[SerialCmd.SendMotor].Error )
			    {
					memcpy(&MotorFaultStatus[GetTempMotor][0],&RequestData[0],4);
					MotorFaultStatus[GetTempMotor][4] = 0;
					if(MotorFaultStatus[GetTempMotor][0] == '1')
					{/* Übertemp. */
						Alarm36Delay.IN = TRUE;
						FaultMotor = GetTempMotor;
					}
					if(   Motors[GetTempMotor].ReferenceOk
					  && (MotorFaultStatus[GetTempMotor][1] == '1') )
					{/* Überstrom */
						Alarm37Delay.IN = TRUE;
						FaultMotor = GetTempMotor;
					}
					if(MotorFaultStatus[GetTempMotor][2] == '1')
					{/* Unterspannung */
						Alarm38Delay.IN = TRUE;
						FaultMotor = GetTempMotor;
					}
					if(MotorFaultStatus[GetTempMotor][3] == '1')
					{/* Überspannung */
						Alarm39Delay.IN = TRUE;
						FaultMotor = GetTempMotor;
					}
				}

				GetTempMotor = NextMotor(GetTempMotor);
				GetTempStep = 0;
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


