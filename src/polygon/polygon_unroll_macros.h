// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef POLYGON_UNROLL_MACROS_H_INCLUDED
#define POLYGON_UNROLL_MACROS_H_INCLUDED

#define EVENODD_LINE_UNROLL_INIT() \
        int __evenodd_line_xp__;

#define EVENODD_LINE_UNROLL_0(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_0; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x01;

#define EVENODD_LINE_UNROLL_1(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_1; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x02;

#define EVENODD_LINE_UNROLL_2(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_2; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x04;

#define EVENODD_LINE_UNROLL_3(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_3; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x08;

#define EVENODD_LINE_UNROLL_4(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_4; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x10;

#define EVENODD_LINE_UNROLL_5(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_5; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x20;

#define EVENODD_LINE_UNROLL_6(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_6; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x40;

#define EVENODD_LINE_UNROLL_7(aScanline,aX,aSlope,aOffsets) \
        __evenodd_line_xp__ = aX +  SUBPIXEL_OFFSET_FIXED_7; \
        aX += aSlope; \
        __evenodd_line_xp__ = FIXED_TO_INT(__evenodd_line_xp__); \
        aScanline[__evenodd_line_xp__] ^= 0x80;

#endif
