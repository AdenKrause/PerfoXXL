#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*
                             *******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : AUXFUNC.C                                                    **
** version  : 1.00                                                         **
** date     : 27.10.2006                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Implementation of function for common use, e.g. showing an error        **
** message                                                                 **
**                                                                         **
** Copyright (c) 2006, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 27.10.2006
Revised by  : Herbert Aden
Description : Original version.
*/
#define _AUXFUNC_C_SRC


/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include "glob_var.h"
#include "in_out.h"
#include "egmglob_var.h"
#include "asstring.h"
#include <string.h>

#include "auxfunc.h"

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

/****************************************************************************/
/**                                                                        **/
/**                           GLOBAL VARIABLES                             **/
/**                                                                        **/
/****************************************************************************/
_GLOBAL	UINT	EmptyReturnPic;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
void ShowMessage(const int T1,const int T2,const int T3,
                 const int Icon, const int Button, const BOOL pIgnoreButtons)
{
	switch (Button)
	{
		case NONE:
		{
			OK_CancelButtonInv = TRUE;
			OK_ButtonInv = TRUE;
			break;
		}
		case OKONLY:
		{
			OK_CancelButtonInv = TRUE;
			OK_ButtonInv = FALSE;
			break;
		}
		case OKCANCEL:
		{
			OK_CancelButtonInv = FALSE;
			OK_ButtonInv = TRUE;
			break;
		}
		default:
		{
			break;
		}
	}
	AbfrageIcon = Icon;
	AbfrageText1 = T1;
	AbfrageText2 = T2;
	AbfrageText3 = T3;
	IgnoreButtons = pIgnoreButtons;
	if(wBildAktuell != MESSAGEPIC )
	{
		if(wBildAktuell == EMPTYPIC)
		{
			if(EmptyReturnPic != MESSAGEPIC)
				OrgBild = EmptyReturnPic;
		}
		else
			OrgBild = wBildAktuell;
	}
	wBildNeu = MESSAGEPIC;
	AbfrageOK = 0;
	AbfrageCancel = 0;
}


void SwitchFeederVacuum(USINT PlateType, USINT Config, BOOL fct)
{
	switch (Config)
	{
		case LEFT:
		{
			Out_FeederVacuumOn[0] = fct;
			Out_FeederVacuumOn[2] = fct;
			break;
		}
		case RIGHT:
		{
			Out_FeederVacuumOn[1] = fct;
			Out_FeederVacuumOn[3] = fct;
			break;
		}
		case BOTH:
		{
			Out_FeederVacuumOn[0] = fct;
			Out_FeederVacuumOn[1] = fct;
			Out_FeederVacuumOn[2] = fct;
			Out_FeederVacuumOn[3] = fct;
			break;
		}
		case PANORAMA:
		{
			switch (PlateType)
			{
				case 1:
				{
					Out_FeederVacuumOn[0] = fct;
					Out_FeederVacuumOn[1] = fct;
					break;
				}
				case 2:
				{
					Out_FeederVacuumOn[0] = fct;
					Out_FeederVacuumOn[1] = fct;
					Out_FeederVacuumOn[2] = fct;
					Out_FeederVacuumOn[3] = fct;
					break;
				}
				default:
				{
					Out_FeederVacuumOn[1] = fct;
					Out_FeederVacuumOn[3] = fct;
					break;
				}
			}
/*
			Out_FeederVacuumOn[0] = fct;
			Out_FeederVacuumOn[1] = fct;
			Out_FeederVacuumOn[2] = fct;
			Out_FeederVacuumOn[3] = fct;
*/
			break;
		}
		case BROADSHEET:
		{
			Out_FeederVacuumOn[0] = fct;
			Out_FeederVacuumOn[1] = fct;
			Out_FeederVacuumOn[4] = fct;
			break;
		}
		case ALL:
		{
			Out_FeederVacuumOn[0] = fct;
			Out_FeederVacuumOn[1] = fct;
			Out_FeederVacuumOn[2] = fct;
			Out_FeederVacuumOn[3] = fct;
			Out_FeederVacuumOn[4] = fct;
			break;
		}
	}
}

