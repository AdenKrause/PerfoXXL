#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* ===================================================== HEADER FILES ===================================================== */

#include <bur/plc.h>
#include <bur/plctypes.h>
#include <sys_lib.h>
#include <string.h>
#include <asiomman.h>
#include <brsystem.h>
#include <standard.h>
#include "auxfunc.h"



/* ===================================================== VARAIBLE DECLARATION ==================================================== */

#define ERR_FUB_BUSY 65535
#define MAXHWMODULES (20)
#define ERR_IO_CHECK_NOMODULE (5555)


typedef struct
{
	STRING Name[20];
	UINT HWStatus;
	USINT HWFamily;
	USINT HWUseType;
	USINT HWModuleTyp;
	USINT HWMasterNo;
	USINT HWSlaveNo;
	USINT HWModuleAdr;
	USINT HWSlotNo;
}HW_Info_typ;


_GLOBAL USINT       step;                           /* step for the main routine */

/* CYCLIC */
_LOCAL  AsIOMMCreate_typ    AsIOMMCreate_Mod;       /* function for creating the new BR module on the target */
_LOCAL  AsIOMMRemove_typ    AsIOMMRemove_Mod;       /* function for removing the BR module on the target */
_LOCAL  AsIOMMCopy_typ      AsIOMMCopy_Mod;         /* function for copying the BR module to a datamodule */
_LOCAL  char           error_string[128];

/* shows the last error which occured */
_LOCAL  USINT           error_step;                 /* step where the error occurred */
_LOCAL  UINT            error_status;               /* status of the FBK where the error occured */
static TON_10ms_typ     WaitTimer;                  /*waits before actually changing config*/
/************************************/


_LOCAL	HWInfo_typ HWInfo_01;

_LOCAL  BOOL            CopyArConfig,CopyIomap,InstallConfig1,InstallConfig2;
_LOCAL  BOOL            next;
_LOCAL	HW_Info_typ		myHWInfo[MAXHWMODULES];
_LOCAL	INT				cnt;
_GLOBAL SINT            IOConfiguration _VAR_RETAIN,NewIOConfig	_VAR_RETAIN;
_GLOBAL SINT 			IOChangeStep	_VAR_RETAIN;

_INIT void InitialisierungsProgramm(void) {
        HWInfo_01.enable = 1;     /* Funktionsblock wird nur ausgeführt
                                   wenn ENABLE <> 0 ist. */
        HWInfo_01.first = 1;      /* Bestimmen des Hardwaremoduls auf dem die
                                   Funktion angewendet werden soll. */
        cnt = 0;
        next = 1;

        InstallConfig1 = 0;
        InstallConfig2 = 0;
        NewIOConfig = 0;
		WaitTimer.IN = 0;
		PanelIsTFT = FALSE;
}



