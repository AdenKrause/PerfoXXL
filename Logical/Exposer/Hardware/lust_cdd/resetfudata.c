#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*  	Hilfsfunktion														*/
/*		Rücksetzen der internen Statusinformation (State Machine)			*/
/****************************************************************************/

#include "bit_func.h"				/*	Macros für die Bitbearbeitung		*/
#include "glob_var.h"


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
extern CAN_OBJEKT_FUtyp	Can_FU;

/****************************************************************************/
/*		Rücksetzen der internen Statusinformation (State Machine)			*/
/****************************************************************************/
void resetFUData(void)
{
	int i;
/* Statusinformationen in Einzelbits intialisieren */
/* Byte 0 */
				FU.Error 			= FALSE;
				FU.CANStatus		= FALSE;
				FU.SollwertErreicht	= FALSE;
				FU.Activ 			= FALSE;
				FU.Rot0 			= FALSE;
				FU.CReady 			= FALSE;
/* Byte 1*/
				FU.ENPO 			= FALSE;
				FU.OSD00 			= FALSE;
				FU.OSD01 			= FALSE;
/* Byte 2 */
				FU.RefOk 			= FALSE;
				FU.Auto 			= FALSE;
				FU.Ablauf 			= FALSE;

				for (i=0;i<8;i++)
				{
					Can_FU.SteuReq[i] = 0;
					Can_FU.StatRes[i] = 0;
				}
}