void SwitchAdjustVacuum(USINT PlateType, USINT Config, BOOL fct)
{
	switch (Config)
	{
		case LEFT:
		{
			Out_AdjustSuckerVacuumOn[0] = fct;
			break;
		}
		case RIGHT:
		{
			Out_AdjustSuckerVacuumOn[1] = fct;
			break;
		}
		case BOTH:
		{
			Out_AdjustSuckerVacuumOn[0] = fct;
			Out_AdjustSuckerVacuumOn[1] = fct;
			break;
		}
		case PANORAMA:
		{
			Out_AdjustSuckerVacuumOn[0] = fct;
			Out_AdjustSuckerVacuumOn[1] = fct;
			break;
		}
		case BROADSHEET:
		{
			Out_AdjustSuckerVacuumOn[2] = fct;
			break;
		}
		case ALL:
		{
			Out_AdjustSuckerVacuumOn[0] = fct;
			Out_AdjustSuckerVacuumOn[1] = fct;
			Out_AdjustSuckerVacuumOn[2] = fct;
			break;
		}
	}
}

void SwitchAdjustTableVacuum(USINT PlateType, USINT Config, BOOL fct)
{
	switch (Config)
	{
		case LEFT:
		{
			AdjustVacuumOn[0] = fct;
			AdjustVacuumOn[2] = fct;
			break;
		}
		case RIGHT:
		{
			AdjustVacuumOn[1] = fct;
			AdjustVacuumOn[2] = fct;
			break;
		}
		case BOTH:
		{
			AdjustVacuumOn[0] = fct;
			AdjustVacuumOn[1] = fct;
			AdjustVacuumOn[2] = fct;
			break;
		}
		case PANORAMA:
		{
			switch (PlateType)
			{
				case 1:
				{
					AdjustVacuumOn[0] = fct;
					break;
				}
				case 2:
				{
					AdjustVacuumOn[0] = fct;
					AdjustVacuumOn[1] = fct;
					break;
				}
			}
/*
			AdjustVacuumOn[0] = fct;
			AdjustVacuumOn[1] = fct;
			AdjustVacuumOn[2] = fct;
*/
			break;
		}
		case BROADSHEET:
		{
			AdjustVacuumOn[2] = fct;
			break;
		}
		case ALL:
		{
			AdjustVacuumOn[0] = fct;
			AdjustVacuumOn[1] = fct;
			AdjustVacuumOn[2] = fct;
			break;
		}
	}
}


/****************************************************************************/
/* Hilfsfunktion: Papiersensoren prüfen */
/* PlateSet                     */
/* LEFT, RIGHT, BOTH/PANO, BROADSHEET */
/****************************************************************************/
BOOL CheckForPaper(USINT Config)
{
	BOOL PaperDetected = FALSE;
	switch (Config)
	{
		case LEFT:
		{
			PaperDetected = In_PaperSensor[0];
			break;
		}
		case RIGHT:
		{
			PaperDetected = In_PaperSensor[1];
			break;
		}
		case BOTH:
		{
			PaperDetected = In_PaperSensor[0] || In_PaperSensor[1];
			break;
		}
		case PANORAMA:
		{
			PaperDetected = In_PaperSensor[0] || In_PaperSensor[1];
			break;
		}
		case BROADSHEET:
		{
			PaperDetected = In_PaperSensor[0] || In_PaperSensor[1];
			break;
		}
	}

	if(!GlobalParameter.PaperRemoveEnabled || PlateTransportSim)
		PaperDetected = FALSE;

	return PaperDetected;

}

