#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Zielpos anfahren für LUST FU CDD3000 über CAN			 				*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			09.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include <stdlib.h>
#include "bit_func.h"				/*	Macros für die Bitbearbeitung		*/
#include "glob_var.h"


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/

/****************************************************************************/
/*		Variablendeklaration												*/
/****************************************************************************/

/* Struktur für zu schreibende/lesende Parameter */
extern	FU_Para_Typ		FU_Param;
extern REAL ParkSpeed_Old;

_LOCAL USINT TargetStep;

void ZielPosition(void)
{

/*safety!*/
	if(!FU.Activ || !FU.ENPO || !FU.Auto)
	{
		TargetStep = 0;
		FU.cmd_Target = 0;
		FU.BefehlsMerker[1] = FALSE; /* starten*/
		FU.BefehlsMerker[3] = FALSE; /* starten*/
	}
	if (!FU.cmd_Target)
		TargetStep = 0;

	switch (TargetStep)
	{
		case 0:
		{
			if(!FU.cmd_Target)
				break;
		/* check, if no sending active*/
			if(!FU.SetPara)
			{
		/* Targetposition */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 19;
				FU.TargetPosition = FU.TargetPosition_mm * X_Param.InkrementeProMm;
				FU_Param.Wert = FU.TargetPosition;
				FU.SetPara = 1;
				FU_Param.Ready = FALSE;
				TargetStep = 2;
			}
			break;
		}
		case 2:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				if (abs(ParkSpeed_Old - X_Param.ParkSpeed)< 1)
					TargetStep = 4;
				else
					TargetStep++;
			}
			break;
		}
		case 3:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				ParkSpeed_Old = X_Param.ParkSpeed;
				TargetStep++;
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
/*Fahrt zur Parkpos triggern, wenn Rückmeldung nächster Schritt*/
			if(FU.BefehlsMerker[3] == TRUE && FU.ZustandsMerker[4] == TRUE)
			{
				TargetStep++;
			}
			else
			{
				FU.BefehlsMerker[1] = TRUE; /* starten*/
				FU.BefehlsMerker[3] = TRUE; /* starten*/
			}
			break;
		}
		case 5:
		{
/* auf fertig warten*/
			if(FU.ZustandsMerker[5])
			{
/*Befehl wegnehmen*/
				FU.BefehlsMerker[1] = FALSE; /**/
				FU.BefehlsMerker[3] = FALSE; /**/
				TargetStep++;
			}
			break;
		}
		case 6:
		{
/* auf weggehen der Rückmeldungen warten*/
			if( !FU.ZustandsMerker[4] && !FU.ZustandsMerker[5] )
			{
				TargetStep = 0;
				FU.cmd_Target = 0;
			}
			break;
		}
	}
}


