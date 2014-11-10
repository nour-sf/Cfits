#include <fitsio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <sched.h>
#include <ncurses.h>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>




#include "CLIcore.h"
#include "00CORE/00CORE.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_arith/COREMOD_arith.h"
#include "linopt_imtools/linopt_imtools.h"
#include "AOloopControl/AOloopControl.h"
#include "image_filter/image_filter.h"

#include "ZernikePolyn/ZernikePolyn.h"


#ifdef HAVE_CUDA
#include "cudacomp/cudacomp.h"
#endif


# ifdef _OPENMP
# include <omp.h>
#define OMP_NELEMENT_LIMIT 1000000
# endif


int wcol, wrow; // window size


long aoconfID_WFS = -1;
long aoconfID_WFSdark = -1;
long aoconfID_WFS0 = -1;
long aoconfID_WFS1 = -1;
long aoconfID_WFS2 = -1;
long aoconfID_refWFS = -1;
long aoconfID_DM = -1;
long aoconfID_DMRM = -1;
long aoconfID_DMmodes = -1;


// Fourier Modes
long aoconfID_cmd_modes = -1;
long aoconfID_cmd1_modes = -1;
long aoconfID_RMS_modes = -1;
long aoconfID_AVE_modes = -1;
long aoconfID_GAIN_modes = -1;
long aoconfID_LIMIT_modes = -1;
long aoconfID_MULTF_modes = -1;




long aoconfID_cmd_modesRM = -1;

long aoconfID_respM = -1;
long aoconfID_contrM = -1;

long aoconfIDlog0 = -1;
long aoconfIDlog1 = -1;




int RMACQUISITION = 0;  // toggles to 1 when resp matrix is being acquired




extern DATA data;


#define NB_AOloopcontrol 10 // max number of loops
long LOOPNUMBER = 0; // current loop index
int AOloopcontrol_meminit = 0;
int AOlooploadconf_init = 0;

#define AOconfname "/tmp/AOconf.shm"
AOLOOPCONTROL_CONF *AOconf; // configuration - this can be an array
int *AOconf_loaded = 0;
int *AOconf_fd; 

float *arrayftmp;
unsigned short *arrayutmp;
int avcamarraysInit = 0;


// CLI commands
//
// function CLI_checkarg used to check arguments
// 1: float
// 2: long
// 3: string
// 4: existing image
//


int AOloopControl_makeTemplateAOloopconf_cli()
{
    if(CLI_checkarg(1,2)==0)
        AOloopControl_makeTemplateAOloopconf(data.cmdargtoken[1].val.numl);
    else
        return 1;
}


int AOloopControl_mkModes_cli()
{ 
  if(CLI_checkarg(1,3)+CLI_checkarg(2,2)+CLI_checkarg(3,1)+CLI_checkarg(4,1)+CLI_checkarg(5,1)+CLI_checkarg(6,1)+CLI_checkarg(7,1)+CLI_checkarg(8,1)==0)
    AOloopControl_mkModes(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numf, data.cmdargtoken[6].val.numf, data.cmdargtoken[7].val.numf, data.cmdargtoken[8].val.numf);
  else
    return 1;      
}

int AOloopControl_camimage_extract2D_sharedmem_loop_cli()
{
  if(CLI_checkarg(1,4)+CLI_checkarg(2,3)+CLI_checkarg(3,2)+CLI_checkarg(4,2)+CLI_checkarg(5,2)+CLI_checkarg(6,2)==0)
    {
      AOloopControl_camimage_extract2D_sharedmem_loop(data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.string , data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl, data.cmdargtoken[5].val.numl, data.cmdargtoken[6].val.numl);
      return 0;
    }
  else
    return 1;
}


int AOloopControl_loadconfigure_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,3)==0)
    {
      AOloopControl_loadconfigure(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string, 1);
      return 0;
    }
  else
    return 1;
}

int AOloopControl_setLoopNumber_cli()
{
  if(CLI_checkarg(1,2)==0)
    {
      AOloopControl_setLoopNumber(data.cmdargtoken[1].val.numl);
      return 0;
    }
  else
    return 1;
}


int AOloopControl_setgain_cli()
{
  if(CLI_checkarg(1,1)==0)
    {
      AOloopControl_setgain(data.cmdargtoken[1].val.numf);
      return 0;
    }
  else
    return 1;
}

int AOloopControl_setmaxlimit_cli()
{
  if(CLI_checkarg(1,1)==0)
    {
      AOloopControl_setmaxlimit(data.cmdargtoken[1].val.numf);
      return 0;
    }
  else
    return 1;
}

int Measure_ActMap_WFS_cli()
{
	if(CLI_checkarg(1,1)+CLI_checkarg(2,1)+CLI_checkarg(3,2)+CLI_checkarg(4,3)==0)
		{
			Measure_ActMap_WFS(LOOPNUMBER, data.cmdargtoken[1].val.numf, data.cmdargtoken[2].val.numf, data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.string);
			return 0;
    }
  else
    return 1;
}



int AOloopControl_Measure_WFScam_PeriodicError_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,3)==0)
    {
      AOloopControl_Measure_WFScam_PeriodicError(LOOPNUMBER, data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.string);
      return 0;
    }
  else
    return 1;
}




int AOloopControl_Measure_Resp_Matrix_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,1)+CLI_checkarg(3,2)+CLI_checkarg(4,2)+CLI_checkarg(5,2)==0)
    {
      Measure_Resp_Matrix(LOOPNUMBER, data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf, data.cmdargtoken[3].val.numl, data.cmdargtoken[4].val.numl, data.cmdargtoken[5].val.numl);
      return 0;
    }
  else
    return 1;
}




int AOloopControl_setframesAve_cli()
{
  if(CLI_checkarg(1,2)==0)
    {
      AOloopControl_setframesAve(data.cmdargtoken[1].val.numl);
      return 0;
    }
  else
    return 1;
}


int AOloopControl_computeCM_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,4)+CLI_checkarg(3,3)+CLI_checkarg(4,1)+CLI_checkarg(5,2)==0)
    {
      compute_ControlMatrix(LOOPNUMBER, data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.string, data.cmdargtoken[3].val.string, "evecM", data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numl);
      save_fits("evecM","!evecM.fits");
      delete_image_ID("evecM");
    }
  else
    return 1;
}


int AOloopControl_loadCM_cli()
{
  if(CLI_checkarg(1,3)==0)
    {
      AOloopControl_loadCM(LOOPNUMBER, data.cmdargtoken[1].val.string);
      return 0;
    }
  else
    return 1;
}




int AOloopControl_loopstep_cli()
{
  if(CLI_checkarg(1,2)==0)
    {
      AOloopControl_loopstep(LOOPNUMBER, data.cmdargtoken[1].val.numl);
      return 0;
    }
  else
    return 1;
}




int AOloopControl_loopMonitor_cli()
{
 if(CLI_checkarg(1,1)+CLI_checkarg(2,2)==0)
   {
     AOloopControl_loopMonitor(LOOPNUMBER, data.cmdargtoken[1].val.numf, data.cmdargtoken[2].val.numl);
     return 0;
   }
 else
   {
     AOloopControl_loopMonitor(LOOPNUMBER, 1.0, 8);
     return 0;
   }
}


int AOloopControl_setgainrange_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0)
    {
      AOloopControl_setgainrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_setlimitrange_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0)
    {
      AOloopControl_setlimitrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_setmultfrange_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)==0)
    {
      AOloopControl_setmultfrange(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf);
      return 0;
    }
  else
    return 1; 
}



int AOloopControl_setgainblock_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0)
    {
      AOloopControl_setgainblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_setlimitblock_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0)
    {
      AOloopControl_setlimitblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_setmultfblock_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0)
    {
      AOloopControl_setmultfblock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
      return 0;
    }
  else
    return 1; 
}




int AOloopControl_InjectMode_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,1)==0)
    {
      AOloopControl_InjectMode(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numf);
      return 0;
    }
  else
    return 1; 
}



int AOloopControl_scanGainBlock_cli()
{
  if(CLI_checkarg(1,2)+CLI_checkarg(2,2)+CLI_checkarg(3,1)+CLI_checkarg(4,1)+CLI_checkarg(5,2)==0)
    {
      AOloopControl_scanGainBlock(data.cmdargtoken[1].val.numl, data.cmdargtoken[2].val.numl, data.cmdargtoken[3].val.numf, data.cmdargtoken[4].val.numf, data.cmdargtoken[5].val.numl);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_tuneWFSsync_cli()
{
  if(CLI_checkarg(1,3)==0)
    {
      AOloopControl_tuneWFSsync(LOOPNUMBER, data.cmdargtoken[1].val.string);
      return 0;
    }
  else
    return 1; 
}


int AOloopControl_setparam_cli()
{
 if(CLI_checkarg(1,3)+CLI_checkarg(2,1)==0)
    {
		AOloopControl_setparam(LOOPNUMBER, data.cmdargtoken[1].val.string, data.cmdargtoken[2].val.numf);
      return 0;
    }
  else
    return 1; 
}




int init_AOloopControl()
{
  FILE *fp;
  int r;

  if((fp=fopen("loopnb.txt","r"))!=NULL)
    {
      r = fscanf(fp,"%ld", &LOOPNUMBER);
      printf("LOOP NUMBER = %ld\n", LOOPNUMBER);
      fclose(fp);
    }
  else
    LOOPNUMBER = 0;
  

  strcpy(data.module[data.NBmodule].name, __FILE__);
  strcpy(data.module[data.NBmodule].info, "AO loop control");
  data.NBmodule++;


  strcpy(data.cmd[data.NBcmd].key,"aolmkconf");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_makeTemplateAOloopconf_cli;
  strcpy(data.cmd[data.NBcmd].info,"make template configuration file");
  strcpy(data.cmd[data.NBcmd].syntax,"<loopnb [long]>");
  strcpy(data.cmd[data.NBcmd].example,"aolmkconf 2");
  strcpy(data.cmd[data.NBcmd].Ccall,"long AOloopControl_makeTemplateAOloopconf(long loopnb)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolmkmodes");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_mkModes_cli;
  strcpy(data.cmd[data.NBcmd].info,"make control modes");
  strcpy(data.cmd[data.NBcmd].syntax,"<output modes> <size> <max CPA> <delta CPA> <cx> <cy> <r0> <r1>");
  strcpy(data.cmd[data.NBcmd].example,"aolmkmodes modes 50 5.0 0.8");
  strcpy(data.cmd[data.NBcmd].Ccall,"long AOloopControl_mkModes(char *ID_name, long msize, float CPAmax, float deltaCPA, double xc, double yx, double r0, double r1)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"cropshim");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_camimage_extract2D_sharedmem_loop_cli;
  strcpy(data.cmd[data.NBcmd].info,"crop shared mem image");
  strcpy(data.cmd[data.NBcmd].syntax,"<input image> <output image>");
  strcpy(data.cmd[data.NBcmd].example,"cropshim imin imout");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_camimage_extract2D_sharedmem_loop(char *in_name, char *out_name, long size_x, long size_y, long xstart, long ystart)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolloadconf");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loadconfigure_cli;
  strcpy(data.cmd[data.NBcmd].info,"load AO loop configuration from file");
  strcpy(data.cmd[data.NBcmd].syntax,"<loop #> <conf file>");
  strcpy(data.cmd[data.NBcmd].example,"AOlooploadconf 1 aoloop1.conf");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loadconfigure(long loopnb, char *fname, 1)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolmeasactmap");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = Measure_ActMap_WFS_cli;
  strcpy(data.cmd[data.NBcmd].info,"measure AO loop map of actuator influences");
  strcpy(data.cmd[data.NBcmd].syntax,"<ampl [float]> <delay second [float]> <nb frames per position [long]> <output image [string]>");
  strcpy(data.cmd[data.NBcmd].example,"aolameasactmap 0.05 0.02 20 actmap");
  strcpy(data.cmd[data.NBcmd].Ccall,"long Measure_ActMap_WFS(long loop, double ampl, double delays, long NBave, char *WFS_actmap)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolacqwfscampe");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_Measure_WFScam_PeriodicError_cli;
  strcpy(data.cmd[data.NBcmd].info,"acquire WFS camera periodic error");
  strcpy(data.cmd[data.NBcmd].syntax,"<nbframes [long]> <nb pha [long]> <outcube>");
  strcpy(data.cmd[data.NBcmd].example,"aolacqwfscampe 10000 100 wfscampe");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_Measure_WFScam_PeriodicError(long loop, long NBframes, long NBpha, char *IDout_name)");
  data.NBcmd++;



  strcpy(data.cmd[data.NBcmd].key,"aolacqresp");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_Measure_Resp_Matrix_cli;
  strcpy(data.cmd[data.NBcmd].info,"acquire AO response matrix and WFS reference");
  strcpy(data.cmd[data.NBcmd].syntax,"<ave# [long]> <ampl [float]> <nbloop [long]> <frameDelay [long]> <NBiter [long]>");
  strcpy(data.cmd[data.NBcmd].example,"aolacqresp 50 0.1 5 2");
  strcpy(data.cmd[data.NBcmd].Ccall,"int Measure_Resp_Matrix(long loop, long NbAve, float amp, long nbloop, long fDelay, long NBiter)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolrun");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_run;
  strcpy(data.cmd[data.NBcmd].info,"run AO loop");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"AOlooprun");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_run()");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolkill");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopkill;
  strcpy(data.cmd[data.NBcmd].info,"kill AO loop");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolkill");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setLoopNumber()");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolnb");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setLoopNumber_cli;
  strcpy(data.cmd[data.NBcmd].info,"set AO loop #");
  strcpy(data.cmd[data.NBcmd].syntax,"<loop nb>");
  strcpy(data.cmd[data.NBcmd].example,"AOloopnb 0");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setLoopNumber(long loop)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolon");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopon;
  strcpy(data.cmd[data.NBcmd].info,"turn loop on");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolon");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loopon()");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolstep");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopstep_cli;
  strcpy(data.cmd[data.NBcmd].info,"turn loop on for N steps");
  strcpy(data.cmd[data.NBcmd].syntax,"<nbstep>");
  strcpy(data.cmd[data.NBcmd].example,"aolstep");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loopstep(long loop, long NBstep)");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aoloff");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopoff;
  strcpy(data.cmd[data.NBcmd].info,"turn loop off");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aoloff");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loopoff()");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolreset");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopreset;
  strcpy(data.cmd[data.NBcmd].info,"reset loop, and turn it off");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolreset");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loopreset()");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aollogon");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_logon;
  strcpy(data.cmd[data.NBcmd].info,"turn log on");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aollogon");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_logon()");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aollogoff");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_logoff;
  strcpy(data.cmd[data.NBcmd].info,"turn log off");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aollogoff");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_logoff()");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolsetgain");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setgain_cli;
  strcpy(data.cmd[data.NBcmd].info,"set gain");
  strcpy(data.cmd[data.NBcmd].syntax,"<gain value>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetgain 0.1");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setgain(float gain)");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolsetmaxlim");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setmaxlimit_cli;
  strcpy(data.cmd[data.NBcmd].info,"set max limit for AO mode correction");
  strcpy(data.cmd[data.NBcmd].syntax,"<limit value>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetmaxlim 0.01");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setmaxlimit(float maxlimit)");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolsetnbfr");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setframesAve_cli;
  strcpy(data.cmd[data.NBcmd].info,"set number of frames to be averaged");
  strcpy(data.cmd[data.NBcmd].syntax,"<nb frames>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetnbfr 10");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setframesAve(long nbframes)");
  data.NBcmd++;
  

  strcpy(data.cmd[data.NBcmd].key,"aolcmmake");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_computeCM_cli;
  strcpy(data.cmd[data.NBcmd].info,"make control matrix");
  strcpy(data.cmd[data.NBcmd].syntax,"<NBmodes removed> <RespMatrix> <ContrMatrix>");
  strcpy(data.cmd[data.NBcmd].example,"aolcmmake 8 respm cmat");
  strcpy(data.cmd[data.NBcmd].Ccall,"int compute_ControlMatrix(long loop, long NB_MODE_REMOVED, char *ID_Rmatrix_name, char *ID_Cmatrix_name, char *ID_VTmatrix_name)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolloadcm");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loadCM_cli;
  strcpy(data.cmd[data.NBcmd].info,"load new control matrix from file");
  strcpy(data.cmd[data.NBcmd].syntax,"<fname>");
  strcpy(data.cmd[data.NBcmd].example,"aolloadcm cm32.fits");
  strcpy(data.cmd[data.NBcmd].Ccall,"long AOloopControl_loadCM(long loop, char *CMfname)");
  data.NBcmd++;
  
  strcpy(data.cmd[data.NBcmd].key,"aolmon");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_loopMonitor_cli;
  strcpy(data.cmd[data.NBcmd].info,"monitor loop");
  strcpy(data.cmd[data.NBcmd].syntax,"<frequ> <Nbcols>");
  strcpy(data.cmd[data.NBcmd].example,"aolmon 10.0 3");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_loopMonitor(long loop, double frequ)");
  data.NBcmd++;
 
  strcpy(data.cmd[data.NBcmd].key,"aolsetgainr");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setgainrange_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal gains");
  strcpy(data.cmd[data.NBcmd].syntax,"<modemin [long]> <modemax [long]> <gainval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetgainr 20 30 0.2");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setgainrange(long m0, long m1, float gainval)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsetlimitr");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setlimitrange_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal limits");
  strcpy(data.cmd[data.NBcmd].syntax,"<modemin [long]> <modemax [long]> <limval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetlimitr 20 30 0.02");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setlimitrange(long m0, long m1, float gainval)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsetmultfr");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setmultfrange_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal multf");
  strcpy(data.cmd[data.NBcmd].syntax,"<modemin [long]> <modemax [long]> <multfval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetmultfr 10 30 0.98");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setmultfrange(long m0, long m1, float multfval)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsetgainb");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setgainblock_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal gains by block");
  strcpy(data.cmd[data.NBcmd].syntax,"<block [long]> <gainval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetgainb 2 0.2");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setgainblock(long m0, long m1, float gainval)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsetlimitb");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setlimitblock_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal limits by block");
  strcpy(data.cmd[data.NBcmd].syntax,"<block [long]> <limval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetlimitb 2 0.02");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setlimitblock(long m0, long m1, float gainval)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsetmultfb");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setmultfblock_cli;
  strcpy(data.cmd[data.NBcmd].info,"set modal multf by block");
  strcpy(data.cmd[data.NBcmd].syntax,"<block [long]> <multfval>");
  strcpy(data.cmd[data.NBcmd].example,"aolsetmultfb 2 0.98");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setmultfblock(long m0, long m1, float multfval)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolresetrms");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_resetRMSperf;
  strcpy(data.cmd[data.NBcmd].info,"reset RMS performance monitor");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolresetrms");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_resetRMSperf()");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolscangainb");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_scanGainBlock_cli;
  strcpy(data.cmd[data.NBcmd].info,"scan gain for block");
  strcpy(data.cmd[data.NBcmd].syntax,"<blockNB> <NBAOsteps> <gainstart> <gainend> <NBgainpts>");
  strcpy(data.cmd[data.NBcmd].example,"aolscangainb");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_scanGainBlock(long NBblock, long NBstep, float gainStart, float gainEnd, long NBgain)");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolstatusstats");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_statusStats;
  strcpy(data.cmd[data.NBcmd].info,"measures distribution of status values");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolstatusstats");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_statusStats()");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolinjectmode");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_InjectMode_cli;
  strcpy(data.cmd[data.NBcmd].info,"inject single mode error into RM channel");
  strcpy(data.cmd[data.NBcmd].syntax,"<index> <ampl>");
  strcpy(data.cmd[data.NBcmd].example,"aolinjectmode 20 0.1");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_InjectMode()");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolautotune");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_AutoTune;
  strcpy(data.cmd[data.NBcmd].info,"auto tuning of loop parameters");
  strcpy(data.cmd[data.NBcmd].syntax,"no arg");
  strcpy(data.cmd[data.NBcmd].example,"aolautotune");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_AutoTune()");
  data.NBcmd++;

  strcpy(data.cmd[data.NBcmd].key,"aolsyncwfs");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_tuneWFSsync_cli;
  strcpy(data.cmd[data.NBcmd].info,"tune WFS modulation to camera speed");
  strcpy(data.cmd[data.NBcmd].syntax,"<out image>");
  strcpy(data.cmd[data.NBcmd].example,"aolsyncwfs");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_tuneWFSsync(long loop, char *IDout_name)");
  data.NBcmd++;


  strcpy(data.cmd[data.NBcmd].key,"aolset");
  strcpy(data.cmd[data.NBcmd].module,__FILE__);
  data.cmd[data.NBcmd].fp = AOloopControl_setparam_cli;
  strcpy(data.cmd[data.NBcmd].info,"set parameter");
  strcpy(data.cmd[data.NBcmd].syntax,"<parameter> <value>");
  strcpy(data.cmd[data.NBcmd].example,"aolset");
  strcpy(data.cmd[data.NBcmd].Ccall,"int AOloopControl_setparam(long loop, char *key, double value)");
  data.NBcmd++;

  
  
  

  // add atexit functions here
  // atexit((void*) SCEXAO_DM_unloadconf);
  
  return 0;
}








