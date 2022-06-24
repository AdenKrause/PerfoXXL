/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : MOTORFUNC.H                                                  **
** version  : 1.02                                                         **
** date     : 24.01.2008                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Declares the functions for motor control functions  	                   **
**                                                                         **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
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
Description : signature of MoveToAbsPosition has changed

Version     : 1.02
Date        : 24.01.2008
Revised by  : Herbert Aden
Description : Added define for ANSW0 command

*/

#ifndef _MOTORFUNC_INCLUDED
#define _MOTORFUNC_INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define CMDSTRINGLENGTH        (10)
#define DONT_SEND_SPEED        (0L)

#define CMD_RELATIVE_POS            "LR"
#define CMD_ABSOLUTE_POS            "LA"
#define CMD_SPEED                   "SP"
#define CMD_START_MOTION            "M"
#define CMD_CONT_CURRENT            "LCC"
#define CMD_PEAK_CURRENT            "LPC"
#define CMD_STOP                    "V0"
#define CMD_MOVE_AT                 "V"
/* for reference */
#define CMD_HOME                    "HO"
#define CMD_ENABLE                  "EN"
#define CMD_SERIAL                  "SOR0"
#define CMD_ACCELERATION            "AC"
#define CMD_PROPFACT                "POR"
#define CMD_INTFACT                 "I"
#define CMD_NOANSW                  "ANSW0"
#define CMD_COMPATIBLE              "COMPATIBLE1"

#define CMD_GETTEMP                 "TEM"
#define CMD_GETFAULTSTAT            "GFS"

/****************************************************************************/
/**                                                                        **/
/**                      TYPEDEFS AND STRUCTURES                           **/
/**                                                                        **/
/****************************************************************************/
typedef struct
{
	STRING	CmdString[CMDSTRINGLENGTH];  /* command string*/
	DINT	Parameter;          /* Parameter for the command */
	BOOL	UseParameter;       /* if true, the parameter is added to the command
	                             *  else the parameter is ignored
	                             */
	BOOL	UseRefDirection;    /* if true, the ref direction of the motor is used
	                             * to determine the direction of movement
	                             * hence the sign of parameter is irrelevant;
	                             * else the sign is used to determine direction
	                             */
	INT     SendMotor;          /* the motor which shall receive the command*/
	BOOL	Send;               /* the actual send command, stay TRUE until
	                             * sending finished
	                             */
	BOOL	Request;            /* indicates that data is requested from motor */
}SerialCmd_Typ;

/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
#ifndef _MOTORFUNC_C_SRC
extern _GLOBAL SerialCmd_Typ SerialCmd;
#endif

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/** Send commands to a motor                                                */
/****************************************************************************/
/**
 * Sends commands to a specified motor.
 * Example: Call MotorCmd(1,"LA",TRUE,200000)
 *          will send "1LA200000" to the interface
 *
 * Parameters: UINT  Motor        the motor to which the command is being sent
 *             char *CmdString    the command string (NULL terminated)
 *             BOOL  UseValue     if true, then the value will be added to the
 *                                command
 *             DINT  Value        the value to be added to the command
 *
 * Returns:    OK                 when finished sending
 *             NOT_OK             while waiting for sending
 *
**/
BOOL	SendMotorCmd(const INT pMotor,const char *CmdStr,const BOOL UseValue,const DINT Value);



/****************************************************************************/
/** Check, if a motor is in a certain position window                       */
/****************************************************************************/
/**
 * Checks, if the position of the specified motor is in a specified range
 * (Position window)
 * Example call: PositionOk(1,200000,100)
 *               returns false, as long as the motorposition is <199900
 *                                                           or >200100
 * Parameters: UINT  Motor        the motor to be checked
 *             DINT  Pos          the position where the motor should be
 *             DINT  Deviation    the maximum tolerated deviaition (+/-)
 *
 *
 * Returns:    OK                 if current motor position is in range
 *             NOT_OK             if current motor position is out of range
 *
**/
BOOL	isPositionOk(const INT Motor,DINT Pos,DINT Deviation)/*@modifies nothing@*/;


/****************************************************************************/
/** Check, if a motor is below a certain position                           */
/****************************************************************************/
/**
 * Checks, if the position of the specified motor is below a specified position
 * ATTENTION: Does not work, if actual position has a different sign than the
 *            given max position! May only be used with all positive or all negative
 *            values. E.g. if the atual position is 1000 and we test for -1000 the
 *            function will return true, which wouold be incorrect!!
 *
 * Example call: isPositionLowerThan(1,65000)
 *               returns true, as long as the motorposition is <65000
 *
 * Parameters: UINT  Motor        the motor to be checked
 *             DINT  Pos          the position below which the motor should be
 *
 * Returns:    OK                 if current motor position is lower than
 *                                checkvalue
 *             NOT_OK             if current motor position is higher than or
 *                                equal to checkvalue
 *
**/
BOOL	isPositionLowerThan(const INT Motor,DINT Pos);


