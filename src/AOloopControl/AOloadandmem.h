/**
 * @file    AOload.h
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


#ifndef LOAD_H
#define LOAD_H

int_fast8_t AOloopControl_loadconfigure(long loop, int mode, int level);

int_fast8_t AOloopControl_InitializeMemory(int mode);

#endif