long AOloopControl_makeTemplateAOloopconf(long loopnb)
{
    FILE *fp;
    char fname[200];

    sprintf(fname, "AOloop.conf");

    fp = fopen(fname, "w");
    fprintf(fp, "logsize         1000            number of consecutive entries in single log file\n");
    fprintf(fp, "logdir          ./\n");
    fprintf(fp, "NBMblocks	3		number of modes blocks\n");

    fclose(fp);

    return(0);
}




/*** /brief creates AO control modes
 * 
 *	
 * creates image "modesfreqcpa" which contains CPA value for each mode
 * 
 */

long AOloopControl_mkModes(char *ID_name, long msize, float CPAmax, float deltaCPA, double xc, double yc, double r0, double r1)
{
    long ID0;
    long ID;
    long k, ii, jj;

    long IDmask;

    double ave;
    double offset;
    double totm;
    double totvm;

    double a0=0.88;
    double b0=40.0;

    double a1=1.2;
    double b1=12.0;

    double x, y, r, PA, xc1, yc1;
    double val0, val1, rms;

    long IDtm, IDem;
    long IDeModes;

    long kelim = 20;
    double coeff;
    long citer;
    long NBciter = 200;
    long IDg;


    long NBZ;
    long IDz;

    long zindex[10];
    double zcpa[10];  /// CPA for each Zernike (somewhat arbitrary... used to sort modes in CPA)
    long IDfreq;

    long IDmfcpa; /// modesfreqcpa ID


    /// if Mmask exists, use it, otherwise create it

    IDmask = image_ID("Mmask");


    if(IDmask==-1)
    {
        IDmask = create_2Dimage_ID("Mmask", msize, msize);
        for(ii=0; ii<msize; ii++)
            for(jj=0; jj<msize; jj++)
            {
                x = 1.0*ii-xc;
                y = 1.0*jj-yc;
                r = sqrt(x*x+y*y)/r1;
                val1 = 1.0-exp(-pow(a1*r,b1));
                r = sqrt(x*x+y*y)/r0;
                val0 = exp(-pow(a0*r,b0));
                data.image[IDmask].array.F[jj*msize+ii] = val0*val1;
            }
        //     save_fits("Mmask", "!Mmask.fits");
        xc1 = xc;
        yc1 = yc;
    }
    else /// extract xc and yc from mask
    {
        xc1 = 0.0;
        yc1 = 0.0;
        totm = 0.0;
        for(ii=0; ii<msize; ii++)
            for(jj=0; jj<msize; jj++)
            {
                xc1 += 1.0*ii*data.image[IDmask].array.F[jj*msize+ii];
                yc1 += 1.0*jj*data.image[IDmask].array.F[jj*msize+ii];
                totm += data.image[IDmask].array.F[jj*msize+ii];
            }
        xc1 /= totm;
        yc1 /= totm;
    }

    totm = arith_image_total("Mmask");
    msize = data.image[IDmask].md[0].size[0];




    NBZ = 5; /// 3: tip, tilt, focus

    zindex[0] = 1; // tip
    zcpa[0] = 0.0;

    zindex[1] = 2; // tilt
    zcpa[1] = 0.0;

    zindex[2] = 4; // focus
    zcpa[2] = 0.25;

    zindex[3] = 3; // astig
    zcpa[3] = 0.4;

    zindex[4] = 5; // astig
    zcpa[4] = 0.4;

    zindex[5] = 7; // coma
    zcpa[5] = 0.6;

    zindex[6] = 8; // coma
    zcpa[6] = 0.6;

    zindex[7] = 6; // trefoil
    zcpa[7] = 1.0;

    zindex[8] = 9; // trefoil
    zcpa[8] = 1.0;

    zindex[9] = 12;
    zcpa[9] = 1.5;


	
    linopt_imtools_makeCPAmodes("CPAmodes", msize, CPAmax, deltaCPA, 0.5*msize, 1.2, 0);
    ID0 = image_ID("CPAmodes");
    
    IDfreq	= image_ID("cpamodesfreq");
  //  list_image_ID();
    
    
    printf("  %ld %ld %ld\n", msize, msize, data.image[ID0].md[0].size[2]-1 );
    ID = create_3Dimage_ID(ID_name, msize, msize, data.image[ID0].md[0].size[2]-1+NBZ);

    IDmfcpa = create_2Dimage_ID("modesfreqcpa", data.image[ID0].md[0].size[2]-1+NBZ, 1);

    /*** Create TTF first */
    zernike_init();
    for(k=0; k<NBZ; k++)
    {
        data.image[IDmfcpa].array.F[k] = zcpa[k];
        for(ii=0; ii<msize; ii++)
            for(jj=0; jj<msize; jj++)
            {
                x = 1.0*ii-xc1;
                y = 1.0*jj-yc1;
                r = sqrt(x*x+y*y)/r1;
                PA = atan2(y,x);
                data.image[ID].array.F[k*msize*msize+jj*msize+ii] = Zernike_value(zindex[k], r, PA);
            }
    }
    for(k=0; k<data.image[ID0].md[0].size[2]-1; k++)
    {
        data.image[IDmfcpa].array.F[k+NBZ] = data.image[IDfreq].array.F[k+1];
        for(ii=0; ii<msize*msize; ii++)
            data.image[ID].array.F[(k+NBZ)*msize*msize+ii] = data.image[ID0].array.F[(k+1)*msize*msize+ii];
    }


    for(k=0; k<data.image[ID0].md[0].size[2]-1+NBZ; k++)
    {
        /// Remove excluded modes
        IDeModes = image_ID("emodes");
        if(IDeModes!=-1)
        {
            IDtm = create_2Dimage_ID("tmpmode", msize, msize);

            for(ii=0; ii<msize*msize; ii++)
                data.image[IDtm].array.F[ii] = data.image[ID].array.F[k*msize*msize+ii];
            linopt_imtools_image_fitModes("tmpmode", "emodes", "Mmask", 1.0e-5, "lcoeff", 0);
            linopt_imtools_image_construct("emodes", "lcoeff", "em00");
            delete_image_ID("lcoeff");
            IDem = image_ID("em00");

            coeff = 1.0-exp(-pow(1.0*k/kelim,6.0));
            if(k>2.0*kelim)
                coeff = 1.0;
            for(ii=0; ii<msize*msize; ii++)
                data.image[ID].array.F[k*msize*msize+ii] = data.image[IDtm].array.F[ii] - coeff*data.image[IDem].array.F[ii];

            delete_image_ID("em00");
            delete_image_ID("tmpmode");
        }


        ave = 0.0;
        totvm = 0.0;
        for(ii=0; ii<msize*msize; ii++)
        {
            //	  data.image[ID].array.F[k*msize*msize+ii] = data.image[ID0].array.F[(k+1)*msize*msize+ii];
            totvm += data.image[ID].array.F[k*msize*msize+ii]*data.image[IDmask].array.F[ii];
        }
        offset = totvm/totm;

        for(ii=0; ii<msize*msize; ii++)
        {
            data.image[ID].array.F[k*msize*msize+ii] -= offset;
            data.image[ID].array.F[k*msize*msize+ii] *= data.image[IDmask].array.F[ii];
        }

        offset = 0.0;
        for(ii=0; ii<msize*msize; ii++)
            offset += data.image[ID].array.F[k*msize*msize+ii];

        rms = 0.0;
        for(ii=0; ii<msize*msize; ii++)
        {
            data.image[ID].array.F[k*msize*msize+ii] -= offset/msize/msize;
            rms += data.image[ID].array.F[k*msize*msize+ii]*data.image[ID].array.F[k*msize*msize+ii];
        }
        rms = sqrt(rms/totm);
        printf("Mode %ld   RMS = %lf\n", k, rms);
        for(ii=0; ii<msize*msize; ii++)
            data.image[ID].array.F[k*msize*msize+ii] /= rms;
    }


    for(k=0; k<data.image[ID0].md[0].size[2]-1+NBZ; k++)
    {
        rms = 0.0;
        for(ii=0; ii<msize*msize; ii++)
        {
            data.image[ID].array.F[k*msize*msize+ii] -= offset/msize/msize;
            rms += data.image[ID].array.F[k*msize*msize+ii]*data.image[ID].array.F[k*msize*msize+ii];
        }
        rms = sqrt(rms/totm);
        printf("Mode %ld   RMS = %lf\n", k, rms);
    }

    for(citer=0; citer<NBciter; citer++)
    {
        printf("Convolution [%3ld/%3ld]\n", citer, NBciter);
        gauss_filter(ID_name, "modeg", 4.0*pow(1.0*(NBciter-citer)/NBciter,0.5), 5);
        IDg = image_ID("modeg");
        for(k=0; k<data.image[ID].md[0].size[2]; k++)
        {
            for(ii=0; ii<msize*msize; ii++)
                if(data.image[IDmask].array.F[ii]<0.98)
                    data.image[ID].array.F[k*msize*msize+ii] = data.image[IDg].array.F[k*msize*msize+ii];
        }
        delete_image_ID("modeg");
    }


    return(ID);
}






//
// every time im_name changes (counter increments), crop it to out_name in shared memory
//
int AOloopControl_camimage_extract2D_sharedmem_loop(char *in_name, char *out_name, long size_x, long size_y, long xstart, long ystart)
{
    long iiin,jjin, iiout, jjout;
    long IDin, IDout;
    int atype;
    long *sizeout;
    long long cnt0;
    long IDmask;
    long sizeoutxy;
    long ii;


    sizeout = (long*) malloc(sizeof(long)*2);
    sizeout[0] = size_x;
    sizeout[1] = size_y;
    sizeoutxy = size_x*size_y;

    IDin = image_ID(in_name);
    atype = data.image[IDin].md[0].atype;

    // Create shared memory output image
    IDout = create_image_ID(out_name, 2, sizeout, atype, 1, 0);

    // Check if there is a mask
    IDmask = image_ID("csmask");
    if(IDmask!=-1)
        if((data.image[IDmask].md[0].size[0]!=size_x)||(data.image[IDmask].md[0].size[1]!=size_y))
        {
            printf("ERROR: csmask has wrong size\n");
            exit(0);
        }


    cnt0 = -1;

    switch (atype) {
    case USHORT :
        while(1)
        {
            usleep(10);
            if(data.image[IDin].md[0].cnt0!=cnt0)
            {
                data.image[IDout].md[0].write = 1;
                cnt0 = data.image[IDin].md[0].cnt0;
                for(iiout=0; iiout<size_x; iiout++)
                    for(jjout=0; jjout<size_y; jjout++)
                    {
                        iiin = xstart + iiout;
                        jjin = ystart + jjout;
                        data.image[IDout].array.U[jjout*size_x+iiout] = data.image[IDin].array.U[jjin*data.image[IDin].md[0].size[0]+iiin];
                    }
                if(IDmask!=-1)
                    for(ii=0; ii<sizeoutxy; ii++)
                        data.image[IDout].array.U[ii] *= (int) data.image[IDmask].array.F[ii];

                data.image[IDout].md[0].cnt0 = cnt0;
                data.image[IDout].md[0].write = 0;
            }
        }
        break;
    case FLOAT :
        while(1)
        {
            usleep(50);
            if(data.image[IDin].md[0].cnt0!=cnt0)
            {
                data.image[IDout].md[0].write = 1;
                cnt0 = data.image[IDin].md[0].cnt0;
                for(iiout=0; iiout<size_x; iiout++)
                    for(jjout=0; jjout<size_y; jjout++)
                    {
                        iiin = xstart + iiout;
                        jjin = ystart + jjout;
                        data.image[IDout].array.F[jjout*size_x+iiout] = data.image[IDin].array.F[jjin*data.image[IDin].md[0].size[0]+iiin];
                    }
                if(IDmask!=-1)
                    for(ii=0; ii<sizeoutxy; ii++)
                        data.image[IDout].array.F[ii] *= data.image[IDmask].array.F[ii];
                data.image[IDout].md[0].cnt0 = cnt0;
                data.image[IDout].md[0].write = 0;
            }
        }
        break;
    default :
        printf("ERROR: DATA TYPE NOT SUPPORTED\n");
        exit(0);
        break;
    }
    free(sizeout);

    return(0);
}






/** \brief Computes control matrix using SVD
 *  
 *        Conventions: 
 * 				m: number of actuators (= NB_MODES);  
 * 				n: number of sensors  (= # of pixels)
 *	works even for m != n
 * 
 * 
 *  
 */

