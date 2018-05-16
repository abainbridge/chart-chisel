#include "message_sequence_chart.h"

// Project headers
#include "antialiased_draw.h"
#include "main.h"
#include "tokenizer.h"

// Deadfrog headers
#include "df_bitmap.h"
#include "df_font.h"

// Standard headers
#include <ctype.h>
#include <algorithm>


MessageSequenceChart::MessageSequenceChart()
{
    m_pixelWidth = -1;
    m_scale = 1.0;
    m_yStep = 15.0f * m_scale;
    m_columnWidth = 1;
}


static char *StringDup(char const *str)
{
    int len = strlen(str) + 1;
    char *rv = new char[len];
    memcpy(rv, str, len);
    return rv;
}


static void ReportParseError(Tokenizer *ts, char const *expected, char const *got)
{
    FatalError("Line %d\n\nExpected '%s' got '%s'", ts->m_currentLineNum, expected, got);
}


static void TokenMustBe(Tokenizer *ts, char const *got, char const *expected)
{
    if (stricmp(expected, got) != 0) {
        ReportParseError(ts, expected, got);
    }
}


static bool ReadKeyValuePair(Tokenizer *ts, char **key, char **value)
{
    char *tok = ts->GetToken();
    if (*tok == NULL || !isalpha(tok[0])) {
        return false;
    }
    *key = StringDup(tok);

    tok = ts->GetToken();
    TokenMustBe(ts, tok, "=");

    tok = ts->GetToken();
    if (tok[0] == '"') {
        tok++;
        char *lastChar = tok + strlen(tok) - 1;
        if (*lastChar != '"') {
            delete[] *key;
            *key = NULL;
            return false;
        }
        *lastChar = '\0';
    }
    *value = StringDup(tok);
    return true;
}


static void SplitAtNewLines(char *str, std::vector <char *> *linesOut)
{
    char *start = str;
    char *end = strstr(str, "\\n");
    while (end) {
        *end = '\0';
        char *line = StringDup(str);
        linesOut->push_back(line);

        str = end + 2;
        end = strstr(str, "\\n");
    }

    char *line = StringDup(str);
    linesOut->push_back(line);
    delete[] start;
}


static bool ParseColour(char *str, DfColour *col)
{
    // Should be like "#ff00ff" or "#f0f"
    if (str[0] != '#') {
        return false;
    }
    str++;
    int len = strlen(str);
    char r[3] = "00";
    char g[3] = "00";
    char b[3] = "00";
    if (len == 3) {
        r[1] = str[0];
        g[1] = str[1];
        b[1] = str[2];
    }
    else if (len == 6) {
        r[0] = str[0]; r[1] = str[1];
        g[0] = str[2]; g[1] = str[3];
        b[0] = str[4]; b[1] = str[5];
    }
    else {
        return false;
    }
    int rInt = strtol(r, NULL, 16);
    int gInt = strtol(g, NULL, 16);
    int bInt = strtol(b, NULL, 16);

    *col = Colour(rInt, gInt, bInt);
    return true;
}


// Assumes the opening '[' has already been read. Reads until the closing ']'.
static bool ReadParameters(Tokenizer *ts, Parameters *params)
{
    while (1) {
        char *key;
        char *val;
        if (!ReadKeyValuePair(ts, &key, &val)) {
            return false;
        }

        if (stricmp(key, "label") == 0) {
            delete[] key;
            SplitAtNewLines(val, &params->m_label);
        }
        else if (stricmp(key, "textbgcolour") == 0) {
            delete[] key;
            if (!ParseColour(val, &params->m_bgColour))
                return false;
        }
        else {
            delete[] key;
            delete[] val;
            return false;
        }

        char *tok = ts->GetToken();
        if (stricmp(tok, "]") == 0) {
            break;
        }
        if (stricmp(tok, ",") != 0) {
            ReportParseError(ts, ",", tok);
        }
    }

    return true;
}


