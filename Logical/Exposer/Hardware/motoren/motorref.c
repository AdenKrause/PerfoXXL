#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : MOTORREF.C                                                   **
** version  : 1.11                                                         **
** date     : 24.01.2008                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Sequence for reference cycle of motors                                  **
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
Date        : 07.11.2006
Revised by  : Herbert Aden
Description : -Code review, style improvement

Version     : 1.11
Date        : 24.01.2008
Revised by  : Herbert Aden
Description : Added ANSW0 command at reference to make sure, the motor will
              not send unexpectedly

*/
#define _MOTORREF_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <string.h>
#include "asstring.h"
#include "glob_var.h"
#include "in_out.h"
#include "auxfunc.h"
#include "motorfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/

#define TIMEOUT		10	/*timeout for moving to obstacle*/
#define WAITTIME	5	/*Waittime after M cmd for moving off ref position */


/****************************************************************************/
/**                                                                        **/
/**                      TYPEDEFS AND STRUCTURES                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      PROTOTYPES OF EXTERNAL FUNCTIONS                  **/
/**                                                                        **/
/****************************************************************************/
extern void MotorPicture(void);
extern void MotorManual(void);
extern void MotorTarget(void);
extern void MotorStop(void);

/****************************************************************************/
/**                                                                        **/
/**                      PROTOTYPES OF LOCAL FUNCTIONS                     **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

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
_GLOBAL   UINT                ConveyorBeltOn;

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    INT                 CurrentMotor;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    USINT               OldPriority[MAXMOTORS];
static    UDINT               OldPositionWindow[MAXMOTORS];
static    UDINT               Timeouts[MAXMOTORS];

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                          LOCAL FUNCTIONS                               **/
/**                                                                        **/
/****************************************************************************/


/****************************************************************************/
/** Init function, executed once at startup                                **/
/****************************************************************************/
_INIT void init(void)
{
	int i;
	for(i=0;i<MAXMOTORS;i++)
		Timeouts[i] = 0;
	loadMotorData = TRUE;
	CurrentMotor = 1;
}