int compute_ControlMatrix(long loop, long NB_MODE_REMOVED, char *ID_Rmatrix_name, char *ID_Cmatrix_name, char *ID_VTmatrix_name, double Beta, long NB_MODE_REMOVED_STEP)
{
    FILE *fp;
    long ID;
    long ii1, jj1, k, ii;
    gsl_matrix *matrix_D; /* this is the response matrix */
    gsl_matrix *matrix_Ds; /* this is the pseudo inverse of D */
    gsl_matrix *matrix_Dtra;
    gsl_matrix *matrix_DtraD;
    gsl_matrix *matrix_DtraDinv;
    gsl_matrix *matrix_DtraD_evec;
    gsl_matrix *matrix1;
    gsl_matrix *matrix2;
    gsl_vector *matrix_DtraD_eval;
    gsl_eigen_symmv_workspace *w;

    gsl_matrix *matrix_save;

    long m;
    long n;
    long ID_Rmatrix, ID_Cmatrix, ID_VTmatrix;
    long *arraysizetmp;

    long IDmodes, IDeigenmodes;
    long xsize_modes, ysize_modes, zsize_modes;
    long IDeigenmodesResp;
    long kk, kk1;


	double *CPAcoeff; /// gain applied to modes to enhance low orders in SVD
	
	char fname[200];
	long NB_MR;  /// number of modes removed

	long NB_MODE_REMOVED1;



    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(1);


    arraysizetmp = (long*) malloc(sizeof(long)*3);


    ID_Rmatrix = image_ID(ID_Rmatrix_name);

    n = data.image[ID_Rmatrix].md[0].size[0]*data.image[ID_Rmatrix].md[0].size[1]; //AOconf[loop].NBDMmodes;
    m = data.image[ID_Rmatrix].md[0].size[2]; //AOconf[loop].sizeWFS;


	

    /** in this procedure, m=number of actuators/modes, n=number of WFS elements */
    //  long m = smao[0].NBmode;
    // long n = smao[0].NBwfselem;

    printf("m = %ld actuators (modes), n = %ld sensors\n", m, n);
    fflush(stdout);

	NB_MODE_REMOVED1 = m-1;

    matrix_DtraD_eval = gsl_vector_alloc (m);
    matrix_D = gsl_matrix_alloc (n,m);
    matrix_Ds = gsl_matrix_alloc (m,n);
    matrix_Dtra = gsl_matrix_alloc (m,n);
    matrix_DtraD = gsl_matrix_alloc (m,m);
    matrix_DtraDinv = gsl_matrix_alloc (m,m);
    matrix_DtraD_evec = gsl_matrix_alloc (m,m);


	ID = load_fits("modesfreqcpa.fits", "modesfreqcpa");

	
	CPAcoeff = (double*) malloc(sizeof(double)*m);
	for(k=0; k<m; k++)
	{
		CPAcoeff[k] =  exp(-data.image[ID].array.F[k]*Beta);
		printf("%5ld %5.3f %g\n", k, data.image[ID].array.F[k], CPAcoeff[k]);
	}
	
	
    /* write matrix_D */
    for(k=0; k<m; k++)
    {
        for(ii=0; ii<n; ii++)
            gsl_matrix_set (matrix_D, ii, k, data.image[ID_Rmatrix].array.F[k*n+ii]*CPAcoeff[k]);
	}
    /* compute DtraD */
    gsl_blas_dgemm (CblasTrans, CblasNoTrans, 1.0, matrix_D, matrix_D, 0.0, matrix_DtraD);


    /* compute the inverse of DtraD */

    /* first, compute the eigenvalues and eigenvectors */
    w =   gsl_eigen_symmv_alloc (m);
    matrix_save = gsl_matrix_alloc (m,m);
    gsl_matrix_memcpy(matrix_save, matrix_DtraD);
    gsl_eigen_symmv (matrix_save, matrix_DtraD_eval, matrix_DtraD_evec, w);
    gsl_matrix_free(matrix_save);
    gsl_eigen_symmv_free(w);
    gsl_eigen_symmv_sort (matrix_DtraD_eval, matrix_DtraD_evec, GSL_EIGEN_SORT_ABS_DESC);

    printf("Eigenvalues\n");
    fflush(stdout);

    // Write eigenvalues
    if((fp=fopen("eigenv.dat","w"))==NULL)
    {
        printf("ERROR: cannot create file \"eigenv.dat\"\n");
        exit(0);
    }
    for(k=0; k<m; k++)
        fprintf(fp,"%ld %g\n", k, gsl_vector_get(matrix_DtraD_eval,k));
    fclose(fp);

    for(k=0; k<m; k++)
        printf("Mode %ld eigenvalue = %g\n", k, gsl_vector_get(matrix_DtraD_eval,k));


    /** Write rotation matrix to go from DM modes to eigenmodes */
    arraysizetmp[0] = m;
    arraysizetmp[1] = m;
    ID_VTmatrix = create_image_ID(ID_VTmatrix_name, 2, arraysizetmp, FLOAT, 0, 0);
    for(ii=0; ii<m; ii++) // modes
        for(k=0; k<m; k++) // modes
            data.image[ID_VTmatrix].array.F[k*m+ii] = (float) gsl_matrix_get( matrix_DtraD_evec, k, ii);


    /// Compute eigenmodes responses
    IDeigenmodesResp = create_3Dimage_ID("eigenmodesrespM", data.image[ID_Rmatrix].md[0].size[0], data.image[ID_Rmatrix].md[0].size[1], data.image[ID_Rmatrix].md[0].size[2]);
    printf("Computing eigenmode responses .... \n");
    for(kk=0; kk<m; kk++) /// eigen mode index
    {
        printf("\r eigenmode %4ld / %4ld   ", kk, m);
        fflush(stdout);
        for(kk1=0; kk1<m; kk1++)
        {
            for(ii=0; ii<n; ii++)
                data.image[IDeigenmodesResp].array.F[kk*n + ii] += data.image[ID_VTmatrix].array.F[kk1*m+kk]*data.image[ID_Rmatrix].array.F[kk1*n + ii];
        }
    }
    sprintf(fname, "!eigenmodesrespM_%4.2f.fits", Beta);
    save_fits("eigenmodesrespM", fname); 
   printf("\n");


    /// if modesM exists, compute eigenmodes using rotation matrix
    IDmodes = image_ID("modesM");
    if(IDmodes!=-1)
    {
        xsize_modes = data.image[IDmodes].md[0].size[0];
        ysize_modes = data.image[IDmodes].md[0].size[1];
        zsize_modes = data.image[IDmodes].md[0].size[2];
        if(zsize_modes != m)
            printf("ERROR: zsize (%ld) of modesM does not match expected size (%ld)\n", zsize_modes, m);
        else
        {
            IDeigenmodes = create_3Dimage_ID("eigenmodesM", xsize_modes, ysize_modes, m);
		//	list_image_ID();
            printf("Computing eigenmodes .... \n");
            for(kk=0; kk<m; kk++) /// eigen mode index
            {
                printf("\r eigenmode %4ld / %4ld   ", kk, m);
                fflush(stdout);
                for(kk1=0; kk1<m; kk1++)
                {
                    for(ii=0; ii<xsize_modes*ysize_modes; ii++)
                        data.image[IDeigenmodes].array.F[kk*xsize_modes*ysize_modes + ii] += data.image[ID_VTmatrix].array.F[kk1*m+kk]*data.image[IDmodes].array.F[kk1*xsize_modes*ysize_modes + ii];
                }
            }
			printf("\n");
        }	
        sprintf(fname, "!eigenmodesM_%4.2f.fits", Beta);
		save_fits("eigenmodesM", fname); 
    }
    
    
    
    
    /// second, build the "inverse" of the diagonal matrix of eigenvalues (matrix1)
    matrix1 = gsl_matrix_alloc (m, m);
    matrix2 = gsl_matrix_alloc (m, m);
    arraysizetmp[0] = AOconf[loop].sizexWFS;
    arraysizetmp[1] = AOconf[loop].sizeyWFS;
    arraysizetmp[2] = m;
    ID_Cmatrix = create_image_ID(ID_Cmatrix_name, 3, arraysizetmp, FLOAT, 0, 0);
 
	printf("COMPUTING CMAT .... \n");


	for(NB_MR=0; NB_MR<NB_MODE_REMOVED1; NB_MR+=NB_MODE_REMOVED_STEP)
    {
		printf("\r Number of modes removed : %5ld / %5ld  (step %ld)  ", NB_MR, NB_MODE_REMOVED1, NB_MODE_REMOVED_STEP);
		fflush(stdout);
		for(ii1=0; ii1<m; ii1++)
        for(jj1=0; jj1<m; jj1++)
        {
            if(ii1==jj1)
            {
                if((m-ii1-1)<NB_MR)
                    gsl_matrix_set(matrix1, ii1, jj1, 0.0);
                else
                    gsl_matrix_set(matrix1, ii1, jj1, 1.0/gsl_vector_get(matrix_DtraD_eval,ii1));
            }
            else
                gsl_matrix_set(matrix1, ii1, jj1, 0.0);
        }

   
	printf("-");
	fflush(stdout);

    /* third, compute the "inverse" of DtraD */
    gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, matrix_DtraD_evec, matrix1, 0.0, matrix2);
 	printf("-");
	fflush(stdout);
	gsl_blas_dgemm (CblasNoTrans, CblasTrans, 1.0, matrix2, matrix_DtraD_evec, 0.0, matrix_DtraDinv);
 	printf("-");
	fflush(stdout);
	gsl_blas_dgemm (CblasNoTrans, CblasTrans, 1.0, matrix_DtraDinv, matrix_D, 0.0, matrix_Ds);

    /* write result */
	printf("write result to ID %ld   [%ld %ld]\n", ID_Cmatrix, n, m);
	fflush(stdout);
	list_image_ID();

    for(ii=0; ii<n; ii++) // sensors
        for(k=0; k<m; k++) // actuator modes
            data.image[ID_Cmatrix].array.F[k*n+ii] = (float) gsl_matrix_get(matrix_Ds, k, ii)*CPAcoeff[k];

	
	sprintf(fname, "!cmat_%4.2f_%03ld.fits", Beta, NB_MR);
	printf("  SAVING -> %s\n", fname);
	fflush(stdout);
	save_fits(ID_Cmatrix_name, fname);
	}
	printf("\n\n");
	
    gsl_matrix_free(matrix1);
    gsl_matrix_free(matrix2);
 
    gsl_vector_free(matrix_DtraD_eval);
    gsl_matrix_free(matrix_D);
    gsl_matrix_free(matrix_Ds);
    gsl_matrix_free(matrix_Dtra);
    gsl_matrix_free(matrix_DtraD);
    gsl_matrix_free(matrix_DtraDinv);
    gsl_matrix_free(matrix_DtraD_evec);

    free(arraysizetmp);

	free(CPAcoeff);


    return(ID_Cmatrix);
}













/*** mode = 0 or 1. if mode == 1, simply connect */

int AOloopControl_InitializeMemory(int mode)
{
  int SM_fd;
  struct stat file_stat;
  int create = 0;
  int result;
  long loop;

  SM_fd = open(AOconfname, O_RDWR);
  if(SM_fd==-1)
    {
      printf("Cannot import file -> creating file\n");
      create = 1;
    }
  else
    {
      fstat(SM_fd, &file_stat);
      printf("File %s size: %zd\n", AOconfname, file_stat.st_size);
      if(file_stat.st_size!=sizeof(AOLOOPCONTROL_CONF)*NB_AOloopcontrol)
	{
	  printf("File size wrong -> recreating file\n");
	  create = 1;
	  close(SM_fd);
	}
    }
  
  if(create==1)
    {
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
	AOconf[loop].on = 0;
	AOconf[loop].cnt = 0;	  
	AOconf[loop].cntmax = 0;	  
  }
  
  if(create==1)
    {
      for(loop=0; loop<NB_AOloopcontrol; loop++)
	{
	  AOconf[loop].init = 0;
	  AOconf[loop].on = 0;
	  AOconf[loop].cnt = 0;	  
	  AOconf[loop].cntmax = 0;	  
	  AOconf[loop].maxlimit = 0.3;
	  AOconf[loop].gain = 0.0;
	  AOconf[loop].framesAve = 1;
	}
    }
  else
    {
      for(loop=0; loop<NB_AOloopcontrol; loop++)
	if(AOconf[loop].init == 1)
	  {
	    printf("LIST OF ACTIVE LOOPS:\n");
	    printf("----- Loop %ld   (%s) ----------\n", loop, AOconf[loop].name);
	    printf("  WFS:  %s  [%ld]  %ld x %ld\n", AOconf[loop].WFSname, aoconfID_WFS, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
	    printf("   DM:  %s  [%ld]  %ld x %ld\n", AOconf[loop].DMname, aoconfID_DM, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);
	    printf("DM RM:  %s  [%ld]  %ld x %ld\n", AOconf[loop].DMnameRM, aoconfID_DM, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);
	  }
    }
  
  AOloopcontrol_meminit = 1;


  return 0;
}





/** Read image from WFS camera
 *
 * supports ring buffer
 * puts image from camera buffer aoconfID_WFS into aoconfID_WFS1 (supplied by user)
 *
 * RM = 1 if response matrix
 *
 */

int Average_cam_frames(long loop, long NbAve, int RM)
{
    long imcnt;
    long ii;
    double total;
    char name[200];
    int atype;
    long slice;
    char *ptrv;
    long double tmplv1;
    double tmpf;
    long IDdark;
    char dname[200];

    atype = data.image[aoconfID_WFS].md[0].atype;



    if(avcamarraysInit==0)
    {
        arrayftmp = (float*) malloc(sizeof(float)*AOconf[loop].sizeWFS);
        arrayutmp = (unsigned short*) malloc(sizeof(unsigned short)*AOconf[loop].sizeWFS);
        avcamarraysInit = 1;
    }

    if(NbAve>1)
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[aoconfID_WFS0].array.F[ii] = 0.0;

    if(RM==0)
        AOconf[loop].status = 2;  // 2: WAIT FOR IMAGE
    else
        AOconf[loop].RMstatus = 2;


    if(data.image[aoconfID_WFS].md[0].naxis==2) // single buffer
    {
        switch (atype) {
        case FLOAT :
            imcnt = 0;
            while(imcnt<NbAve)
            {
                usleep(50);
                if(data.image[aoconfID_WFS].md[0].write == 0)
                {
                    if(AOconf[loop].WFScnt!=data.image[aoconfID_WFS].md[0].cnt0)
                    {
                        memcpy (arrayftmp, data.image[aoconfID_WFS].array.F, sizeof(float)*AOconf[loop].sizeWFS);
                        AOconf[loop].WFScnt = data.image[aoconfID_WFS].md[0].cnt0;
                        if(NbAve>1)
                        {

                            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                                data.image[aoconfID_WFS0].array.F[ii] += arrayftmp[ii];
                        }
                        else
                            memcpy(data.image[aoconfID_WFS0].array.F, arrayftmp,  sizeof(float)*AOconf[loop].sizeWFS);
                        imcnt++;
                    }
                }
            }
            break;
        case USHORT :
            imcnt = 0;
            while(imcnt<NbAve)
            {
               usleep(50);
                if(data.image[aoconfID_WFS].md[0].write == 0)
                {
                    if(AOconf[loop].WFScnt!=data.image[aoconfID_WFS].md[0].cnt0)
                    {
                        memcpy (arrayutmp, data.image[aoconfID_WFS].array.U, sizeof(unsigned short)*AOconf[loop].sizeWFS);
                        AOconf[loop].WFScnt = data.image[aoconfID_WFS].md[0].cnt0;
                        if(NbAve>1)
                        {
                            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                                data.image[aoconfID_WFS0].array.F[ii] += arrayutmp[ii];
                        }
                        else
                        {
                            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                                data.image[aoconfID_WFS0].array.F[ii] = arrayutmp[ii];
                        }
                        imcnt++;
                    }
                }           
            }
            break;
        default :
            printf("ERROR: DATA TYPE NOT SUPPORTED\n");
            exit(0);
            break;
        }
    }
    else // ring buffer mode, only works with NbAve = 1
    {
		if(data.image[aoconfID_WFS].sem==0)
		{
        if(RM==0)
        {
            while(AOconf[loop].WFScnt==data.image[aoconfID_WFS].md[0].cnt0) // test if new frame exists
            {
                usleep(50);
                // do nothing, wait
            }
        }
        else
        {
            while(AOconf[loop].WFScntRM==data.image[aoconfID_WFS].md[0].cnt0) // test if new frame exists
            {
                usleep(50);
                // do nothing, wait
            }
        }
		}
		else
		{
			printf("Waiting for semaphore to post .... ");
			fflush(stdout);
			sem_wait(data.image[aoconfID_WFS].semptr);
			printf(" done\n");
			fflush(stdout);
		}
		
        slice = data.image[aoconfID_WFS].md[0].cnt1;
        if(slice==-1)
            slice = data.image[aoconfID_WFS].md[0].size[2];

 
		
        switch (atype) {
        case FLOAT :
            ptrv = (char*) data.image[aoconfID_WFS].array.F;
            ptrv += sizeof(float)*slice* AOconf[loop].sizeWFS;
            memcpy(data.image[aoconfID_WFS0].array.F, ptrv,  sizeof(float)*AOconf[loop].sizeWFS);
            break;
        case USHORT :
            ptrv = (char*) data.image[aoconfID_WFS].array.U;
            ptrv += sizeof(unsigned short)*slice* AOconf[loop].sizeWFS;
            memcpy (arrayutmp, ptrv, sizeof(unsigned short)*AOconf[loop].sizeWFS);
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                data.image[aoconfID_WFS0].array.F[ii] = (float) arrayutmp[ii];
            break;
        default :
            printf("ERROR: DATA TYPE NOT SUPPORTED\n");
            exit(0);
            break;
        }
        if(RM==0)
            AOconf[loop].WFScnt = data.image[aoconfID_WFS].md[0].cnt0;
        else
            AOconf[loop].WFScntRM = data.image[aoconfID_WFS].md[0].cnt0;
    }
    AOconf[loop].status = 3;  // 3: NORMALIZE WFS IMAGE


    // Dark subtract
    sprintf(dname, "aol%ld_wfsdark", loop);
    IDdark = image_ID(dname);
    if(IDdark!=-1)
    {
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[aoconfID_WFS0].array.F[ii] -= data.image[IDdark].array.F[ii];
    }
    
    
  
    // Normalize
    total = arith_image_total(data.image[aoconfID_WFS0].md[0].name);

    data.image[aoconfID_WFS0].md[0].cnt0 ++;

  /*  if(AOconf[loop].WFS_CAM_PER_CORR==1) /// additional processing step here
    {
        data.image[aoconfID_WFS1].md[0].write = 1;
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[aoconfID_WFS1].array.F[ii] = data.image[aoconfID_WFS0].array.F[ii]/total;

        AOconf[loop].WFScamPEcorr_pha = ((long double) (1.0*data.image[aoconfID_WFS].md[0].cnt0))/ ((long double) (AOconf[loop].WFScamPEcorr_period));
        AOconf[loop].WFScamPEcorr_pha = modfl(AOconf[loop].WFScamPEcorr_pha, &tmplv1);
        AOconf[loop].WFScamPEcorr_pha +=  AOconf[loop].WFScamPEcorr_pharef;
        AOloopControl_Remove_WFScamPE(data.image[aoconfID_WFS1].md[0].name, "WFScamPEcorrC", (double) AOconf[loop].WFScamPEcorr_pha);
    }
    else
    {*/
       
       
        data.image[aoconfID_WFS1].md[0].write = 1;
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[aoconfID_WFS1].array.F[ii] = data.image[aoconfID_WFS0].array.F[ii]/total;
        data.image[aoconfID_WFS1].md[0].cnt0 ++;
        data.image[aoconfID_WFS1].md[0].write = 0;

		

   // }

    //printf("Average cam DONE\n");
    //fflush(stdout);


    return(0);
}














long AOloopControl_MakeDMModes(long loop, long NBmodes, char *IDname)
{
  long ID;
  long IDtmp;
  long ii, jj;
  double x, y;
  long size = AOconf[loop].sizexDM;
  long m;
  float rpix;
  long size2;

  size2 = size*size;
  rpix = 0.5*size;

  ID = create_3Dimage_ID_float(IDname, size, size, NBmodes);
  
  for(m=0;m<NBmodes;m++)
    {
      IDtmp = mk_zer("zertmp", size, m+1, rpix);
      for(ii=0;ii<size2;ii++)
	data.image[ID].array.F[size2*m+ii] = data.image[IDtmp].array.F[ii];
      delete_image_ID("zertmp");
    }

  return(ID);
}




long AOloopControl_loadCM(long loop, char *CMfname)
{
    long ID = -1;
    int vOK;
    char name[200];
    long ID0;
    long ii;

    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);

    if( (ID=load_fits(CMfname, "tmpcontrM")) != -1 )
    {
		
        // check size is OK
        vOK = 1;
        if(data.image[ID].md[0].naxis!=3)
        {
            printf("Control matrix has wrong dimension\n");
            vOK = 0;
        }
        if(data.image[ID].md[0].atype!=FLOAT)
        {
            printf("Control matrix has wrong type\n");
            vOK = 0;
        }
        if(vOK==1)
        {
            if(data.image[ID].md[0].size[0]!=AOconf[loop].sizexWFS)
            {
                printf("Control matrix has wrong x size : is %ld, should be %ld\n", data.image[ID].md[0].size[0], AOconf[loop].sizexWFS);
                vOK = 0;
            }
            if(data.image[ID].md[0].size[1]!=AOconf[loop].sizeyWFS)
            {
                printf("Control matrix has wrong y size\n");
                vOK = 0;
            }
            if(data.image[ID].md[0].size[2]!=AOconf[loop].NBDMmodes)
            {
                printf("Control matrix has wrong z size\n");
                vOK = 0;
            }
        }
        
        
        if(vOK==1)
        {
            AOconf[loop].init_CM = 1;
            sprintf(name, "ContrM_%ld", loop);
            ID = image_ID(name);
            if(ID==-1)
                ID = read_sharedmem_image(name);
            ID0 = image_ID("tmpcontrM");
            data.image[ID].md[0].write  = 1;
            for(ii=0; ii<AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS*AOconf[loop].NBDMmodes; ii++)
                data.image[ID].array.F[ii] = data.image[ID0].array.F[ii];
            data.image[ID].md[0].write  = 0;
			data.image[ID].md[0].cnt0++;
        }
        delete_image_ID("tmpcontrM");
    }

    return(ID);
}




