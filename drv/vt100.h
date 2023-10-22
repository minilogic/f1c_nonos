#ifndef VT100_H
#define VT100_H

/* Display Attributes */
#define ATTR_RESET    "\e[0m"
#define ATTR_BRIGHT   "\e[1m"
#define ATTR_DIM      "\e[2m"
#define ATTR_UNDER    "\e[4m"
#define ATTR_BLINK    "\e[5m"
#define ATTR_REVERSE  "\e[7m"
#define ATTR_HIDDEN   "\e[8m"

/* Foreground Colours */
#define FG_BLACK      "\e[30m"
#define FG_RED        "\e[31m"
#define FG_GREEN      "\e[32m"
#define FG_YELLOW     "\e[33m"
#define FG_BLUE       "\e[34m"
#define FG_MAGENTA    "\e[35m"
#define FG_CYAN       "\e[36m"
#define FG_WHITE      "\e[37m"

/* Background Colours */
#define BG_BLACK      "\e[40m"
#define BG_RED        "\e[41m"
#define BG_GREEN      "\e[42m"
#define BG_YELLOW     "\e[43m"
#define BG_BLUE       "\e[44m"
#define BG_MAGENTA    "\e[45m"
#define BG_CYAN       "\e[46m"
#define BG_WHITE      "\e[47m"

/* Erasing Text */
#define CLR_EOL       "\e[K"          /* Erases from cursor to end of line */
#define CLR_SOL       "\e[1K"         /* Erases from cursor to start of line */
#define CLR_LINE      "\e[2K"         /* Erases the entire current line */
#define CLR_DOWN      "\e[J"          /* Erases from curent line to down */
#define CLR_UP        "\e[1J"         /* Erases from curent line to up */
#define CLR_SCR       "\e[2J\e[1;1H"  /* Erases screen, setup home cursor pos */

/* Cursor Control */
#define CURSOR_HIDE   "\e[?25l"       /* Hide cursor */
#define CURSOR_SHOW   "\e[?25h"       /* Show cursor */
#define CURSOR_UP     "\e[A"          /* Moves cursor up */
#define CURSOR_DOWN   "\e[B"          /* Moves cursor down */
#define CURSOR_RIGHT  "\e[C"          /* Moves cursor right */
#define CURSOR_LEFT   "\e[D"          /* Moves cursor left */
#define CURSOR_XY     "\e[%d;%dH"     /* Moves cursor to XY-position */
#define GOTO_XY(x,y)  printf(CURSOR_XY, y, x)

#endif