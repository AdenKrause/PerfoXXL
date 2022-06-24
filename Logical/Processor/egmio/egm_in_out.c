#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
#include "glob_var.h"
#include "egmglob_var.h"

#define OUTMODULES (3)
#define OUTPUTSPERMODULE (12)

_LOCAL    BOOL                EGMModule1Out[12];
_LOCAL    BOOL                EGMModule2Out[12];
_LOCAL    BOOL                EGMModule3Out[12];
_LOCAL    BOOL                EGMModule1In[12];
_LOCAL    BOOL                EGMModule2In[12];

static BOOL dummy;

_INIT void i(void)
{
	MachineType = BLUEFIN_LOWCHEM;
/*	MachineType = BLUEFIN_XS;*/
}

_CYCLIC void c(void)
{

/*
Maschinenlänge in m
	Standard Bluefin 2.80
	XS Bluefin		 2.70
	LowChem Bluefin  2.05
 */

	if(MachineType == BLUEFIN_XS)
	{
		MACHINELENGTH = 2.70;
	/* XS Output */
		EGMModule1Out[0] = Horn;
		EGMModule1Out[1] = LightGreen;
		EGMModule1Out[2] = LightYellow;
		EGMModule1Out[3] = LightRed;
		EGMModule1Out[4] = EGMMainMotor.Enable;
		EGMModule1Out[5] = EGMBrushMotor.Enable;
		EGMModule1Out[6] = Ready;
		EGMModule1Out[7] = NoError;
		EGMModule1Out[8] = ActivateControlVoltage;
		EGMModule1Out[9] = EGMPrewash.Valve;
		EGMModule1Out[10] = EGMRinse.Valve;
		EGMModule1Out[11] = EGMGum.RinseValve;

		EGMModule2Out[0] = HeatingON[0];
		EGMModule2Out[1] = HeatingON[1];
		EGMModule2Out[2] = HeatingON[2];
		EGMModule2Out[3] = HeatingON[3];
		EGMModule2Out[4] = EGMDeveloperTank.HeatingOn;
		EGMModule2Out[5] = EGMGum.Valve;
		EGMModule2Out[6] = EGMDeveloperTank.CoolingOn;
		EGMModule2Out[7] = FALSE;
		EGMModule2Out[8] = FALSE;
		EGMModule2Out[9] = FALSE;
		EGMModule2Out[10] = FALSE;
		EGMModule2Out[11] = FALSE;

		EGMModule3Out[0] = EGMPrewash.Pump;
		EGMModule3Out[1] = EGMRinse.Pump;
		EGMModule3Out[2] = EGMPreheat.FansOn;
		EGMModule3Out[3] = Drying;
		if(EGMGlobalParam.PROV_KitInstalled)
			EGMModule3Out[4] = Out_DevCanister_Circulation;
		else
			EGMModule3Out[4] = EGMDeveloperTank.RegenerationPump;
		EGMModule3Out[5] = EGMGum.Pump;
		EGMModule3Out[6] = EGMDeveloperTank.CirculationPump;
		if(EGMGlobalParam.PROV_KitInstalled)
			EGMModule3Out[7] = Out_DevCanister_Replenish;
		else
			EGMModule3Out[7] = EGMDeveloperTank.Refill;
		EGMModule3Out[8] = FALSE;
		EGMModule3Out[9] = FALSE;
		EGMModule3Out[10] = FALSE;
		EGMModule3Out[11] = FALSE;

	/* XS Input */
		ControlVoltageOk 			= EGMModule1In[0];
		RPMCheck 					= EGMModule1In[1];
		InputSensor 				= EGMModule1In[2];
		EGMPrewash.TankFull 		= EGMModule1In[3];
		EGMRinse.TankFull 			= EGMModule1In[4];
		EGMGum.TankFull				= EGMModule1In[5];
		EGMDeveloperTank.TankFull	= EGMModule1In[6];
/*                                                 7 s.u.ProV */
		OutputSensor				= EGMModule1In[8];
		ServiceKey					= EGMModule1In[9];
		VCP_OK						= EGMModule1In[10];
		if(EGMGlobalParam.PROV_KitInstalled)
		{
			In_DevCanister_Full			= EGMModule1In[7];
			In_DevCanister_Empty		= EGMModule1In[11];
		}
		else
		{
			dummy						= EGMModule1In[7];
			dummy						= EGMModule1In[11];
		}

		GumFlowInput	 			= EGMModule2In[0];
		ReplFlowInput	 			= EGMModule2In[1];
		TopUpFlowInput	 			= EGMModule2In[2];
		DevCircFlowInput 			= EGMModule2In[3];
		PrewashFlowInput	 		= EGMModule2In[4];
		RinseFlowInput	 			= EGMModule2In[5];
/*		In_WasteTankNearlyFull		= EGMModule2In[6];
		In_WasteTankFull 			= EGMModule2In[7];*/
		dummy						= EGMModule2In[8];
		dummy						= EGMModule2In[9];
		dummy						= EGMModule2In[10];
		dummy						= EGMModule2In[11];
	}
	else
	if(MachineType == BLUEFIN_LOWCHEM)
	{
		EGMGlobalParam.PROV_KitInstalled = FALSE;
		MACHINELENGTH = 2.05;
	/* LowChem Output */
		EGMModule1Out[0] = Horn;
		EGMModule1Out[1] = LightGreen;
		EGMModule1Out[2] = LightYellow;
		EGMModule1Out[3] = LightRed;
		EGMModule1Out[4] = EGMMainMotor.Enable;
		EGMModule1Out[5] = EGMBrushMotor.Enable;
		EGMModule1Out[6] = ActivateControlVoltage;
		EGMModule1Out[7] = FALSE;
		EGMModule1Out[8] = FALSE;
		EGMModule1Out[9] = FALSE;
		EGMModule1Out[10] = EGMGum.Valve;
		EGMModule1Out[11] = EGMGum.RinseValve;

		EGMModule2Out[0] = HeatingON[0];
		EGMModule2Out[1] = HeatingON[1];
		EGMModule2Out[2] = EGMDeveloperTank.HeatingOn;
		EGMModule2Out[3] = FALSE;
		EGMModule2Out[4] = FALSE;
		EGMModule2Out[5] = FALSE;
		EGMModule2Out[6] = EGMDeveloperTank.CoolingOn;
		EGMModule2Out[7] = FALSE;
		EGMModule2Out[8] = FALSE;
		EGMModule2Out[9] = FALSE;
		EGMModule2Out[10] = FALSE;
		EGMModule2Out[11] = FALSE;

		EGMModule3Out[0] = EGMPrewash.Pump;
		EGMModule3Out[1] = EGMRinse.Pump;
		EGMModule3Out[2] = EGMPreheat.FansOn;
		EGMModule3Out[3] = Drying;
		EGMModule3Out[4] = EGMDeveloperTank.RegenerationPump;
		EGMModule3Out[5] = EGMGum.Pump;
		EGMModule3Out[6] = EGMDeveloperTank.CirculationPump;
		EGMModule3Out[7] = EGMDeveloperTank.Refill;
		EGMModule3Out[8] = FALSE;
		EGMModule3Out[9] = FALSE;
		EGMModule3Out[10] = Ready;
		EGMModule3Out[11] = NoError;

	/* LowChem Input */
		ControlVoltageOk 			= EGMModule1In[0];
		RPMCheck 					= EGMModule1In[1];
		InputSensor 				= EGMModule1In[2];
		DevCircFlowInput	 		= EGMModule1In[3];
		TopUpFlowInput	 			= EGMModule1In[4];
		EGMGum.TankFull				= EGMModule1In[5];
		EGMDeveloperTank.TankFull	= EGMModule1In[6];
		GumFlowInput				= EGMModule1In[7];
		OutputSensor				= EGMModule1In[8];
		ServiceKey					= EGMModule1In[9];
		VCP_OK						= EGMModule1In[10];
		dummy						= EGMModule1In[11];

		dummy						= EGMModule2In[0];
		ReplFlowInput	 			= EGMModule2In[1];
		dummy						= EGMModule2In[2];
		dummy						= EGMModule2In[3];
		dummy						= EGMModule2In[4];
		dummy						= EGMModule2In[5];
		dummy						= EGMModule2In[6];
		dummy						= EGMModule2In[7];
		dummy						= EGMModule2In[8];
		dummy						= EGMModule2In[9];
		dummy						= EGMModule2In[10];
		dummy						= EGMModule2In[11];
	}
}