long AOloopControl_2Dloadcreate_shmim(char *name, char *fname, long xsize, long ysize)
{
	long ID;
	int CreateSMim;
	int sizeOK;
	long *sizearray;
	char command[500];
	int r;
	long ID1;
	
	
    ID = image_ID(name);
    sizearray = (long*) malloc(sizeof(long)*2);
        
    if(ID==-1)
        {
			CreateSMim = 0;	
			ID = read_sharedmem_image(name);
			if(ID!=-1)
			{
				sizeOK = COREMOD_MEMORY_check_2Dsize(name, xsize, ysize);
				if(sizeOK==0)
                {
                    printf("\n========== EXISTING %s HAS WRONG SIZE -> CREATING BLANK %s ===========\n\n", name, name);
                    delete_image_ID(name);
                    sprintf(command, "rm /tmp/%s.im.shm", name);
                    r = system(command);
                    CreateSMim = 1;
                }
			}
			else
				CreateSMim = 1;
				
			if(CreateSMim == 1)
            {
				sizearray[0] =  xsize;
				sizearray[1] =  ysize;
				printf("Creating %s   [%ld x %ld]\n", name, sizearray[0], sizearray[1]);
				fflush(stdout);
				ID = create_image_ID(name, 2, sizearray, FLOAT, 1, 0);
			}
		}
	free(sizearray);
	
	if(ID==-1)
		{
			printf("ERROR: could not load/create %s\n", name);
			exit(0);
		}
	else
		{
			ID1 = load_fits(fname, "tmp2Dim");
			if(ID1!=-1)
			{
				sizeOK = COREMOD_MEMORY_check_2Dsize("tmp2Dim", xsize, ysize);
				if(sizeOK==1)
					{
						memcpy(data.image[ID].array.F, data.image[ID1].array.F, sizeof(float)*xsize*ysize);
						printf("loaded file \"%s\" to shared memory \"%s\"\n", fname, name);
					}
				else
					printf("File \"%s\" has wrong size (should be 2-D %ld x %ld,  is %ld-D %ld x %ld): ignoring\n", fname, xsize, ysize, data.image[ID1].md[0].naxis, data.image[ID1].md[0].size[0], data.image[ID1].md[0].size[1]);
				delete_image_ID("tmp2Dim");
			}
		}
	
	return ID;
}




long AOloopControl_3Dloadcreate_shmim(char *name, char *fname, long xsize, long ysize, long zsize)
{
	long ID;
	int CreateSMim;
	int sizeOK;
	long *sizearray;
	char command[500];
	int r;
	long ID1;
	
    ID = image_ID(name);
    sizearray = (long*) malloc(sizeof(long)*3);
        
    if(ID==-1)
        {
			CreateSMim = 0;	
			ID = read_sharedmem_image(name);
			if(ID!=-1)
			{
				sizeOK = COREMOD_MEMORY_check_3Dsize(name, xsize, ysize, zsize);
				if(sizeOK==0)
                {
                    printf("\n========== EXISTING %s HAS WRONG SIZE -> CREATING BLANK %s ===========\n\n", name, name);
                    delete_image_ID(name);
                    sprintf(command, "rm /tmp/%s.im.shm", name);
                    r = system(command);
                    CreateSMim = 1;
                }
			}
			else
				CreateSMim = 1;
				
			if(CreateSMim == 1)
            {
				sizearray[0] = xsize;
				sizearray[1] = ysize;
				sizearray[2] = zsize;
				printf("Creating %s   [%ld x %ld x %ld]\n", name, sizearray[0], sizearray[1], sizearray[2]);
				fflush(stdout);
				ID = create_image_ID(name, 3, sizearray, FLOAT, 1, 0);
			}
		}
	free(sizearray);
	
	if(ID==-1)
		{
			printf("ERROR: could not load/create %s\n", name);
			exit(0);
		}
	else
		{
			ID1 = load_fits(fname, "tmp3Dim");
			if(ID1!=-1)
			{
				sizeOK = COREMOD_MEMORY_check_3Dsize("tmp3Dim", xsize, ysize, zsize);
				if(sizeOK==1)
					{
						memcpy(data.image[ID].array.F, data.image[ID1].array.F, sizeof(float)*xsize*ysize*zsize);
						printf("loaded file \"%s\" to shared memory \"%s\"\n", fname, name);
					}
				else
					printf("File \"%s\" has wrong size (should be 3-D %ld x %ld, x %ld  is %ld-D %ld x %ld x %ld): ignoring\n", fname, xsize, ysize, zsize, data.image[ID1].md[0].naxis, data.image[ID1].md[0].size[0], data.image[ID1].md[0].size[1], data.image[ID1].md[0].size[2]);
				delete_image_ID("tmp3Dim");
			}
		}
	
	return ID;
}



//
// load / setup configuration
// mode = 1 loads from ./conf/ directory to shared memory
// mode = 0 simply connects to shared memory
//
int AOloopControl_loadconfigure(long loop, char *config_fname, int mode)
{
    FILE *fp;
    char content[200];
    char name[200];
    char fname[200];
    long ID;
    long *sizearray;
    int vOK;
    int kw;
    long k;
    int r;
    int sizeOK;
    char command[500];
    int CreateSMim;
    long ID1tmp, ID2tmp;


    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);



    // printf("mode = %d\n", mode); // not used yet


    // Name definitions for shared memory

    sprintf(name, "aol%ld_dmC", loop);
    printf("DM file name : %s\n", name);
    strcpy(AOconf[loop].DMname, name);

    sprintf(name, "aol%ld_dmRM", loop);
    printf("DM RM file name : %s\n", name);
    strcpy(AOconf[loop].DMnameRM, name);

    sprintf(name, "aol%ld_wfs", loop);
    printf("WFS file name: %s\n", name);
    strcpy(AOconf[loop].WFSname, name);



    sprintf(name, "aol%ld_DMmodes", loop);
    printf("DMmodes file name: %s\n", name);
    strcpy(AOconf[loop].DMMODESname, name);

    sprintf(name, "aol%ld_RespM", loop);
    printf("respM file name: %s\n", name);
    strcpy(AOconf[loop].respMname, name);

    sprintf(name, "aol%ld_ContrM", loop);
    printf("contrM file name: %s\n", name);
    strcpy(AOconf[loop].contrMname, name);






    sizearray = (long*) malloc(sizeof(long)*3);


    // READ LOOP NAME

    if((fp=fopen("./conf/conf_LOOPNAME.txt","r"))==NULL)
    {
        printf("ERROR: file ./conf/conf_LOOPNAME.txt missing\n");
        exit(0);
    }
    r = fscanf(fp, "%s", content);
    printf("loop name : %s\n", content);
    fclose(fp);
    fflush(stdout);
    strcpy(AOconf[loop].name, content);


    // USE GPUs ?

    if((fp=fopen("./conf/conf_GPU.txt","r"))==NULL)
    {
        printf("ERROR: file ./conf/conf_GPU.txt missing\n");
        exit(0);
    }
    r = fscanf(fp, "%s", content);
    printf("GPU : %d\n", atoi(content));
    fclose(fp);
    fflush(stdout);
    AOconf[loop].GPU = atoi(content);




    // Connect to WFS camera
    // This is where the size of the WFS is fixed
    aoconfID_WFS = read_sharedmem_image(AOconf[loop].WFSname);
    AOconf[loop].sizexWFS = data.image[aoconfID_WFS].md[0].size[0];
    AOconf[loop].sizeyWFS = data.image[aoconfID_WFS].md[0].size[1];
    AOconf[loop].sizeWFS = AOconf[loop].sizexWFS*AOconf[loop].sizeyWFS;



    // The AOloopControl_xDloadcreate_shmim functions work as follows:
    // If file already loaded, use it (we assume it's already been properly loaded)
    // If not, attempt to read it from shared memory
    // If not available in shared memory, create it in shared memory
    // if "fname" exists, attempt to load it into the shared memory image

    sprintf(name, "aol%ld_wfsdark", loop);
    sprintf(fname, "./conf/dark.fits");
    aoconfID_WFSdark = AOloopControl_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);


    sprintf(name, "aol%ld_imWFS0", loop);
    aoconfID_WFS0 = AOloopControl_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);

    sprintf(name, "aol%ld_imWFS1", loop);
    aoconfID_WFS1 = AOloopControl_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);

    sprintf(name, "aol%ld_imWFS2", loop);
    aoconfID_WFS2 = AOloopControl_2Dloadcreate_shmim(name, " ", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);

    sprintf(name, "aol%ld_refWFSim", loop);
    sprintf(fname, "./conf/refwfs.fits");
    aoconfID_refWFS = AOloopControl_2Dloadcreate_shmim(name, fname, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
    AOconf[loop].init_refWFS = 1;





    // Connect to DM
    // Here the DM size is fixed
    //
    aoconfID_DM = image_ID(AOconf[loop].DMname);
    if(aoconfID_DM==-1)
    {
        printf("connect to %s\n", AOconf[loop].DMname);
        aoconfID_DM = read_sharedmem_image(AOconf[loop].DMname);
        if(aoconfID_DM==-1)
        {
            printf("ERROR: cannot connect to shared memory %s\n", AOconf[loop].DMname);
            exit(0);
        }
    }
    AOconf[loop].sizexDM = data.image[aoconfID_DM].md[0].size[0];
    AOconf[loop].sizeyDM = data.image[aoconfID_DM].md[0].size[1];
    AOconf[loop].sizeDM = AOconf[loop].sizexDM*AOconf[loop].sizeyDM;



    aoconfID_DMRM = image_ID(AOconf[loop].DMnameRM);
    if(aoconfID_DMRM==-1)
    {
        printf("connect to %s\n", AOconf[loop].DMnameRM);
        aoconfID_DMRM = read_sharedmem_image(AOconf[loop].DMnameRM);
        if(aoconfID_DMRM==-1)
        {
            printf("ERROR: cannot connect to shared memory %s\n", AOconf[loop].DMnameRM);
            exit(0);
        }
    }







    // Load DM modes (will exit if not successful)

    aoconfID_DMmodes = image_ID(AOconf[loop].DMMODESname); // if already exists, trust it and adopt it

    if(aoconfID_DMmodes==-1) // If not, check file
    {


        // GET SIZE FROM FILE
        ID1tmp = load_fits("./conf/fmodes.fits", "tmp3Dim");
        if(ID1tmp==-1)
        {
            printf("ERROR: no file \"./conf/fmodes.fits\"\n");
            exit(0);
        }

        // check size
        if(data.image[ID1tmp].md[0].naxis != 3)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" is not a 3D image (cube)\n");
            exit(0);
        }
        if(data.image[ID1tmp].md[0].size[0] != AOconf[loop].sizexDM)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" has wrong x size: should be %ld, is %ld\n", AOconf[loop].sizexDM, data.image[ID1tmp].md[0].size[0]);
            exit(0);
        }
        if(data.image[ID1tmp].md[0].size[1] != AOconf[loop].sizeyDM)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" has wrong y size: should be %ld, is %ld\n", AOconf[loop].sizeyDM, data.image[ID1tmp].md[0].size[1]);
            exit(0);
        }
        AOconf[loop].NBDMmodes = data.image[ID1tmp].md[0].size[2];


        // try to read it from shared memory
        ID2tmp = read_sharedmem_image(AOconf[loop].DMMODESname);
        vOK = 0;
        if(ID2tmp != -1) // if shared memory exists, check its size
        {
            vOK = 1;
            if(data.image[ID2tmp].md[0].naxis != 3)
            {
                printf("ERROR: Shared memory File %s is not a 3D image (cube)\n", AOconf[loop].DMMODESname);
                vOK = 0;
            }
            if(data.image[ID2tmp].md[0].size[0] != AOconf[loop].sizexDM)
            {
                printf("ERROR: Shared memory File %s has wrong x size: should be %ld, is %ld\n", AOconf[loop].DMMODESname, AOconf[loop].sizexDM, data.image[ID2tmp].md[0].size[0]);
                vOK = 0;
            }
            if(data.image[ID2tmp].md[0].size[1] != AOconf[loop].sizeyDM)
            {
                printf("ERROR: Shared memory File %s has wrong y size: should be %ld, is %ld\n", AOconf[loop].DMMODESname, AOconf[loop].sizeyDM, data.image[ID2tmp].md[0].size[1]);
                vOK = 0;
            }
            if(data.image[ID2tmp].md[0].size[2] != AOconf[loop].NBDMmodes)
            {
                printf("ERROR: Shared memory File %s has wrong y size: should be %ld, is %ld\n", AOconf[loop].DMMODESname, AOconf[loop].NBDMmodes, data.image[ID2tmp].md[0].size[2]);
                vOK = 0;
            }

            if(vOK==1) // if size is OK, adopt it
                aoconfID_DMmodes = ID2tmp;
            else // if not, erase shared memory 
            {
				printf("SHARED MEM IMAGE HAS WRONG SIZE -> erasing it\n");	
				delete_image_ID(AOconf[loop].DMMODESname);
            }
        }
        
        
        if(vOK==0) // create shared memory 
		{
		
			sizearray[0] = AOconf[loop].sizexDM;
			sizearray[1] = AOconf[loop].sizeyDM;
			sizearray[2] = AOconf[loop].NBDMmodes;
			printf("Creating %s   [%ld x %ld x %ld]\n", AOconf[loop].DMMODESname, sizearray[0], sizearray[1], sizearray[2]);
			fflush(stdout);
			aoconfID_DMmodes = create_image_ID(AOconf[loop].DMMODESname, 3, sizearray, FLOAT, 1, 0);
}

		// put modes into shared memory

		memcpy(data.image[aoconfID_DMmodes].array.F, data.image[ID1tmp].array.F, sizeof(float)*AOconf[loop].sizexDM*AOconf[loop].sizeyDM*AOconf[loop].NBDMmodes);
		
		
		delete_image_ID("tmp3Dim");
	}




