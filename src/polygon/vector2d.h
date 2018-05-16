// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef VECTOR2D_H_INCLUDED
#define VECTOR2D_H_INCLUDED

#include "number_formats.h"

class Vector2d
{
public:
    inline Vector2d() {}

    inline Vector2d(RATIONAL aX, RATIONAL aY)
    {
        x = aX;
        y = aY;
    }

    inline void Set(RATIONAL aX, RATIONAL aY) 
    { 
        x = aX; y = aY; 
    }

    RATIONAL x;
    RATIONAL y;
};

#endif
