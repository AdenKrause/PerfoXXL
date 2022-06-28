#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	serielle Kommunikation mit Thermo Kühl/Heizgerät                        */
/*	inklusive zyklisches Polling der Ist-Temperatur							*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			05.02.07	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
/* header files */
#include <dvframe.h>
#include <math.h>
#include <string.h>
#include <sys_lib.h>
#include "glob_var.h"
#include "egmglob_var.h"

#define THERMO_ADDRESS	(1)
#define READ_ACK		(0x00)
#define READ_STATUS 	(0x09)
#define READ_INT_TEMP	(0x20)
#define READ_EXT_TEMP	(0x21)
#define READ_SETPOINT	(0x70)
#define READ_LIMIT_LOW	(0x40)
#define READ_LIMIT_HIGH	(0x60)
#define READ_HEAT_P		(0x71)
#define READ_HEAT_I		(0x72)
#define READ_HEAT_D		(0x73)
#define READ_COOL_P		(0x74)
#define READ_COOL_I		(0x75)
#define READ_COOL_D		(0x76)

#define SET_SETPOINT	(0xF0)
#define SET_LIMIT_LOW	(0xC0)
#define SET_LIMIT_HIGH	(0xE0)
#define SET_HEAT_P		(0xF1)
#define SET_HEAT_I		(0xF2)
#define SET_HEAT_D		(0xF3)
#define SET_COOL_P		(0xF4)
#define SET_COOL_I		(0xF5)
#define SET_COOL_D		(0xF6)
#define ON_OFF_ARRAY	(0x81)

#define PRECISION_0_1_NOUNITS (0x10)
#define PRECISION_0_01_NOUNITS (0x20)
#define PRECISION_0_1_DEG_C (0x11)
#define PRECISION_0_01_DEG_C (0x21)

#define BIT0	((char)0x01)
#define BIT1	((char)0x02)
#define BIT2	((char)0x04)
#define BIT3	((char)0x08)
#define BIT4	((char)0x10)
#define BIT5	((char)0x20)
#define BIT6	((char)0x40)
#define BIT7	((char)0x80)

#define ANSWER_OK				(0)
#define ANSWER_INVALID			(-1)
#define ANSWER_BAD_CHECKSUM		(-2)
#define ANSWER_BAD_COMMAND		(-3)
#define ANSWER_INVALID_CHECKSUM	(-4)

#define HIGHBYTE(V)	((USINT) ((V>>8) & 0x00ff))
#define LOWBYTE(V)	((USINT) (V & 0x00ff))



/* variable declaration */
FRM_xopen_typ FrameXOpenStruct; 					/* variable of type FRMXOPEN */
FRM_close_typ FrameCloseStruct; 					/* variable of type FRMCLOSE */
XOPENCONFIG XOpenConfigStruct; 						/* variable of type XOPENCONFIG */
FRM_gbuf_typ FrameGetBufferStruct; 					/* variable of type FRMGBUF */
FRM_rbuf_typ FrameReleaseBufferStruct; 				/* variable of type FRMRBUF */
FRM_robuf_typ FrameReleaseOutputBufferStruct; 		/* variable of type FRMROBUF */
FRM_write_typ FrameWriteStruct; 					/* variable of type FRMWRITE */
FRM_read_typ FrameReadStruct; 						/* variable of type FRMREAD */
USINT Error, xopenOK;
USINT StringDevice[30], StringMode[30]; 			/* initialize strings for FRM_xopen */
USINT RequestData[READDATALENGTH]; /* array for requested Data */


_LOCAL BOOL StopPolling;
_LOCAL USINT PollingStep;
_LOCAL USINT RequestStep;
_LOCAL USINT timeout;

_LOCAL	USINT	ResendCounter;
_LOCAL	USINT	ErrorCounter;

_LOCAL	REAL	testval;

_GLOBAL	RTCtime_typ	RTCtime;
		USINT	lastSecond,SecondCounter;

_LOCAL UINT	Cooler_UnitOnInv;
_LOCAL UINT	Cooler_UnitStoppingInv;
_LOCAL UINT	Cooler_UnitFaultedInv;
_LOCAL UINT	Cooler_HTCFaultInv;
_LOCAL UINT	Cooler_PumpOnInv;
_LOCAL UINT	Cooler_CompressorOnInv;
_LOCAL UINT	Cooler_HeaterOnInv;
_LOCAL UINT	Cooler_LevelWarnInv;
_LOCAL UINT	Cooler_LevelFaultInv;
_LOCAL UINT	Cooler_LowTempWarnInv;
_LOCAL UINT	Cooler_HighTempWarnInv;
_LOCAL UINT	Cooler_LowTempFaultInv;
_LOCAL UINT	Cooler_HighTempFaultInv;
_LOCAL UINT	Cooler_RefrigTempHighInv;


