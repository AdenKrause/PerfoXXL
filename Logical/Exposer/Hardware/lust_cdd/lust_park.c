#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Referenzfahrt f�r LUST FU CDD3000 �ber CAN			 					*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		�nderung								von		*/
/*	1.0			09.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include <stdlib.h>
#include "bit_func.h"				/*	Macros f�r die Bitbearbeitung		*/
#include "glob_var.h"


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/

/****************************************************************************/
/*		Variablendeklaration												*/
/****************************************************************************/

/* Struktur f�r zu schreibende/lesende Parameter */
extern	FU_Para_Typ		FU_Param;
extern REAL ParkSpeed_Old;

_LOCAL USINT ParkStep;

void ParkPosition(void)
{
/*safety!*/
	if(!FU.Activ || !FU.ENPO || !FU.Auto)
	{
		ParkStep = 0;
		FU.cmd_Park = 0;
		FU.BefehlsMerker[1] = FALSE;
		FU.BefehlsMerker[3] = FALSE;
	}
	if (!FU.cmd_Park)
		ParkStep = 0;

	switch (ParkStep)
	{
		case 0:
		{
			if(FU.cmd_Park)
			{
				if( (ParkPositionChanged)
				 || (abs(ParkSpeed_Old - X_Param.ParkSpeed)> 1)
				  )
					ParkStep++;
/* Parkpos unver�ndert: keine Parameter�bergabe*/
				else
					ParkStep = 4;
			}
			break;
		}
		case 1:
		{
		/* check, if no sending active*/
			if(!FU.SetPara)
			{
				ParkStep++;
				FU_Param.Ready = FALSE;
			}
			break;
		}
		case 2:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				ParkStep++;
			}
/* set data to send*/
			else
			{
		/* Parkposition */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 14;
				FU_Param.Wert = (UDINT) (X_Param.ParkPosition * X_Param.InkrementeProMm);
				FU.SetPara = 1;
			}
			break;
		}
		case 3:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				ParkStep++;
				ParkPositionChanged = 0;
				ParkSpeed_Old = X_Param.ParkSpeed;
			}
/* set data to send*/
			else
			{
		/* Parkspeed */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 13;
				FU_Param.Wert = (UDINT)(((float)X_Param.ParkSpeed) * X_Param.InkrementeProMm / 1000.0);
				FU.SetPara = 1;
			}
			break;
		}
		case 4:
		{
/*Fahrt zur Parkpos triggern, wenn R�ckmeldung n�chster Schritt*/
			if(FU.BefehlsMerker[1] == TRUE && FU.ZustandsMerker[4] == TRUE)
			{
				ParkStep++;
			}
			else
				FU.BefehlsMerker[1] = TRUE; /* starten*/
			break;
		}
		case 5:
		{
/* auf fertig warten*/
			if(FU.ZustandsMerker[5])
			{
/*Befehl wegnehmen*/
				FU.BefehlsMerker[1] = FALSE; /**/
				ParkStep++;
			}
			break;
		}
		case 6:
		{
/* auf weggehen der R�ckmeldungen warten*/
			if( !FU.ZustandsMerker[4] && !FU.ZustandsMerker[5] )
			{
				ParkStep++;
			}
			break;
		}
		case 7:
		{
			ParkStep = 0;
			FU.cmd_Park = 0;
			break;
		}
	}
}