/****************************************************************************/
/* Hilfsfunktion: Vakuum prüfen */
/* PlateSet                     */
/* LEFT, RIGHT, BOTH/PANO, BROADSHEET */
/****************************************************************************/
BOOL CheckFeederVacuum(USINT PlateType, USINT Config)
{
	BOOL VacuumOK = FALSE;

	switch (Config)
	{
		case LEFT:
		{
			VacuumOK = In_FeederVacuumOK[0] && In_FeederVacuumOK[2];
			break;
		}
		case RIGHT:
		{
			VacuumOK = In_FeederVacuumOK[1] && In_FeederVacuumOK[3];
			break;
		}
		case BOTH:
		{
			VacuumOK = In_FeederVacuumOK[0] && In_FeederVacuumOK[1]
			        && In_FeederVacuumOK[2] && In_FeederVacuumOK[3];
			break;
		}
		case PANORAMA:
		{
			switch (PlateType)
			{
				case 1:
				{
					VacuumOK = In_FeederVacuumOK[0] && In_FeederVacuumOK[1];
					break;
				}
				case 2:
				{
					VacuumOK = In_FeederVacuumOK[0] && In_FeederVacuumOK[1]
								&& In_FeederVacuumOK[2] && In_FeederVacuumOK[3];
					break;
				}
				default:
				{
					VacuumOK = In_FeederVacuumOK[1] && In_FeederVacuumOK[3];
					break;
				}
			}
/*
			VacuumOK = In_FeederVacuumOK[0] && In_FeederVacuumOK[1]
			        && In_FeederVacuumOK[2] && In_FeederVacuumOK[3];
*/
			break;
		}
		case BROADSHEET:
		{
			VacuumOK = In_FeederVacuumOK[0]
			        && In_FeederVacuumOK[1]
			        && In_FeederVacuumOK[4];
			break;
		}
	}

	if(PlateTransportSim)
		VacuumOK = TRUE;
	return VacuumOK;
}

/****************************************************************************/
/* Hilfsfunktion: Vakuum prüfen */
/* PlateSet                     */
/* LEFT, RIGHT, BOTH/PANO, BROADSHEET */
/****************************************************************************/
BOOL CheckAdjustVacuum(USINT PlateType, USINT Config)
{
	BOOL VacuumOK = FALSE;
	switch (Config)
	{
		case LEFT:
		{
			VacuumOK = In_AdjustVacuumOK[0];
			break;
		}
		case RIGHT:
		{
			VacuumOK = In_AdjustVacuumOK[1];
			break;
		}
		case BOTH:
		{
			VacuumOK = In_AdjustVacuumOK[0] && In_AdjustVacuumOK[1];
			break;
		}
		case PANORAMA:
		{
			VacuumOK = In_AdjustVacuumOK[0];
			break;
		}
		case BROADSHEET:
		{
			VacuumOK = In_AdjustVacuumOK[0];
			break;
		}
	}

	if(PlateTransportSim)
		VacuumOK = TRUE;

	return VacuumOK;

}

void MoveAdjustSucker(USINT Config, USINT fct)
{
	switch (Config)
	{
		case LEFT:
		{
			Out_AdjustSuckerActive[0] = fct;
			break;
		}
		case RIGHT:
		{
			Out_AdjustSuckerActive[1] = fct;
			Out_AdjustSuckerActive[2] = fct;
			break;
		}
		case BOTH:
		{
			Out_AdjustSuckerActive[0] = fct;
			Out_AdjustSuckerActive[1] = fct;
			Out_AdjustSuckerActive[2] = fct;
			break;
		}
		case PANORAMA:
		{
			Out_AdjustSuckerActive[0] = fct;
			Out_AdjustSuckerActive[1] = fct;
			Out_AdjustSuckerActive[2] = fct;
			break;
		}
		case BROADSHEET:
		{
			Out_AdjustSuckerActive[2] = fct;
			break;
		}
		case ALL:
		{
			Out_AdjustSuckerActive[0] = fct;
			Out_AdjustSuckerActive[1] = fct;
			Out_AdjustSuckerActive[2] = fct;
			break;
		}
	}
}


BOOL AdjustOK(USINT Config)
{
	BOOL retval = FALSE;
	switch (Config)
	{
		case LEFT:
		{
			retval = In_PlateAdjusted[0];
			break;
		}
		case RIGHT:
		{
			retval = In_PlateAdjusted[1];
			break;
		}
		case BOTH:
		{
			retval = In_PlateAdjusted[0] && In_PlateAdjusted[0];
			break;
		}
		case PANORAMA:
		{
			retval = In_PlateAdjusted[0];
			break;
		}
		case BROADSHEET:
		{
			retval = In_PlateAdjusted[0];
			break;
		}
	}
	if(PlateTransportSim || AdjusterParameter.AdjustSim)
		retval = TRUE;

	return retval;
}