_CYCLIC void c(void)
{

	if(loadMotorData || loadParameterData) return;

	if( GlobalParameter.DeloaderTurnStation == NO_DELOADER )
	{
		Motors[DELOADER_HORIZONTAL].Error = FALSE;
		Motors[DELOADER_HORIZONTAL].Moving = FALSE;
		Motors[DELOADER_HORIZONTAL].ReferenceOk = TRUE;
	}

	if( GlobalParameter.DeloaderTurnStation != DELOADER_AND_TURNSTATION )
	{
		Motors[DELOADER_TURN].Error = FALSE;
		Motors[DELOADER_TURN].Moving = FALSE;
		Motors[DELOADER_TURN].ReferenceOk = TRUE;
	}

	if( GlobalParameter.FlexibleFeeder == FALSE)
	{
		Motors[FEEDER_HORIZONTAL].Error = FALSE;
		Motors[FEEDER_HORIZONTAL].Moving = FALSE;
		Motors[FEEDER_HORIZONTAL].ReferenceOk = TRUE;
	}

/* call external functions */
	MotorManual();   /* in motorman.c */
	MotorTarget();   /* in motortarget.c */
	MotorStop();     /* in motorstop.c */


/*Reference in a loop for all motors*/
	switch (Motors[CurrentMotor].ReferenceStep)
	{
		case 0:
		{
			if(   (Motors[CurrentMotor].StartRef == TRUE)
			   && (Motors[CurrentMotor].Error    == FALSE) )
			   Motors[CurrentMotor].ReferenceStep = 102;
			break;
		}
/*V5.04 send ANSW0 to avoid asynchronous answers from motor */
		case 102:
		{
			if( SendMotorCmd(CurrentMotor, CMD_NOANSW, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 103;
			}
			break;
		}
		case 103:
		{
			if( SendMotorCmd(CurrentMotor, CMD_COMPATIBLE, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 1;
			}
			break;
		}

		case 1:
		{
/* enable Motor */
			if( SendMotorCmd(CurrentMotor, CMD_ENABLE, FALSE, 0L) )
			{
				OldPriority[CurrentMotor] = Motors[CurrentMotor].Parameter.Priority;
				OldPositionWindow[CurrentMotor] = Motors[CurrentMotor].Parameter.PositionWindow;
				Motors[CurrentMotor].Parameter.Priority = 1;
				Motors[CurrentMotor].Parameter.PositionWindow = 50;
				Motors[CurrentMotor].ReferenceStep = 2;
			}
			break;
		}
		case 2:
		{
/*SOR0*/
			if( SendMotorCmd(CurrentMotor, CMD_SERIAL, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 3;
			}
			break;
		}

		case 3:
		{
/*stop motor*/
			if( SendMotorCmd(CurrentMotor, CMD_STOP, FALSE, 0L) )
			{
				if(CurrentMotor == CONVEYORBELT)
					ConveyorBeltOn = FALSE;

				if(CurrentMotor == ADJUSTER || CurrentMotor == CONVEYORBELT)
					Motors[CurrentMotor].ReferenceStep = 20;
				else
					Motors[CurrentMotor].ReferenceStep = 4;
			}
			break;
		}

		case 4:
		{
			if( SendMotorCmd(CurrentMotor, CMD_ACCELERATION, TRUE, Motors[CurrentMotor].Parameter.ReferenceAcceleration) )
			{
				Motors[CurrentMotor].ReferenceStep = 5;
			}
			break;
		}
		case 5:
		{
			if( SendMotorCmd(CurrentMotor, CMD_CONT_CURRENT, TRUE, Motors[CurrentMotor].Parameter.ReferenceContCurrent) )
			{
				Motors[CurrentMotor].ReferenceStep = 10;
			}
			break;
		}
		case 10:
		{
			if( SendMotorCmd(CurrentMotor, CMD_PEAK_CURRENT, TRUE, Motors[CurrentMotor].Parameter.ReferencePeekCurrent) )
			{
				Motors[CurrentMotor].ReferenceStep = 11;
			}
			break;
		}
		case 11:
		{
			if( SendMotorCmd(CurrentMotor, CMD_PROPFACT, TRUE, Motors[CurrentMotor].Parameter.ReferencePropFact) )
			{
				Motors[CurrentMotor].ReferenceStep = 12;
			}
			break;
		}
		case 12:
		{
			if( SendMotorCmd(CurrentMotor, CMD_INTFACT, TRUE, Motors[CurrentMotor].Parameter.ControllerIFact) )
			{
/* HA 20160120 V1.40 added Paletteloading
 * Paperremove vertical: if sensor "up" gives signal, go to step 20
 */
				if( GlobalParameter.EnablePaletteLoading && (CurrentMotor == PAPERREMOVE_VERTICAL))
				{
					if(In_PaperRemoveUp)
					{
						Motors[CurrentMotor].ReferenceStep = 20;
						Timeouts[CurrentMotor] = 0;
						Motors[CurrentMotor].timeout=0;
						break;
					}
				}
				Motors[CurrentMotor].ReferenceStep=13;
				Motors[CurrentMotor].timeout=0;
				Timeouts[CurrentMotor] = 0;
			}
			break;
		}

		case 13:
		{
			DINT	tmpval;
			tmpval = (-1L) * labs(Motors[CurrentMotor].Parameter.ReferenceSpeed);
			if( SendMotorCmd(CurrentMotor, CMD_MOVE_AT, TRUE, tmpval) )
			{
				Motors[CurrentMotor].ReferenceStep = 15;
				Motors[CurrentMotor].timeout=0;
				Timeouts[CurrentMotor] = 0;
			}
			break;
		}

		case 15:
		{
/* Wait, til CurrentMotor moves or timeout (maybe the CurrentMotor is already in endposition!)*/
			if(   (Motors[CurrentMotor].Moving == TRUE)
			   && (Motors[CurrentMotor].Error  == FALSE) )
			{
				Motors[CurrentMotor].ReferenceStep = 16;
				Motors[CurrentMotor].timeout = 0;
				break;
			}
			Motors[CurrentMotor].timeout++;
/* no movement after 12*cyclic time*/
			if(Motors[CurrentMotor].timeout >= TIMEOUT)
			{
				Motors[CurrentMotor].ReferenceStep = 16;
				Motors[CurrentMotor].timeout = 0;
			}
			break;
		}
		case 16:
		{
/* Wait, till CurrentMotor is stopped by obstacle */
/*
 * paperremove vertical: wait for sensor "up"
 */
			if( GlobalParameter.EnablePaletteLoading && (CurrentMotor == PAPERREMOVE_VERTICAL))
			{
				if(In_PaperRemoveUp)
				{
					Motors[CurrentMotor].ReferenceStep = 20;
					Timeouts[CurrentMotor] = 0;
					break;
				}
				Timeouts[CurrentMotor]++;
/* check for timeout */
				if(Timeouts[CurrentMotor] >= 20000) /*20 s*/
				{
					Motors[CurrentMotor].ReferenceStep = 17;
					Timeouts[CurrentMotor] = 0;
					break;
				}
			}
			else
			{
				if((Motors[CurrentMotor].Moving == 0) && (Motors[CurrentMotor].Error == 0))
				{
					Motors[CurrentMotor].ReferenceStep=20;
					Timeouts[CurrentMotor] = 0;
				}
			}
			break;
		}
/* special: Errorhandling PAPERREMOVE_VERTICAL */
		case 17:
		{
			if( SendMotorCmd(CurrentMotor, CMD_STOP, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 0;
				Motors[CurrentMotor].StartRef = FALSE;
				ShowMessage(94,0,95,CAUTION,OKONLY,TRUE);
			}
			break;
		}
		case 20:
		{
			if( SendMotorCmd(CurrentMotor, CMD_HOME, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 21;
			}
			break;
		}
		case 21:
		{
			if( SendMotorCmd(CurrentMotor, CMD_STOP, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 22;
			}
			break;
		}
		case 22:
		{
/* Wait, till sending is ready and CurrentMotor doesn't move*/
			if(Motors[CurrentMotor].Moving == FALSE)
			{
				if( GlobalParameter.EnablePaletteLoading && (CurrentMotor == PAPERREMOVE_VERTICAL))
					Motors[CurrentMotor].ReferenceStep = 110;
				else
				if(CurrentMotor == ADJUSTER || CurrentMotor == CONVEYORBELT)
					Motors[CurrentMotor].ReferenceStep = 30;
				else
					Motors[CurrentMotor].ReferenceStep = 23;
			}
			break;
		}
/****************************************************************/
/* special steps for Paperremove vertical */
/****************************************************************/
		case 110:
		{
/* move down (free from up-sensor */
			if( SendMotorCmd(CurrentMotor, CMD_MOVE_AT, TRUE, labs(Motors[CurrentMotor].Parameter.ReferenceSpeed / 10) ) )
			{
				Motors[CurrentMotor].ReferenceStep = 115;
				Timeouts[CurrentMotor] = 0;
			}
			break;
		}
/* wait for up-sensor to become free */
/* TODO timeout handling */
		case 115:
		{
			if(!In_PaperRemoveUp)
			{
				Motors[CurrentMotor].ReferenceStep = 120;
				Timeouts[CurrentMotor] = 0;
				break;
			}
			break;
		}
/* set 0-Pos on the fly*/
		case 120:
		{
			if( SendMotorCmd(CurrentMotor, CMD_HOME, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 121;
			}
			break;
		}
		case 121:
		{
			if( SendMotorCmd(CurrentMotor, CMD_STOP, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 23;
			}
			break;
		}
/****************************************************************/
/****************************************************************/

		case 23:
		{
			DINT tmpval;
			if( GlobalParameter.EnablePaletteLoading && (CurrentMotor == PAPERREMOVE_VERTICAL))
				tmpval = 0 - labs(Motors[CurrentMotor].Parameter.ReferenceOffset);
			else
				tmpval = labs(Motors[CurrentMotor].Parameter.ReferenceOffset);
		
			if( SendMotorCmd(CurrentMotor, CMD_RELATIVE_POS, TRUE, tmpval) )
			{
				Motors[CurrentMotor].ReferenceStep = 24;
			}
			break;
		}
		case 24:
		{
			if( SendMotorCmd(CurrentMotor, CMD_START_MOTION, FALSE, 0L) )
			{
				Motors[CurrentMotor].timeout = 0;
				Motors[CurrentMotor].ReferenceStep = 25;
			}
			break;
		}
		case 25:
		{
/* Wait, till sending is ready and CurrentMotor doesn't move*/
			Motors[CurrentMotor].timeout++;
			if(Motors[CurrentMotor].timeout > WAITTIME )
			{
				Motors[CurrentMotor].ReferenceStep = 26;
			}
			break;
		}

		case 26:
		{
/* Wait, till sending is ready and CurrentMotor doesn't move*/
			if(SerialCmd.Send == FALSE && Motors[CurrentMotor].Moving == FALSE)
			{
				if( GlobalParameter.EnablePaletteLoading && (CurrentMotor == PAPERREMOVE_VERTICAL))
					Motors[CurrentMotor].ReferenceStep = 27;
				else
					Motors[CurrentMotor].ReferenceStep = 30;
			}
			break;
		}

/****************************************************************/
/* special for Paperremove vertical */

		case 27:
		{
			if( SendMotorCmd(CurrentMotor, CMD_HOME, FALSE, 0L) )
			{
				Motors[CurrentMotor].ReferenceStep = 30;
			}
			break;
		}
/****************************************************************/

		case 30:
		{
			Motors[CurrentMotor].Parameter.Priority = OldPriority[CurrentMotor];
			Motors[CurrentMotor].Parameter.PositionWindow = OldPositionWindow[CurrentMotor];

			if( SendMotorCmd(CurrentMotor, CMD_PROPFACT, TRUE, Motors[CurrentMotor].Parameter.DefaultPropFact) )
			{
				Motors[CurrentMotor].ReferenceStep = 31;
			}
			break;
		}
		case 31:
		{
			if( SendMotorCmd(CurrentMotor, CMD_PEAK_CURRENT, TRUE, Motors[CurrentMotor].Parameter.DefaultPeekCurrent) )
			{
				Motors[CurrentMotor].ReferenceStep = 32;
			}
			break;
		}
		case 32:
		{
			if( SendMotorCmd(CurrentMotor, CMD_CONT_CURRENT, TRUE, Motors[CurrentMotor].Parameter.DefaultContCurrent) )
			{
				Motors[CurrentMotor].ReferenceStep = 33;
			}
			break;
		}
		case 33:
		{
			if( SendMotorCmd(CurrentMotor, CMD_ACCELERATION, TRUE, Motors[CurrentMotor].Parameter.DefaultAcceleration) )
			{
				Motors[CurrentMotor].ReferenceStep = 34;
			}
			break;
		}
		case 34:
		{
			if( SendMotorCmd(CurrentMotor, CMD_SPEED, TRUE, Motors[CurrentMotor].Parameter.MaximumSpeed) )
			{
				Motors[CurrentMotor].ReferenceStep = 35;
			}
			break;
		}
		case 35:
		{
			Motors[CurrentMotor].ReferenceStep = 0;
			Motors[CurrentMotor].StartRef = FALSE;
			Motors[CurrentMotor].ReferenceOk = TRUE;
			break;
		}
	}

	CurrentMotor++;

	if(   (CurrentMotor == FEEDER_HORIZONTAL)
	   && (GlobalParameter.FlexibleFeeder == FALSE) )
		CurrentMotor++;

	if(CurrentMotor == SHUTTLE)
		CurrentMotor++;

	if(CurrentMotor == ADJUSTER)
		CurrentMotor++;

	if(   (CurrentMotor == DELOADER_TURN)
	   && (GlobalParameter.DeloaderTurnStation != DELOADER_AND_TURNSTATION) )
		CurrentMotor++;

	if(   (CurrentMotor == DELOADER_HORIZONTAL)
	   && (GlobalParameter.DeloaderTurnStation == NO_DELOADER) )
		CurrentMotor++;

	if(CurrentMotor > MotorsConnected)
		CurrentMotor = FEEDER_VERTICAL;

}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


