/* Automation Studio Generated Header File, Format Version 1.00 */
/* do not change */
#ifndef LOOPCONT_H_
#define LOOPCONT_H_
#define _WEAK	__attribute__((__weak__))

#include <bur/plctypes.h>

#include <brsystem.h>


/* Constants */
_WEAK const unsigned char LCDBLPID_MODE_TUNE_4 = 6;
_WEAK const unsigned char LCDBLPID_MODE_TUNE_6 = 7;
_WEAK const unsigned char LCDBLPID_MODE_TUNE_Y1 = 8;
_WEAK const unsigned char LCDBLPID_MODE_TUNE_Y2 = 9;
_WEAK const unsigned char LCDBLPID_TSTATE_ERROR = 4;
_WEAK const unsigned char LCDBLPID_TSTATE_FINISHED = 3;
_WEAK const unsigned char LCDBLPID_TSTATE_OFF = 0;
_WEAK const unsigned char LCDBLPID_TSTATE_Y1 = 1;
_WEAK const unsigned char LCDBLPID_TSTATE_Y2 = 2;
_WEAK const unsigned short LCPIDAUTOTUNE_ADDINFO_OSC_AMPL = 100;
_WEAK const unsigned short LCPIDAUTOTUNE_ADDINFO_OSC_PERIOD = 101;
_WEAK const unsigned short LCPIDAUTOTUNE_ADDINFO_PRESCALER = 102;
_WEAK const unsigned short LCPIDAUTOTUNE_ADDINFO_Y_OFF = 103;
_WEAK const unsigned short LCPIDAUTOTUNE_ADDINFO_Y_ON = 104;
_WEAK const unsigned short LCPIDAUTOTUNE_OPT_NEW_WEIGHTING = 4;
_WEAK const unsigned char LCPIDAUTOTUNE_REQU_AUTOTUNE = 3;
_WEAK const unsigned char LCPIDAUTOTUNE_REQU_OFF = 0;
_WEAK const unsigned char LCPIDAUTOTUNE_REQU_OSCILLATE = 2;
_WEAK const unsigned char LCPIDAUTOTUNE_REQU_STEPRESPONSE = 1;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_FINISHED = 50;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_OSCILLATE = 40;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_READY = 0;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_RESET_CTRLR = 49;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_STEPRESPONSE = 20;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_WAIT_MAX = 30;
_WEAK const unsigned short LCPIDAUTOTUNE_STATE_WAIT_STEP = 10;
_WEAK const unsigned char LCPID_CALC_MODE_EXACT = 1;
_WEAK const unsigned char LCPID_CALC_MODE_FAST = 2;
_WEAK const unsigned char LCPID_D_MODE_E = 2;
_WEAK const unsigned char LCPID_D_MODE_X = 1;
_WEAK const unsigned char LCPID_FBK_MODE_EXTERN = 2;
_WEAK const unsigned char LCPID_FBK_MODE_EXT_SELECTOR = 3;
_WEAK const unsigned char LCPID_FBK_MODE_INTERN = 1;
_WEAK const unsigned char LCPID_OUT_MODE_AUTO = 1;
_WEAK const unsigned char LCPID_OUT_MODE_CLOSE = 4;
_WEAK const unsigned char LCPID_OUT_MODE_CLOSE_JOLTFREE = 104;
_WEAK const unsigned char LCPID_OUT_MODE_FREEZE = 5;
_WEAK const unsigned char LCPID_OUT_MODE_FREEZE_JOLTFREE = 105;
_WEAK const unsigned char LCPID_OUT_MODE_MAN = 2;
_WEAK const unsigned char LCPID_OUT_MODE_MAN_JOLTFREE = 102;
_WEAK const unsigned char LCPID_OUT_MODE_OFF = 0;
_WEAK const unsigned char LCPID_OUT_MODE_OPEN = 3;
_WEAK const unsigned char LCPID_OUT_MODE_OPEN_JOLTFREE = 103;
_WEAK const unsigned short LCPID_TUNE_ADDINFO_OSC_PERIOD = 111;
_WEAK const unsigned short LCPID_TUNE_ADDINFO_PRESCALER = 112;
_WEAK const unsigned long LCPID_TUNE_REQU_CHR_DIST_AP = 1000;
_WEAK const unsigned long LCPID_TUNE_REQU_CHR_DIST_OS = 2000;
_WEAK const unsigned long LCPID_TUNE_REQU_CHR_REF_AP = 4000;
_WEAK const unsigned long LCPID_TUNE_REQU_CHR_REF_OS = 5000;
_WEAK const unsigned long LCPID_TUNE_REQU_DIR_NEG = 20;
_WEAK const unsigned long LCPID_TUNE_REQU_DIR_POS = 10;
_WEAK const unsigned long LCPID_TUNE_REQU_OFF = 0;
_WEAK const unsigned long LCPID_TUNE_REQU_OSCILLATE = 1;
_WEAK const unsigned long LCPID_TUNE_REQU_OSC_1 = 10000;
_WEAK const unsigned long LCPID_TUNE_REQU_OSC_2 = 20000;
_WEAK const unsigned long LCPID_TUNE_REQU_OSC_3 = 30000;
_WEAK const unsigned long LCPID_TUNE_REQU_P = 300;
_WEAK const unsigned long LCPID_TUNE_REQU_PER_3 = 300000;
_WEAK const unsigned long LCPID_TUNE_REQU_PER_4 = 400000;
_WEAK const unsigned long LCPID_TUNE_REQU_PER_5 = 500000;
_WEAK const unsigned long LCPID_TUNE_REQU_PI = 200;
_WEAK const unsigned long LCPID_TUNE_REQU_PID = 100;
_WEAK const unsigned long LCPID_TUNE_REQU_STEPRESPONSE = 2;
_WEAK const unsigned long LCPID_TUNE_REQU_ZN_DIST = 3000;
_WEAK const unsigned short LCPID_TUNE_STATE_CALC_PARA = 45;
_WEAK const unsigned short LCPID_TUNE_STATE_FINISHED = 50;
_WEAK const unsigned short LCPID_TUNE_STATE_OSCILLATE = 10;
_WEAK const unsigned short LCPID_TUNE_STATE_OSC_CNTLPAR = 19;
_WEAK const unsigned short LCPID_TUNE_STATE_OSC_PERFORM_1 = 12;
_WEAK const unsigned short LCPID_TUNE_STATE_OSC_PERFORM_2 = 13;
_WEAK const unsigned short LCPID_TUNE_STATE_OSC_SEQ_CNTL = 11;
_WEAK const unsigned short LCPID_TUNE_STATE_OSC_WAIT_NEXT = 14;
_WEAK const unsigned short LCPID_TUNE_STATE_READY = 0;
_WEAK const unsigned short LCPID_TUNE_STATE_RESET_CNTL = 49;
_WEAK const unsigned short LCPID_TUNE_STATE_STEP_MAXGRAD = 21;
_WEAK const unsigned short LCPID_TUNE_STATE_STEP_WAITEQ1 = 20;
_WEAK const unsigned short LCPID_TUNE_STATE_STEP_WAITEQ2 = 22;
_WEAK const unsigned long LCSLIMPID_REQU_CHR_DIST_AP = 1000;
_WEAK const unsigned long LCSLIMPID_REQU_CHR_DIST_OS = 2000;
_WEAK const unsigned long LCSLIMPID_REQU_CHR_REF_AP = 4000;
_WEAK const unsigned long LCSLIMPID_REQU_CHR_REF_OS = 5000;
_WEAK const unsigned long LCSLIMPID_REQU_DIR_NEG = 20;
_WEAK const unsigned long LCSLIMPID_REQU_DIR_POS = 10;
_WEAK const unsigned long LCSLIMPID_REQU_OFF = 0;
_WEAK const unsigned long LCSLIMPID_REQU_OSCILLATE = 1;
_WEAK const unsigned long LCSLIMPID_REQU_OSC_1 = 10000;
_WEAK const unsigned long LCSLIMPID_REQU_OSC_2 = 20000;
_WEAK const unsigned long LCSLIMPID_REQU_OSC_3 = 30000;
_WEAK const unsigned long LCSLIMPID_REQU_P = 300;
_WEAK const unsigned long LCSLIMPID_REQU_PER_3 = 300000;
_WEAK const unsigned long LCSLIMPID_REQU_PER_4 = 400000;
_WEAK const unsigned long LCSLIMPID_REQU_PER_5 = 500000;
_WEAK const unsigned long LCSLIMPID_REQU_PI = 200;
_WEAK const unsigned long LCSLIMPID_REQU_PID = 100;
_WEAK const unsigned long LCSLIMPID_REQU_READ_PARAS = 3;
_WEAK const unsigned long LCSLIMPID_REQU_STEPRESPONSE = 2;
_WEAK const unsigned long LCSLIMPID_REQU_WRITE_PARAS = 4;
_WEAK const unsigned long LCSLIMPID_REQU_ZN_DIST = 3000;
_WEAK const unsigned short LC_ERROR = 12025;
_WEAK const unsigned short LC_ERR_DISABLED = 65534;
_WEAK const unsigned short LC_ERR_LC3PHy_XSETVALUE = 31565;
_WEAK const unsigned short LC_ERR_LCContinServo_TCHANGE = 12035;
_WEAK const unsigned short LC_ERR_LCContinServo_TIMPULSE = 12036;
_WEAK const unsigned short LC_ERR_LCCurveByPoints_TABLE = 12045;
_WEAK const unsigned short LC_ERR_LCDBLPID_DX_DT = 31562;
_WEAK const unsigned short LC_ERR_LCDBLPID_WX_LOW = 31561;
_WEAK const unsigned short LC_ERR_LCIntegrate_TN = 31550;
_WEAK const unsigned short LC_ERR_LCMovAvFilter_BASE = 31551;
_WEAK const unsigned short LC_ERR_LCPIDAutotune_ABORT = 12038;
_WEAK const unsigned short LC_ERR_LCPIDAutotune_PARAMETER = 12039;
_WEAK const unsigned short LC_ERR_LCPIDAutotune_TIMEOUT = 12040;
_WEAK const unsigned short LC_ERR_LCPIDAutotune_W_CHANGED = 12041;
_WEAK const unsigned short LC_ERR_LCPIDTune_ABORT = 12038;
_WEAK const unsigned short LC_ERR_LCPIDTune_BASETIME = 31566;
_WEAK const unsigned short LC_ERR_LCPIDTune_CONTROL_VAR = 31560;
_WEAK const unsigned short LC_ERR_LCPIDTune_INVALID_REQUEST = 31559;
_WEAK const unsigned short LC_ERR_LCPIDTune_MAXMIN = 31564;
_WEAK const unsigned short LC_ERR_LCPIDTune_PARAMETER = 12039;
_WEAK const unsigned short LC_ERR_LCPIDTune_TIMEOUT = 12040;
_WEAK const unsigned short LC_ERR_LCPIDTune_W_CHANGED = 12041;
_WEAK const unsigned short LC_ERR_LCPID_IDENT = 31552;
_WEAK const unsigned short LC_ERR_LCPID_OUTMODE = 31553;
_WEAK const unsigned short LC_ERR_LCPID_PARADAT = 31554;
_WEAK const unsigned short LC_ERR_LCPID_PAR_CALCMODE = 31555;
_WEAK const unsigned short LC_ERR_LCPID_PAR_DMODE = 31556;
_WEAK const unsigned short LC_ERR_LCPID_PAR_DYMAX = 31557;
_WEAK const unsigned short LC_ERR_LCPID_PAR_FBKMODE = 31558;
_WEAK const unsigned short LC_ERR_LCPID_PAR_KFBK = 12025;
_WEAK const unsigned short LC_ERR_LCPID_PAR_KP = 12026;
_WEAK const unsigned short LC_ERR_LCPID_PAR_KW = 12027;
_WEAK const unsigned short LC_ERR_LCPID_PAR_PRESCALER = 12042;
_WEAK const unsigned short LC_ERR_LCPID_PAR_TF = 12028;
_WEAK const unsigned short LC_ERR_LCPID_PAR_TN = 12029;
_WEAK const unsigned short LC_ERR_LCPID_PAR_TV = 12030;
_WEAK const unsigned short LC_ERR_LCPT1_T = 12031;
_WEAK const unsigned short LC_ERR_LCPWM_DMod_TMINPULS = 12032;
_WEAK const unsigned short LC_ERR_LCPWM_DMod_TPERIOD = 12033;
_WEAK const unsigned short LC_ERR_LCPWM_TMINPULS = 12032;
_WEAK const unsigned short LC_ERR_LCPWM_TPERIOD = 12033;
_WEAK const unsigned short LC_ERR_LCRampLim_TUP_TDOWN = 12034;
_WEAK const unsigned short LC_ERR_LCTimBasOnOff_TCHANGE = 12035;
_WEAK const unsigned short LC_ERR_LCTimBasOnOff_TIMPULSE = 12036;
_WEAK const unsigned short LC_ERR_MAXMIN = 12037;
_WEAK const unsigned short LC_ERR_POINTER = 31563;
_WEAK const unsigned short LC_WARN_DT_MAXLIMIT = 12000;
_WEAK const unsigned short LC_WARN_LCDBLPID_TUNE_CHANGE = 12019;
_WEAK const unsigned short LC_WARN_LCPIDTune_CYCLETIME = 12004;
_WEAK const unsigned short LC_WARN_LCPIDTune_OSC_HYST = 12020;
_WEAK const unsigned short LC_WARN_LCPID_I_MAXLIMIT = 12001;
_WEAK const unsigned short LC_WARN_LCPID_I_MINLIMIT = 12002;
_WEAK const unsigned short LC_WARN_LCPID_YFBK_LIMIT = 12003;
_WEAK const unsigned short LC_WARN_LCxPHy_XSTART = 12006;
_WEAK const unsigned short LC_WARN_Tx_DT = 12007;


