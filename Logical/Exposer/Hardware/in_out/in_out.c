#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

#define _IN_OUT_C_SRC

#include "glob_var.h"
#include "egmglob_var.h"
#include "in_out.h"

_GLOBAL BOOL	BUSY;
_GLOBAL BOOL	ERROR,NEWERROR;
_GLOBAL BOOL	STANDBY;
_GLOBAL BOOL	Clock_1s;
/*HA 14.01.04 V1.71 implemented Stop pins on conveyor */
_GLOBAL	BOOL			StopPinsUp;
static TP_10ms_typ ConvLateralPulse;



_INIT void i(void)
{
/*safety! when machine is switched off, the grip closes
so we leave it closed at startup*/
	Out_PaperGripOn[LEFT] = ON;
	Out_PaperGripOn[RIGHT] = ON;
	ConvLateralPulse.IN = FALSE;

}

_CYCLIC void c(void)
{
	Module4[0] = Out_AdjustSuckerActive[1];
	Module4[1] = Out_AdjustSuckerActive[0];
	Module4[10] = Out_AdjustSignalCross;
	Module4[11] = Out_Horn;

	Module5[1] = Out_FeederHorActive;
	Module5[2] = Out_ConveyorbeltPinsUp;
	Module5[3] = Out_AdjustSuckerActive[2];
	Module5[4] = Out_CoverLockOn;

	if ( !PlateTransportSim )
	{
		Module4[2] = Out_FeederVacuumOn[0];
		Module4[3] = Out_FeederVacuumOn[1];
		Module4[4] = Out_FeederVacuumOn[2];
		Module4[5] = Out_FeederVacuumOn[3];
		Module4[6] = Out_AdjustSuckerVacuumOn[1];
		Module4[7] = Out_AdjustSuckerVacuumOn[0];
		Module4[8] = Out_AdjustSuckerVacuumOn[2];
		Module4[9] = Out_FeederVacuumOn[4];
	}
	else
	{
		Module4[2] = 0;
		Module4[3] = 0;
		Module4[4] = 0;
		Module4[5] = 0;
		Module4[6] = 0;
		Module4[7] = 0;
		Module4[8] = 0;
		Module4[9] = 0;
	}

	Module5[0] = !Out_PaperGripOn[LEFT]; /* both grippers via one output */
	Module5[5] = 0;
	Module5[6] = BUSY;
	Module5[7] = STANDBY;
	if (ERROR)
	{
		if (NEWERROR)
		{
			if (Clock_1s)
				Module5[8] = 1;
			else
				Module5[8] = 0;
		}
		else
			Module5[8] = 1;
	}
	else
		Module5[8] = 0;

	ConvLateralPulse.IN = Out_ConveyorbeltLateral;
	if( (DeloaderParameter.ConvLateralPulseTime > 0) && (DeloaderParameter.ConvLateralPulseTime < 500) )
		ConvLateralPulse.PT = DeloaderParameter.ConvLateralPulseTime;
	else
		ConvLateralPulse.PT = 50;

	TP_10ms(&ConvLateralPulse);

	Module5[9] = ConvLateralPulse.Q;
	Module5[10] = 0;
	Module5[11] = 0;

	Module6[0] = Out_AdjustPins4and6Up;
	Module6[1] = Out_AdjustPin3Up;
	Module6[2] = Out_AdjustPinsDown;
	Module6[3] = 0;
	Module6[4] = Out_LiftingPinsUp;
	Module6[5] = Out_LiftingPinsDown;
	if ( !PlateTransportSim )
	{
		Module6[6] = AdjustVacuumOn[0];
		Module6[7] = AdjustVacuumOn[1];
		Module6[8] = AdjustVacuumOn[2];
		Module6[9] = AdjustBlowairOn;
	}
	else
	{
		Module6[6] = 0;
		Module6[7] = 0;
		Module6[8] = 0;
		Module6[9] = 0;
	}
	Module6[10] = 0;
	Module6[11] = 0;

}


