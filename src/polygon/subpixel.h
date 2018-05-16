// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef SUBPIXEL_H_INCLUDED
#define SUBPIXEL_H_INCLUDED

// We implement 8x8 subpixels per pixel. That means there's a shift
// of 3 to convert between pixel and subpixel coordinates.
#define SUBPIXEL_SHIFT 3

/*
 8x8 sparse supersampling mask:

 [][][][][]##[][] 5
 ##[][][][][][][] 0
 [][][]##[][][][] 3
 [][][][][][]##[] 6
 []##[][][][][][] 1
 [][][][]##[][][] 4
 [][][][][][][]## 7
 [][]##[][][][][] 2
*/

#define SUBPIXEL_COUNT (1 << SUBPIXEL_SHIFT)

#define SUBPIXEL_DATA unsigned char
#define SUBPIXEL_COVERAGE(a) (coverageTable[(a)])

#define SUBPIXEL_OFFSET_0 (5.0f/8.0f)
#define SUBPIXEL_OFFSET_1 (0.0f/8.0f)
#define SUBPIXEL_OFFSET_2 (3.0f/8.0f)
#define SUBPIXEL_OFFSET_3 (6.0f/8.0f)
#define SUBPIXEL_OFFSET_4 (1.0f/8.0f)
#define SUBPIXEL_OFFSET_5 (4.0f/8.0f)
#define SUBPIXEL_OFFSET_6 (7.0f/8.0f)
#define SUBPIXEL_OFFSET_7 (2.0f/8.0f)

#define SUBPIXEL_OFFSET_FIXED_0 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_0)
#define SUBPIXEL_OFFSET_FIXED_1 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_1)
#define SUBPIXEL_OFFSET_FIXED_2 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_2)
#define SUBPIXEL_OFFSET_FIXED_3 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_3)
#define SUBPIXEL_OFFSET_FIXED_4 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_4)
#define SUBPIXEL_OFFSET_FIXED_5 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_5)
#define SUBPIXEL_OFFSET_FIXED_6 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_6)
#define SUBPIXEL_OFFSET_FIXED_7 FLOAT_TO_FIXED(SUBPIXEL_OFFSET_7)

#define SUBPIXEL_OFFSETS_FIXED { SUBPIXEL_OFFSET_FIXED_0,\
                                 SUBPIXEL_OFFSET_FIXED_1,\
                                 SUBPIXEL_OFFSET_FIXED_2,\
                                 SUBPIXEL_OFFSET_FIXED_3,\
                                 SUBPIXEL_OFFSET_FIXED_4,\
                                 SUBPIXEL_OFFSET_FIXED_5,\
                                 SUBPIXEL_OFFSET_FIXED_6,\
                                 SUBPIXEL_OFFSET_FIXED_7 }

#define SUBPIXEL_FULL_COVERAGE 0xff


#endif
