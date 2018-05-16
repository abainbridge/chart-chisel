// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#include "polygon.h"
#include <memory.h>
#include <stdlib.h>

#include "df_bitmap.h"
#include "clip_rectangle.h"
#include "coverage_table.h"
#include "number_formats.h"
#include "polygon_unroll_macros.h"


// Fixed point math inevitably introduces rounding error to the DDA. The error is
// fixed every now and then by a separate fix value. The defines below set these.
#define SLOPE_FIX_SHIFT 8
#define SLOPE_FIX_STEP (1 << SLOPE_FIX_SHIFT)
#define SLOPE_FIX_SCANLINES (1 << (SLOPE_FIX_SHIFT - SUBPIXEL_SHIFT))
#define SLOPE_FIX_SCANLINE_MASK (SLOPE_FIX_SCANLINES - 1)


// A optimized representation of a polygon edge.
class PolygonScanEdge
{
public:
    int m_lastLine;
    int m_firstLine;
    FIXED_POINT m_x;
    FIXED_POINT m_slope;
    FIXED_POINT m_slopeFix;
    class PolygonScanEdge *m_nextEdge;
};


enum POLYGON_CLIP_FLAGS
{
    POLYGON_CLIP_NONE = 0x00,
    POLYGON_CLIP_LEFT = 0x01,
    POLYGON_CLIP_RIGHT = 0x02,
    POLYGON_CLIP_TOP = 0x04,
    POLYGON_CLIP_BOTTOM = 0x08,
};


// A class for holding processing data regarding the vertex.
class VertexData
{
public:
    Vector2d m_position;
    int m_clipFlags;
    int m_line;
};



// ****************************************************************************
// PolygonData
// ****************************************************************************

PolygonData::PolygonData(int vertexCount)
{
    m_vertices = new Vector2d[vertexCount];
    m_vertexCount = vertexCount;
    m_vertexData = new VertexData[m_vertexCount + 1];
}


PolygonData::~PolygonData()
{
    delete[] m_vertices;
    delete[] m_vertexData;
}


// Calculates the clip flags for a point.
inline int PolygonData::GetClipFlags(const Vector2d &point, const ClipRectangle &clipRectangle) const
{
    int flags = POLYGON_CLIP_NONE;

    if (point.x < clipRectangle.GetMinXf())
        flags |= POLYGON_CLIP_LEFT;
    else if (point.x >= clipRectangle.GetMaxXf())
        flags |= POLYGON_CLIP_RIGHT;

    if (point.y < clipRectangle.GetMinYf())
        flags |= POLYGON_CLIP_TOP;
    else if (point.y >= clipRectangle.GetMaxYf())
        flags |= POLYGON_CLIP_BOTTOM;

    return flags;
}


