// Scanline edge-flag algorithm for antialiasing
// Copyright (c) 2005-2007 Kiia Kallio
// Refactored by Andrew Bainbridge 2015
// 
// http://mlab.uiah.fi/~kkallio/antialiasing/
//  
// This code is distributed under the three-clause BSD license.
// Read the LICENSE file or visit the URL above for details.


#ifndef POLYGON_H_INCLUDED
#define POLYGON_H_INCLUDED


#include "df_bitmap.h"
#include "span_extents.h"
#include "subpixel.h"
#include "vector2d.h"


class ClipRectangle;
class PolygonScanEdge;
class VertexData;


// A class to store vertex data for a Polygon. Also provides temporary working data
// used for clipping etc.
class PolygonData
{
public:
    // Create a polygon with a specified number of vertices. Once created you can access
    // m_vertexData directly to set the vertex positions.
    PolygonData(int vertexCount);
    ~PolygonData();

    // Calculates the edges of the polygon with transformation and clipping to aEdges array.
    // Note that this may return up to three times the amount of edges that the polygon has vertices,
    // in the unlucky case where both left and right side get clipped for all edges.
    // \param aEdges the array for result edges. This should be able to contain 2*aVertexCount edges.
    // \param aClipRectangle the clip rectangle.
    // \return the amount of edges in the result.
    int GetScanEdges(PolygonScanEdge *edges, const ClipRectangle &clipRectangle) const;

public:
    int m_vertexCount;
    Vector2d *m_vertices;

protected:
    // Calculates the clip flags for a point.
    inline int GetClipFlags(const Vector2d &point, const ClipRectangle &clipRectangle) const;

    VertexData *m_vertexData;
};


// A class used to render a polygon onto a bitmap.
class PolygonFiller
{
public:
    PolygonFiller();
    ~PolygonFiller();

    void DrawPoly(DfBitmap *bmp, const PolygonData *polyData, DfColour colour);

protected:
    // Renders the edges from the current vertical index using even-odd fill.
    inline void RenderEdges(PolygonScanEdge * &activeEdgeTable, SpanExtents &edgeExtents, int currentLine);

    // Renders the mask to the canvas with even-odd fill.
    void Fill(DfColour colour);

    // Resizes the edge storage.
    bool ResizeEdgeStorage(int increment);

    // Returns the amount of free edges in the edge storage.
    inline int GetFreeEdgeCount()
    {
        return m_edgeCount - m_currentEdge - 1;
    }

    void RegisterBitmap(DfBitmap *bmp);
    void UnregisterBitmap();

    DfBitmap *m_bitmap;   // Bitmap that we have registered

    SUBPIXEL_DATA *m_maskBuffer;
    SpanExtents m_verticalExtents;

    PolygonScanEdge **m_edgeTable;
    PolygonScanEdge *m_edgeStorage;
    unsigned int m_edgeCount;
    unsigned int m_currentEdge;
};


#endif