/* Datatypes */
typedef struct lcpid_diff_typ
{
	plcbit init;
	signed short x_old;
	signed short dx;
	unsigned long dt;
	float dx_dt;
} lcpid_diff_typ;

typedef struct lcpid_stepResp_typ
{
	signed short W_old;
	signed short dW;
	signed short x_prev;
	signed short x_inflectionPt;
	unsigned long t_inflectionPt;
	float v;
	float v_max;
	unsigned long t_u;
	signed short x_pre;
	signed short x_max;
} lcpid_stepResp_typ;

typedef struct lcpid_procPar_typ
{
	unsigned short size;
	plcbit valid;
	plcbit stepResp_valid;
	float stepResp_v;
	unsigned long stepResp_t_u;
	unsigned long stepResp_t_g;
	plcbit osc_valid;
	float osc_amplitudeRatio;
	unsigned long osc_tPeriod;
	plcbit force_params;
	float Kp;
	float Kp_tune;
	float Tn;
	float Tn_tune;
	float Tv;
	float Tv_tune;
} lcpid_procPar_typ;

typedef struct lcpid_old_typ
{
	lcpid_procPar_typ processPar;
	signed short P_manualAdjust;
	signed short I_manualAdjust;
	signed short D_manualAdjust;
	signed short Y_min;
	signed short Y_max;
	float dY_max;
	float Tf_Tv;
	float Kw;
	float Kfbk;
	unsigned long request;
	unsigned long time;
	signed short W;
	unsigned char fbk_mode;
	unsigned char d_mode;
	plcbit enable;
} lcpid_old_typ;

