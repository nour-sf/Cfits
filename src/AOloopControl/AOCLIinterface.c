/**
 * @file    AOCLIinterface.c
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



/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 1. INITIALIZATION, configurations
 *  Allocate memory, import/export configurations */
/* =============================================================================================== */
/* =============================================================================================== */

/** @brief CLI function for AOloopControl_loadconfigure */
int_fast8_t AOloopControl_loadconfigure_cli() {
    if(CLI_checkarg(1,2)==0) {
        AOloopControl_loadconfigure(data.cmdargtoken[1].val.numl, 1, 10);
        return 0;
    }
    else return 1;
}


/** @brief CLI function for AOloopControl_stream3Dto2D */
/*int_fast8_t AOloopControl_stream3Dto2D_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,3)+CLI_checkarg(3,2)+CLI_checkarg(4,2)==0) {
        AOloopControl_stream3Dto2D(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl);
        return 0;
    }
    else return 1;
}*/




/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 6. REAL TIME COMPUTING ROUTINES
 *  calls CPU and GPU processing */
/* =============================================================================================== */
/* =============================================================================================== */

/** @brief CLI function for AOloopControl_WFSzpupdate_loop */
int_fast8_t AOloopControl_WFSzpupdate_loop_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)==0) {
        AOloopControl_WFSzpupdate_loop(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_WFSzeropoint_sum_update_loop */
int_fast8_t AOloopControl_WFSzeropoint_sum_update_loop_cli() {
    if(CLI_checkarg(1,3)+CLI_checkarg(2,2)+CLI_checkarg(3,4)+CLI_checkarg(4,4)==0) {
        AOloopControl_WFSzeropoint_sum_update_loop(LOOPNUMBER, data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_CompModes_loop */
int_fast8_t AOloopControl_CompModes_loop_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,4)+CLI_checkarg(5,3)==0) {
        AOloopControl_CompModes_loop(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string, data.cmdargtoken[5].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_GPUmodecoeffs2dm_filt */
int_fast8_t AOloopControl_GPUmodecoeffs2dm_filt_loop_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,2)+CLI_checkarg(5,4)+CLI_checkarg(6,2)+CLI_checkarg(7,2)+CLI_checkarg(8,2)==0) {
        AOloopControl_GPUmodecoeffs2dm_filt_loop(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.numl, data.cmdargtoken[5].val.string, data.cmdargtoken[6].val.numl, data.cmdargtoken[7].val.numl, data.cmdargtoken[8].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_computeWFSresidualimage */
int_fast8_t AOloopControl_computeWFSresidualimage_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,4)==0) {
        AOloopControl_computeWFSresidualimage(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_ComputeOpenLoopModes */
int_fast8_t AOloopControl_ComputeOpenLoopModes_cli() {
    if(CLI_checkarg(1,2)==0) {
        AOloopControl_ComputeOpenLoopModes(data.cmdargtoken[1].val.numl);
        return 0;
    } else return 1;
}

/** @brief CLI function for AOloopControl_AutoTuneGains */
int_fast8_t AOloopControl_AutoTuneGains_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,3)+CLI_checkarg(3,1)+CLI_checkarg(4,2)==0) {
        AOloopControl_AutoTuneGains(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_dm2dm_offload */
int_fast8_t AOloopControl_dm2dm_offload_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,1)+CLI_checkarg(4,1)+CLI_checkarg(5,1)==0) {
        AOloopControl_dm2dm_offload(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_sig2Modecoeff */
int_fast8_t AOloopControl_sig2Modecoeff_cli() {
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,3)==0) {
        AOloopControl_sig2Modecoeff(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string,  data.cmdargtoken[4].val.string);
        return 0;
    } else return 1;
}



/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 8.   LOOP CONTROL INTERFACE */
/* =============================================================================================== */
/* =============================================================================================== */


/** @brief CLI function for AOloopControl_setLoopNumber */
int_fast8_t AOloopControl_setLoopNumber_cli() {
    if(CLI_checkarg(1,2)==0) {
        AOloopControl_setLoopNumber(data.cmdargtoken[1].val.numl);
        return 0;
    }
    else return 1;
}


/* =============================================================================================== */
/** @name AOloopControl - 8.1. LOOP CONTROL INTERFACE - MAIN CONTROL : LOOP ON/OFF START/STOP/STEP/RESET */
/* =============================================================================================== */


/* =============================================================================================== */
/** @name AOloopControl - 8.2. LOOP CONTROL INTERFACE - DATA LOGGING                               */
/* =============================================================================================== */


/* =============================================================================================== */
/** @name AOloopControl - 8.3. LOOP CONTROL INTERFACE - PRIMARY DM WRITE                           */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.4. LOOP CONTROL INTERFACE - INTEGRATOR AUTO TUNING                     */
/* =============================================================================================== */


/* =============================================================================================== */
/** @name AOloopControl - 8.5. LOOP CONTROL INTERFACE - PREDICTIVE FILTER ON/OFF                   */
/* =============================================================================================== */


/* =============================================================================================== */
/** @name AOloopControl - 8.6. LOOP CONTROL INTERFACE - TIMING PARAMETERS                          */
/* =============================================================================================== */


/* =============================================================================================== */
/** @name AOloopControl - 8.7. LOOP CONTROL INTERFACE - CONTROL LOOP PARAMETERS                    */
/* =============================================================================================== */


/** @brief CLI function for AOloopControl_set_modeblock_gain */
int_fast8_t AOloopControl_set_modeblock_gain_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,1)+CLI_checkarg(3,2)==0) {
        AOloopControl_set_modeblock_gain(LOOPNUMBER, data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf, data.cmdargtoken[3].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_loopstep */
int_fast8_t AOloopControl_loopstep_cli() {
    if(CLI_checkarg(1,2)==0) {
        AOloopControl_loopstep(LOOPNUMBER, data.cmdargtoken[1].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_loopfrequ */
int_fast8_t AOloopControl_set_loopfrequ_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_loopfrequ(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_hardwlatency_frame */
int_fast8_t AOloopControl_set_hardwlatency_frame_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_hardwlatency_frame(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_complatency_frame */
int_fast8_t AOloopControl_set_complatency_frame_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_complatency_frame(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_wfsmextrlatency_frame */
int_fast8_t AOloopControl_set_wfsmextrlatency_frame_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_wfsmextrlatency_frame(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_delta */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_delta_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_AUTOTUNE_LIMITS_delta(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_perc */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_perc_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_AUTOTUNE_LIMITS_perc(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_set_AUTOTUNE_LIMITS_mcoeff */
int_fast8_t AOloopControl_set_AUTOTUNE_LIMITS_mcoeff_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_set_AUTOTUNE_LIMITS_mcoeff(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setgain */
int_fast8_t AOloopControl_setgain_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setgain(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgain_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setARPFgain(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgainAutoMin_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setARPFgainAutoMin(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setARPFgain */
int_fast8_t AOloopControl_setARPFgainAutoMax_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setARPFgainAutoMax(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}



/** @brief CLI function for AOloopControl_setWFSnormfloor */
int_fast8_t AOloopControl_setWFSnormfloor_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setWFSnormfloor(data.cmdargtoken[1].val.numf);
        return 0;
    } else return 1;
}

/** @brief CLI function for AOloopControl_setmaxlimit */
int_fast8_t AOloopControl_setmaxlimit_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setmaxlimit(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setmult */
int_fast8_t AOloopControl_setmult_cli() {
    if(CLI_checkarg(1,1)==0) {
        AOloopControl_setmult(data.cmdargtoken[1].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setframesAve */
int_fast8_t AOloopControl_setframesAve_cli() {
    if(CLI_checkarg(1,2)==0) {
        AOloopControl_setframesAve(data.cmdargtoken[1].val.numl);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setgainrange */
int_fast8_t AOloopControl_setgainrange_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0) {
        AOloopControl_setgainrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setlimitrange */
int_fast8_t AOloopControl_setlimitrange_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0) {
        AOloopControl_setlimitrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setmultfrange */
int_fast8_t AOloopControl_setmultfrange_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0) {
        AOloopControl_setmultfrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setgainblock */
int_fast8_t AOloopControl_setgainblock_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0) {
        AOloopControl_setgainblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setlimitblock */
int_fast8_t AOloopControl_setlimitblock_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0) {
        AOloopControl_setlimitblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_setmultfblock */
int_fast8_t AOloopControl_setmultfblock_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0) {
        AOloopControl_setmultfblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
        return 0;
    }
    else return 1;
}

/** @brief CLI function for AOloopControl_scanGainBlock */
int_fast8_t AOloopControl_scanGainBlock_cli() {
    if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)+CLI_checkarg(4,1)+CLI_checkarg(5,2)==0) {
        AOloopControl_scanGainBlock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numl);
        return 0;
    }
    else    return 1;
}





/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 10. FOCAL PLANE SPECKLE MODULATION / CONTROL                             */
/* =============================================================================================== */
/* =============================================================================================== */

/** @brief CLI function for AOloopControl_DMmodulateAB */
int_fast8_t AOloopControl_DMmodulateAB_cli()
{
    if(CLI_checkarg(1,4)+CLI_checkarg(2,4)+CLI_checkarg(3,4)+CLI_checkarg(4,4)+CLI_checkarg(5,4)+CLI_checkarg(6,1)+CLI_checkarg(7,2)==0)   {
        AOloopControl_DMmodulateAB(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, data.cmdargtoken[4].val.string, data.cmdargtoken[5].val.string, data.cmdargtoken[6].val.numf, data.cmdargtoken[7].val.numl);
        return 0;
    }
    else        return 1;
}


/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 11. PROCESS LOG FILES                                                    */
/* =============================================================================================== */
/* =============================================================================================== */

/** @brief CLI function for AOloopControl_logprocess_modeval */
int_fast8_t AOloopControl_logprocess_modeval_cli() {
    if(CLI_checkarg(1,4)==0) {
        AOloopControl_logprocess_modeval(data.cmdargtoken[1].val.string);
        return 0;
    }
    else return 1;
}




// 1: float
// 2: long
// 3: string, not existing image
// 4: existing image
// 5: string




// OBSOLETE ??


int_fast8_t AOloopControl_setparam_cli()
{
    if(CLI_checkarg(1,3)+CLI_checkarg(2,1)==0)
    {
        AOloopControl_setparam(LOOPNUMBER, data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numf);
        return 0;
    }
    else
        return 1;
}










// CODING STANDARD NOTE: minimal required documentation for doxygen
/**
 *  ## Purpose
 * 
 * Initialization of the AO loop Control with all of the CLI command line interface commands
 * 
 * 
 * ## Arguments
 * 
 * @param[in]
 * mode		INT
 * 			mode sets up what function does
 * -		does nothing
 * -		also does nothing
 * 
 */
// CODING STANDARD NOTE: function name start with module name

int_fast8_t init_AOloopControl()
{
    FILE *fp;

#ifdef AOLOOPCONTROL_LOGFUNC
    CORE_logFunctionCall( 0, __FUNCTION__, __LINE__, "");
#endif


    if((fp=fopen("LOOPNUMBER","r"))!=NULL)
    {
        if(fscanf(fp,"%8ld", &LOOPNUMBER) != 1)
            printERROR(__FILE__,__func__,__LINE__, "Cannot read LOOPNUMBER");

        printf("LOOP NUMBER = %ld\n", LOOPNUMBER);
        fclose(fp);
    }
    else
        LOOPNUMBER = 0;


    strcpy(data.module[data.NBmodule].name, __FILE__);
    strcpy(data.module[data.NBmodule].info, "AO loop control");
    data.NBmodule++;



    RegisterCLIcommand("aolloadconf",__FILE__, AOloopControl_loadconfigure_cli, "load AO loop configuration", "<loop #>", "AOlooploadconf 1", "int AOloopControl_loadconfigure(long loopnb, 1, 10)");


/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 2. LOW LEVEL UTILITIES & TOOLS                                           */
/* =============================================================================================== */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 2.1. LOW LEVEL UTILITIES & TOOLS - LOAD DATA STREAMS                     */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 2.2. LOW LEVEL UTILITIES & TOOLS - DATA STREAMS PROCESSING               */
/* =============================================================================================== */

/*    RegisterCLIcommand("aveACshmim", __FILE__, AOloopControl_AveStream_cli, "average and AC shared mem image", "<input image> <coeff> <output image ave> <output AC> <output RMS>" , "aveACshmim imin 0.01 outave outAC outRMS", "int AOloopControl_AveStream(char *IDname, double alpha, char *IDname_out_ave, char *IDname_out_AC, char *IDname_out_RMS)");

    RegisterCLIcommand("aolstream3Dto2D", __FILE__, AOloopControl_stream3Dto2D_cli, "remaps 3D cube into 2D image", "<input 3D stream> <output 2D stream> <# cols> <sem trigger>" , "aolstream3Dto2D in3dim out2dim 4 1", "long AOloopControl_stream3Dto2D(const char *in_name, const char *out_name, int NBcols, int insem)");
*/



/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 6. REAL TIME COMPUTING ROUTINES                                          */
/* =============================================================================================== */
/* =============================================================================================== */
    

    RegisterCLIcommand("aolrun", __FILE__, AOloopControl_run, "run AO loop", "no arg", "aolrun", "int AOloopControl_run()"); // run AO loop

    RegisterCLIcommand("aolzpwfsloop",__FILE__, AOloopControl_WFSzpupdate_loop_cli, "WFS zero point offset loop", "<dm offset [shared mem]> <zonal resp M [shared mem]> <nominal WFS reference>  <modified WFS reference>", "aolzpwfsloop dmZP zrespM wfszp", "int AOloopControl_WFSzpupdate_loop(char *IDzpdm_name, char *IDzrespM_name, char *IDwfszp_name)"); //WFS zero point offset loop

    RegisterCLIcommand("aolzpwfscloop", __FILE__, AOloopControl_WFSzeropoint_sum_update_loop_cli, "WFS zero point offset loop: combine multiple input channels", "<name prefix> <number of channels> <wfsref0> <wfsref>", "aolzpwfscloop wfs2zpoffset 4 wfsref0 wfsref", "int AOloopControl_WFSzeropoint_sum_update_loop(long loopnb, char *ID_WFSzp_name, int NBzp, char *IDwfsref0_name, char *IDwfsref_name)"); //WFS zero point offset loop: combine multiple input channels

    RegisterCLIcommand("aocmlrun", __FILE__, AOloopControl_CompModes_loop_cli, "run AO compute modes loop", "<CM> <wfsref> <WFS image stream> <WFS image total stream> <output stream>", "aocmlrun CM wfsref wfsim wfsimtot aomodeval", "int AOloopControl_CompModes_loop(char *ID_CM_name, char *ID_WFSref_name, char *ID_WFSim_name, char *ID_WFSimtot, char *ID_coeff_name)"); // run AO compute modes loop

    RegisterCLIcommand("aolmc2dmfilt", __FILE__, AOloopControl_GPUmodecoeffs2dm_filt_loop_cli, "convert mode coefficients to DM map", "<GPUMATMULTindex> <mode coeffs> <DMmodes> <sem trigg number> <out> <GPUindex> <loopnb> <offloadMode>", "aolmc2dmfilt aolmodeval DMmodesC 2 dmmapc 0.2 1 2 1", "int AOloopControl_GPUmodecoeffs2dm_filt_loop(const int GPUMATMULTCONFindex, char *modecoeffs_name, char *DMmodes_name, int semTrigg, char *out_name, int GPUindex, long loop, long offloadMode)"); //convert mode coefficients to DM map

    RegisterCLIcommand("aolsig2mcoeff", __FILE__, AOloopControl_sig2Modecoeff_cli, "convert signals to mode coeffs", "<signal data cube> <reference> <Modes data cube> <output image>", "aolsig2mcoeff wfsdata wfsref wfsmodes outim", "long AOloopControl_sig2Modecoeff(char *WFSim_name, char *IDwfsref_name, char *WFSmodes_name, char *outname)"); // convert signals to mode coeffs


/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 8.   LOOP CONTROL INTERFACE                                              */
/* =============================================================================================== */
/* =============================================================================================== */
    
    RegisterCLIcommand("aolnb", __FILE__, AOloopControl_setLoopNumber_cli, "set AO loop #", "<loop nb>", "AOloopnb 0", "int AOloopControl_setLoopNumber(long loop)"); // set AO loop 

/* =============================================================================================== */
/** @name AOloopControl - 8.1. LOOP CONTROL INTERFACE - MAIN CONTROL : LOOP ON/OFF START/STOP/STEP/RESET */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.2. LOOP CONTROL INTERFACE - DATA LOGGING                               */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.3. LOOP CONTROL INTERFACE - PRIMARY DM WRITE                           */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.4. LOOP CONTROL INTERFACE - INTEGRATOR AUTO TUNING                     */
/* =============================================================================================== */
  

/* =============================================================================================== */
/** @name AOloopControl - 8.5. LOOP CONTROL INTERFACE - PREDICTIVE FILTER ON/OFF                   */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.6. LOOP CONTROL INTERFACE - TIMING PARAMETERS                          */
/* =============================================================================================== */

/* =============================================================================================== */
/** @name AOloopControl - 8.7. LOOP CONTROL INTERFACE - CONTROL LOOP PARAMETERS                    */
/* =============================================================================================== */

    RegisterCLIcommand("aolsetgain", __FILE__, AOloopControl_setgain_cli, "set gain", "<gain value>", "aolsetgain 0.1", "int AOloopControl_setgain(float gain)");

    RegisterCLIcommand("aolsetARPFgain", __FILE__, AOloopControl_setARPFgain_cli, "set auto-regressive predictive filter gain", "<gain value>", "aolsetARPFgain 0.1", "int AOloopControl_setARPFgain(float gain)");

    RegisterCLIcommand("aolsetARPFgainAmin", __FILE__, AOloopControl_setARPFgainAutoMin_cli, "set ARPF gain min", "<gain value>", "aolsetARPFgainAmin 0.1", "int AOloopControl_setARPFgainAutoMin(float val)");

    RegisterCLIcommand("aolsetARPFgainAmax", __FILE__, AOloopControl_setARPFgainAutoMax_cli, "set ARPF gain max", "<gain value>", "aolsetARPFgainAmax 9.0", "int AOloopControl_setARPFgainAutoMax(float val)");

    RegisterCLIcommand("aolkill", __FILE__, AOloopControl_loopkill, "kill AO loop", "no arg", "aolkill", "int AOloopControl_setLoopNumber()");

    RegisterCLIcommand("aolon", __FILE__, AOloopControl_loopon, "turn loop on", "no arg", "aolon", "int AOloopControl_loopon()");

    RegisterCLIcommand("aoloff", __FILE__, AOloopControl_loopoff, "turn loop off", "no arg", "aoloff", "int AOloopControl_loopoff()");

    RegisterCLIcommand("aolstep",__FILE__, AOloopControl_loopstep_cli, "turn loop on for N steps", "<nbstep>", "aolstep", "int AOloopControl_loopstep(long loop, long NBstep)");

    RegisterCLIcommand("aolreset", __FILE__, AOloopControl_loopreset, "reset loop, and turn it off", "no arg", "aolreset", "int AOloopControl_loopreset()");

    RegisterCLIcommand("aolsetmbgain",__FILE__, AOloopControl_set_modeblock_gain_cli, "set modal block gain", "<loop #> <gain> <compute sum flag>", "aolsetmbgain 2 0.2 1", "int AOloopControl_set_modeblock_gain(long loop, long blocknb, float gain, int add)");

    RegisterCLIcommand("aolDMprimWon", __FILE__, AOloopControl_DMprimaryWrite_on, "turn DM primary write on", "no arg", "aolDMprimWon", "int AOloopControl_DMprimaryWrite_on()");

    RegisterCLIcommand("aolDMprimWoff", __FILE__, AOloopControl_DMprimaryWrite_off, "turn DM primary write off", "no arg", "aolDMprimWoff", "int AOloopControl_DMprimaryWrite_off()");

    RegisterCLIcommand("aolDMfiltWon", __FILE__, AOloopControl_DMfilteredWrite_on, "turn DM filtered write on", "no arg", "aolDMfiltWon", "int AOloopControl_DMfilteredWrite_on()");

    RegisterCLIcommand("aolDMfiltWoff", __FILE__, AOloopControl_DMfilteredWrite_off, "turn DM filtered write off", "no arg", "aolDMfiltWoff", "int AOloopControl_DMfilteredWrite_off()");

    RegisterCLIcommand("aolAUTOTUNELIMon", __FILE__, AOloopControl_AUTOTUNE_LIMITS_on, "turn auto-tuning modal limits on", "no arg", "aolAUTOTUNELIMon", "int AOloopControl_AUTOTUNE_LIMITS_on()");

    RegisterCLIcommand("aolAUTOTUNELIMoff", __FILE__, AOloopControl_AUTOTUNE_LIMITS_off, "turn auto-tuning modal limits off", "no arg", "aolAUTOTUNELIMoff", "int AOloopControl_AUTOTUNE_LIMITS_off()");

    RegisterCLIcommand("aolsetATlimd", __FILE__, AOloopControl_set_AUTOTUNE_LIMITS_delta_cli, "set auto-tuning modal limits delta", "<delta value [um]>", "aolsetATlimd 0.0001", "int AOloopControl_set_AUTOTUNE_LIMITS_delta(float AUTOTUNE_LIMITS_delta)");

    RegisterCLIcommand("aolsetATlimp", __FILE__, AOloopControl_set_AUTOTUNE_LIMITS_perc_cli, "set auto-tuning modal limits percentile", "<percentile value [percent]>", "aolsetATlimp 1.0", "int AOloopControl_set_AUTOTUNE_LIMITS_perc(float AUTOTUNE_LIMITS_perc)");

    RegisterCLIcommand("aolsetATlimm", __FILE__, AOloopControl_set_AUTOTUNE_LIMITS_mcoeff_cli, "set auto-tuning modal limits multiplicative coeff", "<multiplicative coeff [float]>", "aolsetATlimm 1.5", "int AOloopControl_set_AUTOTUNE_LIMITS_mcoeff(float AUTOTUNE_LIMITS_mcoeff)");

    RegisterCLIcommand("aolAUTOTUNEGAINon", __FILE__, AOloopControl_AUTOTUNE_GAINS_on, "turn auto-tuning modal gains on", "no arg", "aolAUTOTUNEGAINon", "int AOloopControl_AUTOTUNE_GAINS_on()");

    RegisterCLIcommand("aolAUTOTUNEGAINoff", __FILE__, AOloopControl_AUTOTUNE_GAINS_off, "turn auto-tuning modal gains off", "no arg", "aolAUTOTUNEGAINoff", "int AOloopControl_AUTOTUNE_GAINS_off()");

    RegisterCLIcommand("aolARPFon", __FILE__, AOloopControl_ARPFon, "turn auto-regressive predictive filter on", "no arg", "aolARPFon", "int AOloopControl_ARPFon()");

    RegisterCLIcommand("aolARPFoff", __FILE__, AOloopControl_ARPFoff, "turn auto-regressive predictive filter off", "no arg", "aolARPFoff", "int AOloopControl_ARPFoff()");




/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 10. FOCAL PLANE SPECKLE MODULATION / CONTROL                             */
/* =============================================================================================== */
/* =============================================================================================== */




/* =============================================================================================== */
/* =============================================================================================== */
/** @name AOloopControl - 11. PROCESS LOG FILES                                                    */
/* =============================================================================================== */
/* =============================================================================================== */



    RegisterCLIcommand("aolsetloopfrequ",
                       __FILE__,
                       AOloopControl_set_loopfrequ_cli,
                       "set loop frequency",
                       "<loop frequ [Hz]>",
                       "aolsetloopfrequ 2000",
                       "int AOloopControl_set_loopfrequ(float loopfrequ)");



    RegisterCLIcommand("aolsethlat", __FILE__, AOloopControl_set_hardwlatency_frame_cli, "set hardware latency", "<hardware latency [frame]>", "aolsethlat 2.7", "int AOloopControl_set_hardwlatency_frame(float hardwlatency_frame)");

    RegisterCLIcommand("aolsetclat",__FILE__, AOloopControl_set_complatency_frame_cli,"set computation latency", "<computation latency [frame]>", "aolsetclat 0.6", "int AOloopControl_set_complatency_frame(float complatency_frame)");

    RegisterCLIcommand("aolsetwlat", __FILE__, AOloopControl_set_wfsmextrlatency_frame_cli, "set WFS mode extraction latency", "<latency [frame]>", "aolsetwlat 0.8", "int AOloopControl_set_wfsmextrlatency_frame(float wfsmextrlatency_frame)");

    RegisterCLIcommand("aolsetwfsnormf", __FILE__, AOloopControl_setWFSnormfloor_cli, "set WFS normalization floor", "<floor value (total flux)>", "aolsetwfsnormf 10000.0", "int AOloopControl_setWFSnormfloor(float WFSnormfloor)");

    RegisterCLIcommand("aolsetmaxlim", __FILE__, AOloopControl_setmaxlimit_cli, "set max limit for AO mode correction", "<limit value>", "aolsetmaxlim 0.01", "int AOloopControl_setmaxlimit(float maxlimit)");

    RegisterCLIcommand("aolsetmult", __FILE__, AOloopControl_setmult_cli, "set mult coeff for AO mode correction", "<mult value>", "aolsetmult 0.98", "int AOloopControl_setmult(float multcoeff)");

    RegisterCLIcommand("aolsetnbfr",__FILE__, AOloopControl_setframesAve_cli, "set number of frames to be averaged", "<nb frames>", "aolsetnbfr 10", "int AOloopControl_setframesAve(long nbframes)");



    RegisterCLIcommand("aollogprocmodeval",__FILE__, AOloopControl_logprocess_modeval_cli, "process log image modeval", "<modeval image>", "aollogprocmodeval imc", "int AOloopControl_logprocess_modeval(const char *IDname);");



    RegisterCLIcommand("aolsetgainr", __FILE__, AOloopControl_setgainrange_cli, "set modal gains from m0 to m1 included", "<modemin [long]> <modemax [long]> <gainval>", "aolsetgainr 20 30 0.2", "int AOloopControl_setgainrange(long m0, long m1, float gainval)");

    RegisterCLIcommand("aolsetlimitr",__FILE__, AOloopControl_setlimitrange_cli, "set modal limits", "<modemin [long]> <modemax [long]> <limval>", "aolsetlimitr 20 30 0.02", "int AOloopControl_setlimitrange(long m0, long m1, float gainval)");

    RegisterCLIcommand("aolsetmultfr", __FILE__, AOloopControl_setmultfrange_cli, "set modal multf", "<modemin [long]> <modemax [long]> <multfval>", "aolsetmultfr 10 30 0.98", "int AOloopControl_setmultfrange(long m0, long m1, float multfval)");

    RegisterCLIcommand("aolsetgainb", __FILE__, AOloopControl_setgainblock_cli, "set modal gains by block", "<block [long]> <gainval>", "aolsetgainb 2 0.2", "int AOloopControl_setgainblock(long m0, long m1, float gainval)");

    RegisterCLIcommand("aolsetlimitb",__FILE__, AOloopControl_setlimitblock_cli, "set modal limits by block", "<block [long]> <limval>", "aolsetlimitb 2 0.02", "int AOloopControl_setlimitblock(long mb, float limitval)");

    RegisterCLIcommand("aolsetmultfb", __FILE__, AOloopControl_setmultfblock_cli, "set modal multf by block", "<block [long]> <multfval>", "aolsetmultfb 2 0.98", "int AOloopControl_setmultfblock(long mb, float multfval)");

    RegisterCLIcommand("aolscangainb", __FILE__, AOloopControl_scanGainBlock_cli, "scan gain for block", "<blockNB> <NBAOsteps> <gainstart> <gainend> <NBgainpts>", "aolscangainb", "int AOloopControl_scanGainBlock(long NBblock, long NBstep, float gainStart, float gainEnd, long NBgain)");

    RegisterCLIcommand("aolmkwfsres", __FILE__, AOloopControl_computeWFSresidualimage_cli, "compute WFS residual real time", "<loopnb> <averaging coeff image>", "aolmkwfsres 2 coeffim", "long AOloopControl_computeWFSresidualimage(long loop, char *IDalpha_name)");



    RegisterCLIcommand("aolcompolm", __FILE__, AOloopControl_ComputeOpenLoopModes_cli, "compute open loop mode values", "<loop #>", "aolcompolm 2", "long AOloopControl_ComputeOpenLoopModes(long loop)");

    RegisterCLIcommand("aolautotunegains", __FILE__, AOloopControl_AutoTuneGains_cli, "compute optimal gains", "<loop #> <gain stream> <gaincoeff> <NBsamples>", "aolautotunegains 0 autogain 0.1 20000", "long AOloopControl_AutoTuneGains(long loop, const char *IDout_name, float GainCoeff, long NBsamples)");

    RegisterCLIcommand("aoldm2dmoffload", __FILE__, AOloopControl_dm2dm_offload_cli, "slow offload from dm to dm", "<streamin> <streamout> <timestep[sec]> <offloadcoeff> <multcoeff>", "aoldm2dmoffload dmin dmout 0.5 -0.01 0.999", "long AOloopControl_dm2dm_offload(const char *streamin, const char *streamout, float twait, float offcoeff, float multcoeff)");

    RegisterCLIcommand("aolautotune",  __FILE__, AOloopControl_AutoTune, "auto tuning of loop parameters", "no arg", "aolautotune", "int_fast8_t AOloopControl_AutoTune()");

    RegisterCLIcommand("aolset", __FILE__, AOloopControl_setparam_cli, "set parameter", "<parameter> <value>" , "aolset", "int AOloopControl_setparam(long loop, const char *key, double value)");

    RegisterCLIcommand("aoldmmodAB", __FILE__, AOloopControl_DMmodulateAB_cli, "module DM with linear combination of probes A and B", "<probeA> <probeB> <dmstream> <WFS resp mat> <WFS ref stream> <delay [sec]> <NB probes>", "aoldmmodAB probeA probeB wfsrespmat wfsref 0.1 6","int AOloopControl_DMmodulateAB(const char *IDprobeA_name, const char *IDprobeB_name, const char *IDdmstream_name, const char *IDrespmat_name, const char *IDwfsrefstream_name, double delay, long NBprobes)");




    // add atexit functions here
    // atexit((void*) myfunc); atexit = starts a function once the program exits (only if it is not a crash exit)

    return 0;
}

