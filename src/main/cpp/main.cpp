// main.cpp
// Entry point. Console only. No GTK and no WebKit.

#include "about.h"

// Purpose:
//   Construct the program and let it finish the run.
// Parameters:
//   argc, argv — the program parses these.
// Dependencies:
//   About.
// Why This Design:
//   The window, WebKit view, and userscript host are removed.
//   About owns the name, the version, the paths, and the flags.
// CIAO Principles Applied:
//   Caution: no toolkit init.
//   Intentional: one path from argv through About.
// Protection Rule:
//   Do not call gtk_init or create a WebKit view from this file.
//   Do not declare a second type in this file.

int main(int argc, char **argv)
{
    About about;

    return about.run(argc, argv);
}