typedef struct lcpid_osc_val_typ
{
	signed short X;
	unsigned long t;
} lcpid_osc_val_typ;

typedef struct lcpid_osc_per_typ
{
	lcpid_osc_val_typ max;
	lcpid_osc_val_typ min;
} lcpid_osc_per_typ;

typedef struct lcpid_osc_typ
{
	unsigned short i_maxPeriod;
	unsigned short i_period;
	signed long X_amplitude;
	float amplitudeRatio;
	unsigned long tPeriod;
	lcpid_osc_per_typ period[5];
} lcpid_osc_typ;

typedef struct lcpid_addPar_typ
{
	signed short y_autotune_on;
	signed short y_autotune_off;
	unsigned long t_autotune_max;
	signed short diff_dx_max;
	unsigned long diff_dt_max;
	unsigned short osc_nrPeriods;
	signed short osc_minAmplitude;
	unsigned short optSwitches;
	float dY_max;
	float Tf_Tv;
	float Kw;
	float Kfbk;
	unsigned char fbk_mode;
	unsigned char d_mode;
	unsigned char calc_mode;
} lcpid_addPar_typ;

typedef struct lcpid_internal_typ
{
	unsigned short size;
	plcbit valid;
	lcpid_addPar_typ autotune_addpar;
	signed short Y_max;
	signed short Y_min;
	signed long dY_max1;
	signed long Kp1;
	signed long Kp_Tn_1;
	signed long Tv_Tf_1;
	signed long Tf_reciproc_1;
	signed long Kw1;
	signed long Kfbk_Kp_1;
	unsigned char fbk_mode;
	unsigned char d_mode;
	unsigned char calc_mode;
	unsigned short prescalerPow2;
	unsigned char force_outmode;
	signed short Y_force;
	signed short W;
	signed short X;
	unsigned char pid_init;
	unsigned long dt;
	signed long Yp1;
	signed long Yp2;
	signed long dYi1;
	signed long dYi2;
	signed long Yi1;
	signed long Yi2;
	signed long dt_Tf_1;
	signed long a11;
	signed long a12;
	signed long a21;
	signed long a22;
	signed long Yd1;
	signed long Yd2;
	signed long Ytotal1;
	signed long Ytotal2;
	signed long Ylim1;
	signed long Ylim2;
	signed long Y1;
	signed long Y2;
	signed long e_fbk;
} lcpid_internal_typ;

