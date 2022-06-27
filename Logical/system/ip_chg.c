#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	IP Adresse und subnet mask setzen 										*/
/*	Achtung: Zum Einstellen des Gateway ist AR102 Version 2.61 erforderlich!*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			28.02.03	erste Implementation 					HA		*/
/*																			*/
/****************************************************************************/

/***** Include-Files: *****/
#include <bur/plc.h>
#include <bur/plctypes.h>
#include "sys_lib.h"
#include "brsystem.h"
#include "AsArcfg.h"
#include <string.h>
#include "glob_var.h"
#include "asstring.h"
#include "Auxfunc.h"

#define NOERROR	0
#define cfgCONFIGMODE_MANUALLY 0
#define cfgERR_PARAM_NOT_SET 29009
#define cfgERR_SET_NOT_POSSIBLE 29004
#define ERR_FUB_BUSY 65535
#define ERR_VALUE_INVALID 29003

typedef struct
{
	char byte[4];
}IP_typ;



_LOCAL IP_typ	IPBytes;

_LOCAL UINT				SetIPStep;
_LOCAL	STRING		NewIP[16];
_LOCAL	USINT		SetIP,GetIP;

_LOCAL	STRING		OldIP[16],OldGateway[16],OldSubnet[16],entry[50],tmp[20];
_LOCAL	CfgGetIPAddr_typ CfgGetIPAddr_var;
_LOCAL	CfgSetIPAddr_typ CfgSetIPAddr_var;
_LOCAL	CfgGetDefaultGateway_typ CfgGetDefaultGateway_var;
_LOCAL	CfgSetDefaultGateway_typ CfgSetDefaultGateway_var;
_LOCAL	CfgGetSubnetMask_typ CfgGetSubnetMask_var;
_LOCAL	CfgSetSubnetMask_typ CfgSetSubnetMask_var;

_LOCAL CfgSetEthConfigMode_typ CfgSetEthConfigMode_var;

_LOCAL	USINT	IPAdr1,IPAdr2,IPAdr3,IPAdr4;
_LOCAL	USINT	Gateway1,Gateway2,Gateway3,Gateway4;
_LOCAL	USINT	Subnet1,Subnet2,Subnet3,Subnet4;
		BOOL	changed;

_LOCAL STRING OSVersionString[10];
_LOCAL TARGETInfo_typ TARGETInfo_01;
_LOCAL int ErrorCode;
int WaitCounter;
_GLOBAL USINT		gTriggerRedraw;

static char ETHName[50];


void IPString2Bytes(char *IPString,IP_typ *IP)
{
	int i,j,cnt;
	char help[16];
	i=0;
	j=0;
	cnt = 0;

	while(cnt<=3)
	{
		j=0;
		while( IPString[i]!='.' && IPString[i]!= 0 && i<16)
		{
			help[j] = IPString[i];
			i++;
			j++;
		/*safety!*/
			if(i>15) break;
		}
		help[j] = 0;
		IP->byte[cnt] = (char) atoi((UDINT) &help);
		cnt++;
		i++;
		if(i>15) break;
	}
}


void Bytes2IPString(char *IPStr,int Adr1,
								int Adr2,
								int Adr3,
								int Adr4)
{
	strcpy(IPStr,"");
	itoa(Adr1,(UDINT) &tmp[0]);
	strcat(IPStr,tmp);
	strcat(IPStr,".");
	itoa(Adr2,(UDINT) &tmp[0]);
	strcat(IPStr,tmp);
	strcat(IPStr,".");
	itoa(Adr3,(UDINT) &tmp[0]);
	strcat(IPStr,tmp);
	strcat(IPStr,".");
	itoa(Adr4,(UDINT) &tmp[0]);
	strcat(IPStr,tmp);
}


_INIT void init(void)
{
	GetIP = 0;
	SetIP = 0;
	SetIPStep = 0;
#ifdef PPC2100
	strcpy(ETHName,"IF3");
#else
	strcpy(ETHName,"IF5");
#endif
}

