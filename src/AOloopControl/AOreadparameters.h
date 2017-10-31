/**
 * @file    AOreadparameters.h
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


#ifndef READPARAM_H
#define READPARAM_H

float AOloopControl_readParam_float(char *paramname, float defaultValue, FILE *fplog);

int AOloopControl_readParam_int(char *paramname, int defaultValue, FILE *fplog);

char* AOloopControl_readParam_string(char *paramname, char* defaultValue, FILE *fplog);


#endif 