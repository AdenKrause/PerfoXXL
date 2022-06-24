#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Belichtung für LUST FU CDD3000 über CAN				 					*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			14.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include "bit_func.h"				/*	Macros für die Bitbearbeitung		*/
#include "glob_var.h"
#include "math.h"

/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/

/****************************************************************************/
/*		Variablendeklaration												*/
/****************************************************************************/

/* Struktur für zu schreibende/lesende Parameter */
extern	FU_Para_Typ		FU_Param;

_LOCAL USINT 	ExpStep;
REAL speed;
UDINT Vorkomma,Nachkomma;
/* Variablen f Altwerte*/
REAL 	speed_old,StartPosition_old,PEGStartPosition_old,EndPosition_old,
		PEGEndPosition_old;

void Belichtung(void)
{
/*safety!*/
	if(!FU.Activ || !FU.ENPO || !FU.Auto)
	{
		ExpStep = 0;
		FU.cmd_Expose = 0;
		FU.BefehlsMerker[2] = FALSE;
	}
	if (!FU.cmd_Expose)
		ExpStep = 0;

	switch (ExpStep)
	{
		case 0:
		{
			if(FU.cmd_Expose)
			{
		/*any Parameter changed? send all of them*/
				if( speed_old 				!= Expose_Param.ExposeSpeed 		||
					StartPosition_old 		!= Expose_Param.StartPosition 		||
					EndPosition_old 		!= Expose_Param.EndPosition 		||
					PEGStartPosition_old 	!= Expose_Param.PEGStartPosition 	||
					PEGEndPosition_old 		!= Expose_Param.PEGEndPosition 		)
					ExpStep++;
				else
			/* all parameters the same? jump to step 8 directly*/
					ExpStep = 8;
			}
			break;
		}
		case 1:
		{
		/* check, if no sending active*/
			if(!FU.SetPara)
			{
				ExpStep++;
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
				StartPosition_old = Expose_Param.StartPosition;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* Expose Start Position */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 17;
				FU_Param.Wert = (UDINT) (Expose_Param.StartPosition * X_Param.InkrementeProMm);
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
				PEGStartPosition_old = Expose_Param.PEGStartPosition;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* PEG Start Position */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 11;
				FU_Param.Wert = (UDINT) (Expose_Param.PEGStartPosition * X_Param.InkrementeProMm);
				FU.SetPara = 1;
			}
			break;
		}
		case 4:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				EndPosition_old = Expose_Param.EndPosition;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* Expose End Position */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 18;
				FU_Param.Wert = (UDINT) (Expose_Param.EndPosition * X_Param.InkrementeProMm);
				FU.SetPara = 1;
			}
			break;
		}
		case 5:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				PEGEndPosition_old = Expose_Param.PEGEndPosition;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* PEG End Position */
				FU_Param.ParaNr = 728;
				FU_Param.Index = 12;
				FU_Param.Wert = (UDINT) (Expose_Param.PEGEndPosition * X_Param.InkrementeProMm);
				FU.SetPara = 1;
			}
			break;
		}
		case 6:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* Expose speed Vorkomma Anteil*/
				FU_Param.ParaNr = 728;
				FU_Param.Index = 40;
				speed = Expose_Param.ExposeSpeed * X_Param.InkrementeProMm / 1000.0;
				Vorkomma = (UDINT) speed;
				Nachkomma = (UDINT)((speed - Vorkomma) * 10000.0);
				FU_Param.Wert = Vorkomma;
				FU.SetPara = 1;
			}
			break;
		}
		case 7:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				speed_old = Expose_Param.ExposeSpeed;
				ExpStep++;
			}
/* set data to send*/
			else
			{
		/* Expose speed Nachkomma Anteil*/
				FU_Param.ParaNr = 728;
				FU_Param.Index = 41;
				FU_Param.Wert = Nachkomma;
				FU.SetPara = 1;
			}
			break;
		}
		case 8:
		{
/*Belichten triggern, wenn Rückmeldung nächster Schritt*/
			if(FU.BefehlsMerker[2] == TRUE && FU.ZustandsMerker[2] == TRUE)
			{
				ExpStep++;
			}
			else
				FU.BefehlsMerker[2] = TRUE; /*Belichtung starten*/
			break;
		}
		case 9:
		{
/* auf Belichtung fertig warten*/
			if(FU.ZustandsMerker[3])
			{
/*Befehl wegnehmen*/
				FU.BefehlsMerker[2] = FALSE; /**/
				ExpStep++;
			}
			break;
		}
		case 10:
		{
/* auf weggehen der Rückmeldungen warten*/
			if( !FU.ZustandsMerker[2] && !FU.ZustandsMerker[3] )
			{
				ExpStep++;
			}
			break;
		}
		case 11:
		{
			ExpStep = 0;
			FU.cmd_Expose = 0;
			break;
		}
	}
}


