#include "CLIcore.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "COREMOD_arith/COREMOD_arith.h"
#include "COREMOD_iofits/COREMOD_iofits.h"
#include "COREMOD_tools/COREMOD_tools.h"
#include "fft/fft.h"
#include "info/info.h"
#include "statistic/statistic.h"
#include "linopt_imtools/linopt_imtools.h"
#include "image_filter/image_filter.h"
#include "image_basic/image_basic.h"
#include "WFpropagate/WFpropagate.h"
#include "ZernikePolyn/ZernikePolyn.h"
#include "coronagraphs/coronagraphs.h"
#include "OptSystProp/OptSystProp.h"
#include "PIAACMCsimul/PIAACMCsimul.h"
#include "image_format/image_format.h"
#include "img_reduce/img_reduce.h"
#include "AOloopControl_DM/AOloopControl_DM.h"
#include "AOsystSim/AOsystSim.h"

#include "AOloopControl_IOtools/AOloopControl_IOtools.h"
#include "AOloopControl_PredictiveControl/AOloopControl_PredictiveControl.h"
#include "AOloopControl_acquireCalib/AOloopControl_acquireCalib.h"
#include "AOloopControl_computeCalib/AOloopControl_computeCalib.h"
#include "AOloopControl_perfTest/AOloopControl_perfTest.h"
#include "AOloopControl_compTools/AOloopControl_compTools.h"

#include "FPAOloopControl/FPAOloopControl.h"
#include "psf/psf.h"
#include "AtmosphereModel/AtmosphereModel.h"
#include "AtmosphericTurbulence/AtmosphericTurbulence.h"
#include "cudacomp/cudacomp.h"
#include "SCExAO_control/SCExAO_control.h"
#include "TransitLC/TransitLC.h"
#include "linARfilterPred/linARfilterPred.h"


extern DATA data;

int init_modules()
{

//  init_COREMOD_memory();
//  init_COREMOD_arith();
//  init_COREMOD_iofits();
//  init_COREMOD_tools();
//  init_fft();
//  init_info();
//  init_statistic();
//  init_linopt_imtools();

//  init_image_filter();
//  init_image_basic();
//  init_WFpropagate();
//  init_ZernikePolyn();
//  init_coronagraphs();
//  init_OptSystProp();
//  init_PIAACMCsimul();
//  init_image_format();
//  init_img_reduce();
//  init_AOloopControl_DM();
//  init_AOsystSim();

//  init_AOloopControl_IOtools();
//  init_AOloopControl_PredictiveControl();
//  init_AOloopControl_acquireCalib();
//  init_AOloopControl_computeCalib();
//  init_AOloopControl_perfTest();
//  init_AOloopControl_compTools();

//  init_FPAOloopControl();
//  init_psf();
//  init_AtmosphereModel();
//  init_AtmosphericTurbulence();
//  init_cudacomp();
//  init_SCExAO_control();
//  init_TransitLC();
//  init_linARfilterPred();
 
  return 0;
}


