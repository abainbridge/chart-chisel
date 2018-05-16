// Own header
#include "main.h"

// Standard headers
#include <stdarg.h>
#include <stdio.h>

// Deadfrog headers
#include "df_font.h"
#include "df_time.h"
#include "df_window.h"

// Project headers
#include "antialiased_draw.h"
#include "message_sequence_chart.h"
#include "token_stream.h"
#include "vector2.h"


void FatalError(char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
}


int main(int argc, char *argv[])
{
    char const *filename = "../../message_sequence_charts/add_with_data.msc";
    if (argc == 2) {
        filename = argv[1];
    }

    g_defaultFont = FontCreate("Arial", 10, 7);
    g_antialiasedDraw = new AntialiasedDraw;

    MessageSequenceChart msc;
    msc.Load(filename);

    // Setup the window
    int width, height;
    GetDesktopRes(&width, &height);
    CreateWin(1200, height - 100, WT_WINDOWED, "Chart Chisel");
    BitmapClear(g_window->bmp, g_colourWhite);

    // Continue to display the window until the user presses escape or clicks the close icon
    double lastTime = GetRealTime();
    while (!g_window->windowClosed && !g_input.keys[KEY_ESC])
    {
        InputManagerAdvance();
        BitmapClear(g_window->bmp, g_colourWhite);
        msc.Render(g_window->bmp);
        UpdateWin();
        SleepMillisec(4);
    }

    return 0;
}