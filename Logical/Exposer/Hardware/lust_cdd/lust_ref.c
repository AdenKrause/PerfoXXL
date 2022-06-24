#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Referenzfahrt für LUST FU CDD3000 über CAN			 					*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			09.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/
#include "bit_func.h"				/*	Macros für die Bitbearbeitung		*/
#include "glob_var.h"
#include "math.h"

/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
#define WAITCYCLES (200)

/****************************************************************************/
/*		Variablendeklaration												*/
/****************************************************************************/

/* Struktur für zu schreibende/lesende Parameter */
extern	FU_Para_Typ		FU_Param;
extern REAL ParkSpeed_Old;

_LOCAL USINT RefStep;
UINT WaitCounter;

void ReferenzFahrt(void)
{
/*V2.00 safety*/
	if(!FU.ENPO)
	{
		RefStep = 0;
		FU.cmd_Ref = 0;
		FU.BefehlsMerker[0] = FALSE;
	}
	if (!FU.cmd_Ref)
		RefStep = 0;

	switch (RefStep)
	{
		case 0:
		{
			if(FU.cmd_Ref)
				RefStep++;
			break;
		}
		case 1:
		{
		/* check, if no sending active*/
			if(!FU.SetPara)
			{
				RefStep++;
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
				RefStep++;
			}
/* set data to send*/
			else
			{
				FU_Param.ParaNr = 724;
				FU_Param.Wert = (UDINT)(((float)X_Param.RefSpeed1) * X_Param.InkrementeProMm / 1000.0);
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
				RefStep++;
			}
/* set data to send*/
			else
			{
				FU_Param.ParaNr = 725;
				FU_Param.Wert = (UDINT)(((float)X_Param.RefSpeed2) * X_Param.InkrementeProMm / 1000.0);
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
				RefStep++;
			}
/* set data to send*/
			else
			{
				FU_Param.ParaNr = 726;
				FU_Param.Wert = (UDINT)(((float)X_Param.RefSpeed3) * X_Param.InkrementeProMm / 1000.0);
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
				RefStep++;
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
		case 6:
		{
/* Wait for sending ready*/
			if(FU_Param.Ready)
			{
				FU_Param.Ready = FALSE;
				RefStep++;
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
		case 7:
		{
			/* Wait for sending ready*/
						if(FU_Param.Ready)
						{
							FU_Param.Ready = FALSE;
							RefStep++;
						}
			/* set data to send*/
						else
						{
							FU_Param.Index = 0;
			/* manuell langsam */
							FU_Param.ParaNr = 716;
							FU_Param.Wert = (UDINT) (X_Param.ManSpeed1 * X_Param.InkrementeProMm / 1000.0);
							FU.SetPara = 1;
						}
						break;
		}
		case 8:
		{
			/* Wait for sending ready*/
						if(FU_Param.Ready)
						{
							FU_Param.Ready = FALSE;
							RefStep = 20;
						}
			/* set data to send*/
						else
						{
							FU_Param.Index = 0;
			/* manuell langsam */
							FU_Param.ParaNr = 715;
							FU_Param.Wert = (UDINT) (X_Param.ManSpeed2 * X_Param.InkrementeProMm / 1000.0);
							FU.SetPara = 1;
						}
						break;
		}

		case 9:
		{
/*Referenz triggern, wenn Rückmeldung nächster Schritt*/
			if(FU.BefehlsMerker[0] == TRUE && FU.ZustandsMerker[0] == TRUE)
			{
				RefStep++;
			}
			else
				FU.BefehlsMerker[0] = TRUE; /*Ref starten*/
			break;
		}
		case 10:
		{
/* auf Referenz fertig warten*/
			if(FU.ZustandsMerker[1])
			{
/*Befehl wegnehmen*/
				FU.BefehlsMerker[0] = FALSE; /**/
				RefStep++;
			}
			break;
		}
		case 11:
		{
/* auf weggehen der Rückmeldungen warten*/
			if( !FU.ZustandsMerker[0] && !FU.ZustandsMerker[0] )
			{
				RefStep++;
			}
			break;
		}
		case 12:
		{
			RefStep = 0;
			FU.cmd_Ref = 0;
			break;
		}

/*extra steps between 8 and 9 for starting auto mode*/
		case 20:
		{
			if(FU.Activ)
			{
				RefStep = 22;
				WaitCounter = 0;
				break;
			}
			FU.cmd_Start = 1;
			break;
		}
/*wait a little...*/
		case 22:
		{
			WaitCounter++;
			if(WaitCounter > WAITCYCLES)
			{
				WaitCounter = 0;
				RefStep = 25;
				break;
			}
			break;
		}

		case 25:
		{
			if(FU.Auto)
			{
				RefStep = 27;
				WaitCounter = 0;
				break;
			}
			FU.cmd_Auto = 1;
			break;
		}
/*wait a little...*/
		case 27:
		{
			WaitCounter++;
			if(WaitCounter > WAITCYCLES)
			{
				WaitCounter = 0;
				RefStep = 9;
				break;
			}
			break;
		}
	}
}


