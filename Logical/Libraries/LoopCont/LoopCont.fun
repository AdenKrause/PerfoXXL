(********************************************************************
 * COPYRIGHT (C) BERNECKER + RAINER, AUSTRIA, A-5142 EGGELSBERG
 ********************************************************************
 * Library: LoopCont
 * File: LoopCont.fun
 * Created: 02.11.2006
 ********************************************************************
 * Functions and function blocks of library LoopCont
 ********************************************************************)
FUNCTION_BLOCK LCSlimPID
	VAR_INPUT
		enable	:BOOL;
		W	:INT;
		X	:INT;
		request	:UDINT;
		pPar	:REFERENCE TO lcslimpid_par_typ;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		e	:DINT;
		Y	:INT;
		status	:UINT;
		addInfo	:UINT;
	END_VAR
	VAR
		MovAvgFlt_inst	:LCMovAvFilter;
		PIDTune_inst	:LCPIDTune;
		PID_inst	:LCPID;
		enable_old	:BOOL;
		requ_old	:UDINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPWM
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		max_value	:INT;
		min_value	:INT;
		t_min_pulse	:UINT;
		t_period	:UINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		pulse	:BOOL;
	END_VAR
	VAR
		area	:DINT;
		period	:UINT;
		time_old	:UDINT;
		last	:UINT;
		x_old	:INT;
		enable_old	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCContinServo
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		max_value	:INT;
		min_value	:INT;
		t_impulse	:UINT;
		t_change_up	:UINT;
		t_change_down	:UINT;
		ref	:BOOL;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		up	:BOOL;
		down	:BOOL;
		hysteresis_up	:UINT;
		hysteresis_down	:UINT;
		refOk	:BOOL;
	END_VAR
	VAR
		enable_old	:BOOL;
		time_old	:UDINT;
		ref_old	:BOOL;
		t_cnt	:DINT;
		y_fix31_1	:DINT;
		y_fix31_2	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCMinMax
	VAR_INPUT
		reset	:BOOL;
		in	:INT;
	END_VAR
	VAR_OUTPUT
		out_min	:INT;
		out_max	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPIDTune
	VAR_INPUT
		enable	:BOOL;
		Y_min	:INT;
		Y_max	:INT;
		Y0	:INT;
		Y1	:INT;
		okToStep	:BOOL;
		X0	:INT;
		X_min	:INT;
		X_max	:INT;
		P_manualAdjust	:INT;
		I_manualAdjust	:INT;
		D_manualAdjust	:INT;
		request	:UDINT;
		basetime	:UDINT;
		pAddPar	:REFERENCE TO lcpid_tune_addpar_typ;
		pOptions_osc	:REFERENCE TO lcpid_tune_osc_options_typ;
		pOptions_step	:REFERENCE TO lcpid_tune_step_options_typ;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		addInfo	:UINT;
		ident	:UDINT;
		state	:UINT;
		rdyToStep	:BOOL;
	END_VAR
	VAR
		dt	:UDINT;
		t_autotune	:UDINT;
		status_tmp	:UINT;
		MovAvgFlt_inst	:LCMovAvFilter;
		old	:lcpid_old_typ;
		processPar	:lcpid_procPar_typ;
		internal_data	:lcpid_internal_typ;
		addPar	:lcpid_tune_addpar_typ;
		oscillation	:lcpid_tune_osc_typ;
		stepresponse	:lcpid_tune_step_typ;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCDblActPID
	VAR_INPUT
		enable	:BOOL;
		W	:INT;
		X	:INT;
		Y_man	:INT;
		hold_I	:BOOL;
		mode	:USINT;
		okToStep	:BOOL;
		basetime	:UDINT;
		pPar1	:REFERENCE TO lcdblpid_par_typ;
		pPar2	:REFERENCE TO lcdblpid_par_typ;
		pAddPar	:REFERENCE TO lcpid_tune_addpar_typ;
		pOpt	:REFERENCE TO lcdblpid_tune_typ;
	END_VAR
	VAR_OUTPUT
		e	:DINT;
		Y1	:INT;
		Y2	:INT;
		status	:UINT;
		tuneState	:USINT;
		rdyToStep	:BOOL;
	END_VAR
	VAR
		enable_old	:BOOL;
		pid	:LCPID;
		tune	:LCPIDTune;
		gradient	:REAL;
		Y_avg	:DINT;
		tcnt	:UDINT;
		Tmeas	:INT;
		y_lim_old	:INT;
		use_par	:USINT;
		meas	:USINT;
		mode_old	:USINT;
		tune_step	:USINT;
		tune_grad	:USINT;
		tune_2nd	:USINT;
		invert	:USINT;
		mode_int	:USINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPFM
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		max_value	:INT;
		min_value	:INT;
		t_pulse	:UINT;
		t_pause	:UINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		pulse	:BOOL;
	END_VAR
	VAR
		enable_old	:BOOL;
		range	:INT;
		area	:DINT;
		area_c	:DINT;
		time_old	:UDINT;
		last	:DINT;
		tpls	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCCurveByPoints
	VAR_INPUT
		x	:INT;
		NoOfPoints	:UINT;
		ptr_table	:REFERENCE TO lcCurveByPoints_TabEntry_type;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
	END_VAR
	VAR
		i_tab	:INT;
		iterations	:UINT;
		scal1	:LCScINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPWM_DMod
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		max_value	:INT;
		min_value	:INT;
		tminpuls	:UINT;
		tperiod	:UINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:BOOL;
	END_VAR
	VAR
		area	:DINT;
		period	:UINT;
		time_old	:UDINT;
		last	:UINT;
		x_old	:INT;
		enable_old	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPIDpara
	VAR_INPUT
		enable	:BOOL;
		enter	:BOOL;
		Y_max	:INT;
		Y_min	:INT;
		dY_max	:REAL;
		Kp	:REAL;
		Tn	:REAL;
		Tv	:REAL;
		Tf	:REAL;
		Kw	:REAL;
		Kfbk	:REAL;
		fbk_mode	:USINT;
		d_mode	:USINT;
		calc_mode	:USINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		ident	:UDINT;
	END_VAR
	VAR
		internal_data	:lcpid_internal_typ;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCRampLim
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		tup	:UINT;
		tdown	:UINT;
		max_value	:INT;
		min_value	:INT;
		yset	:INT;
		set	:BOOL;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
		xreached	:BOOL;
		max_limit	:BOOL;
		min_limit	:BOOL;
	END_VAR
	VAR
		time_old	:UDINT;
		rest	:UINT;
		enable_old	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPID
	VAR_INPUT
		enable	:BOOL;
		ident	:UDINT;
		W	:INT;
		X	:INT;
		A	:INT;
		Y_man	:INT;
		Y_fbk	:INT;
		hold_I	:BOOL;
		out_mode	:USINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		e	:DINT;
		Y	:INT;
		Yp	:DINT;
		Yi	:DINT;
		Yd	:DINT;
	END_VAR
	VAR
		time_old	:UDINT;
		Tv_Tf_old	:DINT;
		enable_old	:BOOL;
		e_old	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCTimeBasedOnOff
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		max_value	:INT;
		min_value	:INT;
		timpulse	:UINT;
		tchange_up	:UINT;
		tchange_dwn	:UINT;
		ref	:BOOL;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		up	:BOOL;
		down	:BOOL;
		hysteresis_up	:UINT;
		hysteresis_dwn	:UINT;
		refOK	:BOOL;
	END_VAR
	VAR
		enable_old	:BOOL;
		time_old	:UDINT;
		ref_old	:BOOL;
		t_cnt	:DINT;
		y_fix31_1	:DINT;
		y_fix31_2	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCDifferentiate
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		tv	:UINT;
		tf	:UINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
	END_VAR
	VAR
		time_old	:UDINT;
		enable_old	:BOOL;
		a2n	:DINT;
		a1	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPIDAutoTune
	VAR_INPUT
		enable	:BOOL;
		Y_max	:INT;
		Y_min	:INT;
		P_manualAdjust	:INT;
		I_manualAdjust	:INT;
		D_manualAdjust	:INT;
		request	:USINT;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		ident	:UDINT;
		addInfo	:UINT;
		state	:UINT;
	END_VAR
	VAR
		dt	:UDINT;
		t_autotune	:UDINT;
		diff	:lcpid_diff_typ;
		old	:lcpid_old_typ;
		stepResponse	:lcpid_stepResp_typ;
		oscillation	:lcpid_osc_typ;
		processPar	:lcpid_procPar_typ;
		internal_data	:lcpid_internal_typ;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LC2PHy
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		xstart	:INT;
		xsetvalue	:INT;
		hysteresis	:UINT;
		ymax	:INT;
		ymin	:INT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
	END_VAR
	VAR
		y1	:BOOL;
		enable_old	:BOOL;
		x_old	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCScUINT
	VAR_INPUT
		x	:UINT;
		x1	:UINT;
		y1	:UINT;
		x2	:UINT;
		y2	:UINT;
	END_VAR
	VAR_OUTPUT
		y	:UINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LC3PHy
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		xstart	:INT;
		xsetvalue1	:INT;
		xsetvalue2	:INT;
		hysteresis1	:UINT;
		hysteresis2	:UINT;
		ymax	:INT;
		ymid	:INT;
		ymin	:INT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
		ymx	:BOOL;
		ymn	:BOOL;
	END_VAR
	VAR
		enable_old	:BOOL;
		x_old	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCComparator
	VAR_INPUT
		enable	:BOOL;
		xp	:INT;
		xn	:INT;
		sd	:UINT;
	END_VAR
	VAR_OUTPUT
		greater	:BOOL;
		equal	:BOOL;
		lower	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCCounter
	VAR_OUTPUT
		muscnt	:UDINT;
		mus100cnt	:UDINT;
		mscnt	:UDINT;
		ms100cnt	:UDINT;
		seccnt	:UDINT;
	END_VAR
	VAR
		state	:UINT;
		sysinf_inst	:SysInfo;
		musecs_old	:UINT;
		bootkey_old	:UDINT;
		sys_ticks_old	:UDINT;
		to_100us	:UDINT;
		to_1ms	:UDINT;
		to_100ms	:UINT;
		to_1s	:UINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCIntegrate
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		tn	:UINT;
		max_value	:INT;
		min_value	:INT;
		yset	:INT;
		set	:BOOL;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
		max_limit	:BOOL;
		min_limit	:BOOL;
	END_VAR
	VAR
		time_old	:UDINT;
		rest	:DINT;
		enable_old	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCLimit
	VAR_INPUT
		enable	:BOOL;
		in	:DINT;
		max_value	:INT;
		min_value	:INT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		out	:INT;
		max_limit	:BOOL;
		min_limit	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCLiScDINT
	VAR_INPUT
		x	:DINT;
		x1	:DINT;
		y1	:DINT;
		x2	:DINT;
		y2	:DINT;
	END_VAR
	VAR_OUTPUT
		y	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCLiScINT
	VAR_INPUT
		x	:INT;
		x1	:INT;
		y1	:INT;
		x2	:INT;
		y2	:INT;
	END_VAR
	VAR_OUTPUT
		y	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCLiScUINT
	VAR_INPUT
		x	:UINT;
		x1	:UINT;
		y1	:UINT;
		x2	:UINT;
		y2	:UINT;
	END_VAR
	VAR_OUTPUT
		y	:UINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCMovAvFilter
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		base	:UINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
	END_VAR
	VAR
		sum_old	:DINT;
		x_old_ptr	:UDINT;
		x_old	:ARRAY[0..31] OF INT;
		xIdx	:UINT;
		base_old	:UINT;
		enable_old	:BOOL;
		bootkey_old	:UDINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCNegDINT
	VAR_INPUT
		in	:DINT;
	END_VAR
	VAR_OUTPUT
		out	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCNegINT
	VAR_INPUT
		in	:INT;
	END_VAR
	VAR_OUTPUT
		out	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCNegPeakVal
	VAR_INPUT
		in	:INT;
		reset	:BOOL;
	END_VAR
	VAR_OUTPUT
		out	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPosPeakVal
	VAR_INPUT
		in	:INT;
		reset	:BOOL;
	END_VAR
	VAR_OUTPUT
		out	:INT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCPT1
	VAR_INPUT
		enable	:BOOL;
		x	:INT;
		t	:UINT;
		yset	:INT;
		set	:BOOL;
		basetime	:UDINT;
	END_VAR
	VAR_OUTPUT
		status	:UINT;
		y	:INT;
	END_VAR
	VAR
		time_old	:UDINT;
		rest	:DINT;
		enable_old	:BOOL;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCScDINT
	VAR_INPUT
		x	:DINT;
		x1	:DINT;
		y1	:DINT;
		x2	:DINT;
		y2	:DINT;
	END_VAR
	VAR_OUTPUT
		y	:DINT;
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK LCScINT
	VAR_INPUT
		x	:INT;
		x1	:INT;
		y1	:INT;
		x2	:INT;
		y2	:INT;
	END_VAR
	VAR_OUTPUT
		y	:INT;
	END_VAR
END_FUNCTION_BLOCK
