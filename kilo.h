#ifndef KILO_H
#define KILO_H
/** 
 *  The header file of Kilo class. 
 * 
 */
#include <string>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <ctime>

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

    std::string filename;
    std::string statusMessage;
    time_t statusMsgTime;    // record every timestamp when a status message is set

    // Current position of the cursor in the entire file. 
    // x: horizontal position, y: vertical position. 
    int cx, cy;    
    int rx;    // position in the render row.

    int rowOffset;    // row offset of the current display window.
    int colOffset; 
    // text data
    int numRows;
    std::vector<std::string> rows;        // content of text
    std::vector<std::string> renders;     // content to be display on the screen


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
    void scroll();

    int getRowLen() { return (cy < numRows) ? rows[cy].length() : 0; }


    /***  Output Handling  ***/
    void reposCursor(std::ostream& os=std::cout);
    void reposCursor(int x, int y, std::ostream& os=std::cout);
    void refreshScreen();
    void moveCursor(int);
    void setStatusMessage(std::string msg);
    void drawRows(std::ostream& os=std::cout);
    void drawStatusBar(std::ostream& os=std::cout);
    void drawMessageBar(std::ostream& os=std::cout);

    /***  Row Operations  ***/
    std::string renderRow(std::string& row);

    /***  File IO  ***/
    void openFile(std::string& fileName);

public:
    Kilo(std::string& file);
    virtual ~Kilo() { disableRawMode(); }

    void run();
};
#endif //KILO_H
