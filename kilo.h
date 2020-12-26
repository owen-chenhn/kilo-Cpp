#ifndef KILO_H
#define KILO_H
/** 
 *  The header file of Kilo class. 
 * 
 */
#include <string>
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
        KEY_END,
        KEY_DELETE
    };

    struct termios orig_termios;
    int screenRows;
    int screenCols;

    // Current position of the cursor. x: horizontal position, y: vertical position. 
    int cx, cy;    
    // Info about a row of text data
    int numRows;
    std::string row;        // content of a row


    /*** Error handling function. ***/
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


    /***  File IO  ***/
    void openFile();

public:
    Kilo();
    virtual ~Kilo() { disableRawMode(); }

    void run();
};
#endif //KILO_H