static int Counter;
static int SendWaitCounter;
static int CurrentReadCommand,CurrentSetCommand;
static INT tmpval, SetPointOnly;

static int ReadCommands[] = {READ_STATUS, READ_INT_TEMP, READ_EXT_TEMP, READ_SETPOINT,
                             READ_LIMIT_LOW, READ_LIMIT_HIGH,
                             READ_HEAT_P, READ_HEAT_I, READ_HEAT_D,
                             READ_COOL_P, READ_COOL_I, READ_COOL_D };

static int SetCommands[] = {SET_SETPOINT,
                             SET_LIMIT_LOW, SET_LIMIT_HIGH,
                             SET_HEAT_P, SET_HEAT_I, SET_HEAT_D,
                             SET_COOL_P, SET_COOL_I, SET_COOL_D };


_LOCAL USINT StartUnit,StopUnit,ApplySetpoint;
static TON_10ms_typ	SetpointChangeTimer,SetpointChangeTimer2;
static BOOL SetpointChangeFlag;

/*------------------------------------------------------------------------------------
 initialize program starts here
-------------------------------------------------------------------------------------*/
_INIT void InitProgram(void)
{
	Cooler.NewValues.SetPoint = 22;
	Cooler.NewValues.LimitLow = 1.5;
	Cooler.NewValues.LimitHigh = 82;
	Cooler.NewValues.HeatP = 0.6;
	Cooler.NewValues.HeatI = 0.6;
	Cooler.NewValues.HeatD = 0.0;
	Cooler.NewValues.CoolP = 0.6;
	Cooler.NewValues.CoolI = 0.6;
	Cooler.NewValues.CoolD = 0.0;

	CurrentReadCommand = 0;
	CurrentSetCommand = 0;
	SetPointOnly = FALSE;
	lastSecond = 0;
	SecondCounter = 0;
	SendWaitCounter=0;
	Counter = 0;
	StopPolling = 0;
	ResendCounter = 0;
	ErrorCounter = 0;
	xopenOK = 0;
	strcpy(StringDevice, "IF1"); 							/* interface #1 */
	strcpy(StringMode, "/BD=19200 /PA=N /DB=8 /SB=1"); 		/* RS232 interface, 57600 BAUD, no parity, 8 data bits, 1 stop bit */

	/* initialize config structure */
	XOpenConfigStruct.idle = 6;                           /* in Zeichenlängen*/
	XOpenConfigStruct.delimc = 0;		/* keine Frame Ende Zeichen */
	XOpenConfigStruct.delim[0] = 10;	/* nicht benutzt, weil delimc==0 */
	XOpenConfigStruct.delim[1] = 13;	/* nicht benutzt, weil delimc==0 */
	XOpenConfigStruct.tx_cnt = 2;
	XOpenConfigStruct.rx_cnt = 2;
	XOpenConfigStruct.tx_len = 32;
	XOpenConfigStruct.rx_len = 32;
	XOpenConfigStruct.argc = 0;
	XOpenConfigStruct.argv = 0;

	/* initialize open structure */
	FrameXOpenStruct.enable = 1;
	FrameXOpenStruct.device = (UDINT) StringDevice;
	FrameXOpenStruct.mode = (UDINT) StringMode;
	FrameXOpenStruct.config = (UDINT) &XOpenConfigStruct;

	FRM_xopen(&FrameXOpenStruct); 						/* open an interface */

	if (FrameXOpenStruct.status == 0) 					/* check status */
	{
		xopenOK = 1;									/* set error level for FRM_xopen */

		/* initialize get buffer structure */
		FrameGetBufferStruct.enable = 1;
		FrameGetBufferStruct.ident = FrameXOpenStruct.ident;				/* get ident */

		/* initialize write structure */
		FrameWriteStruct.enable = 1;
		FrameWriteStruct.ident  = FrameXOpenStruct.ident;					/* get ident */

		/* initialize release output buffer structure */
		FrameReleaseOutputBufferStruct.enable = 1;
		FrameReleaseOutputBufferStruct.ident = FrameXOpenStruct.ident;		/* get ident */

		/* initialize read structure */
		FrameReadStruct.enable = 1;
		FrameReadStruct.ident = FrameXOpenStruct.ident;						/* get ident */

		/* initialize release buffer structure */
		FrameReleaseBufferStruct.enable = 1;
		FrameReleaseBufferStruct.ident = FrameXOpenStruct.ident;			/* get ident */

		/* initialize close structure */
		FrameCloseStruct.enable = 1;
		FrameCloseStruct.ident = FrameXOpenStruct.ident;					/* get ident */
	}
	else
	{
		xopenOK = 0;
	}
}



