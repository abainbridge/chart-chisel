#ifndef INCLUDED_ANTIALIASED_DRAW_H
#define INCLUDED_ANTIALIASED_DRAW_H


#include "df_bitmap.h"
#include "df_colour.h"
#include "vector2.h"


class PolygonData;
class PolygonFiller;


class AntialiasedDraw
{
private:
    PolygonFiller *m_polyFiller;
    PolygonData *m_polyData;

public:
    AntialiasedDraw();
    ~AntialiasedDraw();

    void DrawSingleArrow(DfBitmap *bmp, Vector2 start, Vector2 end, float thickness, DfColour c);
    void DrawEllipse(DfBitmap *bmp, Vector2 centre, float width, float height, float thickness, DfColour c);
};


extern AntialiasedDraw *g_antialiasedDraw;


#endif
