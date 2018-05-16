// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef NUMBER_FORMATS_H_INCLUDED
#define NUMBER_FORMATS_H_INCLUDED

#include <float.h>

// Defines for the rational number format.
#define RATIONAL float
#define INT_TO_RATIONAL(a) ((float)(a))
#define FLOAT_TO_RATIONAL(a) (a)

/*
#define RATIONAL double
#define INT_TO_RATIONAL(a) ((double)(a))
#define FLOAT_TO_RATIONAL(a) ((double)(a))
*/

// Fixed point conversions
#define FIXED_POINT int
#define FIXED_POINT_SHIFT 16
#define INT_TO_FIXED(a) ((a) << FIXED_POINT_SHIFT)
#define FLOAT_TO_FIXED(a) (int)((a)*((float)(1 << FIXED_POINT_SHIFT)))
#define FIXED_TO_INT(a) ((a) >> FIXED_POINT_SHIFT) 

// Define for 64-bit integer number format
#define INTEGER64 __int64


#define USE_ASM_OPTIMIZATIONS

// Converts rational to int using direct asm.
/*! Typical conversions using _ftol are slow on Pentium 4, as _ftol sets the control
 *  state of the fpu unit. Also, we need a special rounding operation that rounds down,
 *  not towards zero. Therefore we use asm here. This requires that the rounding mode
 *  is set to round down by calling _controlfp(_MCW_RC,_RC_DOWN). The state of the
 *  control mode is thread specific, so other applications won't mix with this.
 */
inline int rationalToIntRoundDown(RATIONAL a)
{
#ifdef USE_ASM_OPTIMIZATIONS
    int i;
    _asm
    {
        fld a
        fistp i
    }
    return i;
#else
    return ((int)((a) + INT_TO_RATIONAL(1)) - 1);
#endif
}


// The rational to fixed conversion is implemented with asm for performance reasons.
inline int rationalToFixed(RATIONAL a)
{
#ifdef USE_ASM_OPTIMIZATIONS
    int i;
    static const double scale = (double)(1 << FIXED_POINT_SHIFT);
    _asm
    {
        fld a
        fmul scale
        fistp i
    }
    return i;
#else
    return FLOAT_TO_FIXED(a);
#endif
}


// Sets the FPU round down mode to correct value for the asm conversion routines.
inline void setRoundDownMode()
{
#ifdef USE_ASM_OPTIMIZATIONS
    _controlfp(_MCW_RC, _RC_DOWN);
#endif
}


#endif
