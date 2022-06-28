#include <bur\plctypes.h>

/* INPUT */
_GLOBAL	BOOL	In_ServiceMode;
_GLOBAL	BOOL	CoverLockOK;

_GLOBAL	BOOL	In_PaperSensor[2];
_GLOBAL	BOOL	In_FeederHorActive;
_GLOBAL	BOOL	In_FeederHorInactive;
_GLOBAL	BOOL	In_FeederVacuumOK[6];
_GLOBAL	BOOL	In_AdjustVacuumOK[2];

_GLOBAL	BOOL	In_PlateAdjusted[2];
_GLOBAL	BOOL	In_LiftingPinsDown;
_GLOBAL	BOOL	In_ConveyorBeltSensor[2];

_GLOBAL	BOOL	TrolleyCodeSensor;
_GLOBAL	BOOL	BeltLoose;
_GLOBAL	BOOL	DoorsOK;
_GLOBAL	BOOL	In_PaperRemoveUp;

/* evtl für Später, erstmal nicht vorgesehen */
#if !defined MODE_BOTH
_GLOBAL BOOL	In_VCPOK;
_GLOBAL BOOL	InEGM1;
_GLOBAL BOOL	InEGM2;
#endif

/* OUTPUT */
_GLOBAL	BOOL	Out_FeederVacuumOn[6];
_GLOBAL	BOOL	Out_AdjustSuckerActive[3];
_GLOBAL	BOOL	Out_AdjustSuckerVacuumOn[3];
_GLOBAL	BOOL	Out_AdjustSignalCross;
_GLOBAL	BOOL	Out_Horn;    		/*option*/
_GLOBAL	BOOL	Out_PaperGripOn[2];
_GLOBAL BOOL	Out_FeederHorActive;
_GLOBAL	BOOL	Out_ConveyorbeltPinsUp;	/*option*/
_GLOBAL	BOOL	Out_CoverLockOn;		/**/
_GLOBAL	BOOL	Out_ConveyorbeltLateral;
_GLOBAL	BOOL	Out_AdjustPins4and6Up;	/* benötigt für single2*/
_GLOBAL	BOOL	Out_AdjustPin3Up;		/* benötigt für single2 und single quer*/
_GLOBAL	BOOL	Out_AdjustPinsDown;
_GLOBAL	BOOL	Out_LiftingPinsUp;
_GLOBAL	BOOL	Out_LiftingPinsDown;
/* Tischvakuum*/
_GLOBAL	BOOL	AdjustVacuumOn[3];
_GLOBAL	BOOL	AdjustBlowairOn;

/* OUTPUTS */
#ifdef _IN_OUT_C_SRC
_LOCAL BOOL Module4[12];
_LOCAL BOOL Module5[12];
_LOCAL BOOL Module6[12];
#endif