bool MessageSequenceChart::Load(char const *filename)
{
    Tokenizer ts;
    if (!ts.Open(filename))
        FatalError("Couldn't open '%s'", filename);

    char *tok = ts.GetToken();
    TokenMustBe(&ts, tok, "msc");
    tok = ts.GetToken();
    TokenMustBe(&ts, tok, "{");

    while (1) {
        tok = ts.GetToken();
        
        if (stricmp(tok, "}") == 0) {
            break;
        }

        else if (stricmp(tok, "width") == 0) {
            tok = ts.GetToken();
            TokenMustBe(&ts, tok, "=");
            char const *val = ts.GetToken();
            int intVal = strtol(val, NULL, 10);
            if (intVal < 40 || intVal > 9000) {
                return false;
            }

            m_pixelWidth = intVal;
            tok = ts.GetToken();
            TokenMustBe(&ts, tok, ";");
        }

        else if (stricmp(tok, "title") == 0) {
            ts.UnGetToken();
            char *key;
            char *val;
            if (!ReadKeyValuePair(&ts, &key, &val)) {
                return false;
            }
            delete[] key;
            SplitAtNewLines(val, &m_title);
            tok = ts.GetToken();
            TokenMustBe(&ts, tok, ";");
        }

        else if (m_entities.size() == 0) {
            // Must be the entities declarations
            while (1) {
                Entity *e = new Entity;
                e->m_name = StringDup(tok);
                if (stricmp(ts.GetToken(), "[") == 0) {
                    if (!ReadParameters(&ts, &e->m_params)) {
                        return false;
                    }
                }

                m_entities.push_back(e);

                tok = ts.GetToken();
                if (stricmp(tok, ";") == 0) {
                    break;
                }

                TokenMustBe(&ts, tok, ",");
                tok = ts.GetToken();
            }
        }

        else {
            // Must be an arc declaration

            Arc *arc = new Arc;

            if (stricmp(tok, "|||") == 0) {
                arc->m_type = Arc::TYPE_SPACER;
            }
            else if (stricmp(tok, "...") == 0) {
                arc->m_type = Arc::TYPE_ELLIPSIS;
            }
            else if (stricmp(tok, "---") == 0) {
                arc->m_type = Arc::TYPE_COMMENT;
            }
            else {
                arc->m_entities[0] = GetEntityByName(tok);
                if (!arc->m_entities[0]) {
                    ReportParseError(&ts, "entity name", tok);
                }

                tok = ts.GetToken();

                if (stricmp(tok, "=>") == 0) {
                    arc->m_type = Arc::TYPE_ARROW;
                }
                else if (stricmp(tok, "box") == 0) {
                    arc->m_type = Arc::TYPE_BOX;
                }
                else {
                    return false;
                }

                tok = ts.GetToken();
                arc->m_entities[1] = GetEntityByName(tok);
                if (!arc->m_entities[1]) {
                    return false;
                }
            }

            tok = ts.GetToken();
            if (stricmp(tok, "[") == 0) {
                if (!ReadParameters(&ts, &arc->m_params)) {
                    return false;
                }
                tok = ts.GetToken();

                if (stricmp(tok, ",") == 0) {
                    m_arcs.push_back(arc);
                    arc = new Arc;
                    arc->m_type = Arc::TYPE_UNSPACER;
                }
                else if (stricmp(tok, ";") != 0) {
                    return false;
                }
            }

            m_arcs.push_back(arc);
        }
    }

    return true;
}


Entity *MessageSequenceChart::GetEntityByName(char const *name)
{
    for (int i = 0; i < m_entities.size(); i++) {
        if (stricmp(m_entities[i]->m_name, name) == 0) {
            return m_entities[i];
        }
    }

    return NULL;
}


static int GetTextRenderWidth(std::vector <char *> *linesList)
{
    int width = 0;
    for (int i = 0; i < linesList->size(); i++) {
        int thisWidth = GetTextWidth(g_defaultFont, (*linesList)[i]);
        width = std::max(thisWidth, width);
    }

    return width;
}


