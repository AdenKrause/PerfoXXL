#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Bedienbild für X-Achse bearbeiten 								 		*/
/*	(Eingaben auswerten, Ausgaben ansteuern)								*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			07.08.02	erste Implementation					HA		*/
/*																			*/
/*																			*/
/****************************************************************************/

#include "glob_var.h"

#define ROT 0x002D
#define GRUEN 0x00E3
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

#define SCHWARZ_AUF_GRUEN 0x00E3
#define GELB_AUF_ROT 0x1633;

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

static INT ENPOColor,AutoColor,ErrorColor,ActiveColor,RefOkColor,BackgroundColor;
static INT ButtonActivColor,ButtonActivColorPressed;
static INT ButtonHandColor,ButtonHandColorPressed;
static INT ButtonRefColor,ButtonRefColorPressed;
static INT ButtonPark,ButtonParkPressed;
static INT ButtonTarget,ButtonTargetPressed;

_INIT void init(void)
{
	loadXData = 1;
	ACHSE_PIC = 11;
	ACHSE_PARAM_PIC = 14;
	SendManSpeedStep = 0;
	ENPOColor = SCHWARZ;
	AutoColor = SCHWARZ;
	ErrorColor = SCHWARZ;
	ActiveColor = SCHWARZ;
	RefOkColor = SCHWARZ;
	ButtonActivColor = SCHWARZ_AUF_GRAU;
	ButtonActivColorPressed = WEISS_AUF_SCHWARZ;
	ButtonHandColor = SCHWARZ_AUF_GRAU;
	ButtonHandColorPressed = WEISS_AUF_SCHWARZ;
	ButtonRefColor = SCHWARZ_AUF_GRAU;
	ButtonRefColorPressed = WEISS_AUF_SCHWARZ;
	ButtonPark = SCHWARZ_AUF_GRAU;
	ButtonParkPressed = WEISS_AUF_SCHWARZ;
	ButtonTarget = SCHWARZ_AUF_GRAU;
	ButtonTargetPressed = WEISS_AUF_SCHWARZ;
	BackgroundColor = WEISS;
}

_CYCLIC void cyclic(void)
{
	if(PanelIsTFT)
	{
		ENPOColor = GRUEN;
		AutoColor = GRUEN;
		ErrorColor = ROT;
		ActiveColor = GRUEN;
		RefOkColor = GRUEN;
		ButtonActivColor = SCHWARZ_AUF_GRAU;
		ButtonActivColorPressed = SCHWARZ_AUF_GRUEN;
		ButtonHandColor = SCHWARZ_AUF_GRAU;
		ButtonHandColorPressed = GELB_AUF_ROT;
		ButtonRefColor = SCHWARZ_AUF_GRAU;
		ButtonRefColorPressed = GELB_AUF_ROT;
		ButtonPark = SCHWARZ_AUF_GRAU;
		ButtonParkPressed = GELB_AUF_ROT;
		ButtonTarget = SCHWARZ_AUF_GRAU;
		ButtonTargetPressed = GELB_AUF_ROT;
		BackgroundColor = WEISS;
	}

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

	/* Farbumschläge */
		if(FU.Error || FU.CANError)
			FU_Colors.Error = ErrorColor;
		else
			FU_Colors.Error = BackgroundColor;

		if(FU.Activ)
		{
			FU_Colors.Activ = ActiveColor;
			FU_Colors.ButtonActiv = ButtonActivColorPressed;
		}
		else
		{
			FU_Colors.Activ = WEISS;
			FU_Colors.ButtonActiv = ButtonActivColor;
		}

		if(FU.Auto)
		{
			FU_Colors.Auto = AutoColor;
			FU_Colors.ButtonHand = ButtonHandColor;
		}
		else
		{
			FU_Colors.Auto = BackgroundColor;
			FU_Colors.ButtonHand = ButtonHandColorPressed;
		}

		if(FU.ENPO)
			FU_Colors.ENPO = ENPOColor;
		else
			FU_Colors.ENPO = BackgroundColor;

		if(FU.RefOk)
			FU_Colors.RefOk = RefOkColor;
		else
			FU_Colors.RefOk = BackgroundColor;

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
			FU_Colors.ButtonRef = ButtonRefColorPressed;
		}
		else
		{
			FU_Colors.ButtonRef = ButtonRefColor;
		}

		if (FU.cmd_Park)
		{
			FU_Colors.ButtonParkPos = ButtonParkPressed;
		}
		else
		{
			FU_Colors.ButtonParkPos = ButtonPark;
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
Manual speeds senden, wenn geändert
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