typedef struct lcCurveByPoints_TabEntry_type
{
	signed short x;
	signed short y;
} lcCurveByPoints_TabEntry_type;

typedef struct lcdblpid_par_typ
{
	signed short Y_max;
	signed short Y_min;
	signed short K_fact;
	float Kp;
	float Tn;
	float Tv;
} lcdblpid_par_typ;

typedef struct lcpid_tune_osc_options_typ
{
	signed short osc_minAmplitude;
	float Q_min;
} lcpid_tune_osc_options_typ;

typedef struct lcpid_tune_step_options_typ
{
	signed short eqDeltaX;
	signed short eqDeltaWX;
	unsigned long eqDeltat;
	signed short evalDeltaX;
	unsigned short evalNfilter;
	unsigned short exitNotMaxdXCount;
	unsigned short exitdXRatio;
} lcpid_tune_step_options_typ;

typedef struct lcdblpid_tune_typ
{
	signed short P1_manualAdjust;
	signed short I1_manualAdjust;
	signed short D1_manualAdjust;
	signed short P2_manualAdjust;
	signed short I2_manualAdjust;
	signed short D2_manualAdjust;
	signed short X_min;
	signed short X_max;
	signed short X0;
	signed short de_min;
	signed short Y0;
	signed short Y1step;
	signed short Y2step;
	unsigned long tuneY1_opt;
	unsigned long tuneY2_opt;
	unsigned char tune_first;
	signed short hyst;
	signed short measDelta;
	lcpid_tune_osc_options_typ osc_opt;
	lcpid_tune_step_options_typ step_opt;
} lcdblpid_tune_typ;