/*
    if(aoconfID_DMmodes!=-1)
    {

        printf("reading from shared memory %s   [%ld x %ld x %ld]\n", name, data.image[aoconfID_DMmodes].md[0].size[0], data.image[aoconfID_DMmodes].md[0].size[1], data.image[aoconfID_DMmodes].md[0].size[2]);
        AOconf[loop].NBDMmodes = data.image[aoconfID_DMmodes].md[0].size[2];
    }

    aoconfID_DMmodes = load_fits("./conf/fmodes.fits", "tmp3Dim");
    if(aoconfID_DMmodes!=-1)
    {
        if(data.image[aoconfID_DMmodes].md[0].naxis != 3)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" is not a 3D image (cube)\n");
            exit(0);
        }
        if(data.image[aoconfID_DMmodes].md[0].size[0] != AOconf[loop].sizexDM)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" has wrong x size: should be %ld, is %ld\n", AOconf[loop].sizexDM, data.image[aoconfID_DMmodes].md[0].size[0]);
            exit(0);
        }
        if(data.image[aoconfID_DMmodes].md[0].size[1] != AOconf[loop].sizexDM)
        {
            printf("ERROR: File \"./conf/fmodes.fits\" has wrong y size: should be %ld, is %ld\n", AOconf[loop].sizeyDM, data.image[aoconfID_DMmodes].md[0].size[1]);
            exit(0);
        }
    }




    if(aoconfID_DMmodes == -1)
    {
        printf("ERROR: NO DMmodes\n");
        exit(0);
    }



    // VERIFY DM MODES SIZE
    vOK = 0;
    if(aoconfID_DMmodes != -1)
    {
        vOK = 1;
        if(data.image[aoconfID_DMmodes].md[0].naxis != 3)
        {
            printf("DM modes has wrong dimension\n");
            vOK = 0;
        }
        if(data.image[aoconfID_DMmodes].md[0].atype != FLOAT)
        {
            printf("DM modes has wrong type\n");
            vOK = 0;
        }
        if(vOK==1)
        {
            if(data.image[aoconfID_DMmodes].md[0].size[0]!=AOconf[loop].sizexDM)
            {
                printf("DM modes has wrong x size : is %ld, should be %ld\n", data.image[ID].md[0].size[0], AOconf[loop].sizexDM);
                vOK = 0;
            }
            if(data.image[aoconfID_DMmodes].md[0].size[1]!=AOconf[loop].sizeyDM)
            {
                printf("DM modes has wrong y size : is %ld, should be %ld\n", data.image[ID].md[0].size[0], AOconf[loop].sizexDM);
                vOK = 0;
            }
        }
        if(vOK==1)
        {
            AOconf[loop].NBDMmodes = data.image[aoconfID_DMmodes].md[0].size[2];
            printf("%ld DM modes\n", AOconf[loop].NBDMmodes);
        }
    }
    if(vOK == 0)
    {
        printf("\n");
        printf("========== ERROR: NEED DM MODES TO START AO LOOP ===========\n");
        printf("\n");
        exit(0);
    }
*/






    // modes blocks

    if(read_config_parameter(config_fname, "NBMblocks", content)==0)
        AOconf[loop].NBMblocks = 1;
    else
        AOconf[loop].NBMblocks = atoi(content);

    printf("NBMblocks : %ld\n", AOconf[loop].NBMblocks);
    fflush(stdout);



    if(AOconf[loop].NBMblocks==1)
        AOconf[loop].indexmaxMB[0] = AOconf[loop].NBDMmodes;
    else
    {
        for(k=0; k<AOconf[loop].NBMblocks; k++)
            AOconf[loop].indexmaxMB[k] = (long) (pow(1.0*(k+1.0)/AOconf[loop].NBMblocks,2.0)*AOconf[loop].NBDMmodes);
        AOconf[loop].indexmaxMB[AOconf[loop].NBMblocks-1] = AOconf[loop].NBDMmodes;
    }


    for(k=0; k<AOconf[loop].NBMblocks; k++)
    {
        AOconf[loop].gainMB[k] = 1.0;
        AOconf[loop].limitMB[k] = 1.0;
        AOconf[loop].multfMB[k] = 1.0;
    }



    // Allocate / create logging data files/memory
    if(read_config_parameter(config_fname, "logdir", content)==0)
    {
        printf("parameter logdir missing\n");
        exit(0);
    }
    strcpy(AOconf[loop].logdir, content);
    if(read_config_parameter(config_fname, "logsize", content)==0)
    {
        printf("parameter logsize missing\n");
        exit(0);
    }
    AOconf[loop].logsize = atol(content);
    // time [s]       (1)
    // gains          ( AOconf[loop].NBDMmodes )
    // ID_cmd_modes   ( AOconf[loop].NBDMmodes )
    // ID_cmd1_modes  ( AOconf[loop].NBDMmodes )
    sizearray[0] = 1+3*AOconf[loop].NBDMmodes;
    sizearray[1] = AOconf[loop].logsize;
    sprintf(name, "loop%ldlog0", loop);
    aoconfIDlog0 = create_image_ID(name, 2, sizearray, FLOAT, 1, 10);
    ID = aoconfIDlog0;
    data.image[ID].md[0].NBkw = 1;
    kw = 0;
    strcpy(data.image[ID].kw[kw].name, "TIMEORIGIN");
    data.image[ID].kw[kw].type = 'L';
    data.image[ID].kw[kw].value.numl = 0;
    strcpy(data.image[ID].kw[kw].comment, "time offset [sec]");

    sprintf(name, "loop%ldlog1", loop);
    aoconfIDlog1 = create_image_ID(name, 2, sizearray, FLOAT, 1, 10);
    ID = aoconfIDlog1;
    data.image[ID].md[0].NBkw = 1;
    kw = 0;
    strcpy(data.image[ID].kw[kw].name, "TIMEORIGIN");
    data.image[ID].kw[kw].type = 'L';
    data.image[ID].kw[kw].value.numl = 0;
    strcpy(data.image[ID].kw[kw].comment, "time offset [sec]");



    AOconf[loop].logcnt = 0;
    AOconf[loop].logfnb = 0;
    strcpy(AOconf[loop].userLOGstring, "");

    // AOconf[loop].ID_DMmodes = AOloopControl_MakeDMModes(loop, 5, name);

    printf("%ld modes\n", AOconf[loop].NBDMmodes);





	// load ref WFS image
    sprintf(name, "aol%ld_refWFSim", loop);
    aoconfID_refWFS = AOloopControl_2Dloadcreate_shmim(name, "./conf/refwfs.fits", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);



    // Load/create modal command vector memory
    sprintf(name, "aol%ld_DMmode_cmd", loop);
    ID = image_ID(name);
    aoconfID_cmd_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_cmd_modes].array.F[k] = 0.0;

    sprintf(name, "aol%ld_DMmode_cmd1", loop);
    ID = image_ID(name);
    aoconfID_cmd1_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_cmd1_modes].array.F[k] = 0.0;

    sprintf(name, "aol%ld_DMmode_AVE", loop);
    ID = image_ID(name);
    aoconfID_AVE_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_AVE_modes].array.F[k] = 0.0;

    sprintf(name, "aol%ld_DMmode_RMS", loop);
    ID = image_ID(name);
    aoconfID_RMS_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_RMS_modes].array.F[k] = 0.0;

    sprintf(name, "aol%ld_DMmode_GAIN", loop);
    ID = image_ID(name);
    aoconfID_GAIN_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_GAIN_modes].array.F[k] = 1.0;

    sprintf(name, "aol%ld_DMmode_LIMIT", loop);
    ID = image_ID(name);
    aoconfID_LIMIT_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_LIMIT_modes].array.F[k] = 1.0;

    sprintf(name, "aol%ld_DMmode_MULTF", loop);
    ID = image_ID(name);
    aoconfID_MULTF_modes = AOloopControl_2Dloadcreate_shmim(name, "", AOconf[loop].NBDMmodes, 1);
    if(ID==-1)
        for(k=0; k<AOconf[loop].NBDMmodes; k++)
            data.image[aoconfID_MULTF_modes].array.F[k] = 1.0;









    AOconf[loop].init_RM = 0;

    aoconfID_respM = AOloopControl_3Dloadcreate_shmim(AOconf[loop].respMname, "./conf/respm.fits", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);
    AOconf[loop].init_RM = 1;

    AOconf[loop].init_CM = 0;
    aoconfID_contrM = AOloopControl_3Dloadcreate_shmim(AOconf[loop].contrMname, "./conf/cmat.fits", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);
    AOconf[loop].init_CM = 1;



    free(sizearray);


    printf("   init_WFSref    %d\n", AOconf[loop].init_refWFS);
    printf("   init_RM        %d\n", AOconf[loop].init_RM);
    printf("   init_CM        %d\n", AOconf[loop].init_CM);

    AOconf[loop].init = 1;

    return(0);

}










int set_DM_modes(long loop)
{
    long k;
    long i, j;
    float *arrayf;
    double a;
    long cnttest;

    if(AOconf[loop].GPU == 0)
    {
        arrayf = (float*) malloc(sizeof(float)*AOconf[loop].sizeDM);
        for(j=0; j<AOconf[loop].sizeDM; j++)
            arrayf[j] = 0.0;

        for(i=0; i<AOconf[loop].sizeDM; i++)
            for(k=0; k < AOconf[loop].NBDMmodes; k++)
                arrayf[i] += data.image[aoconfID_cmd_modes].array.F[k] * data.image[aoconfID_DMmodes].array.F[k*AOconf[loop].sizeDM+i];

        data.image[aoconfID_DM].md[0].write = 1;
        memcpy (data.image[aoconfID_DM].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
        data.image[aoconfID_DM].md[0].cnt0++;
        data.image[aoconfID_DM].md[0].write = 0;

        free(arrayf);
    }
    else
    {
		#ifdef HAVE_CUDA
        GPU_loop_MultMat_setup(1, data.image[aoconfID_DMmodes].md[0].name, data.image[aoconfID_cmd_modes].md[0].name, data.image[aoconfID_DM].md[0].name, AOconf[loop].GPU, 1);
        GPU_loop_MultMat_execute(1);
        #endif
    }
    AOconf[loop].DMupdatecnt ++;

    return(0);
}




int set_DM_modesRM(long loop)
{
  long k;
  long i, j;
  float *arrayf;



  arrayf = (float*) malloc(sizeof(float)*AOconf[loop].sizeDM);



	//printf("============ %ld %ld   %ld\n", aoconfID_cmd_modesRM, aoconfID_DMmodes, AOconf[loop].sizeDM);
	//fflush(stdout);
	//list_image_ID();


  for(j=0;j<AOconf[loop].sizeDM;j++)
    arrayf[j] = 0.0;

  for(k=0; k < AOconf[loop].NBDMmodes; k++)
    {
      for(i=0;i<AOconf[loop].sizeDM;i++)
	arrayf[i] += data.image[aoconfID_cmd_modesRM].array.F[k] * data.image[aoconfID_DMmodes].array.F[k*AOconf[loop].sizeDM+i];
    }


  data.image[aoconfID_DMRM].md[0].write = 1;
  memcpy (data.image[aoconfID_DMRM].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
  data.image[aoconfID_DMRM].md[0].cnt0++;
  data.image[aoconfID_DMRM].md[0].write = 0;

  free(arrayf);
  AOconf[loop].DMupdatecnt ++;


  return(0);
}





/** Maps amplitude of actuator effect on WFS */

long Measure_ActMap_WFS(long loop, double ampl, double delays, long NBave, char *WFS_actmap)
{
    long IDmap;
    long act, j, ii, kk;
    double value;
    long delayus;
    float *arrayf;
	char fname[200];
	char name[200];
    long IDpos, IDneg;
	float tot, v1, rms;
	long *sizearray;

	sizearray = (long*) malloc(sizeof(long)*2);

    delayus = (long) (1000000.0*delays);

    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);
    
    
    sprintf(fname, "./conf/AOloop.conf");
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);
    //exit(0);

    printf("Importing DM response matrix channel shared memory ...\n");
    aoconfID_DMRM = read_sharedmem_image(AOconf[loop].DMnameRM);

    printf("Importing WFS camera image shared memory ... \n");
    aoconfID_WFS = read_sharedmem_image(AOconf[loop].WFSname);



    sprintf(name, "aol%ld_imWFS1RM", loop);
    sizearray[0] = AOconf[loop].sizexWFS;
    sizearray[1] = AOconf[loop].sizeyWFS;
    aoconfID_WFS1 = create_image_ID(name, 2, sizearray, FLOAT, 1, 0);


    arrayf = (float*) malloc(sizeof(float)*AOconf[loop].sizeDM);

//    IDmap = create_2Dimage_ID(WFS_actmap, AOconf[loop].sizexDM, AOconf[loop].sizeyDM);
	sizearray[0] = AOconf[loop].sizexDM;
	sizearray[1] = AOconf[loop].sizeyDM;
    IDmap = create_image_ID(WFS_actmap, 2, sizearray, FLOAT, 1, 5);

    IDpos = create_2Dimage_ID("wfsposim", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
    IDneg = create_2Dimage_ID("wfsnegim", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);

    for(act=0; act<AOconf[loop].sizeDM; act++)
    {

        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
        {
            data.image[IDpos].array.F[ii] = 0.0;
            data.image[IDneg].array.F[ii] = 0.0;
        }

        /** Positive displacement */

        for(j=0; j<AOconf[loop].sizeDM; j++)
            arrayf[j] = 0.0;

        arrayf[act] = ampl;

        data.image[aoconfID_DMRM].md[0].write = 1;
        memcpy (data.image[aoconfID_DMRM].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
        data.image[aoconfID_DMRM].md[0].cnt0++;
        data.image[aoconfID_DMRM].md[0].write = 0;
        AOconf[loop].DMupdatecnt ++;

        usleep(delayus);

        for(kk=0; kk<NBave; kk++)
        {
            Average_cam_frames(loop, 1, 0);
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                data.image[IDpos].array.F[ii] += data.image[aoconfID_WFS1].array.F[ii];
        }

       /** Negative displacement */

        for(j=0; j<AOconf[loop].sizeDM; j++)
            arrayf[j] = 0.0;

        arrayf[act] = -ampl;

        data.image[aoconfID_DMRM].md[0].write = 1;
        memcpy (data.image[aoconfID_DMRM].array.F, arrayf, sizeof(float)*AOconf[loop].sizeDM);
        data.image[aoconfID_DMRM].md[0].cnt0++;
        data.image[aoconfID_DMRM].md[0].write = 0;
        AOconf[loop].DMupdatecnt ++;

        usleep(delayus);

        for(kk=0; kk<NBave; kk++)
        {
            Average_cam_frames(loop, 1, 0);
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                data.image[IDneg].array.F[ii] += data.image[aoconfID_WFS1].array.F[ii];
        }



		/** compute value */
		
		tot = 0.0;
		for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
			tot += data.image[IDpos].array.F[ii];
		tot /= AOconf[loop].sizeWFS;
		for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
			data.image[IDpos].array.F[ii] -= tot;

		tot = 0.0;
		for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
			tot += data.image[IDneg].array.F[ii];
		tot /= AOconf[loop].sizeWFS;
		for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
			data.image[IDneg].array.F[ii] -= tot;
	

		rms = 0.0;
		for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
			{
				v1 = data.image[IDneg].array.F[ii] - data.image[IDpos].array.F[ii];
				rms += v1*v1;
			}

        data.image[IDmap].array.F[act] = sqrt(rms);
    }

    free(arrayf);
    free(sizearray);

    return(IDmap);
}









/** Record periodic camera signal (to be used if there is a periodic camera error)
 *
 * folds the signal onto one period
 * 
 */

int AOloopControl_Measure_WFScam_PeriodicError(long loop, long NBframes, long NBpha, char *IDout_name)
{
    FILE *fp;
    char fname[200];
    long ii, jj, kk, kk1, kkmax;
    long IDrc, IDrefim;
    long IDout;

    double period; /// in frames
    double period_start = 1000.0;
    double period_end = 1200.0;
    double period_step;
    double pha;
    long *phacnt;
    long phal;
    long double rmsval;
    double rmsvalmin, rmsvalmax;
    double periodopt;
    long cnt;
    long p, p0, p1, pmin, pmax;

    double intpart;
    double tmpv1;

    double *coarsermsarray;
    double rmsvalmin1;
    int lOK;
    double level1, level2, level3;
    long pp1, pp2, pp3;
    int level1OK, level2OK, level3OK;

	long kw;
	char kname[200];
	char comment[200];
	

    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);


    IDrc = create_3Dimage_ID("Rcube", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, NBframes);
    IDout = create_3Dimage_ID(IDout_name, AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, NBpha);

    printf("SETTING UP... (loop %ld)\n", LOOPNUMBER);
    fflush(stdout);
    
    sprintf(fname, "./conf/AOloop.conf");
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);
    //exit(0);

    printf("Importing WFS camera image shared memory ... \n");
    aoconfID_WFS = read_sharedmem_image(AOconf[loop].WFSname);



    for(kk=0; kk<NBframes; kk++)
    {
        Average_cam_frames(loop, 1, 0);
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii] = data.image[aoconfID_WFS1].array.F[ii];
    }

    save_fits("Rcube", "!Rcube.fits");

    IDrefim = create_2Dimage_ID("refim", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
    for(kk=0; kk<NBframes; kk++)
    {
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDrefim].array.F[ii] += data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii];
    }
    for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
        data.image[IDrefim].array.F[ii] /= NBframes;

    for(kk=0; kk<NBframes; kk++)
    {
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii] -= data.image[IDrefim].array.F[ii];
    }
    save_fits("Rcube", "!R1cube.fits");


    /** find periodicity ( coarse search ) */
    fp = fopen("wfscampe_coarse.txt","w");
    fclose(fp);

    pmax = (long) NBframes/2;
    pmin = 0;
    rmsvalmin = 1.0e20;



    rmsvalmax = 0.0;
    p0 = 200;
    coarsermsarray = (double*) malloc(sizeof(double)*pmax);
    for(p=p0; p<pmax; p++)
    {
        rmsval = 0.0;
        kkmax = 100;
        if(kkmax+pmax>NBframes)
        {
            printf("ERROR: pmax, kkmax not compatible\n");
            exit(0);
        }

        for(kk=0; kk<kkmax; kk++)
        {
            kk1 = kk+p;
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            {
                tmpv1 = data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii] - data.image[IDrc].array.F[kk1*AOconf[loop].sizeWFS+ii];
                rmsval += tmpv1*tmpv1;
            }
        }
        rmsval = sqrt(rmsval/kkmax/AOconf[loop].sizeWFS);

        if(rmsval<rmsvalmin)
        {
            rmsvalmin = rmsval;
            pmin = p;
        }
        if(rmsval>rmsvalmax)
            rmsvalmax = rmsval;

        coarsermsarray[p] = rmsval;

        printf("%20ld  %20g     [ %20ld  %20g ]\n", p, (double) rmsval, pmin, rmsvalmin);
        fp = fopen("wfscampe_coarse.txt","a");
        fprintf(fp, "%20ld %20g\n", p, (double) rmsval);
        fclose(fp);
    }

    level1 = rmsvalmin + 0.2*(rmsvalmax-rmsvalmin);
    level1OK = 0; /// toggles to 1 when curve first goes above level1

    level2 = rmsvalmin + 0.8*(rmsvalmax-rmsvalmin);
    level2OK = 0; /// toggles to 1 when curve first goes above level2 after level1OK

    level3 = rmsvalmin + 0.2*(rmsvalmax-rmsvalmin);
    level3OK = 0; /// toggles to 1 when curve first goes above level3 after level2OK

    p = p0;
    p1 = 0;
    lOK = 0;
    rmsvalmin1 = rmsvalmax;
    while((lOK==0)&&(p<pmax))
    {
        if(level1OK==0)
            if(coarsermsarray[p]>level1)
                {
					level1OK = 1;
					pp1 = p;
				}
				
        if((level1OK==1)&&(level2OK==0))
            if(coarsermsarray[p]>level2)
                {
					level2OK = 1;
					pp2 = p;
				}
				
        if((level1OK==1)&&(level2OK==1)&&(level3OK==0))
            if(coarsermsarray[p]<level3)
				{
					pp3 = p;
					level3OK = 1;
				}
				
        if((level1OK==1)&&(level2OK==1)&&(level3OK==1))
        {
            if(coarsermsarray[p] < rmsvalmin1)
            {
                rmsvalmin1 = coarsermsarray[p];
                p1 = p;
            }

            if(coarsermsarray[p]>level2)
                lOK = 1;
        }
        p++;
    }

    free(coarsermsarray);


    printf("APPROXIMATE PERIOD = %ld   [%ld %ld %ld]  [%f %f %f]\n", p1, pp1, pp2, pp3, level1, level2, level3);

    /** find periodicity ( fine search ) */

    periodopt = 0.0;
    rmsvalmax = 0.0;

    fp = fopen("wfscampe.txt","w");
    fclose(fp);

    period_start = 1.0*p1 - 15.0;
    period_end = 1.0*p1 + 15.0;

    phacnt = (long*) malloc(sizeof(long)*NBpha);
    period_step = (period_end-period_start)/300.0;
    for(period=period_start; period<period_end; period += period_step)
    {
        for(kk=0; kk<NBpha; kk++)
            phacnt[kk] = 0;

        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDout].array.F[phal*AOconf[loop].sizeWFS+ii] = 0.0;

        for(kk=0; kk<NBframes; kk++)
        {
            pha = 1.0*kk/period;
            pha = modf(pha, &intpart);
            phal = (long) (1.0*NBpha*pha);

            if(phal>NBpha-1)
                phal = NBpha-1;
            if(phal<0)
                phal = 0;

            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                data.image[IDout].array.F[phal*AOconf[loop].sizeWFS+ii] += data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii];

            phacnt[phal]++;
        }

        rmsval = 0.0;
        cnt = 0;
        for(kk=0; kk<NBpha; kk++)
        {
            if(phacnt[kk]>0)
            {
                cnt++;
                for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                {
                    data.image[IDout].array.F[kk*AOconf[loop].sizeWFS+ii] /= phacnt[kk];
                    rmsval = data.image[IDout].array.F[kk*AOconf[loop].sizeWFS+ii]*data.image[IDout].array.F[kk*AOconf[loop].sizeWFS+ii];
                }
            }
        }


        rmsval = sqrt(rmsval/AOconf[loop].sizeWFS/cnt);
        if(rmsval>rmsvalmax)
        {
            rmsvalmax = rmsval;
            periodopt = period;
        }
        printf("%20f  %20g     [ %20f  %20g ]\n", period, (double) rmsval, periodopt, rmsvalmax);
        fp = fopen("wfscampe.txt","a");
        fprintf(fp, "%20f %20g\n", period, (double) rmsval);
        fclose(fp);
    }

    printf("EXACT PERIOD = %f\n", periodopt);

    kw = 0;
    sprintf(kname, "PERIOD");
    strcpy(data.image[IDout].kw[kw].name, kname);
    data.image[IDout].kw[kw].type = 'D';
    data.image[IDout].kw[kw].value.numf = (double) periodopt;
    sprintf(comment, "WFS cam error period");
    strcpy(data.image[IDout].kw[kw].comment, comment);


    /// building phase cube
    period = periodopt;

    for(kk=0; kk<NBpha; kk++)
        phacnt[kk] = 0;
    for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
        data.image[IDout].array.F[phal*AOconf[loop].sizeWFS+ii] = 0.0;

    for(kk=0; kk<NBframes; kk++)
    {
        pha = 1.0*kk/period;
        pha = modf(pha, &intpart);
        phal = (long) (1.0*NBpha*pha);

        if(phal>NBpha-1)
            phal = NBpha-1;
        if(phal<0)
            phal = 0;

        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDout].array.F[phal*AOconf[loop].sizeWFS+ii] += data.image[IDrc].array.F[kk*AOconf[loop].sizeWFS+ii];
        phacnt[phal]++;
    }

    rmsval = 0.0;
    cnt = 0;
    for(kk=0; kk<NBpha; kk++)
    {
        if(phacnt[kk]>0)
        {
            cnt++;
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            {
                data.image[IDout].array.F[kk*AOconf[loop].sizeWFS+ii] /= phacnt[kk];
            }
        }
    }



    free(phacnt);

    return(0);
}





