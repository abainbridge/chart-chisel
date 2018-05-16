// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef CLIP_RECTANGLE_H_INCLUDED
#define CLIP_RECTANGLE_H_INCLUDED


#include "number_formats.h"


class ClipRectangle
{
public:
    ClipRectangle()
    {
        SetClip(0, 0, 0, 0, 1);
    }

    // Constructor with initialization.
    ClipRectangle(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        SetClip(aX, aY, aWidth, aHeight, aScale);
    }

    void SetClip(int aX, int aY, int aWidth, int aHeight, int aScale)
    {
        m_minXi = aX;
        m_minYi = aY * aScale;
        m_maxXi = aX + aWidth;
        m_maxYi = (aY + aHeight) * aScale;

        m_xOffset = FLOAT_TO_RATIONAL(0.99f) / (RATIONAL)aScale;

        m_minXf = (RATIONAL)m_minXi + m_xOffset;
        m_maxXf = (RATIONAL)m_maxXi - m_xOffset;

        m_minYf = (RATIONAL)m_minYi;
        m_maxYf = (RATIONAL)m_maxYi;
    }

    // Returns the minimum Y as integer.
    inline int GetMinYi() const { return m_minYi; }

    // Returns the maximum Y as integer.
    inline int GetMaxYi() const { return m_maxYi; }

    // Returns the minimum X as rational number (typically float).
    inline RATIONAL GetMinXf() const { return m_minXf; }

    // Returns the minimum Y as rational number (typically float).
    inline RATIONAL GetMinYf() const { return m_minYf; }

    // Returns the maximum X as rational number (typically float).
    inline RATIONAL GetMaxXf() const { return m_maxXf; }

    // Returns the maximum Y as rational number (typically float).
    inline RATIONAL GetMaxYf() const { return m_maxYf; }

protected:
    int m_minXi;
    int m_minYi;
    int m_maxXi;
    int m_maxYi;
    RATIONAL m_minXf;
    RATIONAL m_minYf;
    RATIONAL m_maxXf;
    RATIONAL m_maxYf;
    RATIONAL m_xOffset;
};

#endif
