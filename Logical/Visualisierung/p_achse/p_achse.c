#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Bedienbild f�r X-Achse bearbeiten 								 		*/
/*	(Eingaben auswerten, Ausgaben ansteuern)								*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		�nderung								von		*/
/*	1.0			07.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"

#define ROT 0x002D
#define GRUEN 0x000A
#define GRAU 0x0007
#define SCHWARZ 0x0000
#define RAHMEN_ROT 0x2D00
#define RAHMEN_SCHWARZ 0x0000
#define RAHMEN_GRAU 0x0700

#define WEISS 15
#define WEISS_AUF_SCHWARZ 0x0F01
#define WEISS_AUF_GRAU 0x0F3B
#define SCHWARZ_AUF_GRAU 0x013B
#define SCHWARZ_AUF_WEISS 0x010F

typedef struct
{
	BOOL en_tipp_plus;
	BOOL en_tipp_minus;
	BOOL en_Auto;
	BOOL en_Schnell;
	BOOL en_Activ;
	BOOL en_Ref;
	BOOL en_ParkPos;
	BOOL en_TargetPos;
} FU_Keys_Typ;


typedef struct
{
	UINT Error;
	UINT Activ;
	UINT Auto;
	UINT ENPO;
	UINT RefOk;
	UINT Schnell;
	UINT Langsam;
	UINT ButtonHand;
	UINT ButtonActiv;
	UINT ButtonRef;
	UINT ButtonParkPos;
	UINT ButtonTargetPos;
}FU_Colors_Typ;

_GLOBAL	FU_Para_Typ		FU_Param;

_LOCAL FU_Keys_Typ		FU_Keys;
_LOCAL FU_Colors_Typ 	FU_Colors;

_LOCAL UINT SendManSpeedStep;
_LOCAL USINT	ExitOK,ExitCancel;

_INIT void init(void)
{
	loadXData = 1;
	ACHSE_PIC = 11;
	ACHSE_PARAM_PIC = 14;
	SendManSpeedStep = 0;
}

_CYCLIC void cyclic(void)
{

	if (wBildNr == ACHSE_PIC)
	{

/* Tasten Freigeben/sperren */
		FU_Keys.en_tipp_plus 	= !FU.Auto && !FU.state_tipp_minus && FU.Activ;
		FU_Keys.en_tipp_minus 	= !FU.Auto && !FU.state_tipp_plus && FU.Activ;
		FU_Keys.en_Auto 		= !FU.state_tipp_minus && !FU.state_tipp_minus && FU.Activ;
		FU_Keys.en_Schnell 		= !FU.state_tipp_minus && !FU.state_tipp_minus && !FU.Auto && FU.Activ;
		FU_Keys.en_Activ 		= !FU.state_tipp_minus && !FU.state_tipp_minus;
		FU_Keys.en_Ref 			= !FU.state_tipp_minus && !FU.state_tipp_minus && FU.Auto;
		FU_Keys.en_ParkPos 		= !FU.state_tipp_minus && !FU.state_tipp_minus && FU.Auto;
		FU_Keys.en_TargetPos	= !FU.state_tipp_minus && !FU.state_tipp_minus && FU.Auto;

	/* Farbumschl�ge */
		if(FU.Error || FU.CANError)
			FU_Colors.Error = SCHWARZ;
		else
			FU_Colors.Error = WEISS;

		if(FU.Activ)
		{
			FU_Colors.Activ = SCHWARZ;
			FU_Colors.ButtonActiv = WEISS_AUF_SCHWARZ;
		}
		else
		{
			FU_Colors.Activ = WEISS;
			FU_Colors.ButtonActiv = SCHWARZ_AUF_GRAU;
		}

		if(FU.Auto)
			FU_Colors.Auto = SCHWARZ;
		else
			FU_Colors.Auto = WEISS;

		if(!FU.Auto)
			FU_Colors.ButtonHand = WEISS_AUF_SCHWARZ;
		else
			FU_Colors.ButtonHand = SCHWARZ_AUF_GRAU;

		if(FU.ENPO)
			FU_Colors.ENPO = SCHWARZ;
		else
			FU_Colors.ENPO = WEISS;

		if(FU.RefOk)
			FU_Colors.RefOk = SCHWARZ;
		else
			FU_Colors.RefOk = WEISS;

		if(FU.cmd_tipp_schnell)
		{
			FU_Colors.Schnell = RAHMEN_ROT;
			FU_Colors.Langsam = RAHMEN_GRAU;
		}
		else
		{
			FU_Colors.Schnell = RAHMEN_GRAU;
			FU_Colors.Langsam = RAHMEN_ROT;
		}

		if (FU.cmd_Ref)
		{
			FU_Colors.ButtonRef = WEISS_AUF_SCHWARZ;
		}
		else
		{
			FU_Colors.ButtonRef = SCHWARZ_AUF_GRAU;
		}

		if (FU.cmd_Park)
		{
			FU_Colors.ButtonParkPos = WEISS_AUF_SCHWARZ;
		}
		else
		{
			FU_Colors.ButtonParkPos = SCHWARZ_AUF_GRAU;
		}

		if (FU.cmd_Target)
		{
			FU_Colors.ButtonTargetPos = WEISS_AUF_SCHWARZ;
		}
		else
		{
			FU_Colors.ButtonTargetPos = SCHWARZ_AUF_GRAU;
		}
	}

/***************************************************************************************
Manual speeds senden, wenn ge�ndert
*****************************/
		if (gSendManSpeeds)
		{
			SendManSpeedStep = 1;
			gSendManSpeeds = 0;
		}


		switch (SendManSpeedStep)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
/*damit man das Bild bei nicht angeschlossenem FU verlassen kann...*/
				if (FU.CANError)
				{
					SendManSpeedStep = 0;
					break;
				}
	/* check, if no sending active*/
				if(!FU.SetPara)
				{
					SendManSpeedStep++;
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
					SendManSpeedStep++;
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
			case 3:
			{
	/* Wait for sending ready*/
				if(FU_Param.Ready)
				{
					FU_Param.Ready = FALSE;
					SendManSpeedStep++;
				}
	/* set data to send*/
				else
				{
					FU_Param.Index = 0;
	/* manuell schnell */
					FU_Param.ParaNr = 715;
					FU_Param.Wert = (UDINT) (X_Param.ManSpeed2 * X_Param.InkrementeProMm / 1000.0);
					FU.SetPara = 1;
				}
				break;
			}
			case 4:
			{
				SendManSpeedStep = 0;
				break;
			}
		} /*switch*/
}

