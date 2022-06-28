#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : MOTORFUNC.C                                                  **
** version  : 1.02                                                         **
** date     : 07.11.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Implementation of motor control functions (sending commands and         **
** check for positions)                                                    **
**                                                                         **
** Copyright (c) 2007, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 20.10.2006
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.01
Date        : 12.10.2007
Revised by  : Herbert Aden
Description : - a variable for holding the current step in function MoveToAbsPosition
                must now be provided by the caller (a pointer to that var of course!)
              - in Fct. IsPositionOK the check for being in a certain position window
                failed, if that window was 0, i.e. if an exact position is to be found
                the expression "currpos - checkpos < deviation" results in false, so it
                was changed to "currpos - checkpos <= deviation" so that if
                currpos==checkpos the result is still true for deviaition==0

Version     : 1.02
Date        : 07.11.2007
Revised by  : Herbert Aden
Description : - bugfix: in RequestMotorData the var SerialCmd.Request was set TRUE
                unconditionally. Caused sporadic Errormessages for serial comm.

*/
#define _MOTORFUNC_C_SRC


/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include "glob_var.h"
#include "AsBrStr.h"
#include <stdlib.h>
#include <string.h>

#include "motorfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
/*@checked@*/
_GLOBAL   SerialCmd_Typ       SerialCmd;