typedef struct lcpid_tune_step_typ
{
	lcpid_tune_step_options_typ options;
	signed short Y0;
	signed short Y1;
	unsigned long t_jump;
	signed short W;
	signed short X0;
	signed short Xmax;
	signed short Xmin;
	signed long Xfiltered;
	plcbit deltaXok;
	plcbit deltaWXok;
	unsigned long eqTime;
	signed char dir;
	signed short deltaX;
	signed short X1;
	unsigned long t1;
	plcbit t1set;
	float dX_dt;
	float dX_dt_max;
	signed short XmaxGrad;
	signed short maxX1;
	signed short maxX2;
	unsigned long maxt1;
	unsigned long maxt2;
	unsigned long tmaxGrad;
	float gainFactor;
	unsigned long deadTime;
	unsigned long riseTime;
	unsigned short notMaxCount;
	unsigned short gradientRatio;
	unsigned long cycleTime;
} lcpid_tune_step_typ;

typedef struct lcpid_tune_osc_typ
{
	lcpid_tune_osc_options_typ options;
	unsigned short oscPhase;
	signed char dir;
	plcbit enHi;
	plcbit enLo;
	signed short Y_hi;
	signed short Y_lo;
	unsigned short i_maxPeriod;
	unsigned short i_period;
	signed long X_ampl;
	signed long X_avg;
	signed long X_min_avg;
	signed long X_max_avg;
	signed long Y_avg;
	signed long deltaY;
	unsigned long t_Ylohi[5];
	unsigned long t_Yhilo[5];
	float Q_act;
	signed short a_wx;
	float amplitudeRatio;
	float Ku;
	unsigned long tPeriod;
	lcpid_osc_per_typ period[5];
} lcpid_tune_osc_typ;

typedef struct lcpid_tune_addpar_typ
{
	unsigned long t_max_tune;
	float dY_max;
	float Tf_Tv;
	float Kw;
	float Kfbk;
	unsigned char fbk_mode;
	unsigned char d_mode;
	unsigned char calc_mode;
} lcpid_tune_addpar_typ;

typedef struct lcslimpid_par_typ
{
	signed short Y_max;
	signed short Y_min;
	float Kp;
	float Tn;
	float Tv;
	float Kfbk;
} lcslimpid_par_typ;



/* Datatypes of function blocks */
typedef struct LCScINT
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short x1;
	signed short y1;
	signed short x2;
	signed short y2;
	/* VAR_OUTPUT (analogous) */
	signed short y;
} LCScINT_typ;

typedef struct LCScDINT
{
	/* VAR_INPUT (analogous) */
	signed long x;
	signed long x1;
	signed long y1;
	signed long x2;
	signed long y2;
	/* VAR_OUTPUT (analogous) */
	signed long y;
} LCScDINT_typ;

typedef struct LCPT1
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short t;
	signed short yset;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long rest;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit set;
	/* VAR (digital) */
	plcbit enable_old;
} LCPT1_typ;

typedef struct LCPosPeakVal
{
	/* VAR_INPUT (analogous) */
	signed short in;
	/* VAR_OUTPUT (analogous) */
	signed short out;
	/* VAR_INPUT (digital) */
	plcbit reset;
} LCPosPeakVal_typ;

typedef struct LCNegPeakVal
{
	/* VAR_INPUT (analogous) */
	signed short in;
	/* VAR_OUTPUT (analogous) */
	signed short out;
	/* VAR_INPUT (digital) */
	plcbit reset;
} LCNegPeakVal_typ;

typedef struct LCNegINT
{
	/* VAR_INPUT (analogous) */
	signed short in;
	/* VAR_OUTPUT (analogous) */
	signed short out;
} LCNegINT_typ;

typedef struct LCNegDINT
{
	/* VAR_INPUT (analogous) */
	signed long in;
	/* VAR_OUTPUT (analogous) */
	signed long out;
} LCNegDINT_typ;

