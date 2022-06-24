#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
 Handling of Preheat

 - Heating, Temperature Measurement

*************************************************************************************************/


#include "EGMglob_var.h"

extern void PreheatPID(void);
extern void PreheatPID_DefaultParam(void);

int i;
_GLOBAL BOOL	HeatingON[4];
_INIT void init()
{

	PreheatPID_DefaultParam();
}

_CYCLIC void cyclic()
{

	PreheatPID();
	return;
}