USINT CreateChecksum(char *buf, int length)
{
	int i,sum;

	sum = 0;
/* start at index 1 to leave out leading 0xCA */
	for(i=1;i<=length;i++)
	{
		sum += buf[i];
	}
	return  ~((USINT)sum);
}


int CreateCommand(char *cmdstring, INT Addr, USINT command, INT Value)
{
	int retval=0;

	cmdstring[0] = 0xCA;	/* Start cmd RS232 always $CA */
	cmdstring[1] = 0x00;	/* Addr MSB always 0 */
	if ( Addr <= 64 )
		cmdstring[2] = (USINT) Addr;
	else
		cmdstring[2] = 1;

	cmdstring[3] = command;		 	/* command  */

	switch (command)
	{
		case READ_STATUS:
		case READ_INT_TEMP:
		case READ_EXT_TEMP:
		case READ_SETPOINT:
		case READ_LIMIT_LOW:
		case READ_LIMIT_HIGH:
		case READ_HEAT_P:
		case READ_HEAT_I:
		case READ_HEAT_D:
		case READ_COOL_P:
		case READ_COOL_I:
		case READ_COOL_D:
		{
			cmdstring[4] = 0x00;			/* number of data bytes */
/* calculate checksum */
			cmdstring[5] = CreateChecksum(cmdstring,4);
/* length of command w/out data is always 6 bytes */
			retval = 6;
			break;
		}
		case SET_SETPOINT:
		case SET_LIMIT_LOW:
		case SET_LIMIT_HIGH:
		case SET_HEAT_P:
		case SET_HEAT_I:
		case SET_HEAT_D:
		case SET_COOL_P:
		case SET_COOL_I:
		case SET_COOL_D:
		{
			cmdstring[4] = 0x02;			/* number of data bytes */

			cmdstring[5] = HIGHBYTE(Value);
			cmdstring[6] = LOWBYTE(Value);
/* calculate checksum */
			cmdstring[7] = CreateChecksum(cmdstring,6);
/* length of command with data is always 8 bytes */
			retval = 8;
			break;
		}
		case ON_OFF_ARRAY:
		{
			cmdstring[4] = 0x08;			/* number of data bytes */
			if (Value == 1)
				cmdstring[5] = 0x01; /* Unit ON */
			else
			if (Value == 0)
				cmdstring[5] = 0x00; /* Unit OFF */

			cmdstring[6] = 0x02;
			cmdstring[7] = 0x02;
			cmdstring[8] = 0x02;
			cmdstring[9] = 0x02;
			cmdstring[10] = 0x00; /* 0.01 °C enable OFF */
			cmdstring[11] = 0x02;
			cmdstring[12] = 0x02;
/* calculate checksum */
			cmdstring[13] = CreateChecksum(cmdstring,12);
/* length of command with data is always 8 bytes */
			retval = 14;
			break;
		}
	}

	return retval;
}