/*
 * 
 * 
 * 
 * 
 * ../libtool  --tag=CC   --mode=link gcc  -g -O2  -I/C/common/inc/ -I/include -I/usr/local/cuda/include/ -DADD_ -I/usr/local/magma/include/  -L/lib64 -L/usr/local/cuda/lib64/ -L/usr/local/magma/lib/ -o cfitsTK cfitsTK-CLIcore.o cfitsTK-calc_bison.o cfitsTK-calc_flex.o linARfilterPred/liblinarfilterpred.la TransitLC/libtransitlc.la SCExAO_control/libscexaocontrol.la AtmosphericTurbulence/libatmosphericturbulence.la AtmosphereModel/libatmospheremodel.la psf/libpsf.la AOloopControl/libaoloopcontrol.la FPAOloopControl/libfpaoloopcontrol.la AOloopControl_perfTest/libaoloopcontrolperftest.la AOloopControl_computeCalib/libaoloopcontrolcomputecalib.la AOloopControl_acquireCalib/libaoloopcontrolacquirecalib.la AOloopControl_PredictiveControl/libaoloopcontrolpredictivecontrol.la AOloopControl_IOtools/libaoloopcontroliotools.la AOloopControl_compTools/libaoloopcontrolcomptools.la AOsystSim/libaosystsim.la AOloopControl_DM/libaoloopcontroldm.la img_reduce/libimgreduce.la image_format/libimageformat.la PIAACMCsimul/libpiaacmcsimul.la OptSystProp/liboptsystprop.la OpticsMaterials/libopticsmaterials.la coronagraphs/libcoronagraphs.la ZernikePolyn/libzernikepolyn.la WFpropagate/libwfpropagate.la image_basic/libimagebasic.la image_filter/libimagefilter.la kdtree/libkdtree.la image_gen/libimagegen.la linopt_imtools/liblinoptimtools.la cudacomp/libcudacomp.la statistic/libstatistic.la info/libinfo.la fft/libfft.la COREMOD_arith/libcoremodarith.la COREMOD_iofits/libcoremodiofits.la COREMOD_memory/libcoremodmemory.la COREMOD_tools/libcoremodtools.la 00CORE/lib00core.la ImageStreamIO/libimagestreamio.la -lgomp -lfftw3 -lfftw3f -lncurses -lreadline -lcfitsio -lgsl -lgslcblas -lrt -ldl -lm  -lcuda -lcudart -lcufft -lcublas -lcusolver -lmagma  -lpthread
 * 
 * gcc -g -O2 -I/C/common/inc/ -I/include -I/usr/local/cuda/include/ -DADD_ -I/usr/local/magma/include/ -o .libs/cfitsTK cfitsTK-CLIcore.o cfitsTK-calc_bison.o cfitsTK-calc_flex.o  -L/lib64 -L/usr/local/cuda/lib64/ -L/usr/local/magma/lib/ linARfilterPred/.libs/liblinarfilterpred.so TransitLC/.libs/libtransitlc.so SCExAO_control/.libs/libscexaocontrol.so AtmosphericTurbulence/.libs/libatmosphericturbulence.so AtmosphereModel/.libs/libatmospheremodel.so psf/.libs/libpsf.so AOloopControl/.libs/libaoloopcontrol.so FPAOloopControl/.libs/libfpaoloopcontrol.so AOloopControl_perfTest/.libs/libaoloopcontrolperftest.so AOloopControl_computeCalib/.libs/libaoloopcontrolcomputecalib.so AOloopControl_acquireCalib/.libs/libaoloopcontrolacquirecalib.so AOloopControl_PredictiveControl/.libs/libaoloopcontrolpredictivecontrol.so AOloopControl_IOtools/.libs/libaoloopcontroliotools.so AOloopControl_compTools/.libs/libaoloopcontrolcomptools.so AOsystSim/.libs/libaosystsim.so AOloopControl_DM/.libs/libaoloopcontroldm.so img_reduce/.libs/libimgreduce.so image_format/.libs/libimageformat.so PIAACMCsimul/.libs/libpiaacmcsimul.so OptSystProp/.libs/liboptsystprop.so OpticsMaterials/.libs/libopticsmaterials.so coronagraphs/.libs/libcoronagraphs.so ZernikePolyn/.libs/libzernikepolyn.so WFpropagate/.libs/libwfpropagate.so image_basic/.libs/libimagebasic.so image_filter/.libs/libimagefilter.so kdtree/.libs/libkdtree.so image_gen/.libs/libimagegen.so linopt_imtools/.libs/liblinoptimtools.so cudacomp/.libs/libcudacomp.so statistic/.libs/libstatistic.so info/.libs/libinfo.so fft/.libs/libfft.so COREMOD_arith/.libs/libcoremodarith.so COREMOD_iofits/.libs/libcoremodiofits.so COREMOD_memory/.libs/libcoremodmemory.so COREMOD_tools/.libs/libcoremodtools.so 00CORE/.libs/lib00core.so ImageStreamIO/.libs/libimagestreamio.so -lgomp -lfftw3 -lfftw3f -lncurses -lreadline -lcfitsio -lgsl -lgslcblas -lrt -ldl -lm -lcuda -lcudart -lcufft -lcublas -lcusolver -lmagma -lpthread -Wl,-rpath -Wl,/home/olivier/src/Cfits/lib
 * 
 * 
 */ 