int MessageSequenceChart::RenderLineList(DfBitmap *bmp, std::vector <char *> *lines, int x, int y)
{
    for (int i = 0; i < lines->size(); i++) {
        DrawTextCentre(g_defaultFont, g_colourBlack, bmp, x, y, (*lines)[i]);
        y += m_yStep;
    }

    return m_yStep * lines->size();
}


int MessageSequenceChart::RenderArrow(DfBitmap *bmp, int y, Arc *a)
{
    int startX = a->m_entities[0]->xPos;
    int endX = a->m_entities[1]->xPos;
    int midX = (startX + endX) / 2;
    int yInc = a->m_params.m_label.size() * g_defaultFont->charHeight;
    int width = GetTextRenderWidth(&a->m_params.m_label);
    DfColour bgColour = g_colourWhite;
    RectFill(bmp, midX - width/2, y, width, yInc, bgColour);
    RenderLineList(bmp, &a->m_params.m_label, midX, y);
    yInc += 6.0f * m_scale;
    y += yInc;
    Vector2 start(startX, y);
    Vector2 end(endX, y);
    g_antialiasedDraw->DrawSingleArrow(bmp, start, end, 1.4f * m_scale, g_colourBlack);

    return m_yStep * 1.5 + yInc;
}


int MessageSequenceChart::RenderBox(DfBitmap *bmp, int y, Arc *a)
{
    float borderSize = 4.0f * m_scale;
    int startX = a->m_entities[0]->xPos - m_columnWidth / 2.0;
    int endX = a->m_entities[1]->xPos + m_columnWidth / 2.0;
    int midX = (startX + endX) / 2;
    int width = endX - startX;

    int yInc = a->m_params.m_label.size() * g_defaultFont->charHeight;
    yInc += borderSize * 2.0f;
    RectFill(bmp, startX + 1, y + 1, width - 2, yInc - 2, a->m_params.m_bgColour);
    RectOutline(bmp, startX, y, width, yInc, g_colourBlack);
    RenderLineList(bmp, &a->m_params.m_label, midX, y + borderSize);

    return m_yStep + yInc;
}


void MessageSequenceChart::Render(DfBitmap *bmp)
{
    float y = 10.0f * m_scale + g_defaultFont->charHeight;

    if (m_title.size() > 0) {
        y += RenderLineList(bmp, &m_title, bmp->width / 2.0, y) + 20.0f * m_scale;
    }

    m_columnWidth = (float)m_pixelWidth * m_scale / (float)(m_entities.size() + 2);

    // Draw entity labels
    int maxYInc = 0;
    for (int i = 0; i < m_entities.size(); i++) {
        Entity *e = m_entities[i];
        e->xPos = m_columnWidth + i * m_columnWidth;
        int yInc = m_yStep;
        if (e->m_params.m_label.size() > 0) {
            yInc = RenderLineList(bmp, &e->m_params.m_label, e->xPos, y);
        }
        else {
            DrawTextCentre(g_defaultFont, g_colourBlack, bmp, e->xPos, y, e->m_name);
        }
        maxYInc = std::max(maxYInc, yInc);
    }
    y += maxYInc + 10 * m_scale;

    // Draw a vertical line for each entity
    for (int i = 0; i < m_entities.size(); i++) {
        Entity *e = m_entities[i];
        VLine(bmp, e->xPos, y, bmp->height - y, g_colourBlack);
    }
    y += 10 * m_scale;

    int yInc = 0;
    for (int i = 0; i < m_arcs.size(); i++) {
        Arc *a = m_arcs[i];

        switch (a->m_type) {
        case Arc::TYPE_ARROW:
            yInc = RenderArrow(bmp, y, a);
            break;
        case Arc::TYPE_BOX:
            yInc = RenderBox(bmp, y, a);
            break;
        case Arc::TYPE_UNSPACER:
            y -= yInc * 2.0f;
            break;
        }

        y += yInc;
    }
}