SINT ParseAnswer(USINT *buf, INT Addr, INT length, USINT command, REAL *value)
{
	BOOL retval = ANSWER_INVALID;
	USINT qualifier;

	if( buf[0] != 0xCA
	 || buf[1] != 0
	 || buf[2] != (USINT) Addr
	  )
	  return ANSWER_INVALID;

	if(buf[3] == 0x0F)
	{
		switch (buf[5])
		{
			case 1: /* bad coommand */
			{
				retval = ANSWER_BAD_COMMAND;
				break;
			}
			case 3:/* bad checksum */
			{
				retval = ANSWER_BAD_CHECKSUM;
				break;
			}
			default: /* other cases not possible per definition*/
			{
				retval = ANSWER_INVALID;
				break;
			}
		}
		return retval;
	}
	else
	if(buf[3] != command)
		return ANSWER_INVALID;


	if( buf[4] <= 8)   /* max 8 data bytes allowed */
	{
/* Telegramm ok, auswerten */
		switch (command)
		{
			case READ_INT_TEMP:
			case READ_EXT_TEMP:
			case READ_SETPOINT:
			case READ_LIMIT_LOW:
			case READ_LIMIT_HIGH:
			case READ_HEAT_P:
			case READ_HEAT_I:
			case READ_HEAT_D:
			case READ_COOL_P:
			case READ_COOL_I:
			case READ_COOL_D:
			case SET_SETPOINT:
			case SET_LIMIT_LOW:
			case SET_LIMIT_HIGH:
			case SET_HEAT_P:
			case SET_HEAT_I:
			case SET_HEAT_D:
			case SET_COOL_P:
			case SET_COOL_I:
			case SET_COOL_D:
			{
				if( buf[4] == 3)
				{
					qualifier = buf[5];
		/* Wert aus Antwort isolieren und umrechnen */
					*value = buf[6]*256 + buf[7];
					if( qualifier == PRECISION_0_1_NOUNITS || qualifier == PRECISION_0_1_DEG_C)
						*value /= 10.0;
					else
					if( qualifier == PRECISION_0_01_NOUNITS || qualifier == PRECISION_0_01_DEG_C)
						*value /= 100.0;

					retval = ANSWER_OK;
				}
				else
					retval = ANSWER_INVALID;
				break;
			}
			case READ_STATUS:
			{
				if (buf[4] == 5)
				{
					Cooler.RawDataB1 = buf[5];
					Cooler.RawDataB2 = buf[6];
					Cooler.RawDataB3 = buf[7];
					Cooler.RawDataB4 = buf[8];
					Cooler.RawDataB5 = buf[9];

					Cooler.RTD1OpenFault         = (Cooler.RawDataB1 & BIT7) != 0;
					Cooler.RTD1ShortedFault      = (Cooler.RawDataB1 & BIT6) != 0;
					Cooler.RTD1Open              = (Cooler.RawDataB1 & BIT5) != 0;
					Cooler.RTD1Shorted           = (Cooler.RawDataB1 & BIT4) != 0;
					Cooler.RTD3OpenFault         = (Cooler.RawDataB1 & BIT3) != 0;
					Cooler.RTD3ShortedFault      = (Cooler.RawDataB1 & BIT2) != 0;
					Cooler.RTD3Open              = (Cooler.RawDataB1 & BIT1) != 0;
					Cooler.RTD3Shorted           = (Cooler.RawDataB1 & BIT0) != 0;

					Cooler.RTD2OpenFault         = (Cooler.RawDataB2 & BIT7) != 0;
					Cooler.RTD2ShortedFault      = (Cooler.RawDataB2 & BIT6) != 0;
					Cooler.RTD2OpenWarn          = (Cooler.RawDataB2 & BIT5) != 0;
					Cooler.RTD2ShortedWarn       = (Cooler.RawDataB2 & BIT4) != 0;
					Cooler.RTD2Open              = (Cooler.RawDataB2 & BIT3) != 0;
					Cooler.RTD2Shorted           = (Cooler.RawDataB2 & BIT2) != 0;
					Cooler.RefrigHighTemp        = (Cooler.RawDataB2 & BIT1) != 0;
					Cooler.HTCFault              = (Cooler.RawDataB2 & BIT0) != 0;

					Cooler.HighFixedTempFault    = (Cooler.RawDataB3 & BIT7) != 0;
					Cooler.LowFixedTempFault     = (Cooler.RawDataB3 & BIT6) != 0;
					Cooler.HighTempFault         = (Cooler.RawDataB3 & BIT5) != 0;
					Cooler.LowTempFault          = (Cooler.RawDataB3 & BIT4) != 0;
					Cooler.LowLevelFault         = (Cooler.RawDataB3 & BIT3) != 0;
					Cooler.HighTempWarn          = (Cooler.RawDataB3 & BIT2) != 0;
					Cooler.LowTempWarn           = (Cooler.RawDataB3 & BIT1) != 0;
					Cooler.LowLevelWarn          = (Cooler.RawDataB3 & BIT0) != 0;

					Cooler.BuzzerOn              = (Cooler.RawDataB4 & BIT7) != 0;
					Cooler.AlarmMuted            = (Cooler.RawDataB4 & BIT6) != 0;
					Cooler.UnitFaulted           = (Cooler.RawDataB4 & BIT5) != 0;
					Cooler.UnitStopping          = (Cooler.RawDataB4 & BIT4) != 0;
					Cooler.UnitOn                = (Cooler.RawDataB4 & BIT3) != 0;
					Cooler.PumpOn                = (Cooler.RawDataB4 & BIT2) != 0;
					Cooler.CompressorOn          = (Cooler.RawDataB4 & BIT1) != 0;
					Cooler.HeaterOn              = (Cooler.RawDataB4 & BIT0) != 0;

					Cooler.RTD2Controlling       = (Cooler.RawDataB5 & BIT7) != 0;
					Cooler.HeatLEDFlashing       = (Cooler.RawDataB5 & BIT6) != 0;
					Cooler.CoolLEDFlashing       = (Cooler.RawDataB5 & BIT5) != 0;
					Cooler.CoolLEDOn             = (Cooler.RawDataB5 & BIT4) != 0;

					retval = ANSWER_OK;
				}
				else
					retval = ANSWER_INVALID;
				break;
			}
		}
	}
	else	/* number of data bytes not valid (>8) */
		retval = ANSWER_INVALID;


	return retval;
}

