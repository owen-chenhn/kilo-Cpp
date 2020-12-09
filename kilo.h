#ifndef KILO_H
#define KILO_H
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

/** 
 *  The main class of the editor. 
 * 
 */
class Kilo {
    enum KeyType {
        ARROW_UP = 1000,
        ARROW_LEFT,
        ARROW_DOWN,
        ARROW_RIGHT,
        PAGE_UP,
        PAGE_DOWN,
        KEY_HOME,
        KEY_END
    };

    struct termios orig_termios;
    int screenRows;
    int screenCols;

    // Current position of the cursor. x: horizontal position, y: vertical position. 
    int cx, cy;    

    /* Error handling function. */
    void die(const char *str);

    /***  Terminal setup  ***/
    int setWindowSize();
    
    // Turn on/off Raw Mode of terminal.
    void enableRawMode();
    void disableRawMode() {
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
            die("tcsetattr failed");
    }

    /***  Input Handling  ***/
    int  readKey();
    bool processKeypress();

    /***  Output Handling  ***/
    void drawRows();
    void refreshScreen();
    void moveCursor(int);


public:
    // Constructor and destructor
    Kilo() {
        cx = cy = 0;
        enableRawMode();
        if (setWindowSize() == -1) die("setWindowSize");
        refreshScreen();
    }
    virtual ~Kilo() { disableRawMode(); }

    void run();
};
#endif //KILO_H
