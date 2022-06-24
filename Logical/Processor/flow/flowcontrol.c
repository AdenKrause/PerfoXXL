#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
 Flow control

 - checking Flow control sensor

*************************************************************************************************/

#include "egmglob_var.h"

BOOL				Pulse_1s,Overflow;
TON_10ms_typ		PulseTimer1;

_LOCAL	BOOL		CounterReset;

_LOCAL INT			DevCircFlowInv,PrewashFlowInv,RinseFlowInv,
					GumFlowInv, ReplFlowInv, TopUpFlowInv;


void FlowControl(FlowCount_type *FC,BOOL In, INT MinFreq)
{
	void NoFlow(void)
	{
		FC->LastCV = 0;
		FC->Flowing = FALSE;
		FC->EdgeCounter.RESET = TRUE;

		FC->LitersPerHour = 0;
		FC->LitersPerMinute = 0;
		FC->Frequency = 0;
		FC->OverflowCounter = 0;
	}


	if (Pulse_1s)
	{
		/*
		 * Sonderfall: wenn genau vor einem Zyklus ein Overflow stattfand
		 * dann ist aktueller Zählerstand = letzter Zählerstand = 0
		 * in dem Fall soll kein Reset erfolgen, sondern eine
		 * weitere Sekunde gewartet werden
		 */
		if( !Overflow )
		{
			if(FC->EdgeCounter.CV > FC->LastCV)
			{
				FC->Frequency = (FC->EdgeCounter.CV - FC->LastCV);
				FC->LastCV = FC->EdgeCounter.CV;
				if( FC->Frequency >= MinFreq )
				{
					FC->Flowing = TRUE;
					FC->LitersPerHour = (FC->Frequency / (REAL)FC->PulsesPerLiter)*3600.0;
					FC->LitersPerMinute = (FC->Frequency / (REAL)FC->PulsesPerLiter)*60.0;
				}
				else
					NoFlow();
			}
			else /* counter value has not been increased since 1 sec.: no flow */
				NoFlow();
		} /* if(!Overflow) */
		Overflow = FALSE;
	} /* if(Pulse_1s) */

/* reset value from somewhere else */
	if(FC->ResetMlFlown)
	{
		FC->ResetMlFlown = 0;
		FC->EdgeCounter.RESET = TRUE;
		FC->MlFlown = 0;
		FC->LastCV = 0;
		FC->OverflowCounter = 0;
	}

	if(CounterReset)
	{
		NoFlow();
		FC->MlFlown = 0;
	}

	/* counter overflow at 30000 */
	if(FC->EdgeCounter.CV >= 30000)
	{
		FC->EdgeCounter.RESET = TRUE;
		FC->LastCV = 0;
		FC->OverflowCounter++;
		Overflow = TRUE;
	}
	FC->EdgeCounter.CU = In;
	CTU(&(FC->EdgeCounter));
	FC->EdgeCounter.RESET = FALSE;

	if( FC->EdgeCounter.CV != 0 )
		FC->MlFlown = 1000.0 *
					( ((30000.0 * FC->OverflowCounter) + FC->EdgeCounter.CV)
					    / (REAL)FC->PulsesPerLiter
					 );
}





void InitFlowControl(FlowCount_type *FC)
{
	FC->EdgeCounter.CU = 0;
	FC->EdgeCounter.RESET = 1;
	FC->EdgeCounter.PV = 0;
	FC->OverflowCounter = 0;
	FC->LastCV = 0;
	FC->MlFlown = 0;
	FC->ResetMlFlown = 0;
}


_INIT void init(void)
{
	InitFlowControl(&GumFC);
	InitFlowControl(&ReplFC);
	InitFlowControl(&TopUpFC);

	PulseTimer1.IN = 0;
	PulseTimer1.PT = 100;
}


_CYCLIC void cyclic(void)
{
	DevCircFlowInv = !DevCircFlowInput;
	PrewashFlowInv = !PrewashFlowInput;
	RinseFlowInv = !RinseFlowInput;
	GumFlowInv = !GumFC.Flowing;
	ReplFlowInv = !ReplFC.Flowing;
	TopUpFlowInv = !TopUpFC.Flowing;

	PulseTimer1.IN = 1;
	if(PulseTimer1.Q)
		PulseTimer1.IN = 0;
	TON_10ms(&PulseTimer1);
	Pulse_1s = PulseTimer1.Q;

	if(    (EGMGlobalParam.GumPulsesPerLiter < 500)
		|| (EGMGlobalParam.GumPulsesPerLiter > 5000) )
		EGMGlobalParam.GumPulsesPerLiter = 1200;

	if(    (EGMGlobalParam.ReplenisherPulsesPerLiter < 500)
		|| (EGMGlobalParam.ReplenisherPulsesPerLiter > 5000) )
		EGMGlobalParam.ReplenisherPulsesPerLiter = 1200;

	if(    (EGMGlobalParam.DeveloperPulsesPerLiter < 500)
		|| (EGMGlobalParam.DeveloperPulsesPerLiter > 5000) )
		EGMGlobalParam.DeveloperPulsesPerLiter = 1200;

	GumFC.PulsesPerLiter = EGMGlobalParam.GumPulsesPerLiter;
	ReplFC.PulsesPerLiter = EGMGlobalParam.ReplenisherPulsesPerLiter;
	TopUpFC.PulsesPerLiter = EGMGlobalParam.DeveloperPulsesPerLiter;

	FlowControl(&GumFC,GumFlowInput,2);
	FlowControl(&ReplFC,ReplFlowInput,9);
	FlowControl(&TopUpFC,TopUpFlowInput,9);

}


