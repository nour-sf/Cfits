/**
 * @file    AOloopStruct.h
 * @brief   Adaptive Optics Control defines
 * 
 * AO engine uses stream data structure
 *  
 * @author  O. Guyon
 * @date    12 Oct 2017 -- 
 *
 * @bug No known bugs.
 * 
 * @see http://oguyon.github.io/AdaptiveOpticsControl/src/AOloopControl/doc/AOloopControl.html
 *  
 * @defgroup AOloopControl_streams Image streams
 * @defgroup AOloopControl_AOLOOPCONTROL_CONF AOloopControl main data structure
 * 
 */


#define _GNU_SOURCE



#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h> // needed for tid = syscall(SYS_gettid);



#ifdef __MACH__   // for Mac OS X - 
#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int clk_id, struct mach_timespec *t) {
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t time;
    time = mach_absolute_time();
    double nseconds = ((double)time * (double)timebase.numer)/((double)timebase.denom);
    double seconds = ((double)time * (double)timebase.numer)/((double)timebase.denom * 1e9);
    t->tv_sec = seconds;
    t->tv_nsec = nseconds;
    return 0;
}
#else
#include <time.h>
#endif

#include <math.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <sched.h>
//#include <ncurses.h>
#include <semaphore.h>


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <pthread.h>

#include <fitsio.h>


//libraries created by O. Guyon 
#include "CLIcore.h"
#include "00CORE/00CORE.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_tools/COREMOD_tools.h"
#include "COREMOD_arith/COREMOD_arith.h"
#include "linopt_imtools/linopt_imtools.h"
#include "AOloopControl/AOloopControl.h"
#include "image_filter/image_filter.h"
#include "info/info.h"
#include "ZernikePolyn/ZernikePolyn.h"
#include "linopt_imtools/linopt_imtools.h"
#include "image_gen/image_gen.h"
#include "statistic/statistic.h"
#include "fft/fft.h"


#include "AOloopControl_IOtools/AOloopControl_IOtools.h"
#include "AOloopControl_PredictiveControl/AOloopControl_PredictiveControl.h"
#include "AOloopControl_acquireCalib/AOloopControl_acquireCalib.h"
#include "AOloopControl_computeCalib/AOloopControl_computeCalib.h"
#include "AOloopControl_perfTest/AOloopControl_perfTest.h"



#ifdef HAVE_CUDA
#include "cudacomp/cudacomp.h"
#endif

# ifdef _OPENMP
# include <omp.h>
#define OMP_NELEMENT_LIMIT 1000000
# endif



/* =============================================================================================== */
/* =============================================================================================== */
/*                                      DEFINES, MACROS                                            */
/* =============================================================================================== */
/* =============================================================================================== */




#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

// data passed to each thread
//typedef struct
//{
//   long nelem;
//    float *arrayptr;
//    float *result; // where to white status
//} THDATA_IMTOTAL;


/* =============================================================================================== */
/* =============================================================================================== */
/*                                  GLOBAL DATA DECLARATION                                        */
/* =============================================================================================== */
/* =============================================================================================== */




/* =============================================================================================== */
/*                				    LOGGING ACCESS TO FUNCTIONS           	                       */
/* =============================================================================================== */

// uncomment at compilation time to enable logging of function entry/exit
//#define AOLOOPCONTROL_LOGFUNC
static int AOLOOPCONTROL_logfunc_level = 0;
static int AOLOOPCONTROL_logfunc_level_max = 2; // log all levels equal or below this number
static char AOLOOPCONTROL_logfunc_fname[] = "AOloopControl.fcall.log";
static char flogcomment[200];


// GPU MultMat indexes
//
// 0: main loop CM multiplication
//
// 1: set DM modes:
//         int set_DM_modes(long loop)
//
// 2: compute modes loop
//         int AOloopControl_CompModes_loop(char *ID_CM_name, char *ID_WFSref_name, char *ID_WFSim_name, char *ID_WFSimtot_name, char *ID_coeff_name)
//
// 3: coefficients to DM shape
//         int AOloopControl_GPUmodecoeffs2dm_filt_loop(const int GPUMATMULTCONFindex, char *modecoeffs_name, char *DMmodes_name, int semTrigg, char *out_name, int GPUindex, long loop, int offloadMode)
//
// 4: Predictive control (in modules linARfilterPred)
//
// 5: coefficients to DM shape, PF
// 			int AOloopControl_GPUmodecoeffs2dm_filt_loop(const int GPUMATMULTCONFindex, char *modecoeffs_name, char *DMmodes_name, int semTrigg, char *out_name, int GPUindex, long loop, int offloadMode)
//
// 6: coefficients to DM shape, OL
//			int AOloopControl_GPUmodecoeffs2dm_filt_loop(const int GPUMATMULTCONFindex, char *modecoeffs_name, char *DMmodes_name, int semTrigg, char *out_name, int GPUindex, long loop, int offloadMode)
//




