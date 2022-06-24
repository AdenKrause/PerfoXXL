
TYPE
	CANIOcmd_typ : 	STRUCT 
		busnr : USINT;
		nodenr : USINT;
		cmd : UDINT;
		res : UDINT;
		status : UINT;
		initcnt : UDINT;
		fcb_ptr : UDINT;
		fcb_cnt : UDINT;
		tiotick : UDINT;
		node_ptr : UDINT;
		local_flag : USINT;
		first_flag : USINT;
		state : USINT;
		pp_ix : USINT;
		enable : BOOL;
	END_STRUCT;
	Command : 	STRUCT 
		code : USINT;
		comcode : USINT;
		param1 : USINT;
		param2 : USINT;
		data1 : USINT;
		data2 : USINT;
		data3 : USINT;
		data4 : USINT;
	END_STRUCT;
	EGMPreWashParam_Type : 	STRUCT 
		ReplenishingMode : UINT;
		ReplenishmentPerSqm : REAL;
		ReplenishmentPerPlate : REAL;
		ReplenishmentIntervall : UINT;
		PumpMlPerSec : REAL;
		MinPumpOnTime : UINT;
	END_STRUCT;
	EGMPreWash_Type : 	STRUCT 
		TankFull : BOOL;
		Pump : BOOL;
		Valve : BOOL;
		HeatingOn : BOOL;
		LevelNotInRange : USINT;
		RealTemperature : INT;
		Auto : BOOL;
		PumpCmd : BOOL;
	END_STRUCT;
	Modules_typ : 	STRUCT 
		name : STRING[19];
		info : MoVerStruc_typ;
	END_STRUCT;
	fiLOCAL_OBJ : 	STRUCT 
		StateMan : UINT;
		ErrMan : UINT;
		Init : UDINT;
	END_STRUCT;
END_TYPE