/*--------------------------------------------------------------------------------------
 cyclic program start here
--------------------------------------------------------------------------------------*/
_CYCLIC void CyclicProgram(void)
{
	Cooler_UnitOnInv              = !Cooler.UnitOn;
	Cooler_UnitStoppingInv        = !Cooler.UnitStopping;
	Cooler_UnitFaultedInv         = !Cooler.UnitFaulted;
	Cooler_PumpOnInv              = !Cooler.PumpOn;
	Cooler_CompressorOnInv        = !Cooler.CompressorOn;
	Cooler_HeaterOnInv            = !Cooler.HeaterOn;
	Cooler_LevelWarnInv           = !Cooler.LowLevelWarn;
	Cooler_LevelFaultInv          = !Cooler.LowLevelFault;
	Cooler_LowTempWarnInv         = !Cooler.LowTempWarn;
	Cooler_HighTempWarnInv        = !Cooler.HighTempWarn;
	Cooler_LowTempFaultInv        = !Cooler.LowTempFault;
	Cooler_HighTempFaultInv       = !Cooler.HighTempFault;
	Cooler_RefrigTempHighInv      = !Cooler.RefrigHighTemp;
	Cooler_HTCFaultInv            = !Cooler.HTCFault;


/*
 * if New setpoint and current Setpoint differ less than 3 °C but are not equal,
 * then we send the new setpoint because it will not be sent automatically due
 * to if(diff>3) clause
 */
	TON_10ms(&SetpointChangeTimer);
	TON_10ms(&SetpointChangeTimer2);

	SetpointChangeTimer.PT = 6000; /* 1 min */
	if( (fabs(Cooler.CurrentValues.SetPoint-Cooler.NewValues.SetPoint) < 3.0)
	&&  (fabs(Cooler.CurrentValues.SetPoint-Cooler.NewValues.SetPoint) > 0.2)
	&&  !SetpointChangeTimer.Q
	 )
		SetpointChangeTimer.IN = TRUE;
	else
		SetpointChangeTimer.IN = FALSE;

/*
 * more than 3 °C diff for more than 20 sec: apply new setpoint
 */
	TON_10ms(&SetpointChangeTimer2);
	SetpointChangeTimer2.PT = 2000; /* 20 sec */
	if( (fabs(Cooler.CurrentValues.SetPoint-Cooler.NewValues.SetPoint) > 3.0)
	&&  !SetpointChangeTimer2.Q
	 )
		SetpointChangeTimer2.IN = TRUE;
	else
		SetpointChangeTimer2.IN = FALSE;

	if(SetpointChangeTimer.Q || SetpointChangeTimer2.Q)
		SetpointChangeFlag = TRUE;

	if (xopenOK == 1)
	{

/******************************************************************/
/* Cyclic polling of current Temperature and status */
/******************************************************************/

		switch (PollingStep)
		{
			case 0:
			{
			/*
			 * Start command from Developertank task
			 */
				if( Cooler.Cmd_Start )
					StartUnit = TRUE;

				if (StartUnit)
				{
					Cooler.Cmd_Start = FALSE;
					PollingStep = 10;
	 				ResendCounter = 0;
					break;
				}
			/*
			 * Stop command from Developertank task
			 */
				if( Cooler.Cmd_Stop )
					StopUnit = TRUE;

				if (StopUnit)
				{
					Cooler.Cmd_Stop = FALSE;
					PollingStep = 10;
	 				ResendCounter = 0;
					break;
				}

				if (ApplySetpoint	/* button */
				|| (SetpointChangeFlag) /* flag from timer */
				)
				{
					if(!ApplySetpoint) /*not the button, but SetPoint changed by Application */
						SetPointOnly = TRUE;
					else
						SetPointOnly = FALSE;

					SetpointChangeFlag = FALSE;
					PollingStep = 20;
					ApplySetpoint = 0;
	 				ResendCounter = 0;
	 				CurrentSetCommand = 0;
					break;
				}

/* every 1 sec */
				if( RTCtime.second != lastSecond)
				{
					if(SecondCounter >= 1 )
					{
						PollingStep++;
						ResendCounter = 0;
						SecondCounter = 0;
					}
					else
						SecondCounter++;
					lastSecond = RTCtime.second;
					break;
				}

				break;
			}
/* 1st Step: send the GET Temp command */
			case 1:
			{
				FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

				if (FrameGetBufferStruct.status == 0) 			/* check status */
				{
					int length;

					length = CreateCommand((USINT*)FrameGetBufferStruct.buffer,
					                        THERMO_ADDRESS, ReadCommands[CurrentReadCommand], 0);
					/* initialize write structure */
					FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;		/* get adress of send buffer */
					FrameWriteStruct.buflng = length;

					FRM_write(&FrameWriteStruct); 				/* write data to interface */

					if (FrameWriteStruct.status != 0) 			/* check status */
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
				}
	/*TODO: Fehler auswerten wenn getBuffer fehlschlägt (8071)*/
				PollingStep++;
				timeout = 0;
				break;
			}
/* wait for the answer */
			case 2:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data from interface */

				if (FrameReadStruct.status == 0) 					/* check status */
				{
					/* Checksum ok? */
					USINT tmp = CreateChecksum((USINT *)FrameReadStruct.buffer,FrameReadStruct.buflng-2);

					if( tmp == ((USINT *)FrameReadStruct.buffer)[FrameReadStruct.buflng-1] )
					{
						/* checksum OK */
						if(ParseAnswer((USINT *)FrameReadStruct.buffer, THERMO_ADDRESS,
						            FrameReadStruct.buflng, ReadCommands[CurrentReadCommand], &testval) != 0)
						{
							Error = 12; /* answer not correct*/
						}
						else	/* Answer formal OK */
						{
							/* Answer formal OK, value in testval */
							switch (ReadCommands[CurrentReadCommand])
							{
								case READ_STATUS: break;/* status is put into struct direcrly in parser */
								case READ_INT_TEMP:
								{
									Cooler.IntTemp = testval;
									break;
								}
								case READ_EXT_TEMP:
								{
									Cooler.ExtTemp = testval;
									break;
								}
								case READ_SETPOINT:
								{
									Cooler.CurrentValues.SetPoint = testval;
									break;
								}
								case READ_LIMIT_LOW:
								{
									Cooler.CurrentValues.LimitLow = testval;
									break;
								}
								case READ_LIMIT_HIGH:
								{
									Cooler.CurrentValues.LimitHigh = testval;
									break;
								}
								case READ_HEAT_P:
								{
									Cooler.CurrentValues.HeatP = testval;
									break;
								}
								case READ_HEAT_I:
								{
									Cooler.CurrentValues.HeatI = testval;
									break;
								}
								case READ_HEAT_D:
								{
									Cooler.CurrentValues.HeatD = testval;
									break;
								}
								case READ_COOL_P:
								{
									Cooler.CurrentValues.CoolP = testval;
									break;
								}
								case READ_COOL_I:
								{
									Cooler.CurrentValues.CoolI = testval;
									break;
								}
								case READ_COOL_D:
								{
									Cooler.CurrentValues.CoolD = testval;
									break;
								}
							}
						}

					}
					else
					{
						/* checksum error */
						Error = 11;
					}

					/* initialize release buffer structure */
					FrameReleaseBufferStruct.buffer = FrameReadStruct.buffer;	/* get adress of read buffer */
					FrameReleaseBufferStruct.buflng = FrameReadStruct.buflng; 	/* get length of read buffer */

					FRM_rbuf(&FrameReleaseBufferStruct); 			/* release read buffer */

					ResendCounter = 0;

					if (FrameReleaseBufferStruct.status != 0) 				/* check status */
					{
						Error = 6; 					/* set error level for FRM_rbuf */
					}
					if (CurrentReadCommand < 11)
					{
						CurrentReadCommand++;
						PollingStep = 1;
					}
					else
					{
						CurrentReadCommand = 0;
						PollingStep = 0;
					}
				}
				else
				{
					Error = 5; 						/* set error level for FRM_read */
					timeout++;
					if (timeout >= 20) /* 20*cyclic (->2s) time no answer */
					{
						if(ResendCounter >= 5) /*5 times tried? -> error*/
						{
							if (CurrentReadCommand < 11)
							{
								CurrentReadCommand++;
								PollingStep = 1;
							}
							else
							{
								CurrentReadCommand = 0;
								PollingStep = 0;
							}
							ResendCounter = 0;
						}
						else
						{
							ErrorCounter++;
							ResendCounter++;
							PollingStep = 1;
						}
					}
				}
				break;
			}



/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*  send the Start Unit command */
			case 10:
			{
				FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

				if (FrameGetBufferStruct.status == 0) 			/* check status */
				{
					int length;

					if (StartUnit)
					{
						length = CreateCommand((USINT*)FrameGetBufferStruct.buffer,
					                        THERMO_ADDRESS, ON_OFF_ARRAY, 1 );
					    StartUnit = 0;
					}
					else
					if (StopUnit)
					{
						length = CreateCommand((USINT*)FrameGetBufferStruct.buffer,
					                        THERMO_ADDRESS, ON_OFF_ARRAY, 0 );
					    StopUnit = 0;
					}

					/* initialize write structure */
					FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;		/* get adress of send buffer */
					FrameWriteStruct.buflng = length;

					FRM_write(&FrameWriteStruct); 				/* write data to interface */

					if (FrameWriteStruct.status != 0) 			/* check status */
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
				}
	/*TODO: Fehler auswerten wenn getBuffer fehlschlägt (8071)*/
				PollingStep++;
				timeout = 0;
				break;
			}
/* wait for the answer */
			case 11:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == 0) 					/* check status */
				{
					/* Checksum ok? */
					USINT tmp = CreateChecksum((USINT *)FrameReadStruct.buffer,FrameReadStruct.buflng-2);
					if( tmp == ((USINT *)FrameReadStruct.buffer)[FrameReadStruct.buflng-1] )
					{
						/* checksum OK */
						if(ParseAnswer((USINT *)FrameReadStruct.buffer, THERMO_ADDRESS,
						            FrameReadStruct.buflng, ON_OFF_ARRAY, &testval) != 0)
						{
							Error = 12; /* answer not correct*/
						}
						else	/* Answer formal OK */
						{
							/* Answer formal OK, values in Cooler struct */
						}

					}
					else
					{
						/* checksum error */
						Error = 11;
					}

					/* initialize release buffer structure */
					FrameReleaseBufferStruct.buffer = FrameReadStruct.buffer;	/* get adress of read buffer */
					FrameReleaseBufferStruct.buflng = FrameReadStruct.buflng; 	/* get length of read buffer */

					FRM_rbuf(&FrameReleaseBufferStruct); 			/* release read buffer */

					if (FrameReleaseBufferStruct.status != 0) 				/* check status */
					{
						Error = 6; 					/* set error level for FRM_rbuf */
					}
					PollingStep = 0;
					ResendCounter = 0;
				}
				else
				{
					Error = 5; 						/* set error level for FRM_read */
					timeout++;
					if (timeout >= 20) /* 20*cyclic (->2s) time no answer */
					{
						if(ResendCounter >= 5) /*5 times tried? -> error*/
						{
							PollingStep = 0;
							ResendCounter = 0;
						}
						else
						{
							ErrorCounter++;
							ResendCounter++;
							PollingStep = 10;
						}
					}
				}
				break;
			}


