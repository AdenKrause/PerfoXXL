#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/****************************************************************************/
/*	Kommunikationsroutine für CAN 						 					*/
/*																			*/
/*  - Öffnen der Schnittstelle im INIT Teil									*/
/*  - zyklisches bearbeiten der Tabellenorientierten Kommunikation			*/
/*																			*/
/*																			*/
/*	Versionsgeschichte:														*/
/*  Version		Datum		Änderung								von		*/
/*	1.0			06.08.02	erste Implemetation						HA		*/
/*																			*/
/****************************************************************************/

#include <bur/plctypes.h>
#include <can_lib.h>								/*prototypes of can_lib*/
#include <dataobj.h>								/*prototypes of dataobj*/

#ifdef PPC2100
#define CAN_DEVICE "SL1.IF3"
#else
#define CAN_DEVICE "SL1.SS1.IF1"
#endif

CANopen_typ _LOCAL CANopen_01;
CANdftab_typ _LOCAL CANdftab_01;
CANrwtab_typ _LOCAL CANrwtab_01;

/* === variables declaration === */
_LOCAL UDINT error_var;
_LOCAL DatObjInfo_typ DOInfo;
_GLOBAL BOOL CAN_ok;


void _INIT init( void )								/*init part of task*/
{
	CANopen_01.enable = 1;
	CANopen_01.baud_rate = 25;						/*define baud rate - 250 kBit/s, 12 -> 125KB/s */
	CANopen_01.cob_anz = 20;							/*dfine number  of COB`s*/
/*	CANopen_01.device = (UDINT) "IF3"; */				/*define device*/
	CANopen_01.device = (UDINT) CAN_DEVICE;
	CANopen_01.error_adr = (UDINT)&error_var;				/*define error adress*/
	CANopen(&CANopen_01);							/*initiate CANopen*/

	if( CANopen_01.status == 0)						/*check error level*/
	{
		DOInfo.enable = 1;
		DOInfo.pName = (UDINT) "can_tab";			/* Datenmodulname für CAN Objekte */

		/* Call FBK */
		DatObjInfo(&DOInfo);

		/* Verify status */
		if (DOInfo.status == 0)
		{
		 	CANdftab_01.enable = 1;
		 	CANdftab_01.us_ident = CANopen_01.us_ident;		/*set us_ident*/
		 	CANdftab_01.table_adr = DOInfo.pDatObjMem;			/*define adress of data module*/
			CANdftab_01.tab_num = DOInfo.len / 72;				/*define number of values*/
	 		CANdftab (&CANdftab_01);     				/*initiate CANdftab*/

			if(CANdftab_01.status == 0)						/*check error level*/
			{
				CANrwtab_01.enable = 1;
	 			CANrwtab_01.tab_ident=CANdftab_01.tab_ident;			/*set tab_id*/
				CAN_ok = 1;
			}
			else
				CAN_ok = 0;
		}/*if DOInfo.status==0*/
		else
			CAN_ok = 0;
	} /* if canopen01.status==0 */
	else
		CAN_ok = 0;
}

void _CYCLIC cyclic( void )							/*cyclic part of task*/
{
	if(CANdftab_01.status == 0)						/*check error level*/
	{
		CANrwtab(&CANrwtab_01); 					/*initiate CANwrtab*/
		CAN_ok = 1;

	}
	else
		CAN_ok = 0;

}


