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

/** 
 *  The main class of the editor. 
 * 
 */
class Kilo {
    static const char *CLEAR_SCREEN;
    static const char *REPOS_CURSOR;

    struct termios orig_termios;
    int screenRows;
    int screenCols;

    // Clear the screen and reposition the cursor.
    void clearScreen() { std::cout << CLEAR_SCREEN; }
    void reposCursor() { std::cout << REPOS_CURSOR; }

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
        clearScreen();
        reposCursor();
        drawRows();
        reposCursor();
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