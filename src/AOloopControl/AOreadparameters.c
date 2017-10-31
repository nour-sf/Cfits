/**
 * @file    AOreadparamaters.c
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
/** @brief Read parameter value - float, char or int                                               */
/* =============================================================================================== */

/**
 * ## Purpose
 * 
 * Read parameter value (float) from file 
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname	CHAR*
 * 				parameter name
 * 
 * @param[in]
 * defaultValue	FLOAT
 * 				default value if file conf/param_paramname.txt not found
 *
 * @param[in]
 * fplog		FILE*
 * 				log file. If NULL, do not log
 *  
 */
float AOloopControl_readParam_float(char *paramname, float defaultValue, FILE *fplog)
{
	FILE *fp;
	char fname[200];
	float value;
	int wParamFile = 0;
	
	sprintf(fname, "./conf/param_%s.txt", paramname);
	if((fp=fopen(fname, "r"))==NULL)
    {
        printf("WARNING: file %s missing\n", fname);
        value = defaultValue;
        wParamFile = 1;
    }
    else
    {
        if(fscanf(fp, "%50f", &value) != 1){
            printERROR(__FILE__,__func__,__LINE__, "Cannot read parameter for file");
			value = defaultValue;
			wParamFile = 1;
		}
        fclose(fp);
	}

	if(wParamFile == 1) // write file
	{
		fp = fopen(fname, "w");
		fprintf(fp, "%f", value);
		fclose(fp);
	}


    if(fplog!=NULL)
		fprintf(fplog, "parameter %20s = %f\n", paramname, value);
    
	
	return value;
}



/**
 * ## Purpose
 * 
 * Read parameter value (int) from file 
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname	CHAR*
 * 				parameter name
 * 
 * @param[in]
 * defaultValue	INT
 * 				default value if file conf/param_paramname.txt not found
 *
 * @param[in]
 * fplog		FILE*
 * 				log file. If NULL, do not log
 *  
 */
int AOloopControl_readParam_int(char *paramname, int defaultValue, FILE *fplog)
{
	FILE *fp;
	char fname[200];
	int value;
	int wParamFile = 0;
	
	sprintf(fname, "./conf/param_%s.txt", paramname);
	if((fp=fopen(fname, "r"))==NULL)
    {
        printf("WARNING: file %s missing\n", fname);
        value = defaultValue;
        wParamFile = 1;
    }
    else
    {
        if(fscanf(fp, "%50d", &value) != 1){
            printERROR(__FILE__,__func__,__LINE__, "Cannot read parameter for file");
			value = defaultValue;
			wParamFile = 1;
		}
        fclose(fp);
	}

	if(wParamFile == 1) // write file
	{
		fp = fopen(fname, "w");
		fprintf(fp, "%d", value);
		fclose(fp);
	}


    if(fplog!=NULL)
		fprintf(fplog, "parameter %20s = %d\n", paramname, value);
    
	
	return value;
}



/**
 * ## Purpose
 * 
 * Read parameter value (char*) from file 
 * 
 * ## Arguments
 * 
 * @param[in]
 * paramname	CHAR*
 * 				parameter name
 * 
 * @param[in]
 * defaultValue	CHAR*
 * 				default value if file conf/param_paramname.txt not found
 *
 * @param[in]
 * fplog		FILE*
 * 				log file. If NULL, do not log
 *  
 */
char* AOloopControl_readParam_string(char *paramname, char* defaultValue, FILE *fplog)
{
	FILE *fp;
	char fname[200];
	char* value = " ";
	int wParamFile = 0;
	
	sprintf(fname, "./conf/param_%s.txt", paramname);
	if((fp=fopen(fname, "r"))==NULL)
    {
        printf("WARNING: file %s missing\n", fname);
        strcpy(value, defaultValue);
        wParamFile = 1;
    }
    else
    {
        if(fscanf(fp, "%200s", value) != 1){
            printERROR(__FILE__,__func__,__LINE__, "Cannot read parameter for file");
			strcpy(value, defaultValue);
			wParamFile = 1;
		}
        fclose(fp);
	}

	if(wParamFile == 1) // write file
	{
		fp = fopen(fname, "w");
		fprintf(fp, "%s", value);
		fclose(fp);
	}


    if(fplog!=NULL)
		fprintf(fplog, "parameter %20s = %s\n", paramname, value);
    
	
	return value;
}