// TIMING
static struct timespec tnow;
static struct timespec tdiff;
static double tdiffv;



static int initWFSref_GPU[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int initcontrMcact_GPU[100] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float GPU_alpha = 0.0;
float GPU_beta = 0.0;


static int COMPUTE_PIXELSTREAMING = 0; // multiple pixel groups
int PIXSTREAM_NBSLICES = 1; // number of image slices (= pixel groups)
int PIXSTREAM_SLICE; // slice index 0 = all pixels




// static int MATRIX_COMPUTATION_MODE = 0;
// 0: compute sequentially modes and DM commands
// 1: use combined control matrix





/* =============================================================================================== */
/*                    aoconfID are global variables for convenience                                */
/*  aoconfID can be used in other modules as well (with extern)                                   */
/* =============================================================================================== */


// Hardware connections
long aoconfID_wfsim = -1;
uint8_t WFSatype;
long aoconfID_dmC = -1;
long aoconfID_dmRM = -1;

long aoconfID_wfsdark = -1;
long aoconfID_imWFS0 = -1;
long aoconfID_imWFS0tot = -1;
long aoconfID_imWFS1 = -1;
long aoconfID_imWFS2 = -1;
static long aoconfID_wfsref0 = -1;
long aoconfID_wfsref = -1;
static long long aoconfcnt0_wfsref_current = -1;

long aoconfID_DMmodes = -1;
static long aoconfID_dmdisp = -1;  // to notify DMcomb that DM maps should be summed



// Control Modes
long aoconfID_cmd_modes = -1;
long aoconfID_meas_modes = -1; // measured
long aoconfID_RMS_modes = -1;
long aoconfID_AVE_modes = -1;
long aoconfID_modeARPFgainAuto = -1;
long aoconfID_modevalPF = -1;

// mode gains, multf, limit are set in 3 tiers
// global gain
// block gain
// individual gains

// blocks
long aoconfID_gainb = -1; // block modal gains
long aoconfID_multfb = -1; // block modal gains
long aoconfID_limitb = -1; // block modal gains

// individual modes
long aoconfID_DMmode_GAIN = -1;
long aoconfID_LIMIT_modes = -1;
long aoconfID_MULTF_modes = -1;

long aoconfID_cmd_modesRM = -1;

long aoconfID_wfsmask = -1;
static long aoconfID_dmmask = -1;

static long aoconfID_respM = -1;
static long aoconfID_contrM = -1; // pixels -> modes
static long long aoconfcnt0_contrM_current = -1;
static long aoconfID_contrMc = -1; // combined control matrix: pixels -> DM actuators
static long aoconfID_meas_act = -1;
static long aoconfID_contrMcact[100] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// pixel streaming
long aoconfID_pixstream_wfspixindex; // index of WFS pixels

// timing
long aoconfID_looptiming = -1; // control loop timing data. Pixel values correspond to time offset
// currently has 20 timing slots
// beginning of iteration is defined when entering "wait for image"
// md[0].atime.ts is absolute time at beginning of iteration
//
// pixel 0 is dt since last iteration
//
// pixel 1 is time from beginning of loop to status 01
// pixel 2 is time from beginning of loop to status 02
// ...
long AOcontrolNBtimers = 35;

static long aoconfIDlogdata = -1;
static long aoconfIDlog0 = -1;
static long aoconfIDlog1 = -1;



int *WFS_active_map; // used to map WFS pixels into active array
int *DM_active_map; // used to map DM actuators into active array
static long aoconfID_meas_act_active;
static long aoconfID_imWFS2_active[100];

float normfloorcoeff = 1.0;

static long wfsrefcnt0 = -1;
static long contrMcactcnt0[100] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};;


// variables used by functions

static int GPUcntMax = 100;
static int *GPUset0;
static int *GPUset1;



/* =============================================================================================== */
/*                                     MAIN DATA STRUCTURES                                        */
/* =============================================================================================== */

extern DATA data;

#define NB_AOloopcontrol 10 // max number of loops
long LOOPNUMBER = 0; // current loop index

int AOloopcontrol_meminit = 0;
static int AOlooploadconf_init = 0;

#define AOconfname "/tmp/AOconf.shm"
AOLOOPCONTROL_CONF *AOconf; // configuration - this can be an array

// CLI commands
//
// function CLI_checkarg used to check arguments
// CLI_checkarg ( CLI argument index , type code )
//
// type codes:
// 1: float
// 2: long
// 3: string, not existing image
// 4: existing image
// 5: string
//