_CYCLIC void cyclic( void )
{
	if(next || HWInfo_01.first)
	{
        HWInfo_01.pName = (UDINT) &(myHWInfo[cnt].Name);
        HWInfo(&HWInfo_01);
        myHWInfo[cnt].HWStatus    = HWInfo_01.status;     /* Fehlernummer, 0 ist kein Fehler. */
        myHWInfo[cnt].HWFamily    = HWInfo_01.family;     /* Familienbezeichnung */
        myHWInfo[cnt].HWUseType   = HWInfo_01.usetype;   /* Hardware-Typ */
        myHWInfo[cnt].HWModuleTyp = HWInfo_01.module_typ; /* Typcode des Moduls */
        myHWInfo[cnt].HWMasterNo  = HWInfo_01.master_no;   /* Logische Nummer des IO-Masters, des RIO-Masters bzw.
                                                des CAN-Busses bei CANIO. */
        myHWInfo[cnt].HWSlaveNo   = HWInfo_01.slave_no;     /* Slave Nummer */
        myHWInfo[cnt].HWModuleAdr = HWInfo_01.module_adr;    /* Moduladresse des Hardwaremoduls */
        myHWInfo[cnt].HWSlotNo    = HWInfo_01.slot_no;          /* Steckplatz des Einschub-oder Anpassungsmoduls */

		HWInfo_01.first = 0;

		/* sobald ein Typcode fuer ein 5AP1120 gefunden wird, ist es ein Farb-Display */
		if(HWInfo_01.module_typ == 0xE7AA)
			PanelIsTFT = TRUE;
			
		/* sobald ein Typcode fuer ein PP420 gefunden wird, ist es ein LCD-Display (monochrom) */
		if(  (HWInfo_01.module_typ == 0xA52E) 
			|| (HWInfo_01.module_typ == 0x23B9)
			)
			PanelIsTFT = FALSE;

		if(cnt < MAXHWMODULES)
			cnt++;

		if( (HWInfo_01.status == ERR_IO_CHECK_NOMODULE)
		 || (cnt >= MAXHWMODULES)
		  )
		{
			next = 0;
			cnt = 0;
		}
	}

/***********************************************************/

/* change request from pics task (user changes config.)*/

	if(NewIOConfig == 1)
	{
		InstallConfig1 = 1;
		NewIOConfig = 0;
	}
	else
	if(NewIOConfig == 2)
	{
		InstallConfig2 = 1;
		NewIOConfig = 0;
	}

	TON_10ms(&WaitTimer);

	switch (step)
	{
		case 0:
		{
            if(IOChangeStep != 0)
            {
	            if(IOChangeStep == 12)
	            	InstallConfig1 = 1;
	            if(IOChangeStep == 13)
	            	InstallConfig2 = 1;
            	step = 20;
            	IOChangeStep = 0;
            	break;
            }

			if(CopyArConfig)
			{
				CopyArConfig = 0;
				step = 1;
				break;
			}
			if(CopyIomap)
			{
				CopyIomap = 0;
				step = 2;
				break;
			}

			if(InstallConfig1 || InstallConfig2)
			{
				if(WaitTimer.Q)
				{
					WaitTimer.IN = 0;
					step = 5;
					break;
				}
				WaitTimer.IN = 1;
				WaitTimer.PT = 200;
				ShowMessage(128,129,0,INFO,NONE, 1);
				break;
			}
			break;
		}
		case 1:
		{
            AsIOMMCopy_Mod.enable = 1;                            /* enable */
            AsIOMMCopy_Mod.pModuleName = (UDINT) "arconfig";      /* name of the *.br module which has to be removed */
            AsIOMMCopy_Mod.pNewModule = (UDINT) "arcfg2";      /* name of the data module which has to be created */
			AsIOMMCopy_Mod.memType = 2 /*doUSRROM*/;
			AsIOMMCopy_Mod.option = 1;
            /* Call FBK */
            AsIOMMCopy(&AsIOMMCopy_Mod);

            /* Verify status */
            if (AsIOMMCopy_Mod.status == 0 || AsIOMMCopy_Mod.status == iommERR_NOSUCH_MODULE)
            {
                 step = 0;                                          /* next step */
            }
            else
            {
                if (AsIOMMCopy_Mod.status != 0xFFFF)
                {
                    error_step = step;
                    error_status = AsIOMMCopy_Mod.status;
                    step = 100;                                     /* error step */
                    strcpy(error_string, "br_copy_arconfig");
                }
            }
        	break;
		}
		case 2:
		{
            AsIOMMCopy_Mod.enable = 1;                            /* enable */
            AsIOMMCopy_Mod.pModuleName = (UDINT) "iomap";      /* name of the *.br module which has to be removed */
            AsIOMMCopy_Mod.pNewModule = (UDINT) "iomap2";      /* name of the data module which has to be created */
			AsIOMMCopy_Mod.memType = 2 /*doUSRROM*/;
			AsIOMMCopy_Mod.option = 1;
            /* Call FBK */
            AsIOMMCopy(&AsIOMMCopy_Mod);

            /* Verify status */
            if (AsIOMMCopy_Mod.status == 0 || AsIOMMCopy_Mod.status == iommERR_NOSUCH_MODULE)
            {
                 step = 0;                                          /* next step */
            }
            else
            {
                if (AsIOMMCopy_Mod.status != 0xFFFF)
                {
                    error_step = step;
                    error_status = AsIOMMCopy_Mod.status;
                    step = 100;                                     /* error step */
                    strcpy(error_string, "br_copy_iomap");
                }
            }
        	break;
		}


        case 5:     /* CASE 5: Delete "arconfig.br" from target */

            /* Initialize AsIOMMRemove */
            AsIOMMRemove_Mod.enable = 1;                            /* enable */
            AsIOMMRemove_Mod.pModuleName = (UDINT) "arconfig";      /* name of the *.br module which has to be removed */

            /* Call FBK */
            AsIOMMRemove(&AsIOMMRemove_Mod);

            /* Verify status */
            if (AsIOMMRemove_Mod.status == 0)
            {
                step = 6;                                       /* next step */
            }
           else
           {
                if (AsIOMMRemove_Mod.status != 0xFFFF || AsIOMMRemove_Mod.status == iommERR_NOSUCH_MODULE)
                {
                    error_step = step;
                    error_status = AsIOMMRemove_Mod.status;
                    step = 100;                                     /* error step */
                    strcpy(error_string, "br_remove_arconfig");
                }
            }
            break;

        case 6:     /* CASE 6: Delete "iomap.br" from target */

            /* Initialize AsIOMMRemove */
            AsIOMMRemove_Mod.enable = 1;                            /* enable */
            AsIOMMRemove_Mod.pModuleName = (UDINT) "iomap";         /* name of the *.br module which has to be removed */

            /* Call FBK */
            AsIOMMRemove(&AsIOMMRemove_Mod);

            /* Verify status */
            if (AsIOMMRemove_Mod.status == 0 || AsIOMMRemove_Mod.status == iommERR_NOSUCH_MODULE)
            {
                 step = 10;                                       /* next step */
            }
            else
            {
                 if (AsIOMMRemove_Mod.status != 0xFFFF)
                 {
                        error_step = step;
                        error_status = AsIOMMRemove_Mod.status;
                        step = 100;                                 /* error step */
                        strcpy(error_string, "br_remove_iomap");
                 }
            }
            break;


		case 10:
        {
            AsIOMMCreate_Mod.enable = 1;                            /* enable */
			if(InstallConfig1)
			{
	            AsIOMMCreate_Mod.pDataObject = (UDINT) "arcfg1";       /* name of datamodule where the data should be taken from */
    	        AsIOMMCreate_Mod.pNewModule = (UDINT) "arconfig";          /* name of the new *.br module */
    	    }
			else
			{
	            AsIOMMCreate_Mod.pDataObject = (UDINT) "arcfg2";       /* name of datamodule where the data should be taken from */
    	        AsIOMMCreate_Mod.pNewModule = (UDINT) "arconfig";          /* name of the new *.br module */
    	    }
            AsIOMMCreate_Mod.moduleKind = 2 /*config*/;                        /* which type of *.br module should be created */
            AsIOMMCreate_Mod.memType = 2 /*USRROM*/;                           /* where should the *.br module be stored */

            /* Call FBK */
            AsIOMMCreate(&AsIOMMCreate_Mod);

            /* Verify status */
            if (AsIOMMCreate_Mod.status == 0)
            {
                step = 11;                                      /* next step */
            }
            else
            {
                if (AsIOMMCreate_Mod.status != 0xFFFF)
                {
                    error_step = step;
                    error_status = AsIOMMCreate_Mod.status;
                    step = 100;                                     /* error step */
                    strcpy(error_string, "br_create_config");
                }
            }
            break;
         }

		 case 11:
         {
			if(InstallConfig1)
			{
	            IOChangeStep = 12;
         		IOConfiguration = 1;
	        }
	        else
			if(InstallConfig2)
			{
	            IOChangeStep = 13;
         		IOConfiguration = 2;
	        }

            SYSreset(1, 1);                                         /* warm restart for activating new arconfig.br */
            break;
         }


		case 20:
        {

            AsIOMMCreate_Mod.enable = 1;                            /* enable */
			if(InstallConfig1)
			{
	            AsIOMMCreate_Mod.pDataObject = (UDINT) "iomap1";       /* name of datamodule where the data should be taken from */
    	        AsIOMMCreate_Mod.pNewModule = (UDINT) "iomap";          /* name of the new *.br module */
    	    }
			else
			{
	            AsIOMMCreate_Mod.pDataObject = (UDINT) "iomap2";       /* name of datamodule where the data should be taken from */
    	        AsIOMMCreate_Mod.pNewModule = (UDINT) "iomap";          /* name of the new *.br module */
    	    }
            AsIOMMCreate_Mod.moduleKind = 1 /*mapping*/;                        /* which type of *.br module should be created */
            AsIOMMCreate_Mod.memType = 2 /*USRROM*/;                           /* where should the *.br module be stored */

            /* Call FBK */
            AsIOMMCreate(&AsIOMMCreate_Mod);

            /* Verify status */
            if (AsIOMMCreate_Mod.status == 0)
            {
                step = 30;                                      /* next step */
            }
            else
            {
                if (AsIOMMCreate_Mod.status != 0xFFFF)
                {
                    error_step = step;
                    error_status = AsIOMMCreate_Mod.status;
                    step = 100;                                     /* error step */
                    strcpy(error_string, "br_create_iomap");
                }
            }
            break;
         }
		 case 30:
         {
         	if(InstallConfig1)
         		IOConfiguration = 1;
         	else
         	if(InstallConfig2)
         		IOConfiguration = 2;

         	InstallConfig1 = 0;
         	InstallConfig2 = 0;
         	step = 0;
         	IOChangeStep = 0;
            break;
         }

	}/* switch */
}



