/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : LS JET II                                                    **
** filename : AUXFUNC.H                                                    **
** version  : 1.00                                                         **
** date     : 27.10.2006                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** Declares the functions for common features          	                   **
**                                                                         **
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

#ifndef _AUXFUNC_INCLUDED
#define _AUXFUNC_INCLUDED

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include "glob_var.h"

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
#undef CAUTION
#undef INFO
#undef REQUEST
enum IconType   {CAUTION, INFO, REQUEST};
enum ButtonType {NONE, OKONLY, OKCANCEL};

/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
#ifndef _AUXFUNC_C_SRC
#endif

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/

void ShowMessage(const int T1,const int T2,const int T3,
                 const int Icon, const int Button, const BOOL pIgnoreButtons);

void SwitchFeederVacuum(USINT PlateType, USINT Config, BOOL fct);
void SwitchAdjustVacuum(USINT PlateType, USINT Config, BOOL fct);
void SwitchAdjustTableVacuum(USINT PlateType, USINT Config, BOOL fct);
BOOL CheckFeederVacuum(USINT PlateType, USINT Config);
BOOL CheckAdjustVacuum(USINT PlateType, USINT Config);
BOOL CheckForPaper(USINT Config);
void MoveAdjustSucker(USINT Config, USINT fct);
BOOL AdjustOK(USINT Config);
BOOL AdjPins(USINT Config,USINT *Step, TON_10ms_typ *Timer);
void ClearStation(PlateInSystem_Type *Station);

#endif
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


