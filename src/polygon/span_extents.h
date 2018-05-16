// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef SPAN_EXTENTS_H_INCLUDED
#define SPAN_EXTENTS_H_INCLUDED

// A class for keeping track of minimum and maximum values of a span.
class SpanExtents
{
public:
    // Marks a span. start and end should be sorted.
    inline void mark(int start, int end)
    {
        if (start < m_minimum)
            m_minimum = start;
        if (end > m_maximum)
            m_maximum = end;
    }

    // Marks a span. aStart and aEnd don't have to be sorted.
    inline void markWithSort(int start, int end)
    {
        if (start <= end)
            mark(start, end);
        else
            mark(end, start);
    }

    inline void reset()
    {
        m_minimum = 0x7fffffff;
        m_maximum = 0x80000000;
    }

    int m_minimum;
    int m_maximum;
};

#endif