// Calculates the edges of the polygon with transformation and clipping to edges array.
int PolygonData::GetScanEdges(PolygonScanEdge *edges, const ClipRectangle &clipRectangle) const
{
    for (int n = 0; n < m_vertexCount; n++)
    {
        // Transform vertex data into "subpixel" space
        m_vertexData[n].m_position.x = m_vertices[n].x + FLOAT_TO_RATIONAL(0.5f/SUBPIXEL_COUNT);
        m_vertexData[n].m_position.y = m_vertices[n].y * INT_TO_RATIONAL(SUBPIXEL_COUNT) + FLOAT_TO_RATIONAL(-0.5f);
    }

    for (int n = 0; n < m_vertexCount; n++)
    {
        // Calculate clip flags for all vertices.
        m_vertexData[n].m_clipFlags = GetClipFlags(m_vertexData[n].m_position, clipRectangle);

        // Calculate line of the vertex. If the vertex is clipped by top or bottom, the line
        // is determined by the clip rectangle.
        if (m_vertexData[n].m_clipFlags & POLYGON_CLIP_TOP)
        {
            m_vertexData[n].m_line = clipRectangle.GetMinYi();
        }
        else if (m_vertexData[n].m_clipFlags & POLYGON_CLIP_BOTTOM)
        {
            m_vertexData[n].m_line = clipRectangle.GetMaxYi() - 1;
        }
        else
        {
            m_vertexData[n].m_line = rationalToIntRoundDown(m_vertexData[n].m_position.y);
        }
    }

    // Copy the data from 0 to the last entry to make the data to loop.
    m_vertexData[m_vertexCount] = m_vertexData[0];

    // Transform the first vertex; store.
    // Process mVertexCount - 1 times, next is n+1
    // copy the first vertex to
    // Process 1 time, next is n

    int edgeCount = 0;
    for (int n = 0; n < m_vertexCount; n++)
    {
        int clipSum = m_vertexData[n].m_clipFlags | m_vertexData[n + 1].m_clipFlags;
        int clipUnion = m_vertexData[n].m_clipFlags & m_vertexData[n + 1].m_clipFlags;

        if ((clipUnion & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM)) == 0 &&
            m_vertexData[n].m_line != m_vertexData[n + 1].m_line)
        {
            int startIndex, endIndex;
            if (m_vertexData[n].m_position.y < m_vertexData[n + 1].m_position.y)
            {
                startIndex = n;
                endIndex = n + 1;
            }
            else
            {
                startIndex = n + 1;
                endIndex = n;
            }

            int firstLine = m_vertexData[startIndex].m_line + 1;
            int lastLine = m_vertexData[endIndex].m_line;

            if (clipUnion & POLYGON_CLIP_RIGHT)
            {
                // Both clip to right, edge is a vertical line on the right side
                edges[edgeCount].m_firstLine = firstLine;
                edges[edgeCount].m_lastLine = lastLine;
                edges[edgeCount].m_x = rationalToFixed(clipRectangle.GetMaxXf());
                edges[edgeCount].m_slope = INT_TO_FIXED(0);
                edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);

                edgeCount++;
            }
            else if (clipUnion & POLYGON_CLIP_LEFT)
            {
                // Both clip to left, edge is a vertical line on the left side
                edges[edgeCount].m_firstLine = firstLine;
                edges[edgeCount].m_lastLine = lastLine;
                edges[edgeCount].m_x = rationalToFixed(clipRectangle.GetMinXf());
                edges[edgeCount].m_slope = INT_TO_FIXED(0);
                edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);

                edgeCount++;
            }
            else if ((clipSum & (POLYGON_CLIP_RIGHT | POLYGON_CLIP_LEFT)) == 0)
            {
                // No clipping in the horizontal direction
                RATIONAL slope = (m_vertexData[endIndex].m_position.x -
                                  m_vertexData[startIndex].m_position.x) /
                                 (m_vertexData[endIndex].m_position.y -
                                  m_vertexData[startIndex].m_position.y);

                // If there is vertical clip (for the top) it will be processed here. The calculation
                // should be done for all non-clipping edges as well to determine the accurate position
                // where the edge crosses the first scanline.
                RATIONAL startx = m_vertexData[startIndex].m_position.x +
                                  ((RATIONAL)firstLine - m_vertexData[startIndex].m_position.y) * slope;

                edges[edgeCount].m_firstLine = firstLine;
                edges[edgeCount].m_lastLine = lastLine;
                edges[edgeCount].m_x = rationalToFixed(startx);
                edges[edgeCount].m_slope = rationalToFixed(slope);

                if (lastLine - firstLine >= SLOPE_FIX_STEP)
                {
                    edges[edgeCount].m_slopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                (edges[edgeCount].m_slope << SLOPE_FIX_SHIFT);
                }
                else
                {
                    edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                }

                edgeCount++;
            }
            else
            {
                // Clips to left or right or both.
                RATIONAL slope = (m_vertexData[endIndex].m_position.x -
                                  m_vertexData[startIndex].m_position.x) /
                                 (m_vertexData[endIndex].m_position.y -
                                  m_vertexData[startIndex].m_position.y);

                // The edge may clip to both left and right.
                // The clip results in one or two new vertices, and one to three segments.
                // The rounding for scanlines may produce a result where any of the segments is
                // ignored.

                // The start is always above the end. Calculate the clip positions to clipVertices.
                // It is possible that only one of the vertices exist. This will be detected from the
                // clip flags of the vertex later, so they are initialized here.
                VertexData clipVertices[2];

                if (m_vertexData[startIndex].m_position.x <
                    m_vertexData[endIndex].m_position.x)
                {
                    clipVertices[0].m_position.x = clipRectangle.GetMinXf();
                    clipVertices[1].m_position.x = clipRectangle.GetMaxXf();
                    clipVertices[0].m_clipFlags = POLYGON_CLIP_LEFT;
                    clipVertices[1].m_clipFlags = POLYGON_CLIP_RIGHT;
                }
                else
                {
                    clipVertices[0].m_position.x = clipRectangle.GetMaxXf();
                    clipVertices[1].m_position.x = clipRectangle.GetMinXf();
                    clipVertices[0].m_clipFlags = POLYGON_CLIP_RIGHT;
                    clipVertices[1].m_clipFlags = POLYGON_CLIP_LEFT;
                }

                for (int p = 0; p < 2; p++)
                {
                    // Check if either of the vertices crosses the edge marked for the clip vertex
                    if (clipSum & clipVertices[p].m_clipFlags)
                    {
                        // The the vertex is required, calculate it.
                        clipVertices[p].m_position.y = m_vertexData[startIndex].m_position.y +
                                                               (clipVertices[p].m_position.x - 
                                                                m_vertexData[startIndex].m_position.x) / slope;

                        // If there is clipping in the vertical direction, the new vertex may be clipped.
                        if (clipSum & (POLYGON_CLIP_TOP | POLYGON_CLIP_BOTTOM))
                        {
                            if (clipVertices[p].m_position.y < clipRectangle.GetMinYf())
                            {
                                clipVertices[p].m_clipFlags = POLYGON_CLIP_TOP;
                                clipVertices[p].m_line = clipRectangle.GetMinYi();
                            }
                            else if (clipVertices[p].m_position.y > clipRectangle.GetMaxYf())
                            {
                                clipVertices[p].m_clipFlags = POLYGON_CLIP_BOTTOM;
                                clipVertices[p].m_line = clipRectangle.GetMaxYi() - 1;
                            }
                            else
                            {
                                clipVertices[p].m_clipFlags = 0;
                                clipVertices[p].m_line = rationalToIntRoundDown(clipVertices[p].m_position.y);
                            }
                        }
                        else
                        {
                            clipVertices[p].m_clipFlags = 0;
                            clipVertices[p].m_line = rationalToIntRoundDown(clipVertices[p].m_position.y);
                        }
                    }
                }

                // Now there are three or four vertices, in the top-to-bottom order of start, clip0, clip1,
                // end. What kind of edges are required for connecting these can be determined from the
                // clip flags.
                // -if clip vertex has horizontal clip flags, it doesn't exist. No edge is generated.
                // -if start vertex or end vertex has horizontal clip flag, the edge to/from the clip vertex is vertical
                // -if the line of two vertices is the same, the edge is not generated, since the edge doesn't
                //  cross any scanlines.

                // The alternative patterns are:
                // start - clip0 - clip1 - end
                // start - clip0 - end
                // start - clip1 - end

                int topClipIndex;
                int bottomClipIndex;
                if (((clipVertices[0].m_clipFlags | clipVertices[1].m_clipFlags) &
                     (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT)) == 0)
                {
                    // Both sides are clipped, the order is start-clip0-clip1-end
                    topClipIndex = 0;
                    bottomClipIndex = 1;

                    // Add the edge from clip0 to clip1
                    // Check that the line is different for the vertices.
                    if (clipVertices[0].m_line != clipVertices[1].m_line)
                    {
                        int firstClipLine = clipVertices[0].m_line + 1;

                        RATIONAL startx = m_vertexData[startIndex].m_position.x +
                                          ((RATIONAL)firstClipLine - m_vertexData[startIndex].m_position.y) * slope;

                        edges[edgeCount].m_x = rationalToFixed(startx);
                        edges[edgeCount].m_slope = rationalToFixed(slope);
                        edges[edgeCount].m_firstLine = firstClipLine;
                        edges[edgeCount].m_lastLine = clipVertices[1].m_line;

                        if (edges[edgeCount].m_lastLine - edges[edgeCount].m_firstLine >= SLOPE_FIX_STEP)
                        {
                            edges[edgeCount].m_slopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (edges[edgeCount].m_slope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                        }

                        edgeCount++;
                    }
                }
                else
                {
                    // Clip at either side, check which side. The clip flag is on for the vertex
                    // that doesn't exist, i.e. has not been clipped to be inside the rect.
                    if (clipVertices[0].m_clipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        topClipIndex = 1;
                        bottomClipIndex = 1;
                    }
                    else
                    {
                        topClipIndex = 0;
                        bottomClipIndex = 0;
                    }
                }

                // Generate the edges from start - clip top and clip bottom - end
                // Clip top and clip bottom may be the same vertex if there is only one 
                // clipped vertex.

                // Check that the line is different for the vertices.
                if (m_vertexData[startIndex].m_line != clipVertices[topClipIndex].m_line)
                {
                    edges[edgeCount].m_firstLine = firstLine;
                    edges[edgeCount].m_lastLine = clipVertices[topClipIndex].m_line;

                    // If startIndex is clipped, the edge is a vertical one.
                    if (m_vertexData[startIndex].m_clipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        edges[edgeCount].m_x = rationalToFixed(clipVertices[topClipIndex].m_position.x);
                        edges[edgeCount].m_slope = INT_TO_FIXED(0);
                        edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = m_vertexData[startIndex].m_position.x +
                                          ((RATIONAL)firstLine - m_vertexData[startIndex].m_position.y) * slope;

                        edges[edgeCount].m_x = rationalToFixed(startx);
                        edges[edgeCount].m_slope = rationalToFixed(slope);

                        if (edges[edgeCount].m_lastLine - edges[edgeCount].m_firstLine >= SLOPE_FIX_STEP)
                        {
                            edges[edgeCount].m_slopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (edges[edgeCount].m_slope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                        }
                    }

                    edgeCount++;
                }

                // Check that the line is different for the vertices.
                if (clipVertices[bottomClipIndex].m_line != m_vertexData[endIndex].m_line)
                {
                    int firstClipLine = clipVertices[bottomClipIndex].m_line + 1;

                    edges[edgeCount].m_firstLine = firstClipLine;
                    edges[edgeCount].m_lastLine = lastLine;

                    // If endIndex is clipped, the edge is a vertical one.
                    if (m_vertexData[endIndex].m_clipFlags & (POLYGON_CLIP_LEFT | POLYGON_CLIP_RIGHT))
                    {
                        edges[edgeCount].m_x = rationalToFixed(clipVertices[bottomClipIndex].m_position.x);
                        edges[edgeCount].m_slope = INT_TO_FIXED(0);
                        edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                    }
                    else
                    {
                        RATIONAL startx = m_vertexData[startIndex].m_position.x +
                                          ((RATIONAL)firstClipLine - m_vertexData[startIndex].m_position.y) * slope;

                        edges[edgeCount].m_x = rationalToFixed(startx);
                        edges[edgeCount].m_slope = rationalToFixed(slope);

                        if (edges[edgeCount].m_lastLine - edges[edgeCount].m_firstLine >= SLOPE_FIX_STEP)
                        {
                            edges[edgeCount].m_slopeFix = rationalToFixed(slope * SLOPE_FIX_STEP) -
                                                        (edges[edgeCount].m_slope << SLOPE_FIX_SHIFT);
                        }
                        else
                        {
                            edges[edgeCount].m_slopeFix = INT_TO_FIXED(0);
                        }
                    }

                    edgeCount++;
                }

            }
        }
    }

    return edgeCount;
}



