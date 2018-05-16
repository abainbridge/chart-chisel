#pragma once

#include "df_bitmap.h"

#include <stdlib.h>
#include <vector>


struct Parameters
{
    std::vector <char *> m_label;   // One entry per line of text.
    DfColour m_bgColour;

    Parameters() {
        m_bgColour = g_colourWhite;
    }
};


struct Entity {
    char *m_name;
    Parameters m_params;
    int xPos;
};


struct Arc {
    enum {
        TYPE_UNKNOWN,
        TYPE_ARROW,
        TYPE_BOX,
        TYPE_ELLIPSIS,  // We render this arc as vertical "..."s.
        TYPE_SPACER,
        TYPE_UNSPACER,  // We move the render nothing and move Y coordinate up one row when we encounter this type. It is used to put more than one arc on a single row.
        TYPE_COMMENT
    };

    Entity *m_entities[2];
    int m_type;
    Parameters m_params;

    Arc() {
        m_entities[0] = m_entities[1] = NULL;
        m_type = TYPE_UNKNOWN;
    }
};


class MessageSequenceChart
{
public:
    std::vector <char *> m_title;
    std::vector <Entity *> m_entities;
    std::vector <Arc *> m_arcs;
    int m_pixelWidth;
    float m_scale;
    float m_columnWidth;

private:
    float m_yStep;

private:
    int RenderLineList(DfBitmap *bmp, std::vector <char *> *lines, int x, int y);
    int RenderArrow(DfBitmap *bmp, int y, Arc *a);
    int RenderBox(DfBitmap *bmp, int y, Arc *a);

public:
    MessageSequenceChart();
    bool Load(char const *filename);
    Entity *GetEntityByName(char const *name);

    void Render(DfBitmap *bmp);
};
