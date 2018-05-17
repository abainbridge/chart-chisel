// Own header
#include "main.h"

// Standard headers
#include <stdarg.h>
#include <stdio.h>

// Deadfrog headers
#include "df_bmp.h"
#include "df_font.h"
#include "df_message_dialog.h"
#include "df_time.h"
#include "df_window.h"

// Project headers
#include "antialiased_draw.h"
#include "message_sequence_chart.h"
#include "tokenizer.h"
#include "vector2.h"


#define APP_NAME "Chart Chisel"


bool g_interactiveMode = true;


void FatalError(char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    if (g_interactiveMode) {
        char buf[2048] = "ERROR:\n\n";
        vsnprintf(buf + 8, sizeof(buf)-9, fmt, ap);
        buf[sizeof(buf) - 1] = '\0';
        MessageDialog(APP_NAME " Error", buf, MsgDlgTypeOk);
    }
    else {
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap);
    }

    exit(-1);
}


void PrintUsageAndExit()
{
    puts("Usage: " APP_NAME " <mode> <input filename>");
    puts("");
    puts("Creates a message sequence chart as a PNG file.");
    puts("");
    puts("Where mode is either:");
    puts("  batch       - Generates the output PNG and immediately exits.");
    puts("  interactive - Generates the output PNG and displays it.");
    exit(0);
}


int main(int argc, char *argv[])
{
//     char *filename = "../../message_sequence_charts/hello.msc";
//     g_interactiveMode = false;
   char *filename = "../../message_sequence_charts/add_with_data.msc";
    if (argc != 3) {
        PrintUsageAndExit();
    }

    if (stricmp(argv[1], "batch") == 0) {
        g_interactiveMode = false;
    }
    else if (stricmp(argv[1], "interactive") != 0) {
        printf("Unrecognized mode '%s'\n", argv[1]);
        return -1;
    }
    filename = argv[2];

    g_defaultFont = FontCreate("Arial", 10, 7);
    g_antialiasedDraw = new AntialiasedDraw;

    MessageSequenceChart msc;
    msc.Load(filename);

    DfBitmap *bmp = NULL;
    if (g_interactiveMode) {
        // Setup the window
        int width, height;
        GetDesktopRes(&width, &height);
        CreateWin(1200, height - 100, WT_WINDOWED, APP_NAME);
        bmp = g_window->bmp;
        BitmapClear(bmp, g_colourWhite);

        // Continue to display the window until the user presses escape or clicks the close icon
        double lastTime = GetRealTime();
        while (!g_window->windowClosed && !g_input.keys[KEY_ESC])
        {
            InputManagerAdvance();
            BitmapClear(bmp, g_colourWhite);
            msc.Render(bmp);
            UpdateWin();
            SleepMillisec(4);
        }
    }
    else {
        bmp = BitmapCreate(1300, 1600);
        BitmapClear(bmp, g_colourWhite);
        msc.Render(bmp);
    }

    char *outputFilename = strdup(filename);
    int len = strlen(outputFilename);
    memcpy(outputFilename + len - 4, ".bmp", 4);
    if (!SaveBmp(bmp, outputFilename)) {
        FatalError("Couldn't write output to '%s'", outputFilename);
    }

    return 0;
}
