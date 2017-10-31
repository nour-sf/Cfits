/**
 * @file    AOwfs.c
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
/** @name AOloopControl - 6. REAL TIME COMPUTING ROUTINES                                          */
/* =============================================================================================== */
/* =============================================================================================== */



// zero point offset loop
//
// args:
//  DM offset channel (shared memory)
//  zonal resp matrix (shared memory)
//  nominal wfs reference without offset (shared memory)
//  wfs reference to be updated (shared memory)
//
// computation triggered on semaphore wait on semaphore #1 of DM offset
//
// will run until SIGUSR1 received
//
// read LOOPiteration from shared memory stream "aol#_LOOPiteration" if available 
//
int_fast8_t AOloopControl_WFSzpupdate_loop(const char *IDzpdm_name, const char *IDzrespM_name, const char *IDwfszp_name)
{
    long IDzpdm, IDzrespM, IDwfszp;
    uint32_t dmxsize, dmysize, dmxysize;
    long wfsxsize, wfsysize, wfsxysize;
    long IDtmp;
    long elem, act;
    long zpcnt = 0;
    long zpcnt0;
    int semval;
    struct timespec t1;
    struct timespec t2;

	char imname[200];
	

	if(aoconfID_looptiming == -1)
	{
		// LOOPiteration is written in cnt1 of loop timing array
		if(sprintf(imname, "aol%ld_looptiming", LOOPNUMBER) < 1)
			printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
		aoconfID_looptiming = AOloopControl_IOtools_2Dloadcreate_shmim(imname, " ", AOcontrolNBtimers, 1, 0.0);
	}

    IDzpdm = image_ID(IDzpdm_name);

    if(data.image[IDzpdm].md[0].sem<2) // if semaphore #1 does not exist, create it
        COREMOD_MEMORY_image_set_createsem(IDzpdm_name, 2);


    IDzrespM = image_ID(IDzrespM_name);
    IDwfszp = image_ID(IDwfszp_name);


    // array sizes extracted from IDzpdm and IDwfsref

    dmxsize = data.image[IDzpdm].md[0].size[0];
    dmysize = data.image[IDzpdm].md[0].size[1];
    dmxysize = dmxsize*dmysize;
    wfsxsize = data.image[IDwfszp].md[0].size[0];
    wfsysize = data.image[IDwfszp].md[0].size[1];
    wfsxysize = wfsxsize*wfsysize;

    // VERIFY SIZES

    // verify zrespM
    if(data.image[IDzrespM].md[0].size[0]!=wfsxsize)
    {
        printf("ERROR: zrespM xsize %ld does not match wfsxsize %ld\n", (long) data.image[IDzrespM].md[0].size[0], (long) wfsxsize);
        exit(0);
    }
    if(data.image[IDzrespM].md[0].size[1]!=wfsysize)
    {
        printf("ERROR: zrespM ysize %ld does not match wfsysize %ld\n", (long) data.image[IDzrespM].md[0].size[1], (long) wfsysize);
        exit(0);
    }
    if(data.image[IDzrespM].md[0].size[2]!=dmxysize)
    {
        printf("ERROR: zrespM zsize %ld does not match wfsxysize %ld\n", (long) data.image[IDzrespM].md[0].size[1], (long) wfsxysize);
        exit(0);
    }


    IDtmp = create_2Dimage_ID("wfsrefoffset", wfsxsize, wfsysize);


    zpcnt0 = 0;

    if(data.image[IDzpdm].md[0].sem > 1) // drive semaphore #1 to zero
        while(sem_trywait(data.image[IDzpdm].semptr[1])==0) {}
    else
    {
        printf("ERROR: semaphore #1 missing from image %s\n", IDzpdm_name);
        exit(0);
    }

    while(data.signal_USR1==0)
    {
        memset(data.image[IDtmp].array.F, '\0', sizeof(float)*wfsxysize);

        while(zpcnt0 == data.image[IDzpdm].md[0].cnt0)
            usleep(10);

        zpcnt0 = data.image[IDzpdm].md[0].cnt0;

        // TO BE DONE
        //  sem_wait(data.image[IDzpdm].semptr[1]);


        printf("WFS zero point offset update  # %8ld       (%s -> %s)  ", zpcnt, data.image[IDzpdm].name, data.image[IDwfszp].name);
        fflush(stdout);


        clock_gettime(CLOCK_REALTIME, &t1);

# ifdef _OPENMP
        #pragma omp parallel for private(elem)
# endif
        for(act=0; act<dmxysize; act++)
            for(elem=0; elem<wfsxysize; elem++)
                data.image[IDtmp].array.F[elem] += data.image[IDzpdm].array.F[act]*data.image[IDzrespM].array.F[act*wfsxysize+elem];


        clock_gettime(CLOCK_REALTIME, &t2);
        tdiff = info_time_diff(t1, t2);
        tdiffv = 1.0*tdiff.tv_sec + 1.0e-9*tdiff.tv_nsec;

        printf(" [ %10.3f ms]\n", 1e3*tdiffv);
        fflush(stdout);


        // copy results to IDwfszpo
        data.image[IDwfszp].md[0].write = 1;
        memcpy(data.image[IDwfszp].array.F, data.image[IDtmp].array.F, sizeof(float)*wfsxysize);
        COREMOD_MEMORY_image_set_sempost_byID(IDwfszp, -1);
        data.image[IDwfszp].md[0].cnt0 ++;
        data.image[IDwfszp].md[0].cnt1 = data.image[aoconfID_looptiming].md[0].cnt1;
        data.image[IDwfszp].md[0].write = 0;

        zpcnt++;
    }

    return 0;
}




