/**
 * @file    AOCLIinterface.h
 * @brief   Adaptive Optics Control loop engine
 * 
 * AO engine uses stream data structure
 *  
 * @author  O. Guyon
 * @date    15 Oct 2017 -- 
 *
 * @bug No known bugs.
 * 
 * @see http://oguyon.github.io/AdaptiveOpticsControl/src/AOloopControl/doc/AOloopControl.html
 *  
 * @defgroup AOloopControl_streams Image streams
 * @defgroup AOloopControl_AOLOOPCONTROL_CONF AOloopControl main data structure
 * 
 */


#ifndef CLIinterface_H
#define CLIinterface_H


/** @brief CLI function for AOloopControl_loadconfigure */
int_fast8_t AOloopControl_loadconfigure_cli();

/** @brief CLI function for AOloopControl_WFSzpupdate_loop */
int_fast8_t AOloopControl_WFSzpupdate_loop_cli();

/** @brief CLI function for AOloopControl_WFSzeropoint_sum_update_loop */
int_fast8_t AOloopControl_WFSzeropoint_sum_update_loop_cli();

/** @brief CLI function for AOloopControl_CompModes_loop */
int_fast8_t AOloopControl_CompModes_loop_cli();

/** @brief CLI function for AOloopControl_GPUmodecoeffs2dm_filt */
int_fast8_t AOloopControl_GPUmodecoeffs2dm_filt_loop_cli();

/** @brief CLI function for AOloopControl_computeWFSresidualimage */
int_fast8_t AOloopControl_computeWFSresidualimage_cli();

/** @brief CLI function for AOloopControl_ComputeOpenLoopModes */
int_fast8_t AOloopControl_ComputeOpenLoopModes_cli();

/** @brief CLI function for AOloopControl_AutoTuneGains */
int_fast8_t AOloopControl_AutoTuneGains_cli();

/** @brief CLI function for AOloopControl_dm2dm_offload */
int_fast8_t AOloopControl_dm2dm_offload_cli();

/** @brief CLI function for AOloopControl_sig2Modecoeff */
int_fast8_t AOloopControl_sig2Modecoeff_cli();

/** @brief CLI function for AOloopControl_setLoopNumber */
int_fast8_t AOloopControl_setLoopNumber_cli();

/** @brief CLI function for AOloopControl_set_modeblock_gain */
int_fast8_t AOloopControl_set_modeblock_gain_cli();

/** @brief CLI function for AOloopControl_loopstep */
int_fast8_t AOloopControl_loopstep_cli();

/** @brief CLI function for AOloopControl_set_loopfrequ */
int_fast8_t AOloopControl_set_loopfrequ_cli();

/** @brief CLI function for AOloopControl_set_hardwlatency_frame */
int_fast8_t AOloopControl_set_hardwlatency_frame_cli();

/** @brief CLI function for AOloopControl_set_complatency_frame */
int_fast8_t AOloopControl_set_complatency_frame_cli();

/** @brief CLI function for AOloopControl_set_wfsmextrlatency_frame */
int_fast8_t AOloopControl_set_wfsmextrlatency_frame_cli();

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_delta */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_delta_cli();

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_perc */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_perc_cli()

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_mcoeff */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_mcoeff_cli();

/** @brief CLI function for AOloopControl_setgain */
int_fast8_t AOloopControl_setgain_cli();

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgain_cli();

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgainAutoMin_cli();

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgainAutoMax_cli();

/** @brief CLI function for AOloopControl_setWFSnormfloor */
int_fast8_t AOloopControl_setWFSnormfloor_cli();

/** @brief CLI function for AOloopControl_setmaxlimit */
int_fast8_t AOloopControl_setmaxlimit_cli();

/** @brief CLI function for AOloopControl_setmult */
int_fast8_t AOloopControl_setmult_cli();

/** @brief CLI function for AOloopControl_setframesAve */
int_fast8_t AOloopControl_setframesAve_cli();

/** @brief CLI function for AOloopControl_setgainrange */
int_fast8_t AOloopControl_setgainrange_cli();

/** @brief CLI function for AOloopControl_setlimitrange */
int_fast8_t AOloopControl_setlimitrange_cli();

/** @brief CLI function for AOloopControl_setmultfrange */
int_fast8_t AOloopControl_setmultfrange_cli();

/** @brief CLI function for AOloopControl_setgainblock */
int_fast8_t AOloopControl_setgainblock_cli();

/** @brief CLI function for AOloopControl_setlimitblock */
int_fast8_t AOloopControl_setlimitblock_cli();

/** @brief CLI function for AOloopControl_setmultfblock */
int_fast8_t AOloopControl_setmultfblock_cli();

/** @brief CLI function for AOloopControl_scanGainBlock */
int_fast8_t AOloopControl_scanGainBlock_cli();

/** @brief CLI function for AOloopControl_DMmodulateAB */
int_fast8_t AOloopControl_DMmodulateAB_cli();

/** @brief CLI function for AOloopControl_logprocess_modeval */
int_fast8_t AOloopControl_logprocess_modeval_cli();

// Obsolete ?
int_fast8_t AOloopControl_setparam_cli();


int_fast8_t init_AOloopControl();


#endif 