/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
/*  send the SET Setpoint command */
			case 20:
			{
				FRM_gbuf(&FrameGetBufferStruct); 				/* get send buffer */

				if (FrameGetBufferStruct.status == 0) 			/* check status */
				{
					int length;

					switch (SetCommands[CurrentSetCommand])
					{
						case SET_SETPOINT:
							{tmpval = (INT)(Cooler.NewValues.SetPoint*10.0);break;}
						case SET_LIMIT_LOW:
							{tmpval = (INT)(Cooler.NewValues.LimitLow*10.0);break;}
						case SET_LIMIT_HIGH:
							{tmpval = (INT)(Cooler.NewValues.LimitHigh*10.0);break;}
						case SET_HEAT_P:
							{tmpval = (INT)(Cooler.NewValues.HeatP*10.0);break;}
						case SET_HEAT_I:
							{tmpval = (INT)(Cooler.NewValues.HeatI*100.0);break;}
						case SET_HEAT_D:
							{tmpval = (INT)(Cooler.NewValues.HeatD*10.0);break;}
						case SET_COOL_P:
							{tmpval = (INT)(Cooler.NewValues.CoolP*10.0);break;}
						case SET_COOL_I:
							{tmpval = (INT)(Cooler.NewValues.CoolI*100.0);break;}
						case SET_COOL_D:
							{tmpval = (INT)(Cooler.NewValues.CoolD*10.0);break;}
					}
					length = CreateCommand((USINT*)FrameGetBufferStruct.buffer,
					                        THERMO_ADDRESS, SetCommands[CurrentSetCommand], tmpval);
					/* initialize write structure */
					FrameWriteStruct.buffer = FrameGetBufferStruct.buffer;		/* get adress of send buffer */
					FrameWriteStruct.buflng = length;

					FRM_write(&FrameWriteStruct); 				/* write data to interface */

					if (FrameWriteStruct.status != 0) 			/* check status */
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
				}
	/*TODO: Fehler auswerten wenn getBuffer fehlschlägt (8071)*/
				PollingStep++;
				timeout = 0;
				break;
			}
