/**
 * @file    AOload.c
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
/** @brief Load / Setup configuration                                                              */
/* =============================================================================================== */

/**
 * ## Purpose
 * 
 * load / setup configuration - amazingly loooong function, I am proud of you Boss ! 
 *
 * ## Arguments
 * 
 * @param[in]
 * loop		INT
 * 			Loop number
 * 
 * @param[in]
 * mode		INT 
 * - 1 loads from ./conf/ directory to shared memory
 * - 0 simply connects to shared memory
 * 
 * @param[in]
 * level	INT
 * - 2 zonal only
 * - 10+ load all
 * 
 * 
 * 
 * @ingroup AOloopControl_streams
 */
int_fast8_t AOloopControl_loadconfigure(long loop, int mode, int level)
{
    FILE *fp;
    char content[201];
    char name[201];
    char fname[201];
    uint32_t *sizearray;
    int kw;
    long k;
    int r;
    int sizeOK;
    char command[501];
    int CreateSMim;
    long ii;
    long tmpl;
    char testdirname[201];

    int initwfsref;

    FILE *fplog; // human-readable log of load sequence


#ifdef AOLOOPCONTROL_LOGFUNC
	AOLOOPCONTROL_logfunc_level = 0;
    CORE_logFunctionCall( AOLOOPCONTROL_logfunc_level, AOLOOPCONTROL_logfunc_level_max, 0, __FUNCTION__, __LINE__, "");
#endif


	// Create logfile for this function
	//
    if((fplog=fopen("logdir/loadconf.log", "w"))==NULL)
    {
        printf("ERROR: cannot create logdir/loadconf.log\n");
        exit(0);
    }
    loadcreateshm_log = 1;
    loadcreateshm_fplog = fplog;

	/** --- */
	/** # Details */
	
	/** ## 1. Initial setup from configuration files */


	/** - 1.1. Initialize memory */
	fprintf(fplog, "\n\n============== 1.1. Initialize memory ===================\n\n");
    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);


	
	
	//
    /** ### 1.2. Set names of key streams */
    // Here we define names of key streams used by loop

	fprintf(fplog, "\n\n============== 1.2. Set names of key streams ===================\n\n");

	/** - dmC stream  : DM control */
    if(sprintf(name, "aol%ld_dmC", loop)<1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("DM control file name : %s\n", name);
    strcpy(AOconf[loop].dmCname, name);

	/** - dmdisp stream : total DM displacement */
	// used to notify dm combine that a new displacement should be computed
    if(sprintf(name, "aol%ld_dmdisp", loop) < 1) 
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("DM displacement file name : %s\n", name);
    strcpy(AOconf[loop].dmdispname, name);

	/** - dmRM stream : response matrix */
    if(sprintf(name, "aol%ld_dmRM", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("DM RM file name : %s\n", name);
    strcpy(AOconf[loop].dmRMname, name);

	/** - wfsim : WFS image */
    if(sprintf(name, "aol%ld_wfsim", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("WFS file name: %s\n", name);
    strcpy(AOconf[loop].WFSname, name);



    // Modal control

	/** - DMmodes : control modes */
    if(sprintf(name, "aol%ld_DMmodes", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("DMmodes file name: %s\n", name);
    strcpy(AOconf[loop].DMmodesname, name);

	/** - respM : response matrix */
    if(sprintf(name, "aol%ld_respM", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("respM file name: %s\n", name);
    strcpy(AOconf[loop].respMname, name);

	/** - contrM : control matrix */
    if(sprintf(name, "aol%ld_contrM", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    printf("contrM file name: %s\n", name);
    strcpy(AOconf[loop].contrMname, name);




    sizearray = (uint32_t*) malloc(sizeof(uint32_t)*3);


    /** ### 1.3. Read loop name
     * 
     * - ./conf/conf_LOOPNAME.txt -> AOconf[loop].name 
     */
	fprintf(fplog, "\n\n============== 1.3. Read loop name ===================\n\n");

    if((fp=fopen("./conf/conf_LOOPNAME.txt","r"))==NULL)
    {
        printf("ERROR: file ./conf/conf_LOOPNAME.txt missing\n");
        exit(0);
    }
    if(fscanf(fp, "%200s", content) != 1)
    {
        printERROR(__FILE__,__func__,__LINE__, "Cannot read parameter for file");
		exit(0);
	}

    printf("loop name : %s\n", content);
    fflush(stdout);
    fprintf(fplog, "AOconf[%ld].name = %s\n", loop, AOconf[loop].name);
    fclose(fp);
    strcpy(AOconf[loop].name, content);


    /** ### 1.4. Define WFS image normalization mode 
     * 
     * - conf/param_WFSnorm.txt -> AOconf[loop].WFSnormalize
     */ 
    fprintf(fplog, "\n\n============== 1.4. Define WFS image normalization mode ===================\n\n");
    
    AOconf[loop].WFSnormalize = AOloopControl_readParam_int("WFSnorm", 1, fplog);
   


    /** ### 1.5. Read Timing info
     * 
     * - ./conf/param_loopfrequ.txt    -> AOconf[loop].loopfrequ
     * - ./conf/param_hardwlatency.txt -> AOconf[loop].hardwlatency
     * - AOconf[loop].hardwlatency_frame = AOconf[loop].hardwlatency * AOconf[loop].loopfrequ
     * - ./conf/param_complatency.txt  -> AOconf[loop].complatency
     * - AOconf[loop].complatency_frame = AOconf[loop].complatency * AOconf[loop].loopfrequ;
     * - ./conf/param_wfsmextrlatency.txt -> AOconf[loop].wfsmextrlatency
     */
     fprintf(fplog, "\n\n============== 1.5. Read Timing info ===================\n\n");
    
    
    
    AOconf[loop].loopfrequ = AOloopControl_readParam_float("loopfrequ", 1000.0, fplog);
	AOconf[loop].hardwlatency = AOloopControl_readParam_float("hardwlatency", 0.0, fplog);  
    AOconf[loop].hardwlatency_frame = AOconf[loop].hardwlatency * AOconf[loop].loopfrequ;

	AOconf[loop].complatency = AOloopControl_readParam_float("complatency", 0.0, fplog);
    AOconf[loop].complatency_frame = AOconf[loop].complatency * AOconf[loop].loopfrequ;

	AOconf[loop].wfsmextrlatency = AOloopControl_readParam_float("wfsmextrlatency", 0.0, fplog);
    AOconf[loop].wfsmextrlatency_frame = AOconf[loop].wfsmextrlatency * AOconf[loop].loopfrequ;



    /** ### 1.6. Define GPU use
     * 
     * - ./conf/param_GPU0.txt           > AOconf[loop].GPU0 (0 if missing)
     * - ./conf/param_GPU1.txt           > AOconf[loop].GPU1 (0 if missing)
     * - ./conf/param_GPUall.txt        -> AOconf[loop].GPUall
     * - ./conf/param_DMprimWriteON.txt -> AOconf[loop].DMprimaryWriteON
     * 
     */ 
	fprintf(fplog, "\n\n============== 1.6. Define GPU use ===================\n\n");
	
	AOconf[loop].GPU0 = AOloopControl_readParam_int("GPU0", 0, fplog);
	AOconf[loop].GPU1 = AOloopControl_readParam_int("GPU1", 0, fplog);
	AOconf[loop].GPUall = AOloopControl_readParam_int("GPUall", 0, fplog); // Skip CPU image scaling and go straight to GPUs ?
	AOconf[loop].DMprimaryWriteON = AOloopControl_readParam_int("DMprimaryWriteON", 0, fplog);    // Direct DM write ?
	AOconf[loop].DMfilteredWriteON = AOloopControl_readParam_int("DMfilteredWriteON", 0, fplog);    // Filtered DM write ?
    

	/** ### 1.7. WFS image total flux computation mode
	 * 
	 * 
	 */
	 fprintf(fplog, "\n\n============== 1.7. WFS image total flux computation mode ===================\n\n");

    // TOTAL image done in separate thread ?
    AOconf[loop].AOLCOMPUTE_TOTAL_ASYNC = AOloopControl_readParam_int("COMPUTE_TOTAL_ASYNC", 1, fplog);
 

    /** ### 1.8. Read CMatrix mult mode
     * 
     * - ./conf/param_CMMMODE.txt -> CMMODE
     * 		- 0 : WFS signal -> Mode coeffs -> DM act values  (2 sequential matrix multiplications)
     * 		- 1 : WFS signal -> DM act values  (1 combined matrix multiplication)
     */ 

 	fprintf(fplog, "\n\n============== 1.8. Read CMatrix mult mode ===================\n\n");

	AOconf[loop].CMMODE = AOloopControl_readParam_int("CMMODE", 1, fplog);



	/** ### 1.9. Setup loop timing array 
	 */
	fprintf(fplog, "\n\n============== 1.9. Setup loop timing array ===================\n\n");
	// LOOPiteration is written in cnt1 
    if(sprintf(name, "aol%ld_looptiming", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    aoconfID_looptiming = AOloopControl_IOtools_2Dloadcreate_shmim(name, " ", AOcontrolNBtimers, 1, 0.0);


	/** ## 2. Read/load shared memory arrays
	 * 
	 */ 
	fprintf(fplog, "\n\n============== 2. Read/load shared memory arrays ===================\n\n");

    /**
     * ### 2.1. CONNECT to existing streams
     * 
     * Note: these streams MUST exist
     * 
     *  - AOconf[loop].dmdispname  : this image is read to notify when new dm displacement is ready
     *  - AOconf[loop].WFSname     : connect to WFS camera. This is where the size of the WFS is read 
     */
     
     fprintf(fplog, "\n\n============== 2.1. CONNECT to existing streams  ===================\n\n");
     
    aoconfID_dmdisp = read_sharedmem_image(AOconf[loop].dmdispname);
    if(aoconfID_dmdisp==-1)
        fprintf(fplog, "ERROR : cannot read shared memory stream %s\n", AOconf[loop].dmdispname);
    else
        fprintf(fplog, "stream %s loaded as ID = %ld\n", AOconf[loop].dmdispname, aoconfID_dmdisp);

 
    aoconfID_wfsim = read_sharedmem_image(AOconf[loop].WFSname);
    if(aoconfID_wfsim == -1)
        fprintf(fplog, "ERROR : cannot read shared memory stream %s\n", AOconf[loop].WFSname);
    else
        fprintf(fplog, "stream %s loaded as ID = %ld\n", AOconf[loop].WFSname, aoconfID_wfsim);

    AOconf[loop].sizexWFS = data.image[aoconfID_wfsim].md[0].size[0];
    AOconf[loop].sizeyWFS = data.image[aoconfID_wfsim].md[0].size[1];
    AOconf[loop].sizeWFS = AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS;

    fprintf(fplog, "WFS stream size = %ld x %ld\n", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);


    /**
     * 
     * ### 2.2. Read file to stream or connect to existing stream
     * 
     *  The AOloopControl_xDloadcreate_shmim functions are used, and follows these rules:
     * 
     * If file already loaded, use it (we assume it's already been properly loaded) \n
     * If not, attempt to read it from shared memory \n
     * If not available in shared memory, create it in shared memory \n
     * if "fname" exists, attempt to load it into the shared memory image
     *
     * Stream names are fixed: 
     * - aol_wfsdark
     * - aol_imWFS0
     * - aol_imWFS0tot
     * - aol_imWFS1
     * - aol_imWFS2
     * - aol_wfsref0
     * - aol_wfsref
     */
     
     
	fprintf(fplog, "\n\n============== 2.2. Read file to stream or connect to existing stream  ===================\n\n");

    if(sprintf(name, "aol%ld_wfsdark", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    sprintf(fname, "./conf/shmim_wfsdark.fits");
    aoconfID_wfsdark = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);



    if(sprintf(name, "aol%ld_imWFS0", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    aoconfID_imWFS0 = AOloopControl_IOtools_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);
    COREMOD_MEMORY_image_set_createsem(name, 10);

    if(sprintf(name, "aol%ld_imWFS0tot", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    aoconfID_imWFS0tot = AOloopControl_IOtools_2Dloadcreate_shmim(name, " ", 1, 1, 0.0);
    COREMOD_MEMORY_image_set_createsem(name, 10);

    if(sprintf(name, "aol%ld_imWFS1", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    aoconfID_imWFS1 = AOloopControl_IOtools_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);

    if(sprintf(name, "aol%ld_imWFS2", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    aoconfID_imWFS2 = AOloopControl_IOtools_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);




    initwfsref = AOconf[loop].init_wfsref0;

    if(sprintf(name, "aol%ld_wfsref0", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    if(sprintf(fname, "./conf/shmim_wfsref0.fits") < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    aoconfID_wfsref0 = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);
    AOconf[loop].init_wfsref0 = 1;

    if(sprintf(name, "aol%ld_wfsref", loop) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    if(sprintf(fname, "./conf/shmim_wfsref.fits") < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

    aoconfID_wfsref = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 0.0);

    if(initwfsref==0)
    {
        char name1[200];

        if(sprintf(name1, "aol%ld_wfsref0", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        copy_image_ID(name1, name, 1);
    }




    /** ### 2.3. Connect to DM
     * 
     * - AOconf[loop].dmCname : DM control channel
     * 
     *  Here the DM size is read -> Oconf[loop].sizexDM, AOconf[loop].sizeyDM
     */


	AOconf[loop].DMMODE = AOloopControl_readParam_int("DMMODE", 0, fplog); // zonal DM by default

    aoconfID_dmC = image_ID(AOconf[loop].dmCname);
    if(aoconfID_dmC==-1)
    {
        printf("connect to %s\n", AOconf[loop].dmCname);
        aoconfID_dmC = read_sharedmem_image(AOconf[loop].dmCname);
        if(aoconfID_dmC==-1)
        {
            printf("ERROR: cannot connect to shared memory %s\n", AOconf[loop].dmCname);
            exit(0);
        }
    }
    AOconf[loop].sizexDM = data.image[aoconfID_dmC].md[0].size[0];
    AOconf[loop].sizeyDM = data.image[aoconfID_dmC].md[0].size[1];
    AOconf[loop].sizeDM = AOconf[loop].sizexDM*AOconf[loop].sizeyDM;

    fprintf(fplog, "Connected to DM %s, size = %ld x %ld\n", AOconf[loop].dmCname, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);



	/**
	 * - AOconf[loop].dmRMname : DM response matrix channel
	 * 
	 */
    aoconfID_dmRM = image_ID(AOconf[loop].dmRMname);
    if(aoconfID_dmRM==-1)
    {
        printf("connect to %s\n", AOconf[loop].dmRMname);
        aoconfID_dmRM = read_sharedmem_image(AOconf[loop].dmRMname);
        if(aoconfID_dmRM==-1)
        {
            printf("ERROR: cannot connect to shared memory %s\n", AOconf[loop].dmRMname);
            exit(0);
        }
    }
    fprintf(fplog, "stream %s loaded as ID = %ld\n", AOconf[loop].dmRMname, aoconfID_dmRM);



	/// Connect to DM modes shared mem
	///  continue if not successful
	///
	aoconfID_DMmodes = image_ID(AOconf[loop].DMmodesname);
	if(aoconfID_DMmodes==-1)
    {
        printf("connect to %s\n", AOconf[loop].DMmodesname);
        aoconfID_DMmodes = read_sharedmem_image(AOconf[loop].DMmodesname);
        if(aoconfID_DMmodes==-1)
        {
            printf("WARNING: cannot connect to shared memory %s\n", AOconf[loop].DMmodesname);
//			exit(0);
        }
    }
	if(aoconfID_DMmodes!=-1)
	{
		fprintf(fplog, "stream %s loaded as ID = %ld\n", AOconf[loop].DMmodesname, aoconfID_DMmodes);
		AOconf[loop].NBDMmodes = data.image[aoconfID_DMmodes].md[0].size[2];
		printf("NBmodes = %ld\n", AOconf[loop].NBDMmodes);
	}
	

	/** 
	 * ## 3. Load DM modes (if level >= 10)
	 * 
	 * */

	fprintf(fplog, "\n\n============== 3. Load DM modes (if level >= 10)  ===================\n\n");

	
    if(level>=10) // Load DM modes (will exit if not successful)
    {				
		/** 
		 * Load AOconf[loop].DMmodesname \n
		 * if already exists in local memory, trust it and adopt it \n
		 * if not, load from ./conf/shmim_DMmodes.fits \n
		 * 
		 */
		
        aoconfID_DMmodes = image_ID(AOconf[loop].DMmodesname); 

        if(aoconfID_DMmodes == -1) // If not, check file
        {
            long ID1tmp, ID2tmp;
            int vOK;

			

            if(sprintf(fname, "./conf/shmim_DMmodes.fits") < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

            printf("Checking file \"%s\"\n", fname);

            // GET SIZE FROM FILE
            ID1tmp = load_fits(fname, "tmp3Dim", 1);
            if(ID1tmp==-1)
            {
                printf("WARNING: no file \"%s\" -> loading zonal modes\n", fname);

                if(sprintf(fname, "./conf/shmim_DMmodes_zonal.fits") <1)
                    printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

                ID1tmp = load_fits(fname, "tmp3Dim", 1);
                if(ID1tmp==-1)
                {
                    printf("ERROR: cannot read zonal modes \"%s\"\n", fname);
                    exit(0);
                }
            }


            // check size
            if(data.image[ID1tmp].md[0].naxis != 3)
            {
                printf("ERROR: File \"%s\" is not a 3D image (cube)\n", fname);
                exit(0);
            }
            if(data.image[ID1tmp].md[0].size[0] != AOconf[loop].sizexDM)
            {
                printf("ERROR: File \"%s\" has wrong x size: should be %ld, is %ld\n", fname, AOconf[loop].sizexDM, (long) data.image[ID1tmp].md[0].size[0]);
                exit(0);
            }
            if(data.image[ID1tmp].md[0].size[1] != AOconf[loop].sizeyDM)
            {
                printf("ERROR: File \"%s\" has wrong y size: should be %ld, is %ld\n", fname, AOconf[loop].sizeyDM, (long) data.image[ID1tmp].md[0].size[1]);
                exit(0);
            }
            AOconf[loop].NBDMmodes = data.image[ID1tmp].md[0].size[2];

            printf("NUMBER OF MODES = %ld\n", AOconf[loop].NBDMmodes);

            // try to read it from shared memory
            ID2tmp = read_sharedmem_image(AOconf[loop].DMmodesname);
            vOK = 0;
            if(ID2tmp != -1) // if shared memory exists, check its size
            {
                vOK = 1;
                if(data.image[ID2tmp].md[0].naxis != 3)
                {
                    printf("ERROR: Shared memory File %s is not a 3D image (cube)\n", AOconf[loop].DMmodesname);
                    vOK = 0;
                }
                if(data.image[ID2tmp].md[0].size[0] != AOconf[loop].sizexDM)
                {
                    printf("ERROR: Shared memory File %s has wrong x size: should be %ld, is %ld\n", AOconf[loop].DMmodesname, AOconf[loop].sizexDM, (long) data.image[ID2tmp].md[0].size[0]);
                    vOK = 0;
                }
                if(data.image[ID2tmp].md[0].size[1] != AOconf[loop].sizeyDM)
                {
                    printf("ERROR: Shared memory File %s has wrong y size: should be %ld, is %ld\n", AOconf[loop].DMmodesname, AOconf[loop].sizeyDM, (long) data.image[ID2tmp].md[0].size[1]);
                    vOK = 0;
                }
                if(data.image[ID2tmp].md[0].size[2] != AOconf[loop].NBDMmodes)
                {
                    printf("ERROR: Shared memory File %s has wrong y size: should be %ld, is %ld\n", AOconf[loop].DMmodesname, AOconf[loop].NBDMmodes, (long) data.image[ID2tmp].md[0].size[2]);
                    vOK = 0;
                }

                if(vOK==1) // if size is OK, adopt it
                    aoconfID_DMmodes = ID2tmp;
                else // if not, erase shared memory
                {
                    printf("SHARED MEM IMAGE HAS WRONG SIZE -> erasing it\n");
                    delete_image_ID(AOconf[loop].DMmodesname);
                }
            }


            if(vOK==0) // create shared memory
            {

                sizearray[0] = AOconf[loop].sizexDM;
                sizearray[1] = AOconf[loop].sizeyDM;
                sizearray[2] = AOconf[loop].NBDMmodes;
                printf("Creating %s   [%ld x %ld x %ld]\n", AOconf[loop].DMmodesname, (long) sizearray[0], (long) sizearray[1], (long) sizearray[2]);
                fflush(stdout);
                aoconfID_DMmodes = create_image_ID(AOconf[loop].DMmodesname, 3, sizearray, _DATATYPE_FLOAT, 1, 0);
            }

            // put modes into shared memory

            switch (data.image[ID1tmp].md[0].atype) {
            case _DATATYPE_FLOAT :
                memcpy(data.image[aoconfID_DMmodes].array.F, data.image[ID1tmp].array.F, sizeof(float)*AOconf[loop].sizexDM*AOconf[loop].sizeyDM*AOconf[loop].NBDMmodes);
                break;
            case _DATATYPE_DOUBLE :
                for(ii=0; ii<AOconf[loop].sizexDM*AOconf[loop].sizeyDM*AOconf[loop].NBDMmodes; ii++)
                    data.image[aoconfID_DMmodes].array.F[ii] = data.image[ID1tmp].array.D[ii];
                break;
            default :
                printf("ERROR: TYPE NOT RECOGNIZED FOR MODES\n");
                exit(0);
                break;
            }

            delete_image_ID("tmp3Dim");
        }

        fprintf(fplog, "stream %s loaded as ID = %ld, size %ld %ld %ld\n", AOconf[loop].DMmodesname, aoconfID_DMmodes, AOconf[loop].sizexDM, AOconf[loop].sizeyDM, AOconf[loop].NBDMmodes);
    }



    // TO BE CHECKED

    // AOconf[loop].NBMblocks = AOconf[loop].DMmodesNBblock;
    // printf("NBMblocks : %ld\n", AOconf[loop].NBMblocks);
    // fflush(stdout);


    AOconf[loop].AveStats_NBpt = 100;
    for(k=0; k<AOconf[loop].DMmodesNBblock; k++)
    {
        AOconf[loop].block_OLrms[k] = 0.0;
        AOconf[loop].block_Crms[k] = 0.0;
        AOconf[loop].block_WFSrms[k] = 0.0;
        AOconf[loop].block_limFrac[k] = 0.0;

        AOconf[loop].blockave_OLrms[k] = 0.0;
        AOconf[loop].blockave_Crms[k] = 0.0;
        AOconf[loop].blockave_WFSrms[k] = 0.0;
        AOconf[loop].blockave_limFrac[k] = 0.0;
    }

    printf("%ld modes\n", AOconf[loop].NBDMmodes);


    if(level>=10)
    {
        long ID;

        // Load/create modal command vector memory
        if(sprintf(name, "aol%ld_DMmode_cmd", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_cmd_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 0.0);


        if(sprintf(name, "aol%ld_DMmode_meas", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_meas_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 0.0);

        if(sprintf(name, "aol%ld_DMmode_AVE", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_AVE_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 0.0);

        if(sprintf(name, "aol%ld_DMmode_RMS", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_RMS_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 0.0);

        if(sprintf(name, "aol%ld_DMmode_GAIN", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_DMmode_GAIN = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 1.0);

        if(sprintf(name, "aol%ld_DMmode_LIMIT", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_LIMIT_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 1.0);

        if(sprintf(name, "aol%ld_DMmode_MULTF", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_MULTF_modes = AOloopControl_IOtools_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1, 1.0);


        if(sprintf(name, "aol%ld_wfsmask", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        sprintf(fname, "conf/%s.fits", name);
        aoconfID_wfsmask = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, 1.0);
        AOconf[loop].activeWFScnt = 0;
        for(ii=0; ii<AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS; ii++)
            if(data.image[aoconfID_wfsmask].array.F[ii]>0.5)
                AOconf[loop].activeWFScnt++;

        if(sprintf(name, "aol%ld_dmmask", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/%s.fits", name) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_dmmask = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexDM, AOconf[loop].sizeyDM, 1.0);
        AOconf[loop].activeDMcnt = 0;
        for(ii=0; ii<AOconf[loop].sizexDM*AOconf[loop].sizeyDM; ii++)
            if(data.image[aoconfID_dmmask].array.F[ii]>0.5)
                AOconf[loop].activeDMcnt++;

        printf(" AOconf[loop].activeWFScnt = %ld\n", AOconf[loop].activeWFScnt );
        printf(" AOconf[loop].activeDMcnt = %ld\n", AOconf[loop].activeDMcnt );


        AOconf[loop].init_RM = 0;
        if(sprintf(fname, "conf/shmim_respM.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        aoconfID_respM = AOloopControl_IOtools_3Dloadcreate_shmim(AOconf[loop].respMname, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes, 0.0);
        AOconf[loop].init_RM = 1;


        AOconf[loop].init_CM = 0;
        if(sprintf(fname, "conf/shmim_contrM.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        aoconfID_contrM = AOloopControl_IOtools_3Dloadcreate_shmim(AOconf[loop].contrMname, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes, 0.0);
        AOconf[loop].init_CM = 1;

        if((fp=fopen("conf/param_NBmodeblocks.txt", "r"))==NULL)
        {
            printf("Cannot open conf/param_NBmodeblocks.txt.... assuming 1 block\n");
            AOconf[loop].DMmodesNBblock = 1;
        }
        else
        {
            if(fscanf(fp, "%50ld", &tmpl) == 1)
                AOconf[loop].DMmodesNBblock = tmpl;
            else
            {
                printf("Cannot read conf/param_NBmodeblocks.txt.... assuming 1 block\n");
                AOconf[loop].DMmodesNBblock = 1;
            }
            fclose(fp);
        }


        if(sprintf(name, "aol%ld_contrMc", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_contrMc.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_contrMc = AOloopControl_IOtools_3Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].sizeDM, 0.0);

        if(sprintf(name, "aol%ld_contrMcact", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_contrMcact_00.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_contrMcact[0] = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].activeWFScnt, AOconf[loop].activeDMcnt, 0.0);



        if(sprintf(name, "aol%ld_gainb", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_gainb.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_gainb = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].DMmodesNBblock, 1, 0.0);

		if(sprintf(name, "aol%ld_modeARPFgainAuto", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_modeARPFgainAuto.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_modeARPFgainAuto = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].NBDMmodes, 1, 1.0);


        if(sprintf(name, "aol%ld_multfb", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_multfb.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_multfb = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].DMmodesNBblock, 1, 0.0);

        if(sprintf(name, "aol%ld_limitb", loop) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        if(sprintf(fname, "conf/shmim_limitb.fits") < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        aoconfID_limitb = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].DMmodesNBblock, 1, 0.0);


#ifdef _PRINT_TEST
        printf("TEST - INITIALIZE contrMc, contrMcact\n");
        fflush(stdout);
#endif


        uint_fast16_t kk;
        int mstart = 0;
        
        for(kk=0; kk<AOconf[loop].DMmodesNBblock; kk++)
        {
            long ID;

#ifdef _PRINT_TEST
            printf("TEST - BLOCK %3ld gain = %f\n", kk, data.image[aoconfID_gainb].array.F[kk]);
            fflush(stdout);
#endif

            if(sprintf(name, "aol%ld_DMmodes%02ld", loop, kk) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            if(sprintf(fname, "conf/%s.fits", name) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            printf("FILE = %s\n", fname);
            printf("====== LOADING %s to %s\n", fname, name);
            fflush(stdout);
            if((ID=AOloopControl_IOtools_3Dloadcreate_shmim(name, fname, AOconf[loop].sizexDM, AOconf[loop].sizeyDM, 0, 0.0))!=-1)
                AOconf[loop].NBmodes_block[kk] = data.image[ID].md[0].size[2];

			int m;
			for(m=mstart; m<(mstart+AOconf[loop].NBmodes_block[kk]); m++)
				AOconf[loop].modeBlockIndex[m] = kk;
			mstart += AOconf[loop].NBmodes_block[kk];


            if(sprintf(name, "aol%ld_respM%02ld", loop, kk) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            if(sprintf(fname, "conf/%s.fits", name) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            printf("====== LOADING %s to %s\n", fname, name);
            fflush(stdout);
            AOloopControl_IOtools_3Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBmodes_block[kk], 0.0);


            if(sprintf(name, "aol%ld_contrM%02ld", loop, kk) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            if(sprintf(fname, "conf/%s.fits", name) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            printf("====== LOADING %s to %s\n", fname, name);
            fflush(stdout);
            AOloopControl_IOtools_3Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBmodes_block[kk], 0.0);


            if(sprintf(name, "aol%ld_contrMc%02ld", loop, kk) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            if(sprintf(fname, "conf/%s.fits", name) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            ID = AOloopControl_IOtools_3Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].sizexDM*AOconf[loop].sizeyDM, 0.0);
            if(kk==0)
                for(ii=0; ii<AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS*AOconf[loop].sizexDM*AOconf[loop].sizeyDM; ii++)
                    data.image[aoconfID_contrMc].array.F[ii] = 0.0;
            for(ii=0; ii<AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS*AOconf[loop].sizexDM*AOconf[loop].sizeyDM; ii++)
                data.image[aoconfID_contrMc].array.F[ii] += data.image[aoconfID_gainb].array.F[kk]*data.image[ID].array.F[ii];


            if(sprintf(name, "aol%ld_contrMcact%02ld_00", loop, kk) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            if(sprintf(fname, "conf/%s.fits", name) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            //   sprintf(fname, "conf/shmim_contrMcact%02ld_00", kk);
            printf("====== LOADING %s to %s  size %ld %ld\n", fname, name,  AOconf[loop].activeWFScnt, AOconf[loop].activeDMcnt);
            ID = AOloopControl_IOtools_2Dloadcreate_shmim(name, fname, AOconf[loop].activeWFScnt, AOconf[loop].activeDMcnt, 0.0);

            if(kk==0)
                for(ii=0; ii<AOconf[loop].activeWFScnt*AOconf[loop].activeDMcnt; ii++)
                    data.image[aoconfID_contrMcact[0]].array.F[ii] = 0.0;

            for(ii=0; ii<AOconf[loop].activeWFScnt*AOconf[loop].activeDMcnt; ii++)
                data.image[aoconfID_contrMcact[0]].array.F[ii] += data.image[aoconfID_gainb].array.F[kk]*data.image[ID].array.F[ii];

        }
    }
    free(sizearray);



    if(AOconf[loop].DMmodesNBblock==1)
        AOconf[loop].indexmaxMB[0] = AOconf[loop].NBDMmodes;
    else
    {
        AOconf[loop].indexmaxMB[0] = AOconf[loop].NBmodes_block[0];
        for(k=1; k<AOconf[loop].DMmodesNBblock; k++)
            AOconf[loop].indexmaxMB[k] = AOconf[loop].indexmaxMB[k-1] + AOconf[loop].NBmodes_block[k];
    }

    if(sprintf(fname, "./conf/param_blockoffset_%02ld.txt", (long) 0) < 1)
        printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
    fp = fopen(fname, "w");
    fprintf(fp, "   0\n");
    fprintf(fp, "%4ld\n", AOconf[loop].NBmodes_block[0]);
    fclose(fp);
    for(k=1; k<AOconf[loop].DMmodesNBblock; k++)
    {
        if(sprintf(fname, "./conf/param_blockoffset_%02ld.txt", k) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
        fp = fopen(fname, "w");
        fprintf(fp, "%4ld\n", AOconf[loop].indexmaxMB[k-1]);
        fprintf(fp, "%4ld\n", AOconf[loop].NBmodes_block[k]);
        fclose(fp);
    }



    list_image_ID();
    printf(" AOconf[loop].activeWFScnt = %ld\n", AOconf[loop].activeWFScnt );
    printf(" AOconf[loop].activeDMcnt = %ld\n", AOconf[loop].activeDMcnt );
    printf("   init_WFSref0    %d\n", AOconf[loop].init_wfsref0);
    printf("   init_RM        %d\n", AOconf[loop].init_RM);
    printf("   init_CM        %d\n", AOconf[loop].init_CM);


    AOconf[loop].init = 1;

    loadcreateshm_log = 0;
    fclose(fplog);

#ifdef AOLOOPCONTROL_LOGFUNC
	AOLOOPCONTROL_logfunc_level = 0;
    CORE_logFunctionCall( AOLOOPCONTROL_logfunc_level, AOLOOPCONTROL_logfunc_level_max, 1, __FUNCTION__, __LINE__, "");
#endif

    return(0);
}





/**
 * ## Purpose
 * 
 * Initialize memory of the loop  
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname    int
 *              value of the mode
 * 
 *
 *  
 */

/***  */

int_fast8_t AOloopControl_InitializeMemory(int mode)
{
    int SM_fd;
    struct stat file_stat;
    int create = 0;
    long loop;
    int tmpi;
    char imname[200];


#ifdef AOLOOPCONTROL_LOGFUNC
    AOLOOPCONTROL_logfunc_level = 0;
    CORE_logFunctionCall( AOLOOPCONTROL_logfunc_level, AOLOOPCONTROL_logfunc_level_max, 0, __FUNCTION__, __LINE__, "");
#endif

    loop = LOOPNUMBER;



    SM_fd = open(AOconfname, O_RDWR);
    if(SM_fd==-1)
    {
        printf("Cannot import file \"%s\" -> creating file\n", AOconfname);
        create = 1;
    }
    else
    {
        fstat(SM_fd, &file_stat);
        printf("File %s size: %zd\n", AOconfname, file_stat.st_size);
        if(file_stat.st_size!=sizeof(AOLOOPCONTROL_CONF)*NB_AOloopcontrol)
        {
            printf("File \"%s\" size is wrong -> recreating file\n", AOconfname);
            create = 1;
            close(SM_fd);
        }
    }

    if(create==1)
    {
        int result;

        SM_fd = open(AOconfname, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);

        if (SM_fd == -1) {
            perror("Error opening file for writing");
            exit(0);
        }

        result = lseek(SM_fd, sizeof(AOLOOPCONTROL_CONF)*NB_AOloopcontrol-1, SEEK_SET);
        if (result == -1) {
            close(SM_fd);
            perror("Error calling lseek() to 'stretch' the file");
            exit(0);
        }

        result = write(SM_fd, "", 1);
        if (result != 1) {
            close(SM_fd);
            perror("Error writing last byte of the file");
            exit(0);
        }
    }


    AOconf = (AOLOOPCONTROL_CONF*) mmap(0, sizeof(AOLOOPCONTROL_CONF)*NB_AOloopcontrol, PROT_READ | PROT_WRITE, MAP_SHARED, SM_fd, 0);
    if (AOconf == MAP_FAILED) {
        close(SM_fd);
        perror("Error mmapping the file");
        exit(0);
    }


    if((mode==0)||(create==1))
    {
        char cntname[200];

        AOconf[loop].on = 0;
        AOconf[loop].DMprimaryWriteON = 0;
        AOconf[loop].DMfilteredWriteON = 0;
        AOconf[loop].AUTOTUNE_LIMITS_ON = 0;
        AOconf[loop].AUTOTUNE_GAINS_ON = 0;
        AOconf[loop].ARPFon = 0;
        AOconf[loop].ARPFgainAutoMin = 0.99;
        AOconf[loop].ARPFgainAutoMax = 1.01;
        AOconf[loop].LOOPiteration = 0;
        AOconf[loop].cnt = 0;
        AOconf[loop].cntmax = 0;
        AOconf[loop].init_CMc = 0;

        if(sprintf(cntname, "aol%ld_logdata", loop) < 1) // contains loop count (cnt0) and loop gain
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        if((aoconfIDlogdata = image_ID(cntname))==-1)
        {
            uint32_t *sizearray;
            sizearray = (uint32_t*) malloc(sizeof(uint32_t)*2);
            sizearray[0] = 1;
            sizearray[1] = 1;
            aoconfIDlogdata = create_image_ID(cntname, 2, sizearray, _DATATYPE_FLOAT, 1, 0);
            free(sizearray);
        }
    }


    if(create==1)
    {
        for(loop=0; loop<NB_AOloopcontrol; loop++)
        {
            AOconf[loop].init = 0;
            AOconf[loop].on = 0;
            AOconf[loop].DMprimaryWriteON = 0;
            AOconf[loop].DMfilteredWriteON = 0;
            AOconf[loop].ARPFon = 0;
            AOconf[loop].LOOPiteration = 0;
            AOconf[loop].cnt = 0;
            AOconf[loop].cntmax = 0;
            AOconf[loop].maxlimit = 0.3;
            AOconf[loop].mult = 1.00;
            AOconf[loop].gain = 0.0;
            AOconf[loop].AUTOTUNE_LIMITS_perc = 1.0; // percentile threshold
            AOconf[loop].AUTOTUNE_LIMITS_mcoeff = 1.0; // multiplicative coeff
            AOconf[loop].AUTOTUNE_LIMITS_delta = 1.0e-3;
            AOconf[loop].ARPFgain = 0.0;
            AOconf[loop].ARPFgainAutoMin = 0.99;
            AOconf[loop].ARPFgainAutoMax = 1.01;
            AOconf[loop].WFSnormfloor = 0.0;
            AOconf[loop].framesAve = 1;
            AOconf[loop].DMmodesNBblock = 1;
            AOconf[loop].GPUusesem = 1;

            AOconf[loop].loopfrequ = 2000.0;
            AOconf[loop].hardwlatency = 0.0011;
            AOconf[loop].hardwlatency_frame = 2.2;
            AOconf[loop].complatency = 0.0001;
            AOconf[loop].complatency_frame = 0.2;
            AOconf[loop].wfsmextrlatency = 0.0003;
            AOconf[loop].wfsmextrlatency_frame = 0.6;
        }
    }
    else
    {
        for(loop=0; loop<NB_AOloopcontrol; loop++)
            if(AOconf[loop].init == 1)
            {
                printf("LIST OF ACTIVE LOOPS:\n");
                printf("----- Loop %ld   (%s) ----------\n", loop, AOconf[loop].name);
                printf("  WFS:  %s  [%ld]  %ld x %ld\n", AOconf[loop].WFSname, aoconfID_wfsim, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
                printf("   DM:  %s  [%ld]  %ld x %ld\n", AOconf[loop].dmCname, aoconfID_dmC, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);
                printf("DM RM:  %s  [%ld]  %ld x %ld\n", AOconf[loop].dmRMname, aoconfID_dmC, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);
            }
    }

    if(AOloopcontrol_meminit==0)
    {

        printf("INITIALIZING GPUset ARRAYS\n");
        fflush(stdout);

        GPUset0 = (int*) malloc(sizeof(int)*GPUcntMax);

        uint_fast16_t k;

        for(k=0; k<GPUcntMax; k++)
        {
            FILE *fp;
            char fname[200];

            if(sprintf(fname, "./conf/param_GPUset0dev%d.txt", (int) k) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            fp = fopen(fname, "r");
            if(fp!=NULL)
            {
                if(fscanf(fp, "%50d" , &tmpi) != 1)
                    printERROR(__FILE__, __func__, __LINE__, "Cannot read parameter from file");

                fclose(fp);
                GPUset0[k] = tmpi;
            }
            else
                GPUset0[k] = k;
        }


        GPUset1 = (int*) malloc(sizeof(int)*GPUcntMax);
        for(k=0; k<GPUcntMax; k++)
        {
            FILE *fp;
            char fname[200];

            if(sprintf(fname, "./conf/param_GPUset1dev%d.txt", (int) k) < 1)
                printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
            fp = fopen(fname, "r");
            if(fp!=NULL)
            {
                if(fscanf(fp, "%50d" , &tmpi) != 1)
                    printERROR(__FILE__, __func__, __LINE__, "Cannot read parameter from file");

                fclose(fp);
                GPUset1[k] = tmpi;
            }
            else
                GPUset1[k] = k;
        }
    }

    AOloopcontrol_meminit = 1;


#ifdef AOLOOPCONTROL_LOGFUNC
    AOLOOPCONTROL_logfunc_level = 0;
    CORE_logFunctionCall( AOLOOPCONTROL_logfunc_level, AOLOOPCONTROL_logfunc_level_max, 1, __FUNCTION__, __LINE__, "");
#endif

    return 0;
}