/** remove WFS camera periodic error
 *
 * pha: phase from 0.0 to 1.0
 */

int AOloopControl_Remove_WFScamPE(char *IDin_name, char *IDcorr_name, double pha)
{
    long IDin;
    long IDcorr;
    long phal;
    long xsize, ysize, zsize, xysize;
    long ii;


    IDin = image_ID(IDin_name);
    IDcorr = image_ID(IDcorr_name);

    xsize = data.image[IDcorr].md[0].size[0];
    ysize = data.image[IDcorr].md[0].size[1];
    zsize = data.image[IDcorr].md[0].size[2];
    xysize = xsize*ysize;

    phal = (long) (1.0*pha*zsize);
    if(phal>zsize-1)
        phal -= zsize;

	

    for(ii=0; ii<xysize; ii++) {
        data.image[IDin].array.F[ii] -= data.image[IDcorr].array.F[xysize*phal+ii];
    }


    return(0);
}









/** measures response matrix AND reference */

int Measure_Resp_Matrix(long loop, long NbAve, float amp, long nbloop, long fDelay, long NBiter)
{
    long NBloops;
    long kloop;
    long delayus = 0; // delay in us
    long ii, i, imax;
    int Verbose = 1;
    long k1, k, k2;
    char fname[200];
    char name0[200];
    char name[200];

    long kk;
    long RespMatNBframes;
    long IDrmc;
    long kc;

    int recordCube = 1;
    long IDeigenmodes;

    long frameDelay = 0;
    long frameDelayMax = 50;
    long double RMsig;
    long double RMsigold;
    long kc0;
    FILE *fp;
    long NBexcl = 2; // number of frames excluded between DM mode changes
    long kc0min, kc0max;
    long IDrmtest;
    int vOK;


    long iter;
    long IDrmi;
    float beta = 0.0;
    float gain = 0.0001;
    long IDrmcumul;
    long IDrefi;
    long IDrefcumul;

    long *sizearray;

    long IDrespM;
    long IDrefWFS;
	
	int r;

	long IDoptsignal; // optical signal for each mode, cumulative
	long IDoptsignaln; // optical signal for each mode, normalize
	long IDmcoeff; // multiplicative gain to amplify low-oder modes
	long IDoptcnt;
	double rmsval;
	char signame[200];
	
	double normcoeff, normcoeffcnt;





    sizearray = (long*) malloc(sizeof(long)*3);

	printf("Initialize AOconf shared memory\n");
	fflush(stdout);
    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);
	printf("Initialization done\n");
	fflush(stdout);
 
	list_image_ID();
 
 
	printf("SETTING UP... (loop %ld)\n", LOOPNUMBER);
    fflush(stdout);
    
    sprintf(fname, "./conf/AOloop.conf");
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);
  
	
	printf("step 1\n");
	fflush(stdout);
	
	// create output
	IDrefWFS = create_2Dimage_ID("refwfsacq", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
	IDrespM = create_3Dimage_ID_float("respmacq", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);
  
  
	IDoptsignal = create_2Dimage_ID("optsig", AOconf[loop].NBDMmodes, 1);
	IDoptsignaln = create_2Dimage_ID("optsign", AOconf[loop].NBDMmodes, 1);
 	IDmcoeff = create_2Dimage_ID("mcoeff", AOconf[loop].NBDMmodes, 1);
 	IDoptcnt = create_2Dimage_ID("optsigcnt", AOconf[loop].NBDMmodes, 1);

	for(k=0; k<AOconf[loop].NBDMmodes; k++)
		{
			data.image[IDoptcnt].array.F[k] = 0.0;
			data.image[IDoptsignal].array.F[k] = 0.0;
			data.image[IDoptsignaln].array.F[k] = 0.0;
			data.image[IDmcoeff].array.F[k] = 1.0;
		}

	list_image_ID();
  
  	printf("step 2\n");
	fflush(stdout);

  
	RMACQUISITION = 1;


    if(fDelay==-1)
    {
        kc0min = 0;
        kc0max = frameDelayMax;
    }
    else
    {
        kc0min = fDelay;
        kc0max = fDelay+1;
    }


    RespMatNBframes = nbloop*2*AOconf[loop].NBDMmodes*NbAve;
    printf("%ld frames total\n", RespMatNBframes);
    fflush(stdout);

    if(recordCube == 1)
        IDrmc = create_3Dimage_ID("RMcube", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, RespMatNBframes+kc0max);

	

	

    IDrmi = create_3Dimage_ID("RMiter", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);
    IDrmcumul = create_3Dimage_ID("RMcumul", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);

    IDrefi = create_2Dimage_ID("REFiter", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
    IDrefcumul = create_2Dimage_ID("REFcumul", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);



	/// local arrays for image acquision
//	aoconfID_WFS = create_2Dimage_ID("RMwfs", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
	aoconfID_WFS0 = create_2Dimage_ID("RMwfs0", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
	aoconfID_WFS1 = create_2Dimage_ID("RMwfs1", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
	aoconfID_WFS2 = create_2Dimage_ID("RMwfs2", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);


	aoconfID_cmd_modesRM = create_2Dimage_ID("RMmodesloc", AOconf[loop].NBDMmodes, 1);
	

	

    for(iter=0; iter<NBiter; iter++)
    {
		if (file_exist ("stopRM.txt"))
			{
				r = system("rm stopRM.txt");
				iter = NBiter;
			}
		else
    {
		    NBloops = nbloop;

        // initialize RMiter to zero
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            for(k=0; k<AOconf[loop].NBDMmodes; k++)
                data.image[IDrmi].array.F[k*AOconf[loop].sizeWFS+ii] = 0.0;


        // initialize reference to zero
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDrefi].array.F[ii] = 0.0;


//        printf("\n");
  //      printf("Testing (in measure_resp_matrix function) :,  NBloops = %ld, NBmode = %ld\n",  NBloops, AOconf[loop].NBDMmodes);
    //    fflush(stdout);
        sleep(1);

        for(k2 = 0; k2 < AOconf[loop].NBDMmodes; k2++)
            data.image[aoconfID_cmd_modesRM].array.F[k2] = 0.0;
		

        kc = 0;
        for (kloop = 0; kloop < NBloops; kloop++)
        {
            if(Verbose)
            {
                printf("\n Loop %ld / %ld (%f)\n", kloop, NBloops, amp);
                fflush(stdout);
            }


            for(k1 = 0; k1 < AOconf[loop].NBDMmodes; k1++)
            {
				printf("\r  mode %ld / %ld   ", k1, AOconf[loop].NBDMmodes);
				fflush(stdout);
				
//				printf("\n\n  aoconfID_cmd_modesRM = %ld   [%ld]\n", aoconfID_cmd_modesRM, AOconf[loop].NBDMmodes);
				//list_image_ID();
				//fflush(stdout);
				
                for(k2 = 0; k2 < AOconf[loop].NBDMmodes; k2++)
                    data.image[aoconfID_cmd_modesRM].array.F[k2] = 0.0;

                // positive
                data.image[aoconfID_cmd_modesRM].array.F[k1] = amp*data.image[IDmcoeff].array.F[k1];

				
                set_DM_modesRM(loop);
                usleep(delayus);
			
	

                for(kk=0; kk<NbAve; kk++)
                {
                    Average_cam_frames(loop, 1, 1);


                    for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                    {
                        data.image[IDrefi].array.F[ii] += data.image[aoconfID_WFS1].array.F[ii];
                        data.image[IDrmc].array.F[kc*AOconf[loop].sizeWFS+ii] = data.image[aoconfID_WFS1].array.F[ii];
                    }
                    kc++;
                }


                // negative
                data.image[aoconfID_cmd_modesRM].array.F[k1] = 0.0-amp*data.image[IDmcoeff].array.F[k1];
                set_DM_modesRM(loop);

                usleep(delayus);

                for(kk=0; kk<NbAve; kk++)
                {
                    Average_cam_frames(loop, 1, 1);

                    for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                    {
                        data.image[IDrefi].array.F[ii] += data.image[aoconfID_WFS1].array.F[ii];
                        data.image[IDrmc].array.F[kc*AOconf[loop].sizeWFS+ii] = data.image[aoconfID_WFS1].array.F[ii];
                    }
                    kc++;
                }
            }
            
            printf("\n");
            fflush(stdout);
        }
        
        for(kk=0; kk<kc0max; kk++)
        {
            printf("additional frame %ld [%ld/%ld]... ", kk, kc, RespMatNBframes+kc0max);
            fflush(stdout);
            Average_cam_frames(loop, 1, 1);
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            {
                data.image[IDrefi].array.F[ii] += data.image[aoconfID_WFS1].array.F[ii];
                data.image[IDrmc].array.F[kc*AOconf[loop].sizeWFS+ii] = data.image[aoconfID_WFS1].array.F[ii];
            }
            kc++;
            printf("done\n");
            fflush(stdout);
        }


        for(k2 = 0; k2 < AOconf[loop].NBDMmodes; k2++)
            data.image[aoconfID_cmd_modesRM].array.F[k2] = 0.0;
        set_DM_modesRM(loop);







        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
                data.image[IDrmi].array.F[k1*AOconf[loop].sizeWFS+ii] /= (NBloops*2.0*amp*data.image[IDmcoeff].array.F[k1]*NbAve);

        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            data.image[IDrefi].array.F[ii] /= RespMatNBframes+kc0max; //(NBloops*2.0*AOconf[loop].NBDMmodes*NbAve);


//	save_fits("REFiter", "!test0.fits");
	//	save_fits("RMiter", "!test1.fits");
//exit(0);
        printf("Acquisition done, compiling results...");
        fflush(stdout);


 

        // PROCESS RMCUBE
        fp = fopen("TimeDelayRM.txt", "w");
        RMsig = 0.0;
        vOK = 1;
        IDrmtest = create_3Dimage_ID("rmtest", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, AOconf[loop].NBDMmodes);
        for(kc0=kc0min; kc0<kc0max; kc0++)
        {
            // initialize RM to zero
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                for(k=0; k<AOconf[loop].NBDMmodes; k++)
                    data.image[IDrmtest].array.F[k*AOconf[loop].sizeWFS+ii] = 0.0;


            // initialize reference to zero
            kc = kc0;
            for (kloop = 0; kloop < NBloops; kloop++)
            {
                for(k1 = 0; k1 < AOconf[loop].NBDMmodes; k1++)
                {
                    // positive
                    for(kk=0; kk<NbAve-NBexcl; kk++)
                    {
                        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                            data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii] += data.image[IDrmc].array.F[kc*AOconf[loop].sizeWFS+ii];
                        kc++;
                    }
                    kc+=NBexcl;

                    // negative
                    for(kk=0; kk<NbAve-NBexcl; kk++)
                    {
                        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                            data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii] -= data.image[IDrmc].array.F[kc*AOconf[loop].sizeWFS+ii];
                        kc++;
                    }
                    kc+=NBexcl;
                }
            }
            RMsigold = RMsig;
            RMsig = 0.0;
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
                {
                    data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii] /= (NBloops*2.0*amp*data.image[IDmcoeff].array.F[k1]*(NbAve-NBexcl));
                    RMsig += data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii]*data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii];
                }

            if(RMsig<RMsigold)
                vOK = 0;
            printf("Delay = %ld frame(s)   ->  RM signal = %lf   %d\n", kc0, (double) RMsig, vOK);
            fprintf(fp, "%ld %.12g\n", kc0, (double) RMsig);
            if(RMsig<RMsigold)
                vOK = 0;

            if(vOK==1) // ADOPT THIS MATRIX
            {
                for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                    for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
                        data.image[IDrmi].array.F[k1*AOconf[loop].sizeWFS+ii] += data.image[IDrmtest].array.F[k1*AOconf[loop].sizeWFS+ii];
            }
        }
        fclose(fp);




        printf("--- \n");
        fflush(stdout);
        save_fl_fits("rmtest", "!rmtest.fits");
        delete_image_ID("rmtest");




		printf("%ld %ld  %ld  %ld\n", IDrefcumul, IDrmcumul, IDrefWFS, IDrespM);
		
		
        beta = (1.0-gain)*beta + gain;
        for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
        {
            data.image[IDrefcumul].array.F[ii] = (1.0-gain)*data.image[IDrefcumul].array.F[ii] + gain*data.image[IDrefi].array.F[ii];

            data.image[IDrefWFS].array.F[ii] = data.image[IDrefcumul].array.F[ii]/beta;



            for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
           {
                data.image[IDrmcumul].array.F[k1*AOconf[loop].sizeWFS+ii] = (1.0-gain)*data.image[IDrmcumul].array.F[k1*AOconf[loop].sizeWFS+ii] + gain*data.image[IDrmi].array.F[k1*AOconf[loop].sizeWFS+ii];
                data.image[IDrespM].array.F[k1*AOconf[loop].sizeWFS+ii] = data.image[IDrmcumul].array.F[k1*AOconf[loop].sizeWFS+ii]/beta;
            }
        }

		for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
		{
			rmsval = 0.0;
			for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
				rmsval += data.image[IDrespM].array.F[k1*AOconf[loop].sizeWFS+ii]*data.image[IDrespM].array.F[k1*AOconf[loop].sizeWFS+ii];

			data.image[IDoptsignal].array.F[k1] += rmsval;
			data.image[IDoptcnt].array.F[k1] += 1.0; 
			
			data.image[IDoptsignaln].array.F[k1] = data.image[IDoptsignal].array.F[k1]/data.image[IDoptcnt].array.F[k1];
		}
		save_fits("optsignaln","!./tmp/RM_optsign.fits");
		
		sprintf(signame, "./tmp/RM_optsign_%06ld.txt", iter);

		normcoeff = 0.0;
		normcoeffcnt = 0.0;
		for(k1=AOconf[loop].NBDMmodes/2; k1<AOconf[loop].NBDMmodes; k1++)
		{
			normcoeff += data.image[IDoptsignaln].array.F[k1];
			normcoeffcnt += 1.0;
		}
		normcoeff /= normcoeffcnt;


		for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
			{
				data.image[IDmcoeff].array.F[k1] = 0.8*data.image[IDmcoeff].array.F[k1] + 0.2/(data.image[IDoptsignaln].array.F[k1]/normcoeff);
				if(data.image[IDmcoeff].array.F[k1]>5.0)
					data.image[IDmcoeff].array.F[k1] = 5.0;
			}
		

		fp = fopen(signame, "w");
		for(k1=0; k1<AOconf[loop].NBDMmodes; k1++)
			fprintf(fp, "%ld  %g  %g  %g\n", k1, data.image[IDoptsignaln].array.F[k1], data.image[IDoptcnt].array.F[k1], data.image[IDmcoeff].array.F[k1]*amp);
		fclose(fp);
		r = system("cp ./tmp/RM_outsign%06ld.txt ./tmp/RM_outsign.txt");

        save_fits("refwfsacq", "!./tmp/refwfs.fits");
        save_fits("respmacq", "!./tmp/respm.fits");
    }
	}


	fp = fopen("./tmp/rmparams.txt", "w");
	fprintf(fp, "%5ld		NbAve: number of WFS frames per averaging\n", NbAve);
	fprintf(fp, "%f			amp: nominal DM amplitude (RMS)\n", amp);
	fprintf(fp, "%ld		iter: number of iterations\n", iter);
	fprintf(fp, "%ld		nbloop: number of loops per iteration\n", nbloop);
	fprintf(fp, "%ld		fDelay: delay number of frames\n", fDelay);
	fclose(fp);

	

    printf("Done\n");
    free(sizearray);
	
    return(0);
}