//
// Create zero point WFS channels
// watch semaphore 1 on output (IDwfsref_name) -> sum all channels to update WFS zero point
// runs in separate process from RT computation
//
//
//
int_fast8_t AOloopControl_WFSzeropoint_sum_update_loop(long loopnb, const char *ID_WFSzp_name, int NBzp, const char *IDwfsref0_name, const char *IDwfsref_name)
{
    long wfsxsize, wfsysize, wfsxysize;
    long IDwfsref, IDwfsref0;
    long *IDwfszparray;
    long cntsumold;
    int RT_priority = 95; //any number from 0-99
    struct sched_param schedpar;
    long nsecwait = 10000; // 10 us
    struct timespec semwaitts;
    long ch;
    long IDtmp;
    long ii;
    char imname[200];
    int semval;



	if(aoconfID_looptiming == -1)
	{
		// LOOPiteration is written in cnt1 of loop timing array
		if(sprintf(imname, "aol%ld_looptiming", LOOPNUMBER) < 1)
			printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");
		aoconfID_looptiming = AOloopControl_IOtools_2Dloadcreate_shmim(imname, " ", AOcontrolNBtimers, 1, 0.0);
	}


    schedpar.sched_priority = RT_priority;
#ifndef __MACH__
    if(seteuid(euid_called) != 0) //This goes up to maximum privileges
        printERROR(__FILE__, __func__, __LINE__, "seteuid() returns non-zero value");

    sched_setscheduler(0, SCHED_FIFO, &schedpar); //other option is SCHED_RR, might be faster

    if(seteuid(euid_real) != 0) //Go back to normal privileges
        printERROR(__FILE__, __func__, __LINE__, "seteuid() returns non-zero value");
#endif

    IDwfsref = image_ID(IDwfsref_name);
    wfsxsize = data.image[IDwfsref].md[0].size[0];
    wfsysize = data.image[IDwfsref].md[0].size[1];
    wfsxysize = wfsxsize*wfsysize;
    IDtmp = create_2Dimage_ID("wfsrefoffset", wfsxsize, wfsysize);
    IDwfsref0 = image_ID(IDwfsref0_name);


    if(data.image[IDwfsref].md[0].sem > 1) // drive semaphore #1 to zero
        while(sem_trywait(data.image[IDwfsref].semptr[1])==0) {}
    else
    {
        printf("ERROR: semaphore #1 missing from image %s\n", IDwfsref_name);
        exit(0);
    }

    IDwfszparray = (long*) malloc(sizeof(long)*NBzp);
    // create / read the zero point WFS channels
    for(ch=0; ch<NBzp; ch++)
    {
        if(sprintf(imname, "%s%ld", ID_WFSzp_name, ch) < 1)
            printERROR(__FILE__, __func__, __LINE__, "sprintf wrote <1 char");

        AOloopControl_IOtools_2Dloadcreate_shmim(imname, "", wfsxsize, wfsysize, 0.0);
        COREMOD_MEMORY_image_set_createsem(imname, 10);
        IDwfszparray[ch] = image_ID(imname);
    }

    cntsumold = 0;
    for(;;)
    {
        if (clock_gettime(CLOCK_REALTIME, &semwaitts) == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        semwaitts.tv_nsec += nsecwait;
        if(semwaitts.tv_nsec >= 1000000000)
            semwaitts.tv_sec = semwaitts.tv_sec + 1;

        sem_timedwait(data.image[IDwfsref].semptr[1], &semwaitts);

        long cntsum = 0;
        for(ch=0; ch<NBzp; ch++)
            cntsum += data.image[IDwfszparray[ch]].md[0].cnt0;


        if(cntsum != cntsumold)
        {
			// copy wfsref0 to tmp
            memcpy(data.image[IDtmp].array.F, data.image[IDwfsref0].array.F, sizeof(float)*wfsxysize);

            for(ch=0; ch<NBzp; ch++)
                for(ii=0; ii<wfsxysize; ii++)
                    data.image[IDtmp].array.F[ii] += data.image[IDwfszparray[ch]].array.F[ii];

            // copy results to IDwfsref
            data.image[IDwfsref].md[0].write = 1;
            memcpy(data.image[IDwfsref].array.F, data.image[IDtmp].array.F, sizeof(float)*wfsxysize);
            data.image[IDwfsref].md[0].cnt0 ++;
            data.image[IDwfsref].md[0].cnt1 = data.image[aoconfID_looptiming].md[0].cnt1;
            data.image[IDwfsref].md[0].write = 0;

/*            sem_getvalue(data.image[IDwfsref].semptr[0], &semval); // do not update sem 1
            if(semval<SEMAPHORE_MAXVAL)
                COREMOD_MEMORY_image_set_sempost(IDwfsref_name, 0);*/
            COREMOD_MEMORY_image_set_sempost_excl_byID(IDwfsref, 1);
            

            cntsumold = cntsum;
        }
    }

    free(IDwfszparray);


    return(0);
}



int_fast8_t ControlMatrixMultiply( float *cm_array, float *imarray, long m, long n, float *outvect)
{
    long i;

    cblas_sgemv (CblasRowMajor, CblasNoTrans, m, n, 1.0, cm_array, n, imarray, 1, 0.0, outvect, 1);

    return(0);
}



/**
 * ## Purpose
 * 
 * Send modal commands to DM. \n
 * Converts mode coefficient to DM map by matrix-vector multiplication \n
 * Runs in CPU or GPU.
 * 
 * Takes mode values from aol_DMmode_cmd (ID = aoconfID_cmd_modes)
 * 
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname	long
 * 				number of the loop 
 *
 */ 
int_fast8_t set_DM_modes(long loop)
{
    double a;
    long cnttest;
    int semval;



	

    if(AOconf[loop].GPU1 == 0)
    {
        float *arrayf;
        long i, j, k;

        arrayf = (float*) malloc(sizeof(float)*AOconf[loop].sizeDM);

        for(j=0; j<AOconf[loop].sizeDM; j++)
            arrayf[j] = 0.0;

        for(i=0; i<AOconf[loop].sizeDM; i++)
            for(k=0; k < AOconf[loop].NBDMmodes; k++)
                arrayf[i] += data.image[aoconfID_cmd_modes].array.F[k] * data.image[aoconfID_DMmodes].array.F[k*AOconf[loop].sizeDM+i];

        data.image[aoconfID_dmC].md[0].write = 1;
        memcpy (data.image[aoconfID_dmC].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
        if(data.image[aoconfID_dmC].md[0].sem > 0)
        {
            sem_getvalue(data.image[aoconfID_dmC].semptr[0], &semval);
            if(semval<SEMAPHORE_MAXVAL)
                sem_post(data.image[aoconfID_dmC].semptr[0]);
        }
        data.image[aoconfID_dmC].md[0].cnt0++;
		data.image[aoconfID_dmC].md[0].cnt1 = AOconf[loop].LOOPiteration;
        data.image[aoconfID_dmC].md[0].write = 0;

        free(arrayf);
    }
    else
    {
#ifdef HAVE_CUDA


        GPU_loop_MultMat_setup(1, data.image[aoconfID_DMmodes].name, data.image[aoconfID_cmd_modes].name, data.image[aoconfID_dmC].name, AOconf[loop].GPU1, GPUset1, 1, AOconf[loop].GPUusesem, 1, loop);
        AOconf[loop].status = 12;
        clock_gettime(CLOCK_REALTIME, &tnow);
        tdiff = info_time_diff(data.image[aoconfID_looptiming].md[0].atime.ts, tnow);
        tdiffv = 1.0*tdiff.tv_sec + 1.0e-9*tdiff.tv_nsec;
        data.image[aoconfID_looptiming].array.F[32] = tdiffv;

        GPU_loop_MultMat_execute(1, &AOconf[loop].status, &AOconf[loop].GPUstatus[0], 1.0, 0.0, 1, 30);
#endif
    }

    if(aoconfID_dmdisp!=-1)
        if(data.image[aoconfID_dmdisp].md[0].sem > 1)
        {
            sem_getvalue(data.image[aoconfID_dmdisp].semptr[1], &semval);
            if(semval<SEMAPHORE_MAXVAL)
                sem_post(data.image[aoconfID_dmdisp].semptr[1]);
        }

    AOconf[loop].DMupdatecnt ++;

    return(0);
}





/**
 * ## Purpose
 * 
 * Set deformable mirror modes related to the response matrix 
 * 
 * Takes mode values from ????????,
 * 
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname	long
 * 				number of the loop 
 *
 */ 

int_fast8_t set_DM_modesRM(long loop)
{
    long k;
    long i, j;
    float *arrayf;


    arrayf = (float*) malloc(sizeof(float)*AOconf[loop].sizeDM);

    for(j=0; j<AOconf[loop].sizeDM; j++)
        arrayf[j] = 0.0;

    for(k=0; k < AOconf[loop].NBDMmodes; k++)
    {
        for(i=0; i<AOconf[loop].sizeDM; i++)
            arrayf[i] += data.image[aoconfID_cmd_modesRM].array.F[k] * data.image[aoconfID_DMmodes].array.F[k*AOconf[loop].sizeDM+i];
    }


    data.image[aoconfID_dmRM].md[0].write = 1;
    memcpy (data.image[aoconfID_dmRM].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
    data.image[aoconfID_dmRM].md[0].cnt0++;
    data.image[aoconfID_dmRM].md[0].write = 0;

    free(arrayf);
    AOconf[loop].DMupdatecnt ++;

    return(0);
}



