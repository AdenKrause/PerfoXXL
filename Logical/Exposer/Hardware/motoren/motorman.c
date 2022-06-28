#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : MOTORMAN.C                                                   **
** version  : 1.11                                                         **
** date     : 08.11.2007                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Move motor manually                                                     **
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

Version     : 1.11
Date        : 08.11.2007
Revised by  : Herbert Aden
Description : - limits for checking movement are now always positive, and max>min
*/
#define _MOTORMAN_C_SRC

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <stdlib.h>
#include <string.h>
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
/**                      TYPEDEFS AND STRUCTURES                           **/
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
_GLOBAL   UINT                ConveyorBeltOn;

/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    USINT               ManStep;
_LOCAL    USINT               MotorOnDisplay;
_LOCAL    USINT               ManPlus,ManMinus;

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
static    DINT                tmpval;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
void MotorManual(void)
{
	MotorOnDisplay = gMotorOnDisplay;
	switch (ManStep)
	{
		case 0:
		{
			if(Motors[MotorOnDisplay].Error || MotorOnDisplay == 0)
				break;

/* don't start, if limit reached*/
			if ((MotorOnDisplay != CONVEYORBELT) && (MotorOnDisplay != ADJUSTER))
			{
				if( ( ManPlus &&
				      (labs(Motors[MotorOnDisplay].Position) > labs(Motors[MotorOnDisplay].Parameter.MaximumPosition))
				    )
				 ||
				    ( ManMinus &&
					 (labs(Motors[MotorOnDisplay].Position) < labs(Motors[MotorOnDisplay].Parameter.MinimumPosition))
				    )
				  )
				{
					break;
				}
			}

			if( ManPlus || ManMinus )
			{
				ManStep = 1;
				if( ManMinus )
					tmpval = (-1L) * labs(Motors[MotorOnDisplay].Parameter.ManSpeed);
				else
					tmpval = labs(Motors[MotorOnDisplay].Parameter.ManSpeed);
			}
			break;
		}
		case 1:
		{
			if (SendMotorCmd(MotorOnDisplay, CMD_SERIAL, FALSE, 0L ))
			{
				ManStep = 2;
			}
			break;
		}
		case 2:
		{
			if (SendMotorCmd(MotorOnDisplay, CMD_MOVE_AT, TRUE, tmpval ))
			{
				ManStep = 3;
			}
			break;
		}
		case 3:
		{
/*wait for key being released*/
/*V2.10 or limit reached*/
/*V2.13 build 22.06.2005 no limits for Motor 6 and 9 (Belt-motors)*/
			if ((MotorOnDisplay != CONVEYORBELT) && (MotorOnDisplay != ADJUSTER))
			{
				if( ( ManPlus &&
				      (labs(Motors[MotorOnDisplay].Position) > labs(Motors[MotorOnDisplay].Parameter.MaximumPosition))
				    )
				 ||
				    ( ManMinus &&
					 (labs(Motors[MotorOnDisplay].Position) < labs(Motors[MotorOnDisplay].Parameter.MinimumPosition))
				    )
				  )
				{
					ManStep = 4;
					break;
				}
			}

			if( ManPlus || ManMinus )
				break;
			else
				ManStep = 4;

			break;
		}
		case 4:
		{
			if (SendMotorCmd(MotorOnDisplay, CMD_STOP, FALSE, 0L) )
			{
				ManStep = 0;
				if(MotorOnDisplay == CONVEYORBELT)
					ConveyorBeltOn = FALSE;
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