int ControlMatrixMultiply( float *cm_array, float *imarray, long m, long n, float *outvect)
{
  long i;

  cblas_sgemv (CblasRowMajor, CblasNoTrans, m, n, 1.0, cm_array, n, imarray, 1, 0.0, outvect, 1);


  return(0);
}




int AOcompute(long loop)
{
    float total = 0.0;
    long k, k1, k2;
    long ii;
    long i;
    long m, n;
    long index;
    //  long long wcnt;
    // long long wcntmax;
    long cnttest;
    double a;



    // get dark-subtracted image
    AOconf[loop].status = 1;  // 1: READING IMAGE
    Average_cam_frames(loop, AOconf[loop].framesAve, 0);

    AOconf[loop].status = 4;  // 4: REMOVING REF


    for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
        data.image[aoconfID_WFS2].array.F[ii] = data.image[aoconfID_WFS1].array.F[ii] - data.image[aoconfID_refWFS].array.F[ii];


    cnttest = data.image[aoconfID_cmd1_modes].md[0].cnt0;
    data.image[aoconfID_WFS2].md[0].cnt0 ++;


    //  save_fits(data.image[aoconfID_WFS].md[0].name, "!testim.fits");
    // sleep(5);

    AOconf[loop].status = 5; // MULTIPLYING BY CONTROL MATRIX -> MODE VALUES


    if(AOconf[loop].GPU == 0)
    {
        ControlMatrixMultiply( data.image[aoconfID_contrM].array.F, data.image[aoconfID_WFS2].array.F, AOconf[loop].NBDMmodes, AOconf[loop].sizeWFS, data.image[aoconfID_cmd1_modes].array.F);
        data.image[aoconfID_cmd1_modes].md[0].cnt0 ++;
    }
    else
    {
#ifdef HAVE_CUDA
        GPU_loop_MultMat_setup(0, data.image[aoconfID_contrM].md[0].name, data.image[aoconfID_WFS2].md[0].name, data.image[aoconfID_cmd1_modes].md[0].name, AOconf[loop].GPU, 0);
        GPU_loop_MultMat_execute(0);
#endif
    }

    AOconf[loop].status = 6; //  MULTIPLYING BY GAINS


    AOconf[loop].RMSmodes = 0;
    for(k=0; k<AOconf[loop].NBDMmodes; k++)
        AOconf[loop].RMSmodes += data.image[aoconfID_cmd1_modes].array.F[k]*data.image[aoconfID_cmd1_modes].array.F[k];

    AOconf[loop].RMSmodesCumul += AOconf[loop].RMSmodes;
    AOconf[loop].RMSmodesCumulcnt ++;




/*
	list_image_ID();
	printf("UPDATING ARRAYS\n");
	printf("AOconf[loop].NBDMmodes = %ld\n", AOconf[loop].NBDMmodes);
	printf("IDs:  %ld %ld %ld %ld %ld %ld %ld\n", aoconfID_RMS_modes, aoconfID_cmd1_modes, aoconfID_AVE_modes, aoconfID_cmd_modes, aoconfID_GAIN_modes, aoconfID_LIMIT_modes, aoconfID_MULTF_modes);
	fflush(stdout);
	*/
	
	
	
    for(k=0; k<AOconf[loop].NBDMmodes; k++)
    {
        data.image[aoconfID_RMS_modes].array.F[k] = 0.99*data.image[aoconfID_RMS_modes].array.F[k] + 0.01*data.image[aoconfID_cmd1_modes].array.F[k]*data.image[aoconfID_cmd1_modes].array.F[k];
        data.image[aoconfID_AVE_modes].array.F[k] = 0.99*data.image[aoconfID_AVE_modes].array.F[k] + 0.01*data.image[aoconfID_cmd1_modes].array.F[k];

        data.image[aoconfID_cmd_modes].array.F[k] -= AOconf[loop].gain * data.image[aoconfID_GAIN_modes].array.F[k] * data.image[aoconfID_cmd1_modes].array.F[k];

        if(data.image[aoconfID_cmd_modes].array.F[k] < -AOconf[loop].maxlimit * data.image[aoconfID_LIMIT_modes].array.F[k])
            data.image[aoconfID_cmd_modes].array.F[k] = -AOconf[loop].maxlimit * data.image[aoconfID_LIMIT_modes].array.F[k];

        if(data.image[aoconfID_cmd_modes].array.F[k] > AOconf[loop].maxlimit * data.image[aoconfID_LIMIT_modes].array.F[k])
            data.image[aoconfID_cmd_modes].array.F[k] = AOconf[loop].maxlimit * data.image[aoconfID_LIMIT_modes].array.F[k];

        data.image[aoconfID_cmd_modes].array.F[k] *= data.image[aoconfID_MULTF_modes].array.F[k];


        // update total gain
   //     data.image[aoconfID_GAIN_modes].array.F[k+AOconf[loop].NBDMmodes] = AOconf[loop].gain * data.image[aoconfID_GAIN_modes].array.F[k];
    }


    data.image[aoconfID_cmd_modes].md[0].cnt0 ++;



    return(0);
}



int AOloopControl_run()
{
	FILE *fp;
    char fname[200];
    long loop;
    int vOK;

    long ID;
    long j, m;
    struct tm *uttime;
    time_t t;
    struct timespec *thetime = (struct timespec *)malloc(sizeof(struct timespec));
    char logfname[1000];
    char command[1000];
    int r;
    int RT_priority = 90; //any number from 0-99
    struct sched_param schedpar;
    double a;
    long cnttest;
	float tmpf1;


    schedpar.sched_priority = RT_priority;
    r = seteuid(euid_called); //This goes up to maximum privileges
    sched_setscheduler(0, SCHED_FIFO, &schedpar); //other option is SCHED_RR, might be faster
    r = seteuid(euid_real);//Go back to normal privileges

    loop = LOOPNUMBER;

    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);




    printf("SETTING UP...\n");
    sprintf(fname, "./conf/AOloop.conf");
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);
	
    vOK = 1;
    if(AOconf[loop].init_refWFS==0)
    {
        printf("ERROR: CANNOT RUN LOOP WITHOUT WFS REFERENCE\n");
        vOK = 0;
    }
    if(AOconf[loop].init_CM==0)
    {
        printf("ERROR: CANNOT RUN LOOP WITHOUT CONTROL MATRIX\n");
        vOK = 0;
    }


    if(vOK==1)
    {

        AOconf[loop].kill = 0;
        AOconf[loop].on = 0;
        printf("\n");
        while( AOconf[loop].kill == 0)
        {
            printf(" WAITING                    \r");
            fflush(stdout);
            usleep(1000);

			

            while(AOconf[loop].on == 1)
            {
                printf("LOOP IS RUNNING  %llu  %g      Gain = %f \r", AOconf[loop].cnt, AOconf[loop].RMSmodes, AOconf[loop].gain);
                fflush(stdout);
                usleep(10000);

                cnttest = data.image[aoconfID_DM].md[0].cnt0;
				
		/*		list_image_ID();
				printf("COMPUTING\n");
				fflush(stdout);
			*/
				
                AOcompute(loop);
	
	//			printf("DONE COMPUTING\n");
		//		fflush(stdout);
				
			//	list_image_ID();
				//exit(0);

				AOconf[loop].status = 7;

                if(fabs(AOconf[loop].gain)>1.0e-6)
                {
                    set_DM_modes(loop); // note: set_DM_modes will skip computation if GPU=1

                    a = 0.1;
                    while(cnttest==data.image[aoconfID_DM].md[0].cnt0)  // wait for results (only useful for GPU)
                    {
                        a = sqrt(a+0.1);
                    }
                }
                usleep(100); // max 10000kHz
	
                AOconf[loop].status = 8; //  LOGGING, part 1

                clock_gettime(CLOCK_REALTIME, &AOconf[loop].tnow);
                AOconf[loop].time_sec = 1.0*((long) AOconf[loop].tnow.tv_sec) + 1.0e-9*AOconf[loop].tnow.tv_nsec;

                if(AOconf[loop].logfnb==0)
                    ID = aoconfIDlog0;
                else
                    ID = aoconfIDlog1;

                if(AOconf[loop].logcnt==0)
                {
                    AOconf[loop].timeorigin_sec = (long) AOconf[loop].tnow.tv_sec;
                    data.image[ID].kw[0].value.numl = AOconf[loop].timeorigin_sec;
                }


	
                data.image[ID].array.F[AOconf[loop].logcnt*data.image[ID].md[0].size[0]+0] = AOconf[loop].time_sec - 1.0*AOconf[loop].timeorigin_sec;
                j = 1;

                for(m=0; m<AOconf[loop].NBDMmodes; m++)
                {
                    data.image[ID].array.F[AOconf[loop].logcnt*data.image[ID].md[0].size[0]+j] = AOconf[loop].gain;
                    j++;
                    data.image[ID].array.F[AOconf[loop].logcnt*data.image[ID].md[0].size[0]+j] = data.image[aoconfID_cmd1_modes].array.F[m];
                    j++;
                    data.image[ID].array.F[AOconf[loop].logcnt*data.image[ID].md[0].size[0]+j] = data.image[aoconfID_cmd_modes].array.F[m];
                    j++;
                }


                AOconf[loop].status = 9; //  LOGGING, part 2

                AOconf[loop].logcnt++;
                if(AOconf[loop].logcnt==AOconf[loop].logsize)
                {
                    if(AOconf[loop].logon == 1)
                    {
                        printf("Saving to disk...\n");
                        fflush(stdout);

                        t = time(NULL);
                        uttime = gmtime(&t);
                        clock_gettime(CLOCK_REALTIME, thetime);
                        printf("writing file name\n");
                        fflush(stdout);
                        sprintf(logfname, "%s/LOOP%ld_%04d%02d%02d-%02d:%02d:%02d.%09ld%s.log", AOconf[loop].logdir, LOOPNUMBER, 1900+uttime->tm_year, 1+uttime->tm_mon, uttime->tm_mday, uttime->tm_hour, uttime->tm_min, uttime->tm_sec, thetime->tv_nsec, AOconf[loop].userLOGstring);
                        printf("writing file name\n");
                        fflush(stdout);
                        sprintf(command, "cp /tmp/loop%ldlog%d.im.shm %s &", loop, AOconf[loop].logfnb, logfname);
                        printf("Executing command : %s\n", command);
                        fflush(stdout);
                        r = system(command);
                    }
                    AOconf[loop].logfnb++;
                    AOconf[loop].logcnt = 0;
                }
                if(AOconf[loop].logfnb == 2)
                    AOconf[loop].logfnb = 0;

                AOconf[loop].cnt++;

                if(AOconf[loop].cnt == AOconf[loop].cntmax)
                    AOconf[loop].on = 0;
            }

        }
    }

    free(thetime);


    return(0);
}







int AOloopControl_printloopstatus(long loop, long nbcol)
{
  long k, kmax;
  long col;
  float val;
  long nbl = 0;
  float AVElim = 0.01;
  float RMSlim = 0.01;
  
  printw("loop number %ld    ", loop);

  
  if(AOconf[loop].on == 1)
    printw("loop is ON     ");
  else
    printw("loop is OFF    ");

  if(AOconf[loop].logon == 1)
    printw("log is ON   ");
  else
    printw("log is OFF  ");
  

  printw("STATUS = %d  ", AOconf[loop].status);
  
  kmax = (wrow-3)*(nbcol);
  printw("Gain = %f   maxlim = %f     GPU = %d    kmax=%ld\n", AOconf[loop].gain, AOconf[loop].maxlimit, AOconf[loop].GPU, kmax);
  nbl++;

  printw("CNT : %lld  / %lld\n", AOconf[loop].cnt, AOconf[loop].cntmax);
  nbl++;  
  


   
  for(k=0;k<AOconf[loop].NBMblocks;k++)
    {
      if(k==0)
	printw("MODE BLOCK %ld   [ %4ld - %4ld ]  %4.2f  %4.2f  %4.2f\n", k, (long) 0, AOconf[loop].indexmaxMB[k], AOconf[loop].gainMB[k], AOconf[loop].limitMB[k], AOconf[loop].multfMB[k]);
      else
	printw("MODE BLOCK %ld   [ %4ld - %4ld ]  %4.2f  %4.2f  %4.2f\n", k, AOconf[loop].indexmaxMB[k-1], AOconf[loop].indexmaxMB[k], AOconf[loop].gainMB[k], AOconf[loop].limitMB[k], AOconf[loop].multfMB[k]);
      nbl++;
    }
 

  printw("            MODAL RMS (ALL MODES) : %6.4lf     AVERAGE :  %8.6lf       ( %20g / %8lld )\n", sqrt(AOconf[loop].RMSmodes), sqrt(AOconf[loop].RMSmodesCumul/AOconf[loop].RMSmodesCumulcnt), AOconf[loop].RMSmodesCumul, AOconf[loop].RMSmodesCumulcnt);

  
  print_header(" MODES ", '-');
  nbl++;




  if(kmax>AOconf[loop].NBDMmodes)
    kmax = AOconf[loop].NBDMmodes;

  col = 0;
  for(k=0;k<kmax;k++)
    {
      attron(A_BOLD);
      printw("%4ld ", k);
      attroff(A_BOLD);
   
      printw("[%4.2f %4.2f %5.3f] ", data.image[aoconfID_GAIN_modes].array.F[k], data.image[aoconfID_LIMIT_modes].array.F[k], data.image[aoconfID_MULTF_modes].array.F[k]);
      
      // print current value on DM
      val = data.image[aoconfID_cmd_modes].array.F[k];
      if(fabs(val)>0.99*AOconf[loop].maxlimit)
	{
	  attron(A_BOLD | COLOR_PAIR(2));
	  printw("%7.4f ", val);
	  attroff(A_BOLD | COLOR_PAIR(2));
	}
      else	
	{
	  if(fabs(val)>0.99*AOconf[loop].maxlimit*data.image[aoconfID_LIMIT_modes].array.F[k])
	    {
	      attron(COLOR_PAIR(1));
	      printw("%7.4f ", val);
	      attroff(COLOR_PAIR(1));
	    }
	  else
	    printw("%7.4f ", val);
	}

	// last reading from WFS
      printw("%7.4f ", data.image[aoconfID_cmd1_modes].array.F[k]);
      

	// Time average
      val = data.image[aoconfID_AVE_modes].array.F[k];
      if(fabs(val)>AVElim)
	{
	  attron(A_BOLD | COLOR_PAIR(2));
	  printw("%7.4f ", val);
	  attroff(A_BOLD | COLOR_PAIR(2));
	}
      else
	printw("%7.4f ", val);


	// RMS variation
        val = data.image[aoconfID_RMS_modes].array.F[k];
      if(fabs(val)>RMSlim)
	{
	  attron(A_BOLD | COLOR_PAIR(2));
	  printw("%7.4f ", val);
	  attroff(A_BOLD | COLOR_PAIR(2));
	}
      else
	printw("%7.4f ", val);

      col++;
      if(col==nbcol)
	{
	  col = 0;
	  printw("\n");
	}
      else
	printw(" | ");
    }


  return(0);
}



int AOloopControl_loopMonitor(long loop, double frequ, long nbcol)
{
  char name[200];

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  printf("MEMORY HAS BEEN INITIALIZED\n");
  fflush(stdout);

  // load arrays that are required
  if(aoconfID_cmd_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_cmd", loop);
      aoconfID_cmd_modes = read_sharedmem_image(name);
    }
     
  if(aoconfID_cmd1_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_cmd1", loop);
      aoconfID_cmd1_modes = read_sharedmem_image(name);
    }


   if(aoconfID_RMS_modes==-1)
     {
       sprintf(name, "aol%ld_DMmode_RMS", loop);
       aoconfID_RMS_modes = read_sharedmem_image(name);
     }
   
   if(aoconfID_AVE_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_AVE", loop);
      aoconfID_AVE_modes = read_sharedmem_image(name);
    }
   
   if(aoconfID_GAIN_modes==-1)
     {
       sprintf(name, "aol%ld_DMmode_GAIN", loop);
       aoconfID_GAIN_modes = read_sharedmem_image(name);
    }
   
   if(aoconfID_LIMIT_modes==-1)
     {
       sprintf(name, "aol%ld_DMmode_LIMIT", loop);
       aoconfID_LIMIT_modes = read_sharedmem_image(name);
     }

   if(aoconfID_MULTF_modes==-1)
     {
       sprintf(name, "aol%ld_DMmode_MULTF", loop);
       aoconfID_MULTF_modes = read_sharedmem_image(name);
     }

  
   initscr();		
   getmaxyx(stdscr, wrow, wcol);
   

   start_color();
   init_pair(1, COLOR_BLUE, COLOR_BLACK); 
   init_pair(2, COLOR_RED, COLOR_BLACK);
   init_pair(3, COLOR_GREEN, COLOR_BLACK);
   init_pair(4, COLOR_RED, COLOR_BLACK);
   
   while( !kbdhit() )
     {
       usleep((long) (1000000.0/frequ));
       clear();
       attron(A_BOLD);
       print_header(" PRESS ANY KEY TO STOP MONITOR ", '-');
       attroff(A_BOLD);
       
       AOloopControl_printloopstatus(loop, nbcol);
       
       refresh();
     }
   endwin();	
   
   return 0;
}



int AOloopControl_statusStats()
{
  long k;
  long NBkiter = 100000;
  long statusmax = 11;
  long *statuscnt;
  float usec0, usec1;
  int st;

  usec0 = 50.0; 
  usec1 = 150.0;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);
  
  printf("Measyuring loop status distribution \n");
  fflush(stdout);

  statuscnt = (long*) malloc(sizeof(long));

  for(st=0;st<statusmax;st++)
    statuscnt[st] = 0;

  for(k=0;k<NBkiter;k++)
    {
      usleep((long) (usec0+usec1*(1.0*k/NBkiter)));
      st = AOconf[LOOPNUMBER].status;
      if(st<statusmax)
	statuscnt[st]++;
    }
  
  for(st=0;st<statusmax;st++)
    printf("STATUS %2d     %5.2f %%\n", st, 100.0*statuscnt[st]/NBkiter);
    
  free(statuscnt);
  
  return 0;
}