/* wait for the answer */
			case 21:
			{
				/* initialize read structure */
				FRM_read(&FrameReadStruct); 						/* read data form interface */

				if (FrameReadStruct.status == 0) 					/* check status */
				{
					/* Checksum ok? */
					USINT tmp = CreateChecksum((USINT *)FrameReadStruct.buffer,FrameReadStruct.buflng-2);
					if( tmp == ((USINT *)FrameReadStruct.buffer)[FrameReadStruct.buflng-1] )
					{
						/* checksum OK */
						if(ParseAnswer((USINT *)FrameReadStruct.buffer, THERMO_ADDRESS,
						            FrameReadStruct.buflng, SetCommands[CurrentSetCommand], &testval) != 0)
						{
							Error = 12; /* answer not correct*/
						}
						else	/* Answer formal OK */
						{
							/* Answer formal OK, values in Cooler struct */
							switch (SetCommands[CurrentSetCommand])
							{
								case SET_SETPOINT:
									{Cooler.CurrentValues.SetPoint = testval;break;}
								case SET_LIMIT_LOW:
									{Cooler.CurrentValues.LimitLow = testval;break;}
								case SET_LIMIT_HIGH:
									{Cooler.CurrentValues.LimitHigh = testval;break;}
								case SET_HEAT_P:
									{Cooler.CurrentValues.HeatP = testval;break;}
								case SET_HEAT_I:
									{Cooler.CurrentValues.HeatI = testval;break;}
								case SET_HEAT_D:
									{Cooler.CurrentValues.HeatD = testval;break;}
								case SET_COOL_P:
									{Cooler.CurrentValues.CoolP = testval;break;}
								case SET_COOL_I:
									{Cooler.CurrentValues.CoolI = testval;break;}
								case SET_COOL_D:
									{Cooler.CurrentValues.CoolD = testval;break;}
							}
						}

					}
					else
					{
						/* checksum error */
						Error = 11;
					}

					/* initialize release buffer structure */
					FrameReleaseBufferStruct.buffer = FrameReadStruct.buffer;	/* get adress of read buffer */
					FrameReleaseBufferStruct.buflng = FrameReadStruct.buflng; 	/* get length of read buffer */

					FRM_rbuf(&FrameReleaseBufferStruct); 			/* release read buffer */

					if (FrameReleaseBufferStruct.status != 0) 				/* check status */
					{
						Error = 6; 					/* set error level for FRM_rbuf */
					}
					CurrentReadCommand = 0;
					ResendCounter = 0;
					if (CurrentSetCommand < 9 && !SetPointOnly)
					{
						CurrentSetCommand++;
						PollingStep = 20;
					}
					else
					{
						CurrentSetCommand = 0;
						PollingStep = 1;
					}
				}
				else
				{
					Error = 5; 						/* set error level for FRM_read */
					timeout++;
					if (timeout >= 20) /* 20*cyclic (->2s) time no answer */
					{
						if(ResendCounter >= 5) /*5 times tried? -> error*/
						{
							if (CurrentSetCommand < 9)
							{
								CurrentSetCommand++;
								PollingStep = 20;
							}
							else
							{
								CurrentSetCommand = 0;
								PollingStep = 0;
							}
							ResendCounter = 0;
						}
						else
						{
							ErrorCounter++;
							ResendCounter++;
							PollingStep = 20;
						}
					}
				}
				break;
			}

		} /*switch*/
	}
}



