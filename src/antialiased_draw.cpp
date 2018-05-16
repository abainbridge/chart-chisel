#include "antialiased_draw.h"

#include "polygon/polygon.h"
#include <math.h>
#include <stdlib.h>


AntialiasedDraw *g_antialiasedDraw = NULL;


AntialiasedDraw::AntialiasedDraw()
{
    m_polyFiller = new PolygonFiller();
    m_polyData = new PolygonData(4);
}


AntialiasedDraw::~AntialiasedDraw()
{
    delete m_polyFiller;
    delete m_polyData;
}


void AntialiasedDraw::DrawSingleArrow(DfBitmap *bmp, Vector2 start, Vector2 end, float thickness, DfColour c)
{
    Vector2 delta = end - start;
    Vector2 ortho(delta.y, -delta.x);

    float const arrowHeadLen = thickness * 10.0;
    float const arrowHeadWidth = thickness * 7.0;
    delta.SetLen(arrowHeadLen);
    Vector2 lineB = end - delta;

    Vector2d *verts = m_polyData->m_vertices;

    // Draw arrow head        
    ortho.SetLen(arrowHeadWidth/2.0);
    verts[0].Set(end.x, end.y);
    verts[1].Set(lineB.x + ortho.x, lineB.y + ortho.y);
    verts[2].Set(lineB.x - ortho.x, lineB.y - ortho.y);
    m_polyData->m_vertexCount = 3;
    m_polyFiller->DrawPoly(bmp, m_polyData, c);

    // Draw main part of line
    ortho.SetLen(thickness / 2.0);
    verts[0].Set(start.x + ortho.x, start.y + ortho.y);
    verts[1].Set(lineB.x + ortho.x, lineB.y + ortho.y);
    verts[2].Set(lineB.x - ortho.x, lineB.y - ortho.y);
    verts[3].Set(start.x - ortho.x, start.y - ortho.y);
    m_polyData->m_vertexCount = 4;
    m_polyFiller->DrawPoly(bmp, m_polyData, c);
}


void AntialiasedDraw::DrawEllipse(DfBitmap *bmp, Vector2 centre, float width, float height, float thickness, DfColour c)
{
    float const inc = 15.0f / (width + height);
    for (float i = 0.0f; i < 3.14159265f * 2.0f; i += inc)
    {
        Vector2 pos1 = centre;
        pos1.x += width * cos(i);
        pos1.y += height * sin(i);

        Vector2 pos2 = centre;
        pos2.x += width * cos(i + inc);
        pos2.y += height * sin(i + inc);

        Vector2 radial1(cos(i), sin(i));
        radial1.SetLen(thickness/2.0);

        Vector2 radial2(cos(i + inc), sin(i + inc));
        radial2.SetLen(thickness/2.0);

        Vector2d *verts = m_polyData->m_vertices;
        verts[0].Set(pos1.x + radial1.x, pos1.y + radial1.y);
        verts[1].Set(pos2.x + radial2.x, pos2.y + radial2.y);
        verts[2].Set(pos2.x - radial2.x, pos2.y - radial2.y);
        verts[3].Set(pos1.x - radial1.x, pos1.y - radial1.y);

        m_polyData->m_vertexCount = 4;
        m_polyFiller->DrawPoly(bmp, m_polyData, c);
    }
}
