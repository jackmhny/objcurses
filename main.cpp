#include <ncurses.h>

int main()
{
    initscr();
    mvprintw(LINES / 2, (COLS - 13) / 2, "Hello, objcurses!");
    refresh();
    getch();
    endwin();

    return 0;
}