// ****************************************************************************
// PolygonFiller
// ****************************************************************************

PolygonFiller::PolygonFiller()
{
    m_edgeCount = 60;
    m_edgeStorage = new PolygonScanEdge[m_edgeCount];
    m_currentEdge = 0;

    m_maskBuffer = NULL;
    m_edgeTable = NULL;
}


PolygonFiller::~PolygonFiller()
{
    UnregisterBitmap();
    delete[] m_edgeStorage;
}


void PolygonFiller::DrawPoly(DfBitmap *bmp, const PolygonData *poly, DfColour colour)
{
    if (bmp != m_bitmap)
    {
        UnregisterBitmap();
        RegisterBitmap(bmp);
    }

    // Sets the round down mode in case it has been modified.
    setRoundDownMode();

    m_verticalExtents.reset();
    m_currentEdge = 0;

    ClipRectangle clipRect(0, 0, m_bitmap->width, m_bitmap->height, SUBPIXEL_COUNT);

    bool success = true;

    int count = poly->m_vertexCount;
    // The maximum amount of edges is 3 x the vertices.
    int freeCount = GetFreeEdgeCount() - count * 3;
    if (freeCount < 0 && !ResizeEdgeStorage(-freeCount))
    {
        success = false;
    }
    else
    {
        PolygonScanEdge *edges = &m_edgeStorage[m_currentEdge];
        int edgeCount = poly->GetScanEdges(edges, clipRect);

        for (int p = 0; p < edgeCount; p++)
        {
            int firstLine = edges[p].m_firstLine >> SUBPIXEL_SHIFT;
            int lastLine = edges[p].m_lastLine >> SUBPIXEL_SHIFT;

            edges[p].m_nextEdge = m_edgeTable[firstLine];
            m_edgeTable[firstLine] = &edges[p];

            m_verticalExtents.mark(firstLine,lastLine);
        }

        m_currentEdge += edgeCount;
    }

    if (success)
        Fill(colour);
    else
    {
        for (unsigned y = 0; y < m_bitmap->height; y++)
            m_edgeTable[y] = NULL;
    }
}


