(********************************************************************
 * COPYRIGHT (C) BERNECKER + RAINER, AUSTRIA, A-5142 EGGELSBERG
 ********************************************************************
 * Library: LoopCont
 * File: LoopCont.typ
 * Created: 11.11.2003
 ********************************************************************
 * Global data types of library LoopCont
 ********************************************************************)
TYPE
	lcslimpid_par_typ : STRUCT
		Y_max	: INT ;
		Y_min	: INT ;
		Kp	: REAL ;
		Tn	: REAL ;
		Tv	: REAL ;
		Kfbk	: REAL ;
	END_STRUCT;
	lcpid_tune_addpar_typ : STRUCT
		t_max_tune	: UDINT ;
		dY_max	: REAL ;
		Tf_Tv	: REAL ;
		Kw	: REAL ;
		Kfbk	: REAL ;
		fbk_mode	: USINT ;
		d_mode	: USINT ;
		calc_mode	: USINT ;
	END_STRUCT;
	lcpid_tune_osc_options_typ : STRUCT
		osc_minAmplitude	: INT ;
		Q_min	: REAL ;
	END_STRUCT;
	lcpid_osc_val_typ : STRUCT
		X	: INT ;
		t	: UDINT ;
	END_STRUCT;
	lcpid_osc_per_typ : STRUCT
		max	: lcpid_osc_val_typ ;
		min	: lcpid_osc_val_typ ;
	END_STRUCT;
	lcpid_tune_osc_typ : STRUCT
		options	: lcpid_tune_osc_options_typ ;
		oscPhase	: UINT ;
		dir	: SINT ;
		enHi	: BOOL ;
		enLo	: BOOL ;
		Y_hi	: INT ;
		Y_lo	: INT ;
		i_maxPeriod	: UINT ;
		i_period	: UINT ;
		X_ampl	: DINT ;
		X_avg	: DINT ;
		X_min_avg	: DINT ;
		X_max_avg	: DINT ;
		Y_avg	: DINT ;
		deltaY	: DINT ;
		t_Ylohi	: ARRAY[0..4] OF UDINT ;
		t_Yhilo	: ARRAY[0..4] OF UDINT ;
		Q_act	: REAL ;
		a_wx	: INT ;
		amplitudeRatio	: REAL ;
		Ku	: REAL ;
		tPeriod	: UDINT ;
		period	: ARRAY[0..4] OF lcpid_osc_per_typ ;
	END_STRUCT;
	lcpid_tune_step_options_typ : STRUCT
		eqDeltaX	: INT ;
		eqDeltaWX	: INT ;
		eqDeltat	: UDINT ;
		evalDeltaX	: INT ;
		evalNfilter	: UINT ;
		exitNotMaxdXCount	: UINT ;
		exitdXRatio	: UINT ;
	END_STRUCT;
	lcpid_tune_step_typ : STRUCT
		options	: lcpid_tune_step_options_typ ;
		Y0	: INT ;
		Y1	: INT ;
		t_jump	: UDINT ;
		W	: INT ;
		X0	: INT ;
		Xmax	: INT ;
		Xmin	: INT ;
		Xfiltered	: DINT ;
		deltaXok	: BOOL ;
		deltaWXok	: BOOL ;
		eqTime	: UDINT ;
		dir	: SINT ;
		deltaX	: INT ;
		X1	: INT ;
		t1	: UDINT ;
		t1set	: BOOL ;
		dX_dt	: REAL ;
		dX_dt_max	: REAL ;
		XmaxGrad	: INT ;
		maxX1	: INT ;
		maxX2	: INT ;
		maxt1	: UDINT ;
		maxt2	: UDINT ;
		tmaxGrad	: UDINT ;
		gainFactor	: REAL ;
		deadTime	: UDINT ;
		riseTime	: UDINT ;
		notMaxCount	: UINT ;
		gradientRatio	: UINT ;
		cycleTime	: UDINT ;
	END_STRUCT;
	lcdblpid_tune_typ : STRUCT
		P1_manualAdjust	: INT ;
		I1_manualAdjust	: INT ;
		D1_manualAdjust	: INT ;
		P2_manualAdjust	: INT ;
		I2_manualAdjust	: INT ;
		D2_manualAdjust	: INT ;
		X_min	: INT ;
		X_max	: INT ;
		X0	: INT ;
		de_min	: INT ;
		Y0	: INT ;
		Y1step	: INT ;
		Y2step	: INT ;
		tuneY1_opt	: UDINT ;
		tuneY2_opt	: UDINT ;
		tune_first	: USINT ;
		hyst	: INT ;
		measDelta	: INT ;
		osc_opt	: lcpid_tune_osc_options_typ ;
		step_opt	: lcpid_tune_step_options_typ ;
	END_STRUCT;
	lcdblpid_par_typ : STRUCT
		Y_max	: INT ;
		Y_min	: INT ;
		K_fact	: INT ;
		Kp	: REAL ;
		Tn	: REAL ;
		Tv	: REAL ;
	END_STRUCT;
	lcCurveByPoints_TabEntry_type : STRUCT
		x	: INT ;
		y	: INT ;
	END_STRUCT;
	lcpid_addPar_typ : STRUCT
		y_autotune_on	: INT ;
		y_autotune_off	: INT ;
		t_autotune_max	: UDINT ;
		diff_dx_max	: INT ;
		diff_dt_max	: UDINT ;
		osc_nrPeriods	: UINT ;
		osc_minAmplitude	: INT ;
		optSwitches	: UINT ;
		dY_max	: REAL ;
		Tf_Tv	: REAL ;
		Kw	: REAL ;
		Kfbk	: REAL ;
		fbk_mode	: USINT ;
		d_mode	: USINT ;
		calc_mode	: USINT ;
	END_STRUCT;
	lcpid_internal_typ : STRUCT
		size	: UINT ;
		valid	: BOOL ;
		autotune_addpar	: lcpid_addPar_typ ;
		Y_max	: INT ;
		Y_min	: INT ;
		dY_max1	: DINT ;
		Kp1	: DINT ;
		Kp_Tn_1	: DINT ;
		Tv_Tf_1	: DINT ;
		Tf_reciproc_1	: DINT ;
		Kw1	: DINT ;
		Kfbk_Kp_1	: DINT ;
		fbk_mode	: USINT ;
		d_mode	: USINT ;
		calc_mode	: USINT ;
		prescalerPow2	: UINT ;
		force_outmode	: USINT ;
		Y_force	: INT ;
		W	: INT ;
		X	: INT ;
		pid_init	: USINT ;
		dt	: UDINT ;
		Yp1	: DINT ;
		Yp2	: DINT ;
		dYi1	: DINT ;
		dYi2	: DINT ;
		Yi1	: DINT ;
		Yi2	: DINT ;
		dt_Tf_1	: DINT ;
		a11	: DINT ;
		a12	: DINT ;
		a21	: DINT ;
		a22	: DINT ;
		Yd1	: DINT ;
		Yd2	: DINT ;
		Ytotal1	: DINT ;
		Ytotal2	: DINT ;
		Ylim1	: DINT ;
		Ylim2	: DINT ;
		Y1	: DINT ;
		Y2	: DINT ;
		e_fbk	: DINT ;
	END_STRUCT;
	lcpid_osc_typ : STRUCT
		i_maxPeriod	: UINT ;
		i_period	: UINT ;
		X_amplitude	: DINT ;
		amplitudeRatio	: REAL ;
		tPeriod	: UDINT ;
		period	: ARRAY[0..4] OF lcpid_osc_per_typ ;
	END_STRUCT;
	lcpid_procPar_typ : STRUCT
		size	: UINT ;
		valid	: BOOL ;
		stepResp_valid	: BOOL ;
		stepResp_v	: REAL ;
		stepResp_t_u	: UDINT ;
		stepResp_t_g	: UDINT ;
		osc_valid	: BOOL ;
		osc_amplitudeRatio	: REAL ;
		osc_tPeriod	: UDINT ;
		force_params	: BOOL ;
		Kp	: REAL ;
		Kp_tune	: REAL ;
		Tn	: REAL ;
		Tn_tune	: REAL ;
		Tv	: REAL ;
		Tv_tune	: REAL ;
	END_STRUCT;
	lcpid_old_typ : STRUCT
		processPar	: lcpid_procPar_typ ;
		P_manualAdjust	: INT ;
		I_manualAdjust	: INT ;
		D_manualAdjust	: INT ;
		Y_min	: INT ;
		Y_max	: INT ;
		dY_max	: REAL ;
		Tf_Tv	: REAL ;
		Kw	: REAL ;
		Kfbk	: REAL ;
		request	: UDINT ;
		time	: UDINT ;
		W	: INT ;
		fbk_mode	: USINT ;
		d_mode	: USINT ;
		enable	: BOOL ;
	END_STRUCT;
	lcpid_stepResp_typ : STRUCT
		W_old	: INT ;
		dW	: INT ;
		x_prev	: INT ;
		x_inflectionPt	: INT ;
		t_inflectionPt	: UDINT ;
		v	: REAL ;
		v_max	: REAL ;
		t_u	: UDINT ;
		x_pre	: INT ;
		x_max	: INT ;
	END_STRUCT;
	lcpid_diff_typ : STRUCT
		init	: BOOL ;
		x_old	: INT ;
		dx	: INT ;
		dt	: UDINT ;
		dx_dt	: REAL ;
	END_STRUCT;
END_TYPE