typedef struct LCMovAvFilter
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short base;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	signed long sum_old;
	unsigned long x_old_ptr;
	signed short x_old[32];
	unsigned short xIdx;
	unsigned short base_old;
	unsigned long bootkey_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR (digital) */
	plcbit enable_old;
} LCMovAvFilter_typ;

typedef struct LCLiScUINT
{
	/* VAR_INPUT (analogous) */
	unsigned short x;
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
	/* VAR_OUTPUT (analogous) */
	unsigned short y;
} LCLiScUINT_typ;

typedef struct LCLiScINT
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short x1;
	signed short y1;
	signed short x2;
	signed short y2;
	/* VAR_OUTPUT (analogous) */
	signed short y;
} LCLiScINT_typ;

typedef struct LCLiScDINT
{
	/* VAR_INPUT (analogous) */
	signed long x;
	signed long x1;
	signed long y1;
	signed long x2;
	signed long y2;
	/* VAR_OUTPUT (analogous) */
	signed long y;
} LCLiScDINT_typ;

typedef struct LCLimit
{
	/* VAR_INPUT (analogous) */
	signed long in;
	signed short max_value;
	signed short min_value;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short out;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit max_limit;
	plcbit min_limit;
} LCLimit_typ;

typedef struct LCIntegrate
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short tn;
	signed short max_value;
	signed short min_value;
	signed short yset;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long rest;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit set;
	/* VAR_OUTPUT (digital) */
	plcbit max_limit;
	plcbit min_limit;
	/* VAR (digital) */
	plcbit enable_old;
} LCIntegrate_typ;

typedef struct LCCounter
{
	/* VAR_OUTPUT (analogous) */
	unsigned long muscnt;
	unsigned long mus100cnt;
	unsigned long mscnt;
	unsigned long ms100cnt;
	unsigned long seccnt;
	/* VAR (analogous) */
	unsigned short state;
	struct SysInfo sysinf_inst;
	unsigned short musecs_old;
	unsigned long bootkey_old;
	unsigned long sys_ticks_old;
	unsigned long to_100us;
	unsigned long to_1ms;
	unsigned short to_100ms;
	unsigned short to_1s;
} LCCounter_typ;

typedef struct LCComparator
{
	/* VAR_INPUT (analogous) */
	signed short xp;
	signed short xn;
	unsigned short sd;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit greater;
	plcbit equal;
	plcbit lower;
} LCComparator_typ;

typedef struct LC3PHy
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short xstart;
	signed short xsetvalue1;
	signed short xsetvalue2;
	unsigned short hysteresis1;
	unsigned short hysteresis2;
	signed short ymax;
	signed short ymid;
	signed short ymin;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	signed short x_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit ymx;
	plcbit ymn;
	/* VAR (digital) */
	plcbit enable_old;
} LC3PHy_typ;

typedef struct LCScUINT
{
	/* VAR_INPUT (analogous) */
	unsigned short x;
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
	/* VAR_OUTPUT (analogous) */
	unsigned short y;
} LCScUINT_typ;

typedef struct LC2PHy
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short xstart;
	signed short xsetvalue;
	unsigned short hysteresis;
	signed short ymax;
	signed short ymin;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	signed short x_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR (digital) */
	plcbit y1;
	plcbit enable_old;
} LC2PHy_typ;

typedef struct LCPIDAutoTune
{
	/* VAR_INPUT (analogous) */
	signed short Y_max;
	signed short Y_min;
	signed short P_manualAdjust;
	signed short I_manualAdjust;
	signed short D_manualAdjust;
	unsigned char request;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	unsigned long ident;
	unsigned short addInfo;
	unsigned short state;
	/* VAR (analogous) */
	unsigned long dt;
	unsigned long t_autotune;
	struct lcpid_diff_typ diff;
	struct lcpid_old_typ old;
	struct lcpid_stepResp_typ stepResponse;
	struct lcpid_osc_typ oscillation;
	struct lcpid_procPar_typ processPar;
	struct lcpid_internal_typ internal_data;
	/* VAR_INPUT (digital) */
	plcbit enable;
} LCPIDAutoTune_typ;

typedef struct LCDifferentiate
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short tv;
	unsigned short tf;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long a2n;
	signed long a1;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR (digital) */
	plcbit enable_old;
} LCDifferentiate_typ;

typedef struct LCTimeBasedOnOff
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short max_value;
	signed short min_value;
	unsigned short timpulse;
	unsigned short tchange_up;
	unsigned short tchange_dwn;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	unsigned short hysteresis_up;
	unsigned short hysteresis_dwn;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long t_cnt;
	signed long y_fix31_1;
	signed long y_fix31_2;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit ref;
	/* VAR_OUTPUT (digital) */
	plcbit up;
	plcbit down;
	plcbit refOK;
	/* VAR (digital) */
	plcbit enable_old;
	plcbit ref_old;
} LCTimeBasedOnOff_typ;