// Renders the edges from the current vertical index using even-odd fill.
inline void PolygonFiller::RenderEdges(PolygonScanEdge * &activeEdgeTable, SpanExtents &edgeExtents, int currentLine)
{
    static const FIXED_POINT offsets[SUBPIXEL_COUNT] = SUBPIXEL_OFFSETS_FIXED;

    // First, process edges in the active edge table. These are either full height edges
    // or edges that end before the last line. Remove those that end within the scan line.

    // Then, fetch the edges from the edge table. These are either such that the edge is
    // fully within this scanline or the edge starts from within the scanline and continues
    // to the next scanline. Add those that continue to the next scanline to the active
    // edge table. Clear the edge table.

    PolygonScanEdge *prevEdge = NULL;
    PolygonScanEdge *currentEdge = activeEdgeTable;

    while (currentEdge)
    {
        // The plotting of all edges in the AET starts from the top of the scanline.
        // The plotting is divided to two stages: those that end on this scanline and
        // those that span over the full scanline.

        int lastLine = currentEdge->m_lastLine >> SUBPIXEL_SHIFT;

        if (lastLine == currentLine)
        {
            FIXED_POINT x = currentEdge->m_x;
            FIXED_POINT slope = currentEdge->m_slope;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int ye = currentEdge->m_lastLine & (SUBPIXEL_COUNT - 1);

            SUBPIXEL_DATA mask = 1;
            int ySub;
            for (ySub = 0; ySub <= ye; ySub++)
            {
                int xp = FIXED_TO_INT(x + offsets[ySub]);
                m_maskBuffer[xp] ^= mask;
                mask <<= 1;
                x += slope;
            }

            // Last x-value rounded down.
            int xe = FIXED_TO_INT(x - slope);

            // Mark the span.
            edgeExtents.markWithSort(xs,xe);

            // Remove the edge from the active edge table
            currentEdge = currentEdge->m_nextEdge;
            if (prevEdge)
                prevEdge->m_nextEdge = currentEdge;
            else
                activeEdgeTable = currentEdge;
        }
        else
        {
            FIXED_POINT x = currentEdge->m_x;
            FIXED_POINT slope = currentEdge->m_slope;

            // First x value rounded down.
            int xs = FIXED_TO_INT(x);
            int xe;

            EVENODD_LINE_UNROLL_INIT();

            EVENODD_LINE_UNROLL_0(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_1(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_2(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_3(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_4(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_5(m_maskBuffer,x,slope,offsets);
            EVENODD_LINE_UNROLL_6(m_maskBuffer,x,slope,offsets);
#if SUBPIXEL_COUNT == 8
            xe = FIXED_TO_INT(x);
#endif
            EVENODD_LINE_UNROLL_7(m_maskBuffer,x,slope,offsets);

            // Mark the span.
            edgeExtents.markWithSort(xs,xe);

            // Update the edge
            if ((currentLine & SLOPE_FIX_SCANLINE_MASK) == 0)
                currentEdge->m_x = x + currentEdge->m_slopeFix;
            else
                currentEdge->m_x = x;

            // Proceed forward in the AET.
            prevEdge = currentEdge;
            currentEdge = currentEdge->m_nextEdge;
        }
    }

    // Fetch edges from the edge table.
    currentEdge = m_edgeTable[currentLine];

    if (currentEdge)
    {
        // Clear the edge table for this line.
        m_edgeTable[currentLine] = NULL;

        do
        {
            // The plotting of all edges in the edge table starts somewhere from the middle
            // of the scanline, and ends either at the end or at the last scanline. This leads
            // to two cases: either the scanning starts and stops within this scanline, or
            // it continues to the next as well.

            int lastLine = currentEdge->m_lastLine >> SUBPIXEL_SHIFT;

            if (lastLine == currentLine)
            {
                FIXED_POINT x = currentEdge->m_x;
                FIXED_POINT slope = currentEdge->m_slope;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->m_firstLine & (SUBPIXEL_COUNT - 1);
                int ye = currentEdge->m_lastLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub <= ye; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    m_maskBuffer[xp] ^= mask;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                edgeExtents.markWithSort(xs,xe);

                // Ignore the edge (don't add to AET)
            }
            else
            {
                FIXED_POINT x = currentEdge->m_x;
                FIXED_POINT slope = currentEdge->m_slope;

                // First x value rounded down.
                int xs = FIXED_TO_INT(x);

                int ys = currentEdge->m_firstLine & (SUBPIXEL_COUNT - 1);

                SUBPIXEL_DATA mask = 1 << ys;
                int ySub;
                for (ySub = ys; ySub < SUBPIXEL_COUNT; ySub++)
                {
                    int xp = FIXED_TO_INT(x + offsets[ySub]);
                    m_maskBuffer[xp] ^= mask;
                    mask <<= 1;
                    x += slope;
                }

                // Last x-value rounded down.
                int xe = FIXED_TO_INT(x - slope);

                // Mark the span.
                edgeExtents.markWithSort(xs,xe);

                // Update the edge
                currentEdge->m_x = x;

                // Add the edge to AET
                if (prevEdge)
                    prevEdge->m_nextEdge = currentEdge;
                else
                    activeEdgeTable = currentEdge;

                prevEdge = currentEdge;
            }

            currentEdge = currentEdge->m_nextEdge;
        }
        while (currentEdge);
    }

    if (prevEdge)
        prevEdge->m_nextEdge = NULL;
}


// Renders the mask to the canvas with even-odd fill.
void PolygonFiller::Fill(DfColour colour)
{
    int minY = m_verticalExtents.m_minimum;
    int maxY = m_verticalExtents.m_maximum;

    unsigned long cs1 = colour.c & 0xff00ff;
    unsigned long cs2 = (colour.c >> 8) & 0xff00ff;

    unsigned int pitch = m_bitmap->width;
    DfColour *target = &m_bitmap->pixels[minY * pitch];

    PolygonScanEdge *activeEdges = NULL;
    SpanExtents edgeExtents;

    for (int y = minY; y <= maxY; y++)
    {
        edgeExtents.reset();

        RenderEdges(activeEdges, edgeExtents, y);

        int minX = edgeExtents.m_minimum;
        // Offset values are not taken into account when calculating the extents, so add
        // one to the maximum. This makes sure that full spans are included, as offset
        // values are in the range of 0 to 1.
        int maxX = edgeExtents.m_maximum + 1;

        if (minX < maxX)
        {
            DfColour *tp = &target[minX];
            SUBPIXEL_DATA *mb = &m_maskBuffer[minX];
            SUBPIXEL_DATA *end = &m_maskBuffer[maxX + 1];

            // Place a marker data at the end position.
            // Since marker data is placed after the last entry to be rendered (thus mask is 0),
            // it forces a state change. This drops the control to the outer while loop, which
            // terminates because of the compare.
            *end = SUBPIXEL_FULL_COVERAGE;

            SUBPIXEL_DATA mask = *mb;
            *mb++ = 0;

            while (mb <= end)
            {
                // The straightforward implementation reads in the value, modifies the mask with it,
                // calculates the coverage from the mask and renders the result with it.

                // Here the mask value is fetched in the previous round. Therefore the write operation
                // needs to be done before the new mask value is changed. This also means that the
                // end marker will never be rendered, as the loop terminates before that.
                if (mask == 0)
                {
                    // Mask is empty, scan forward until mask changes.
                    SUBPIXEL_DATA *sb = mb;
                    do
                    {
                        mask = *mb++;
                    }
                    while (mask == 0); // && mb <= end);
                    mb[-1] = 0;
                    tp += mb - sb;
                }
                else if (mask == SUBPIXEL_FULL_COVERAGE)
                {
                    // Mask has full coverage, fill with aColor until mask changes.
                    SUBPIXEL_DATA temp;
                    do
                    {
                        *tp++ = colour;
                        temp = *mb++;
                    }
                    while (temp == 0); // && mb <= end);
                    mb[-1] = 0;
                    mask ^= temp;
                }
                else
                {
                    // Mask is semitransparent.
                    do
                    {
                        unsigned long alpha = SUBPIXEL_COVERAGE(mask);

                        // alpha is in range of 0 to SUBPIXEL_COUNT
                        unsigned long invAlpha = SUBPIXEL_COUNT - alpha;

                        unsigned long ct1 = (tp->c & 0xff00ff) * invAlpha;
                        unsigned long ct2 = ((tp->c >> 8) & 0xff00ff) * invAlpha;

                        ct1 = ((ct1 + cs1 * alpha) >> SUBPIXEL_SHIFT) & 0xff00ff;
                        ct2 = ((ct2 + cs2 * alpha) << (8 - SUBPIXEL_SHIFT)) & 0xff00ff00;

                        tp->c = ct1 + ct2;
                        tp++;

                        mask ^= *mb;
                        *mb++ = 0;
                    }
                    while (!(mask == 0 || mask == SUBPIXEL_FULL_COVERAGE)); // && mb <= end);
                }
            }
        }

        target += pitch;
    }
}


bool PolygonFiller::ResizeEdgeStorage(int increment)
{
    unsigned int newCount = m_edgeCount + increment;

    PolygonScanEdge *newStorage = new PolygonScanEdge[newCount];
    if (newStorage == NULL)
        return false;

    // Some pointer arithmetic to keep the linked lists in sync.
    // Make this 64-bit just for future compatibility
    INTEGER64 diff = (INTEGER64)newStorage - (INTEGER64)m_edgeStorage;

    for (unsigned n = 0; n < m_bitmap->height; n++)
    {
        if (m_edgeTable[n] != NULL)
            m_edgeTable[n] = (PolygonScanEdge *)((INTEGER64)m_edgeTable[n] + diff);
    }

    // Copy edge storage...
    memcpy(newStorage,m_edgeStorage,sizeof(PolygonScanEdge)*m_edgeCount);

    // ...and fix the pointers.
    for (unsigned n = 0; n < m_edgeCount; n++)
    {
        PolygonScanEdge *edge = &newStorage[n];
        if (edge->m_nextEdge != NULL)
            edge->m_nextEdge = (PolygonScanEdge *)((INTEGER64)edge->m_nextEdge + diff);
    }

    delete [] m_edgeStorage;
    m_edgeStorage = newStorage;
    m_edgeCount = newCount;

    return true;
}


void PolygonFiller::UnregisterBitmap()
{
    delete[] m_maskBuffer;
    delete[] m_edgeTable;
    m_bitmap = NULL;
}


void PolygonFiller::RegisterBitmap(DfBitmap *bmp)
{
    // The buffer used for filling needs to be three pixels wider than the bitmap.
    // This is because of three reasons: first, the buffer used for filling needs
    // to be one pixel wider because the end flag that turns the fill off is the
    // first pixel after the actually drawn edge. Second, the edge tracking needs
    // to add the maximum sample offset (i.e. 1) to the end of the edge. This
    // requires one pixel more in the buffer to avoid reading of data from the
    // left edge. Third, the rendering is stopped with an edge marker that is placed
    // after the data, thus requiring one pixel more for the maximum case.
    unsigned int bufferWidth = bmp->width + 3;

    m_maskBuffer = new SUBPIXEL_DATA[bufferWidth];
    memset(m_maskBuffer, 0, bufferWidth * sizeof(SUBPIXEL_DATA));

    m_edgeTable = new PolygonScanEdge*[bmp->height];
    memset(m_edgeTable, 0, bmp->height * sizeof(PolygonScanEdge *));

    m_bitmap = bmp;
}