_CYCLIC void cyclic(void)
{
	if(xBildInit && (wBildAktuell == 124 || wBildAktuell == 11) )
		GetIP = 1;

	switch (SetIPStep)
	{
		case 0:
		{
			changed = 0;
	/*wait for start cmd*/
			if(GetIP)
			{
				SetIPStep = 5;
				strcpy(OldIP,"");
				strcpy(OldGateway,"");
				strcpy(OldSubnet,"");
				gTriggerRedraw = 1;
			}
			if(SetIP)
			{
				SetIPStep = 14;
			}
			break;
		}
/* aktuelle IP auslesen */
		case 5:
		{
			strcpy(entry,ETHName);
			CfgGetIPAddr_var.pDevice = (UDINT) &entry;
			CfgGetIPAddr_var.pIPAddr = (UDINT) &OldIP;
			CfgGetIPAddr_var.Len = 16;
			CfgGetIPAddr_var.enable = 1;
			CfgGetIPAddr(&CfgGetIPAddr_var);

			switch (CfgGetIPAddr_var.status)
			{
				case NOERROR:
				{
					IPString2Bytes(OldIP,&IPBytes);
					IPAdr1 = IPBytes.byte[0];
					IPAdr2 = IPBytes.byte[1];
					IPAdr3 = IPBytes.byte[2];
					IPAdr4 = IPBytes.byte[3];
					SetIPStep = 6;
					gTriggerRedraw = 1;
					break;
				}
				case ERR_FUB_BUSY:
					break;
			}
			break;
		}
/* aktuelles Def. Gateway auslesen */
		case 6:
		{
			strcpy(entry,ETHName);
			CfgGetDefaultGateway_var.pDevice = (UDINT) &entry;
			CfgGetDefaultGateway_var.pGateway = (UDINT) &OldGateway;
			CfgGetDefaultGateway_var.Len = 16;
			CfgGetDefaultGateway_var.enable = 1;
			CfgGetDefaultGateway(&CfgGetDefaultGateway_var);

			switch (CfgGetDefaultGateway_var.status)
			{
				case NOERROR:
				{
					IPString2Bytes(OldGateway,&IPBytes);
					Gateway1 = IPBytes.byte[0];
					Gateway2 = IPBytes.byte[1];
					Gateway3 = IPBytes.byte[2];
					Gateway4 = IPBytes.byte[3];
					SetIPStep = 7;
					gTriggerRedraw = 1;
					break;
				}
				case cfgERR_PARAM_NOT_SET:
				{
					SetIPStep = 7;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgGetDefaultGateway_var.status;
					break;
				}
			}
			break;
		}

/* aktuelles Def. Gateway auslesen */
		case 7:
		{
			strcpy(entry,ETHName);
			CfgGetSubnetMask_var.pDevice = (UDINT) &entry;
			CfgGetSubnetMask_var.pSubnetMask = (UDINT) &OldSubnet;
			CfgGetSubnetMask_var.Len = 16;
			CfgGetSubnetMask_var.enable = 1;
			CfgGetSubnetMask(&CfgGetSubnetMask_var);

			switch (CfgGetSubnetMask_var.status)
			{
				case NOERROR:
				{
					IPString2Bytes(OldSubnet,&IPBytes);
					Subnet1 = IPBytes.byte[0];
					Subnet2 = IPBytes.byte[1];
					Subnet3 = IPBytes.byte[2];
					Subnet4 = IPBytes.byte[3];
					SetIPStep = 13;
					gTriggerRedraw = 1;
					break;
				}
				case cfgERR_PARAM_NOT_SET:
				{
					SetIPStep = 13;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgGetSubnetMask_var.status;
					break;
				}
			}
			break;
		}

		case 13:
		{
			GetIP = 0;

		    TARGETInfo_01.enable = 1;                             /* Funktion wird nur ausgeführt wenn ENABLE <> 0
                                                             ist. */
		    TARGETInfo_01.pOSVersion = (UDINT) &OSVersionString;  /* Zeiger auf einen String mit vorgegebener Länge, in
                                                            den die Betriebssystemversion geschrieben wird */

		    TARGETInfo(&TARGETInfo_01);
			SetIPStep = 0;
			GetIP = 0;
			break;
		}

/******************************************************************/
/******************************************************************/
		case 14:
		{
			CfgSetEthConfigMode_var.enable = 1;
			CfgSetEthConfigMode_var.pDevice = (UDINT) &entry;
			CfgSetEthConfigMode_var.ConfigMode = cfgCONFIGMODE_MANUALLY;
			CfgSetEthConfigMode_var.Option = 1;
			CfgSetEthConfigMode(&CfgSetEthConfigMode_var);
			switch (CfgSetEthConfigMode_var.status)
			{
				case NOERROR:
				{
					SetIPStep = 15;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgSetEthConfigMode_var.status;
					break;
				}
			}
			break;
		}


/* set Subnetmask */
		case 15:
		{
			Bytes2IPString(NewIP,Subnet1,Subnet2,Subnet3,Subnet4);
			if( !strcmp(OldSubnet,NewIP) )
			{
				SetIPStep = 20;
				break;
			}

			strcpy(entry,ETHName);
			CfgSetSubnetMask_var.pDevice = (UDINT) &entry;
			CfgSetSubnetMask_var.pSubnetMask = (UDINT) &NewIP;
			CfgSetSubnetMask_var.Option = 1; /*NON-VOLATILE*/
			CfgSetSubnetMask_var.enable = 1;
			CfgSetSubnetMask(&CfgSetSubnetMask_var);

			switch (CfgSetSubnetMask_var.status)
			{
				case NOERROR:
				{
					SetIPStep = 20;
					break;
				}
				case cfgERR_SET_NOT_POSSIBLE:
				{
					SetIPStep = 20;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgSetSubnetMask_var.status;
					break;
				}
			}
			break;
		}


/* IP Adresse auf Änderung prüfen und ggf. setzen  */

/* set IP */
		case 20:
		{
			Bytes2IPString(NewIP,IPAdr1,IPAdr2,IPAdr3,IPAdr4);
			if( !strcmp(OldIP,NewIP) )
			{
				SetIPStep = 25;
				break;
			}

			strcpy(entry,ETHName);
			CfgSetIPAddr_var.pDevice = (UDINT) &entry;
			CfgSetIPAddr_var.pIPAddr = (UDINT) &NewIP;
			CfgSetIPAddr_var.Option = 1; /*NON-VOLATILE*/
			CfgSetIPAddr_var.enable = 1;
			CfgSetIPAddr(&CfgSetIPAddr_var);

			switch (CfgSetIPAddr_var.status)
			{
				case NOERROR:
				{
					SetIPStep = 25;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgSetIPAddr_var.status;
					break;
				}
			}
			break;
		}
/* set Gateway */
		case 25:
		{
			Bytes2IPString(NewIP,Gateway1,Gateway2,Gateway3,Gateway4);
			if( !strcmp(OldGateway,NewIP) )
			{
				SetIPStep = 0;
				SetIP = 0;
				GetIP = 1;
				break;
			}

			strcpy(entry,ETHName);
			CfgSetDefaultGateway_var.pDevice = (UDINT) &entry;
			CfgSetDefaultGateway_var.pGateway = (UDINT) &NewIP;
			CfgSetDefaultGateway_var.Option = 1; /*NON-VOLATILE*/
			CfgSetDefaultGateway_var.enable = 1;
			CfgSetDefaultGateway(&CfgSetDefaultGateway_var);

			switch (CfgSetDefaultGateway_var.status)
			{
				case NOERROR:
				{
					SetIPStep = 0;
					SetIP = 0;
					GetIP = 1;
					break;
				}
				case cfgERR_SET_NOT_POSSIBLE:
				{
					SetIPStep = 0;
					SetIP = 0;
					GetIP = 1;
					break;
				}
				case ERR_FUB_BUSY:
					break;
				default:
				{
					ErrorCode = CfgSetDefaultGateway_var.status;
					SetIPStep = 0;
					SetIP = 0;
					GetIP = 0;
					break;
				}
			}
			break;
		}

	}
}