BOOL AdjPins(USINT Config,USINT *Step, TON_10ms_typ *Timer)
{
	BOOL retval = FALSE;

	if(Config == LEFT)
		return TRUE;
/* Panorama plate or down command: all pins down*/
	if( (Config == PANORAMA)
	 || (Config == PINS_DOWN)
	  )
	{
		switch (*Step)
		{
			case 0:
			{
/* schon unten? -> nichts zu tun, weiter*/
				if(  Out_AdjustPinsDown
				 && !Out_AdjustPins4and6Up
				 && !Out_AdjustPin3Up)
				{
					*Step = 0;
					Timer->IN = FALSE;
					retval = TRUE;
					break;
				}
/* erstmal senken ansteuern */
				if(Timer->Q && Out_AdjustPinsDown)
				{
					Timer->IN = FALSE;
					*Step = 10;
					break;
				}
				Timer->IN = TRUE;
				Timer->PT = 10;
				Out_AdjustPinsDown = ON;
				break;
			}
			case 10:
			{
				if(Timer->Q && !Out_AdjustPins4and6Up && !Out_AdjustPin3Up)
				{
					Timer->IN = FALSE;
					*Step = 20;
					break;
				}
				Timer->IN = TRUE;
				Timer->PT = AdjusterParameter.PinsDownTime;
				Out_AdjustPins4and6Up = OFF;
				Out_AdjustPin3Up = OFF;
				break;
			}
			case 20:
			{
				*Step = 0;
				Timer->IN = FALSE;
				retval = TRUE;
				break;
			}
		}
	}
	else
	{
		switch (*Step)
		{
			case 0:
			{
/* pin config fits plate config: nothing to do, just end*/
				if(  ( (Config == BROADSHEET)
					 && !Out_AdjustPins4and6Up
					 && Out_AdjustPin3Up
					 && !Out_AdjustPinsDown
					 )
				 ||  (((Config == BOTH) || (Config == RIGHT))
					 && Out_AdjustPins4and6Up
					 && Out_AdjustPin3Up
					 && !Out_AdjustPinsDown
				     )
				  )
				{
					retval = TRUE;
					break;
				}
/* Cross: only pin 3 may be up*/
				if( (Config == BROADSHEET)
				 &&  Out_AdjustPins4and6Up
				  )
				{
					*Step = 2;
					Timer->IN = FALSE;
					break;
				}
/* both or right: pins 3,4 and 6 must be up */
				Out_AdjustPinsDown = ON;
				*Step = 10;
				Timer->IN = FALSE;

				break;
			}
/**************************/
/* pins 4 and 6 down for cross format*/
			case 2:
			{
				if(Timer->Q && Out_AdjustPinsDown)
				{
					Timer->IN = FALSE;
					*Step = 4;
					break;
				}
				Out_AdjustPinsDown = ON;
				Timer->IN = TRUE;
				Timer->PT = 10;
				break;
			}
			case 4:
			{
				if(Timer->Q && !Out_AdjustPins4and6Up)
				{
					Timer->IN = FALSE;
					*Step = 10;
					break;
				}
				Out_AdjustPins4and6Up = OFF;
				Timer->IN = TRUE;
				Timer->PT = AdjusterParameter.PinsDownTime;
				break;
			}
/**************************/
			case 10:
			{
				if(Timer->Q && Out_AdjustPin3Up)
				{
					*Step = 20;
					Timer->IN = FALSE;
					break;
				}
				if( (Config == RIGHT)
				 || (Config == BOTH)
				  )
					Out_AdjustPins4and6Up = ON;
	/* pin 3 must always be up */
				Out_AdjustPin3Up = ON;
				Timer->IN = TRUE;
				Timer->PT = 10;
				break;
			}

			case 20:
			{
				if(Timer->Q && !Out_AdjustPinsDown)
				{
					Timer->IN = FALSE;
					*Step = 0;
					retval = TRUE;
					break;
				}
				Out_AdjustPinsDown = OFF;
				Timer->IN = TRUE;
				Timer->PT = AdjusterParameter.PinsDownTime;
				break;
			}
		}
	}
	return retval;
}

void ClearStation(PlateInSystem_Type *Station)
{
	Station->present = FALSE;
/*	Station->PlateConfig = NOPLATE;*/
	Station->PlateType = 0;
	Station->NextPlateType = 0;
	strcpy(Station->FileName,"");
	Station->ErrorCode = 0;
	strcpy(Station->ID,"");
	Station->Status = 0;
}

/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