typedef struct LCPID
{
	/* VAR_INPUT (analogous) */
	unsigned long ident;
	signed short W;
	signed short X;
	signed short A;
	signed short Y_man;
	signed short Y_fbk;
	unsigned char out_mode;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed long e;
	signed short Y;
	signed long Yp;
	signed long Yi;
	signed long Yd;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long Tv_Tf_old;
	signed long e_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit hold_I;
	/* VAR (digital) */
	plcbit enable_old;
} LCPID_typ;

typedef struct LCRampLim
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short tup;
	unsigned short tdown;
	signed short max_value;
	signed short min_value;
	signed short yset;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	unsigned long time_old;
	unsigned short rest;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit set;
	/* VAR_OUTPUT (digital) */
	plcbit xreached;
	plcbit max_limit;
	plcbit min_limit;
	/* VAR (digital) */
	plcbit enable_old;
} LCRampLim_typ;

typedef struct LCPIDpara
{
	/* VAR_INPUT (analogous) */
	signed short Y_max;
	signed short Y_min;
	float dY_max;
	float Kp;
	float Tn;
	float Tv;
	float Tf;
	float Kw;
	float Kfbk;
	unsigned char fbk_mode;
	unsigned char d_mode;
	unsigned char calc_mode;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	unsigned long ident;
	/* VAR (analogous) */
	struct lcpid_internal_typ internal_data;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit enter;
} LCPIDpara_typ;

typedef struct LCPWM_DMod
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short max_value;
	signed short min_value;
	unsigned short tminpuls;
	unsigned short tperiod;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	/* VAR (analogous) */
	signed long area;
	unsigned short period;
	unsigned long time_old;
	unsigned short last;
	signed short x_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit y;
	/* VAR (digital) */
	plcbit enable_old;
} LCPWM_DMod_typ;

typedef struct LCCurveByPoints
{
	/* VAR_INPUT (analogous) */
	signed short x;
	unsigned short NoOfPoints;
	struct lcCurveByPoints_TabEntry_type* ptr_table;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	signed short y;
	/* VAR (analogous) */
	signed short i_tab;
	unsigned short iterations;
	struct LCScINT scal1;
} LCCurveByPoints_typ;

typedef struct LCPFM
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short max_value;
	signed short min_value;
	unsigned short t_pulse;
	unsigned short t_pause;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	/* VAR (analogous) */
	signed short range;
	signed long area;
	signed long area_c;
	unsigned long time_old;
	signed long last;
	signed long tpls;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit pulse;
	/* VAR (digital) */
	plcbit enable_old;
} LCPFM_typ;

typedef struct LCPIDTune
{
	/* VAR_INPUT (analogous) */
	signed short Y_min;
	signed short Y_max;
	signed short Y0;
	signed short Y1;
	signed short X0;
	signed short X_min;
	signed short X_max;
	signed short P_manualAdjust;
	signed short I_manualAdjust;
	signed short D_manualAdjust;
	unsigned long request;
	unsigned long basetime;
	struct lcpid_tune_addpar_typ* pAddPar;
	struct lcpid_tune_osc_options_typ* pOptions_osc;
	struct lcpid_tune_step_options_typ* pOptions_step;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	unsigned short addInfo;
	unsigned long ident;
	unsigned short state;
	/* VAR (analogous) */
	unsigned long dt;
	unsigned long t_autotune;
	unsigned short status_tmp;
	struct LCMovAvFilter MovAvgFlt_inst;
	struct lcpid_old_typ old;
	struct lcpid_procPar_typ processPar;
	struct lcpid_internal_typ internal_data;
	struct lcpid_tune_addpar_typ addPar;
	struct lcpid_tune_osc_typ oscillation;
	struct lcpid_tune_step_typ stepresponse;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit okToStep;
	/* VAR_OUTPUT (digital) */
	plcbit rdyToStep;
} LCPIDTune_typ;

typedef struct LCDblActPID
{
	/* VAR_INPUT (analogous) */
	signed short W;
	signed short X;
	signed short Y_man;
	unsigned char mode;
	unsigned long basetime;
	struct lcdblpid_par_typ* pPar1;
	struct lcdblpid_par_typ* pPar2;
	struct lcpid_tune_addpar_typ* pAddPar;
	struct lcdblpid_tune_typ* pOpt;
	/* VAR_OUTPUT (analogous) */
	signed long e;
	signed short Y1;
	signed short Y2;
	unsigned short status;
	unsigned char tuneState;
	/* VAR (analogous) */
	struct LCPID pid;
	struct LCPIDTune tune;
	float gradient;
	signed long Y_avg;
	unsigned long tcnt;
	signed short Tmeas;
	signed short y_lim_old;
	unsigned char use_par;
	unsigned char meas;
	unsigned char mode_old;
	unsigned char tune_step;
	unsigned char tune_grad;
	unsigned char tune_2nd;
	unsigned char invert;
	unsigned char mode_int;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit hold_I;
	plcbit okToStep;
	/* VAR_OUTPUT (digital) */
	plcbit rdyToStep;
	/* VAR (digital) */
	plcbit enable_old;
} LCDblActPID_typ;