int AOloopControl_showparams(long loop)
{
  printf("loop number %ld\n", loop);
  if(AOconf[loop].on == 1)
    printf("loop is ON\n");
  else
    printf("loop is OFF\n");
  if(AOconf[loop].logon == 1)
    printf("log is ON\n");
  else
    printf("log is OFF\n");
  printf("Gain = %f   maxlim = %f\n  GPU = %d\n", AOconf[loop].gain, AOconf[loop].maxlimit, AOconf[loop].GPU);

  return 0;
}



int AOloopControl_setLoopNumber(long loop)
{
  printf("LOOPNUMBER = %ld\n", loop);
  LOOPNUMBER = loop;
  
  /** append process name with loop number */
  

  return 0;
}

int AOloopControl_loopkill()
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].kill = 1;

  return 0;
}

int AOloopControl_loopon()
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].cntmax = AOconf[LOOPNUMBER].cnt-1;

  AOconf[LOOPNUMBER].on = 1;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}

int AOloopControl_loopstep(long loop, long NBstep)
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[loop].cntmax = AOconf[loop].cnt + NBstep;
  AOconf[LOOPNUMBER].RMSmodesCumul = 0.0;
  AOconf[LOOPNUMBER].RMSmodesCumulcnt = 0;

	//  printf("\nLOOP %ld STEP    %lld %ld %lld\n\n", loop, AOconf[loop].cnt, NBstep, AOconf[loop].cntmax);
	//fflush(stdout);
  
  AOconf[loop].on = 1;

  while(AOconf[loop].on==1)
    usleep(100);

  // AOloopControl_showparams(loop);

  return 0;
}



int AOloopControl_loopoff()
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].on = 0;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}



int AOloopControl_loopreset()
{
  char name[200];
  long k;
  long mb;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_cmd_modes==-1)
    {
      sprintf(name, "DMmode_cmd_%ld", LOOPNUMBER);
      aoconfID_cmd_modes = read_sharedmem_image(name);
    }

  AOconf[LOOPNUMBER].on = 0;
  for(k=0; k<AOconf[LOOPNUMBER].NBDMmodes; k++)
    data.image[aoconfID_cmd_modes].array.F[k] = 0.0;

  for(mb=0;mb<AOconf[LOOPNUMBER].NBMblocks;mb)
    {
      AOloopControl_setgainblock(mb, 0.0);
      AOloopControl_setlimitblock(mb, 0.01);
      AOloopControl_setmultfblock(mb, 0.95);
    }

  return 0;
}






int AOloopControl_logon()
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].logon = 1;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}

int AOloopControl_logoff()
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].logon = 0;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}

int AOloopControl_setgain(float gain)
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].gain = gain;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}

int AOloopControl_setmaxlimit(float maxlimit)
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].maxlimit = maxlimit;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}


int AOloopControl_setframesAve(long nbframes)
{
  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].framesAve = nbframes;
  AOloopControl_showparams(LOOPNUMBER);

  return 0;
}



int AOloopControl_setgainrange(long m0, long m1, float gainval)
{
  long k;
  long kmax;
  char name[200];

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_GAIN_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_GAIN", LOOPNUMBER);
      aoconfID_GAIN_modes = read_sharedmem_image(name);
    }

  kmax = m1+1;
  if(kmax>AOconf[LOOPNUMBER].NBDMmodes)
    kmax = AOconf[LOOPNUMBER].NBDMmodes-1;

  for(k=m0;k<kmax;k++)
    data.image[aoconfID_GAIN_modes].array.F[k] = gainval;

  return 0;
}



int AOloopControl_setlimitrange(long m0, long m1, float limval)
{
  long k;
  long kmax;
  char name[200];

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_LIMIT_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_LIMIT", LOOPNUMBER);
      aoconfID_LIMIT_modes = read_sharedmem_image(name);
    }

  kmax = m1+1;
  if(kmax>AOconf[LOOPNUMBER].NBDMmodes)
    kmax = AOconf[LOOPNUMBER].NBDMmodes-1;

  for(k=m0;k<kmax;k++)
    data.image[aoconfID_LIMIT_modes].array.F[k] = limval;

  return 0;
}


int AOloopControl_setmultfrange(long m0, long m1, float multfval)
{
  long k;
  long kmax;
  char name[200];

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_MULTF_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_MULTF", LOOPNUMBER);
      aoconfID_MULTF_modes = read_sharedmem_image(name);
    }

  kmax = m1+1;
  if(kmax>AOconf[LOOPNUMBER].NBDMmodes)
    kmax = AOconf[LOOPNUMBER].NBDMmodes-1;

  for(k=m0;k<kmax;k++)
    data.image[aoconfID_MULTF_modes].array.F[k] = multfval;

  return 0;
}


int AOloopControl_setgainblock(long mb, float gainval)
{
  long k;
  char name[200];
  long kmin, kmax;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_GAIN_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_GAIN", LOOPNUMBER);
      aoconfID_GAIN_modes = read_sharedmem_image(name);
    }

  if(mb<AOconf[LOOPNUMBER].NBMblocks)
    {
      if(mb==0)
	kmin = 0;
      else
	kmin = AOconf[LOOPNUMBER].indexmaxMB[mb-1];
      kmax = AOconf[LOOPNUMBER].indexmaxMB[mb];

      AOconf[LOOPNUMBER].gainMB[mb] = gainval;

      for(k=kmin; k<kmax; k++)
	data.image[aoconfID_GAIN_modes].array.F[k] = gainval;
    }

  return 0;
}


int AOloopControl_setlimitblock(long mb, float limitval)
{
  long k;
  char name[200];
  long kmin, kmax;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_LIMIT_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_LIMIT", LOOPNUMBER);
      aoconfID_LIMIT_modes = read_sharedmem_image(name);
    }

  if(mb<AOconf[LOOPNUMBER].NBMblocks)
    {
      if(mb==0)
	kmin = 0;
      else
	kmin = AOconf[LOOPNUMBER].indexmaxMB[mb-1];
      kmax = AOconf[LOOPNUMBER].indexmaxMB[mb];

      AOconf[LOOPNUMBER].limitMB[mb] = limitval;

      for(k=kmin; k<kmax; k++)
	data.image[aoconfID_LIMIT_modes].array.F[k] = limitval;
    }

  return 0;
}


int AOloopControl_setmultfblock(long mb, float multfval)
{
  long k;
  char name[200];
  long kmin, kmax;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_MULTF_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_MULTF", LOOPNUMBER);
      aoconfID_MULTF_modes = read_sharedmem_image(name);
    }

  if(mb<AOconf[LOOPNUMBER].NBMblocks)
    {
      if(mb==0)
	kmin = 0;
      else
	kmin = AOconf[LOOPNUMBER].indexmaxMB[mb-1];
      kmax = AOconf[LOOPNUMBER].indexmaxMB[mb];

      AOconf[LOOPNUMBER].multfMB[mb] = multfval;

      for(k=kmin; k<kmax; k++)
	data.image[aoconfID_MULTF_modes].array.F[k] = multfval;
    }

  return 0;
}




int AOloopControl_resetRMSperf()
{
  long k;
  char name[200];
  long kmin, kmax;

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  AOconf[LOOPNUMBER].RMSmodesCumul = 0.0;
  AOconf[LOOPNUMBER].RMSmodesCumulcnt = 0;

  return 0;
}



int AOloopControl_scanGainBlock(long NBblock, long NBstep, float gainStart, float gainEnd, long NBgain)
{
  long k, kg;
  float gain;
  float bestgain= 0.0;
  float bestval = 10000000.0;
  float val;
  char name[200];


 if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

  if(aoconfID_cmd_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_cmd", LOOPNUMBER);
      aoconfID_cmd_modes = read_sharedmem_image(name);
    }


  printf("Block: %ld, NBstep: %ld, gain: %f->%f (%ld septs)\n", NBblock, NBstep, gainStart, gainEnd, NBgain);

  for(kg=0;kg<NBgain;kg++)
    {
      for(k=0; k<AOconf[LOOPNUMBER].NBDMmodes; k++)
	data.image[aoconfID_cmd_modes].array.F[k] = 0.0;

      gain = gainStart + 1.0*kg/(NBgain-1)*(gainEnd-gainStart);
      AOloopControl_setgainblock(NBblock, gain); 
      AOloopControl_loopstep(LOOPNUMBER, NBstep);
      val = sqrt(AOconf[LOOPNUMBER].RMSmodesCumul/AOconf[LOOPNUMBER].RMSmodesCumulcnt);
      printf("%2ld  %6.4f  %10.8lf\n", kg, gain, val);
      
      if(val<bestval)
	{
	  bestval = val;
	  bestgain = gain;
	}
    }
  printf("BEST GAIN = %f\n", bestgain);
  
  AOloopControl_setgainblock(NBblock, bestgain);

  return(0);
}


int AOloopControl_InjectMode( long index, float ampl )
{
  long i;
  float *arrayf;
  char name[200];

 if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);

 if(aoconfID_DMmodes==-1)
   {
     sprintf(name, "aol%ld_DMmodes", LOOPNUMBER);
     aoconfID_DMmodes = read_sharedmem_image(name);
   }

  if(aoconfID_DMRM==-1)
    aoconfID_DMRM = read_sharedmem_image(AOconf[LOOPNUMBER].DMnameRM);


  if((index<0)||(index>AOconf[LOOPNUMBER].NBDMmodes-1))
    {
      printf("Invalid mode index... must be between 0 and %ld\n", AOconf[LOOPNUMBER].NBDMmodes);
    }    
  else
    {
      arrayf = (float*) malloc(sizeof(float)*AOconf[LOOPNUMBER].sizeDM);
      
      for(i=0;i<AOconf[LOOPNUMBER].sizeDM;i++)
	arrayf[i] = ampl*data.image[aoconfID_DMmodes].array.F[index*AOconf[LOOPNUMBER].sizeDM+i];
      
      
      
      data.image[aoconfID_DMRM].md[0].write = 1;
      memcpy (data.image[aoconfID_DMRM].array.F, arrayf, sizeof(float)*AOconf[LOOPNUMBER].sizeDM);
      data.image[aoconfID_DMRM].md[0].cnt0++;
      data.image[aoconfID_DMRM].md[0].write = 0;
      
      free(arrayf);
      AOconf[LOOPNUMBER].DMupdatecnt ++;
    }

  return(0);
}



int AOloopControl_AutoTune()
{
  long block;
  float gainStart = 0.0;
  float gainEnd = 1.0;

  long NBgain = 10;
  long NBstep = 10000;
  float gain;
  char name[200];
  long k, kg;
  float bestgain= 0.0;
  float bestval = 10000000.0;
  float val;
  
  int gOK;

  

  if(AOloopcontrol_meminit==0)
    AOloopControl_InitializeMemory(1);
  
  if(aoconfID_cmd_modes==-1)
    {
      sprintf(name, "aol%ld_DMmode_cmd", LOOPNUMBER);
      aoconfID_cmd_modes = read_sharedmem_image(name);
    }

  // initialize
  for(block=0; block<AOconf[LOOPNUMBER].NBMblocks; block++)
    {
      AOloopControl_setgainblock(block, 0.0);
	AOloopControl_setlimitblock(block, 0.1);
	AOloopControl_setmultfblock(block, 0.8);	
    }

  
  for(block=0; block<AOconf[LOOPNUMBER].NBMblocks; block++)
    {
      // tune block gain
      gOK = 1;
      gain = gainStart;
      bestval = 100000000.0;
      while((gOK==1)&&(gain<gainEnd))
	{
	  for(k=0; k<AOconf[LOOPNUMBER].NBDMmodes; k++)
	    data.image[aoconfID_cmd_modes].array.F[k] = 0.0;
	  
	  gain += 0.01;
	  gain *= 1.1;

	  AOloopControl_setgainblock(block, gain); 
	  AOloopControl_loopstep(LOOPNUMBER, NBstep);
	  val = sqrt(AOconf[LOOPNUMBER].RMSmodesCumul/AOconf[LOOPNUMBER].RMSmodesCumulcnt);
	  printf("%2ld  %6.4f  %10.8lf\n", kg, gain, val);
		  
	  if(val<bestval)
	    {
	      bestval = val;
	      bestgain = gain;
	    }
	  else 
	    gOK = 0;
	}
      printf("BLOCK %ld  : BEST GAIN = %f\n", block, bestgain);
      
      AOloopControl_setgainblock(block, bestgain);  
    }


  return(0);
}



int AOloopControl_tuneWFSsync(long loop, char *IDout_name)
{

    char fname[2000];
    long IDout, IDave;
    long *sizearraytmp;



    sizearraytmp = (long*) malloc(sizeof(long)*2);

    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);

    sprintf(fname, "./conf/AOloop.conf");
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);

    printf("Importing DM response matrix channel shared memory ...\n");
    aoconfID_DMRM = read_sharedmem_image(AOconf[loop].DMnameRM);

    printf("Importing WFS camera image shared memory ... \n");
    aoconfID_WFS = read_sharedmem_image(AOconf[loop].WFSname);




    IDave = create_2Dimage_ID("imWFSave", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);
    IDout = create_image_ID(IDout_name,  2, sizearraytmp, FLOAT, 1, 10);






    free(sizearraytmp);




  /*  FILE *fp;
    long IDout;
    long IDc;
    long IDave;
    long ii, jj, kk;
    char fname[2000];
    char command[2000];
    long delay1us = 2000000; // delay after changing frequency modulation
    long delay2us = 2000000; // delay after changing camera etime

    long double rmsvalue;
    long double avevalue;

    long fmodulator; // [0.1 Hz]
    long etimecam; // [us]

    long fmodulator_start = 4000; // 400 Hz
    long fmodulator_step = 100;
    long fmodulator_NBstep = 20;

    long etimecam_start = 600; // [us]
    long etimecam_step = 20;
    long etimecam_NBstep = 20;

    int r;
    long i, j;

    long NbAve = 2000; /// number of frames acquired



    if(AOloopcontrol_meminit==0)
        AOloopControl_InitializeMemory(0);

    sprintf(fname, "AOloop%ld.conf", LOOPNUMBER);
    AOloopControl_loadconfigure(LOOPNUMBER, fname, 1);

    printf("Importing DM response matrix channel shared memory ...\n");
    aoconfID_DMRM = read_sharedmem_image(AOconf[loop].DMnameRM);

    printf("Importing WFS camera image shared memory ... \n");
    aoconfID_WFS = read_sharedmem_image(AOconf[loop].WFSname);






    //	fp = fopen("WFSsync.log", "w");
    //	fclose(fp);

    IDc = create_3Dimage_ID("imWFScube", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS, NbAve);
    IDave = create_2Dimage_ID("imWFSave", AOconf[loop].sizexWFS, AOconf[loop].sizeyWFS);

    IDout = create_2Dimage_ID(IDout_name, fmodulator_NBstep, etimecam_NBstep);
    for(i=0; i<etimecam_NBstep; i++)
    {
        etimecam = etimecam_start + i*etimecam_step;

        sprintf(command, "zylaetime %f", 0.000570);
        printf("command : %s\n", command);
        r = system(command);
        usleep(delay2us);

        sprintf(command, "zylaetime %f", 1.0e-6*etimecam);
        printf("command : %s\n", command);
        r = system(command);
        usleep(delay2us);


        for(j=0; j<fmodulator_NBstep; j++)
        {

            fmodulator = fmodulator_start + j*fmodulator_step;
            sprintf(command, "modulator frequency %f", 1.0e-7*fmodulator);
            printf("command : %s\n", command);
            r = system(command);
            usleep(delay1us);

            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                data.image[IDave].array.F[ii] = 0.0;

            /// start collecting frames
            for(kk=0; kk<NbAve; kk++)
            {
                Average_cam_frames(loop, 1);
                for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                {
                    data.image[IDc].array.F[kk*AOconf[loop].sizeWFS+ii] = 1.0*data.image[aoconfID_WFS].array.U[ii];
                    data.image[IDave].array.F[ii] += 1.0*data.image[aoconfID_WFS].array.U[ii];
                }
            }

            /// processing
            avevalue = 0.0;
            for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
            {
                data.image[IDave].array.F[ii] /= NbAve;
                avevalue += data.image[IDave].array.F[ii];
            }
            rmsvalue = 0.0;
            for(kk=0; kk<NbAve; kk++)
                for(ii=0; ii<AOconf[loop].sizeWFS; ii++)
                {
                    data.image[IDc].array.F[kk*AOconf[loop].sizeWFS+ii] -= data.image[IDave].array.F[ii];
                    rmsvalue += data.image[IDc].array.F[kk*AOconf[loop].sizeWFS+ii]*data.image[IDc].array.F[kk*AOconf[loop].sizeWFS+ii];
                }
            rmsvalue = sqrt(rmsvalue/AOconf[loop].sizeWFS/NbAve);
            rmsvalue /= avevalue;
            data.image[IDout].array.F[j*etimecam_NBstep+i] = (float) rmsvalue;
            printf("%8.1f   %8.6f   %g   %g\n", 0.1*fmodulator, 1.0e-6*etimecam, (double) avevalue, (double) rmsvalue);
            fflush(stdout);
            fp = fopen("WFSsync.log", "a");
            fprintf(fp, "%8.1f   %8.6f   %g   %g\n", 0.1*fmodulator, 1.0e-6*etimecam, (double) avevalue, (double) rmsvalue);
            fclose(fp);

			printf("saving cube ... ");
			fflush(stdout);
            save_fits(IDout_name, "!WFSsync_out.fits");
			printf("\n");
			fflush(stdout);
        }
    }
    save_fits("imWFScube", "!imWFScube.fits");


*/
    return(0);
}





int AOloopControl_WFScamPEcorr_tryPhaseOffset()
{


	return(0);
}


int AOloopControl_setparam(long loop, char *key, double value)
{
	int pOK=0;
	char kstring[200];
	
	strcpy(kstring, "PEperiod");
	if((strncmp (key, kstring, strlen(kstring)) == 0)&&(pOK==0))
	{
		AOconf[loop].WFScamPEcorr_period = (long double) value;
		pOK = 1;
	}
			
	if(pOK==0)
		printf("Parameter not found\n");
	
	
	
	
	return (0);
}









