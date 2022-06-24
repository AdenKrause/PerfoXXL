#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : MOTORTARGET.C                                                **
** version  : 1.11                                                         **
** date     : 12.10.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Sequence for moving a motor to a certain position (manually)            **
**                                                                         **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 10.09.2002
Revised by  : Herbert Aden
Description : Original version.

Version     : 1.10
Date        : 07.11.2006
Revised by  : Herbert Aden
Description : -Code review, style improvement

Version     : 1.10
Date        : 12.10.2007
Revised by  : Herbert Aden
Description : MoveToAbsPosition has changed, so we must provide a pointer
              to an USINT variable that can hold the internal state of that fct.
*/
#define _MOTORTARGET_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <string.h>
#include "asstring.h"
#include "glob_var.h"
#include "Motorfunc.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
/* NONE */

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
/* NONE */

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL USINT	TargetStep;
_LOCAL USINT	MotorOnDisplay;
_LOCAL USINT	TargetStart;
_LOCAL DINT	TargetPosition;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    USINT               MotStep;       /* holds current step of fct MoveToAbsPosition */

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
void MotorTarget(void)
{
	switch (TargetStep)
	{
		case 0:
		{
			if(		(Motors[MotorOnDisplay].Error == TRUE)
			    ||  (Motors[MotorOnDisplay].ReferenceOk == FALSE)
				||	(MotorOnDisplay == 0) )
				break;

/*check limit positions*/
			if ((MotorOnDisplay != CONVEYORBELT) && (MotorOnDisplay != ADJUSTER))
			{
				if(		(labs(TargetPosition) > labs(Motors[MotorOnDisplay].Parameter.MaximumPosition) )
					||	(labs(TargetPosition) < labs(Motors[MotorOnDisplay].Parameter.MinimumPosition)) )
					break;
			}

			if( TargetStart)
			{
				TargetStep = 1;
				TargetStart = FALSE;
			}
			break;
		}
		case 1:
		{
/*wait, till no other sending active*/
			if( MoveToAbsPosition(MotorOnDisplay,
			                      TargetPosition,
			                      Motors[MotorOnDisplay].Parameter.ManSpeed,
			                      &MotStep) )
			{
				TargetStep = 4;
			}
			break;
		}
		case 4:
		{
/*set maximum speed*/
			if( SendMotorCmd(MotorOnDisplay,
			                 CMD_SPEED,
			                 TRUE,
			                 Motors[MotorOnDisplay].Parameter.MaximumSpeed) )
			{
				TargetStep = 0;
			}
			break;
		}
	}
/*always reset the start command unconditionally*/
	TargetStart = FALSE;

}
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