typedef struct LCMinMax
{
	/* VAR_INPUT (analogous) */
	signed short in;
	/* VAR_OUTPUT (analogous) */
	signed short out_min;
	signed short out_max;
	/* VAR_INPUT (digital) */
	plcbit reset;
} LCMinMax_typ;

typedef struct LCContinServo
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short max_value;
	signed short min_value;
	unsigned short t_impulse;
	unsigned short t_change_up;
	unsigned short t_change_down;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	unsigned short hysteresis_up;
	unsigned short hysteresis_down;
	/* VAR (analogous) */
	unsigned long time_old;
	signed long t_cnt;
	signed long y_fix31_1;
	signed long y_fix31_2;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit ref;
	/* VAR_OUTPUT (digital) */
	plcbit up;
	plcbit down;
	plcbit refOk;
	/* VAR (digital) */
	plcbit enable_old;
	plcbit ref_old;
} LCContinServo_typ;

typedef struct LCPWM
{
	/* VAR_INPUT (analogous) */
	signed short x;
	signed short max_value;
	signed short min_value;
	unsigned short t_min_pulse;
	unsigned short t_period;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	unsigned short status;
	/* VAR (analogous) */
	signed long area;
	unsigned short period;
	unsigned long time_old;
	unsigned short last;
	signed short x_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit pulse;
	/* VAR (digital) */
	plcbit enable_old;
} LCPWM_typ;

typedef struct LCSlimPID
{
	/* VAR_INPUT (analogous) */
	signed short W;
	signed short X;
	unsigned long request;
	struct lcslimpid_par_typ* pPar;
	unsigned long basetime;
	/* VAR_OUTPUT (analogous) */
	signed long e;
	signed short Y;
	unsigned short status;
	unsigned short addInfo;
	/* VAR (analogous) */
	struct LCMovAvFilter MovAvgFlt_inst;
	struct LCPIDTune PIDTune_inst;
	struct LCPID PID_inst;
	unsigned long requ_old;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR (digital) */
	plcbit enable_old;
} LCSlimPID_typ;



/* Prototyping of functions and function blocks */
void LCScINT(LCScINT_typ* inst);
void LCScDINT(LCScDINT_typ* inst);
void LCPT1(LCPT1_typ* inst);
void LCPosPeakVal(LCPosPeakVal_typ* inst);
void LCNegPeakVal(LCNegPeakVal_typ* inst);
void LCNegINT(LCNegINT_typ* inst);
void LCNegDINT(LCNegDINT_typ* inst);
void LCMovAvFilter(LCMovAvFilter_typ* inst);
void LCLiScUINT(LCLiScUINT_typ* inst);
void LCLiScINT(LCLiScINT_typ* inst);
void LCLiScDINT(LCLiScDINT_typ* inst);
void LCLimit(LCLimit_typ* inst);
void LCIntegrate(LCIntegrate_typ* inst);
void LCCounter(LCCounter_typ* inst);
void LCComparator(LCComparator_typ* inst);
void LC3PHy(LC3PHy_typ* inst);
void LCScUINT(LCScUINT_typ* inst);
void LC2PHy(LC2PHy_typ* inst);
void LCPIDAutoTune(LCPIDAutoTune_typ* inst);
void LCDifferentiate(LCDifferentiate_typ* inst);
void LCTimeBasedOnOff(LCTimeBasedOnOff_typ* inst);
void LCPID(LCPID_typ* inst);
void LCRampLim(LCRampLim_typ* inst);
void LCPIDpara(LCPIDpara_typ* inst);
void LCPWM_DMod(LCPWM_DMod_typ* inst);
void LCCurveByPoints(LCCurveByPoints_typ* inst);
void LCPFM(LCPFM_typ* inst);
void LCDblActPID(LCDblActPID_typ* inst);
void LCPIDTune(LCPIDTune_typ* inst);
void LCMinMax(LCMinMax_typ* inst);
void LCContinServo(LCContinServo_typ* inst);
void LCPWM(LCPWM_typ* inst);
void LCSlimPID(LCSlimPID_typ* inst);



#endif /* LOOPCONT_H_ */