/****************************************************************************/
/** Check, if a motor is above a certain position                           */
/****************************************************************************/
/**
 * Checks, if the position of the specified motor is above a specified position
 * ATTENTION: Does not work, if actual position has a different sign than the
 *            given max position! May only be used with all positive or all negative
 *            values. E.g. if the atual position is 1000 and we test for -1000 the
 *            function will return true, which wouold be incorrect!!
 *
 * Example call: isPositionHigherThan(1,65000)
 *               returns true, as long as the motorposition is >65000
 *
 * Parameters: UINT  Motor        the motor to be checked
 *             DINT  Pos          the position below which the motor should be
 *
 * Returns:    OK                 if current motor position is higher than checkvalue
 *             NOT_OK             if current motor position is lower than or equal to
 *                                checkvalue
 *
**/
BOOL	isPositionHigherThan(const INT Motor,DINT Pos);


/****************************************************************************/
/** Request data from a motor                                                */
/****************************************************************************/
/**
 * Requests data from a specified motor.
 * Sets SerialCmd.Request to TRUE, caller must reset this flag after
 * receiving and using the data
 * Example: Call RequestMotorData(1,"TEM")
 *          will send "1TEM" to the interface
 *
 * Parameters: UINT  Motor        the motor to which the command is being sent
 *             char *CmdString    the command string (NULL terminated)
 *
 * Returns:    OK                 when finished sending
 *             NOT_OK             while waiting for sending
 *
**/
BOOL	RequestMotorData(const INT pMotor,const char *CmdString);


/****************************************************************************/
/** Check position of paper remove vertical                                **/
/****************************************************************************/
/**
 * Checks, if the position of the paper remove is in a specified range
 * (Position window)
 * Function is required because PerfoXXL has 2 motors for paper rem. vert.
 *
 * Parameters: DINT  Pos          the position where the motor should be
 *             DINT  Deviation    the maximum tolerated deviaition (+/-)
 *
 *
 * Returns:    OK                 if paper remove vert. position is in range
 *             NOT_OK             if paper remove vert. position is out of range
 *
 * uses        isPositionOk from this module
 *
**/
BOOL	isPapRemVerticalOk(BOOL Config,DINT Pos,UDINT MaxDeviation);



/****************************************************************************/
/* send commands to paper remove vertical */
/****************************************************************************/
/**
 * Sends commands to paper remove vertical.
 * Function is required because PerfoXXL has 2 motors for paper rem. vert.
 *
 * Parameters: char *CmdString    the command string (NULL terminated)
 *             BOOL  UseValue     if true, then the value will be added to the
 *                                command
 *             DINT  Value        the value to be added to the command
 *
 * Returns:    OK                 when finished sending
 *             NOT_OK             while waiting for sending
 *
**/
BOOL	PapRemVerticalCmd(BOOL Config,char *CmdString,BOOL UseValue,DINT Value);


/****************************************************************************/
/* move motor to an absolute position and wait for motion to start */
/****************************************************************************/
/**
 * implements a sequence to send commands to a specified motor to move to an
 * given absolute position at a given speed.
 * The caller must provide a pointer to an USINT variable that will hold the
 * current internal state of the function
 * ATTENTION
 * The function must be called cyclic until it returns OK
 * it returns TRUE only once, as soon as the motion started, then the caller must
 * stop calling, otherwise the motion will start again
 *
 *
 * Parameters: int pMotor         the motor to move
 *             DINT Pos           the position to move to
 *             DINT Speed         the speed of the motion
 *             USINT *pStep       pointer to a USINT var that holds the current step of the
 *                                internal sequence
 *
 * Special parameter values:      -if Speed has the value DONT_SEND_SPEED the speed command
 *                                 will be suppressed and thus one cycle can be saved
 *                                -if all 3 parameters are 0 then the sequence will be reset
 *                                 caller should do this, if the calling sequence is stopped
 *                                 and therefore the function ist no longer called
 *
 * Returns:    TRUE                when motion started
 *             FALSE               during sending
 *
**/
BOOL  MoveToAbsPosition(INT pMotor, DINT Pos, DINT Speed, USINT *pStep);

/****************************************************************************/
/* move motor to an relative position without waiting for motion to start */
/****************************************************************************/
BOOL  MoveToRelPosition(INT pMotor, DINT Pos, DINT Speed, USINT *pStep);


#endif /*_MOTORFUNC_INCLUDED*/

/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