/****************************************************************************/
/**                                                                        **/
/**                           GLOBAL VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/
static  BOOL                  Send1Ready = 0;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/** Send commands to a motor                                                */
/****************************************************************************/
BOOL	SendMotorCmd(const INT pMotor,const char *CmdStr,const BOOL UseValue,const DINT Value)
{
	if(SerialCmd.Send == FALSE) /*wait, till no other sending active*/
	{
		strcpy(SerialCmd.CmdString,CmdStr);
		SerialCmd.UseParameter = UseValue;
		SerialCmd.Parameter = Value;
		if (   (STREQ(CmdStr,CMD_ABSOLUTE_POS))
			|| (STREQ(CmdStr,CMD_RELATIVE_POS))
			|| (STREQ(CmdStr,CMD_MOVE_AT))
		  ) /*bei LA oder LR oder V Refdir verwenden, sonst nicht*/
			SerialCmd.UseRefDirection = TRUE;
		else
			SerialCmd.UseRefDirection = FALSE;

		SerialCmd.SendMotor = pMotor;
		SerialCmd.Send = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}


/****************************************************************************/
/** Request data from a motor                                                */
/****************************************************************************/
BOOL	RequestMotorData(const INT pMotor,const char *CmdString)
{
	if( SerialCmd.Send == TRUE )
		return FALSE;
	else
	{
		SerialCmd.Request = TRUE;
		return SendMotorCmd(pMotor,CmdString,FALSE,0);
	}
}


/****************************************************************************/
/** Check, if a motor is in a certain position window                       */
/****************************************************************************/
BOOL	isPositionOk(const INT Motor,const DINT Pos,const DINT Deviation)
{
	DINT tmp;
	if (labs(Deviation) > 0 )
		tmp = labs(Deviation);
	else	/* if Dev is 0, then use deviation defined in motorparam*/
		tmp = labs(Motors[Motor].Parameter.PositionWindow);

	if( labs( labs(Motors[Motor].Position) - labs(Pos) ) <= tmp )
		return TRUE;
	else
		return FALSE;
}



/****************************************************************************/
/** Check, if a motor is below a certain position                           */
/****************************************************************************/
BOOL	isPositionLowerThan(const INT Motor,DINT Pos)
{
	if( labs(Motors[Motor].Position) < (labs(Pos) ) )
		return TRUE;
	else
		return FALSE;
}


/****************************************************************************/
/** Check, if a motor is above a certain position                           */
/****************************************************************************/
BOOL	isPositionHigherThan(const INT Motor,DINT Pos)
{
	if( labs(Motors[Motor].Position) > (labs(Pos) ) )
		return TRUE;
	else
		return FALSE;
}



/****************************************************************************/
/* send commands to paper remove vertical */
/****************************************************************************/
BOOL	PapRemVerticalCmd(BOOL Config,char *CmdString,BOOL UseValue,DINT Value)
{
	if(Config == LEFT)
		return SendMotorCmd(PAPERREMOVE_VERTICAL, CmdString, UseValue, Value);
	else
	if(Config == RIGHT)
		return SendMotorCmd(PAPERREMOVE_VERTICAL2, CmdString, UseValue, Value);
	else
	if ( Send1Ready == FALSE )
	{
		if( SendMotorCmd(PAPERREMOVE_VERTICAL, CmdString, UseValue, Value) )
			Send1Ready = TRUE;

		return FALSE;
	}
	else
	{
		if( SendMotorCmd(PAPERREMOVE_VERTICAL2, CmdString, UseValue, Value) )
		{
			Send1Ready = FALSE;
			return TRUE;
		}
		return FALSE;
	}
}


/****************************************************************************/
/* Check position of paper remove vertical */
/****************************************************************************/
BOOL	isPapRemVerticalOk(BOOL Config,DINT Pos,UDINT MaxDeviation)
{
	if(Config == LEFT)
		return isPositionOk(PAPERREMOVE_VERTICAL, Pos, MaxDeviation);
	else
	if(Config == RIGHT)
		return isPositionOk(PAPERREMOVE_VERTICAL2, Pos, MaxDeviation);
	else
	if ( isPositionOk(PAPERREMOVE_VERTICAL, Pos, MaxDeviation) )
	{
		if ( isPositionOk(PAPERREMOVE_VERTICAL2, Pos, MaxDeviation) )
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}



/****************************************************************************/
/* move motor to an absolute position and wait for motion to start */
/****************************************************************************/
BOOL  MoveToAbsPosition(INT pMotor, DINT Pos, DINT Speed, USINT *pStep)
{
	int step = *pStep;

	BOOL retval = FALSE;

	if (pMotor == 0 && Pos == 0L && Speed == 0L)
	{
		step = 0;
		*pStep = 0;
		return FALSE;
	}

	switch (step)
	{
		case 0:
		{
/* send target position */
			if( SendMotorCmd(pMotor,CMD_ABSOLUTE_POS,TRUE,Pos) )
			{
				if ( Speed == DONT_SEND_SPEED )
					step = 20;
				else
					step = 10;
			}
			retval = FALSE;
			break;
		}
/* set speed */
		case 10:
		{
			if( SendMotorCmd(pMotor,CMD_SPEED,TRUE,Speed) )
			{
				step = 20;
			}
			retval = FALSE;
			break;
		}
		case 20:
		{
/* start motion */
			if( SendMotorCmd(pMotor,CMD_START_MOTION,FALSE,0L) )
			{
				step = 30;
			}
			retval = FALSE;
			break;
		}
		case 30:
		{
/* Wait til motion begins or Motor is in position (might be already there!)*/
			if( Motors[pMotor].Moving
			    ||
			    isPositionOk(pMotor,Pos,0) )
			{
				step = 0;
				retval = TRUE;
			}
			break;
		}
	}
	*pStep = step;
	return retval;
}

/****************************************************************************/
/* move motor to an relative position without waiting for motion to start */
/****************************************************************************/
BOOL  MoveToRelPosition(INT pMotor, DINT Pos, DINT Speed, USINT *pStep)
{
	int step = *pStep;

	BOOL retval = FALSE;

	if (pMotor == 0 && Pos == 0L && Speed == 0L)
	{
		step = 0;
		*pStep = 0;
		return FALSE;
	}

	switch (step)
	{
		case 0:
		{
/* send target position */
			if( SendMotorCmd(pMotor,CMD_RELATIVE_POS,TRUE,Pos) )
			{
				if ( Speed == DONT_SEND_SPEED )
					step = 20;
				else
					step = 10;
			}
			retval = FALSE;
			break;
		}
/* set speed */
		case 10:
		{
			if( SendMotorCmd(pMotor,CMD_SPEED,TRUE,Speed) )
			{
				step = 20;
			}
			retval = FALSE;
			break;
		}
		case 20:
		{
/* start motion */
			if( SendMotorCmd(pMotor,CMD_START_MOTION,FALSE,0L) )
			{
				step = 30;
			}
			retval = FALSE;
			break;
		}
		case 30:
		{
			step = 0;
			retval = TRUE;
			break;
		}
	}
	*pStep = step;
	return retval;
}

/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/
/* NONE */



/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


