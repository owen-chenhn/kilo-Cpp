/** 
 *  The header file of Kilo class. 
 * 
 */
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define CTRL_KEY(c) ((c) & 0x1f)

// escape sequences to control screen and cursor
#define CLEAR_SCREEN "\x1b[2J"
#define REPOS_CURSOR "\x1b[H"
#define HIDE_CURSOR "\x1b[?25l"
#define DISPLAY_CURSOR "\x1b[?25h"

/** 
 *  The main class of the editor. 
 * 
 */
class Kilo {
    struct termios orig_termios;
    int screenRows;
    int screenCols;

    // Clear the screen, hide and display cursor, and reposition the cursor.
    static void clearScreen() { std::cout << CLEAR_SCREEN; }
    static void reposCursor() { std::cout << REPOS_CURSOR; }
    static void hideCursor() { std::cout << HIDE_CURSOR; }
    static void displayCursor() { std::cout << DISPLAY_CURSOR; }

    /* Error handling function. */
    void die(const char *str) {
        clearScreen();
        reposCursor();
        perror(str);
        exit(1);
    }

    /***  Terminal setup  ***/
    int setWindowSize();
    
    // Turn on/off Raw Mode of terminal.
    void enableRawMode();
    void disableRawMode() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
            die("tcsetattr failed");
    }

    /***  Input Handling  ***/
    char readKey();
    bool processKeypress();

    /***  Output Handling  ***/
    void drawRows();
    void refreshScreen() {
        hideCursor();
        clearScreen();
        reposCursor();
        drawRows();
        reposCursor();
        displayCursor();
    }


public:
    // Constructor and destructor
    Kilo() {
        enableRawMode();
        if (setWindowSize() == -1) die("setWindowSize");
        refreshScreen();

        std::cout << "Text editor Kilo - C++ version.\r\n";
    }
    virtual ~Kilo() { disableRawMode(); }

    void run();
};