/**
* @file gears.h
*
*/

#ifndef GEARS_H
#define GEARS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
*      INCLUDES
*********************/

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**********************
* GLOBAL PROTOTYPES
**********************/
void gears_init(int xsize, int ysize, int mode, void *buf);
void gears_update(void);

/**********************
*      MACROS
**********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*GEARS_H*/
