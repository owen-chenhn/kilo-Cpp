/* The implementation file of kilo-cpp. */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE  // feature test macros

#include "kilo.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <sys/ioctl.h>

#define CTRL_KEY(c) ((c) & 0x1f)
#define TAB_SIZE 8

// escape sequences to control screen and cursor
#define CLEAR_LINE "\x1b[K"
#define CLEAR_SCREEN "\x1b[2J"
#define REPOS_CURSOR "\x1b[H"
#define HIDE_CURSOR "\x1b[?25l"
#define DISPLAY_CURSOR "\x1b[?25h"
#define INVERT_COLOR "\x1b[7m"
#define RESUME_COLOR "\x1b[m"

// helper functions
static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

// Clear the screen, hide and display cursor, and reposition the cursor.
static inline void clearScreen() { std::cout << CLEAR_SCREEN; }
static inline void hideCursor() { std::cout << HIDE_CURSOR; }
static inline void displayCursor() { std::cout << DISPLAY_CURSOR; }

// welcome message
static const std::string welcome = "Text editor Kilo - C++ version.";


/***  Error handling  ***/
void Kilo::die(const char *str) {
    clearScreen();
    reposCursor();
    disableRawMode();
    perror(str);
    exit(1);
}


/***  Terminal setup  ***/
int Kilo::setWindowSize() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) 
        return -1;
    screenRows = ws.ws_row - 2;
    screenCols = ws.ws_col;
    return 0;
}

void Kilo::enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
        die("tcgetattr failed");

    termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr failed");
}


/***  Input Handling  ***/
int Kilo::readKey() {
    char c;
    char seq[3];
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) == EOF ) {}
    if (c == '\x1b') {
        seq[0] = getchar();
        seq[1] = getchar();
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                seq[2] = getchar();
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return KeyType::KEY_HOME;
                        case '3': return KeyType::KEY_DELETE;
                        case '4': return KeyType::KEY_END;
                        case '5': return KeyType::PAGE_UP;
                        case '6': return KeyType::PAGE_DOWN;
                        case '7': return KeyType::KEY_HOME;
                        case '8': return KeyType::KEY_END;
                    }
                }
            }
            switch (seq[1]) {
                case 'A': return KeyType::ARROW_UP;
                case 'B': return KeyType::ARROW_DOWN;
                case 'C': return KeyType::ARROW_RIGHT;
                case 'D': return KeyType::ARROW_LEFT;
                case 'H': return KeyType::KEY_HOME;
                case 'F': return KeyType::KEY_END;
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return KeyType::KEY_HOME;
                case 'F': return KeyType::KEY_END;
            }
        }
    }
    return c;
}

bool Kilo::processKeypress() {
    int c = readKey();
    bool flag = true;

    switch (c) {
    case CTRL_KEY('q'):    // Ctrl-Q
        clearScreen();
        cx = cy = 0;
        flag = false;
        break;

    case KeyType::ARROW_UP:
    case KeyType::ARROW_LEFT:
    case KeyType::ARROW_DOWN:
    case KeyType::ARROW_RIGHT:
        moveCursor(c);
        break;
    case KeyType::PAGE_UP:
        cy = max((cy - screenRows + 1), 0);
        rowOffset = max((rowOffset - screenRows + 1), 0);
        refreshScreen();
        break;
    case KeyType::PAGE_DOWN:
        cy = min((cy + screenRows - 1), numRows);
        rowOffset = min((rowOffset + screenRows - 1), (numRows - screenRows + 1));
        refreshScreen();
        break;
    case KeyType::KEY_HOME:
    case KeyType::KEY_END:
        cx = (c == KEY_HOME) ? 0 : getRowLen();
        break;
    case KeyType::KEY_DELETE:
    case 127:
        // TODO: implement DELETE key
        moveCursor(ARROW_LEFT);
        break;

    default:
        //TODO
        if (iscntrl(c)) {
            std::cout << c << " ";
        }
        else {
            std::cout << (char) c;
        }
        cx++;
        if (cx == screenCols) {
            cy++;
            cx = 0;
        }
    }
    scroll();

    return flag;
}

void Kilo::moveCursor(int direction) {
    switch (direction) {
        case ARROW_UP: {
            if (cy > 0)
                cy--; 
            break;
        }
        case ARROW_LEFT: {
            if (cx > 0) {
                cx--; 
            } else if (cy > 0) {
                cy--;
                cx = rows[cy].length();
            }
            break;
        }
        case ARROW_DOWN: {
            if (cy < numRows)
                cy++; 
            break;
        }
        case ARROW_RIGHT: {
            int len = getRowLen();
            if (cx < len) {
                cx++; 
            } else if (cy < numRows) {
                cy++;
                cx = 0;
            }
            break;
        }
    }

    int len = getRowLen();
    if (cx > len) 
        cx = len;
}

void Kilo::scroll() {
    rx = 0;
    // set the correct value of rx
    if (cy < numRows) {
        for (int i = 0; i < cx; i++) {
            if (rows[cy][i] == '\t') {
                rx += TAB_SIZE - (rx % TAB_SIZE);
            } else {
                rx++;
            }
        }
    }

    if (cy < rowOffset) {
        rowOffset = cy;
    } else if (cy >= rowOffset + screenRows) {
        rowOffset = cy - screenRows + 1;
    } else if (rx < colOffset) {
        colOffset = rx;
    } else if (rx >= colOffset + screenCols) {
        colOffset = rx - screenCols + 1;
    }
}


/***  Output Handling  ***/
void inline Kilo::reposCursor() { std::cout << REPOS_CURSOR; }  // position the cursor to the upper-left corner of the terminal.

void inline Kilo::reposCursor(int x, int y) { 
    // position the cursor to terminal location (x, y)
    char seqBuf[32];
    sprintf(seqBuf, "\x1b[%d;%dH", y-rowOffset+1, x-colOffset+1);
    std::cout << seqBuf;
}

void Kilo::setStatusMessage(std::string msg) {
    statusMessage = msg;
    statusMsgTime = time(NULL);
}

void Kilo::drawRows() {
    // Draw tilds at the beginning of each row.
    for (int r = 0; r < screenRows; r++) {
        int row = r + rowOffset;
        if (row < numRows) {
            int len = renders[row].length();
            std::cout << renders[row].substr(min(colOffset, len), screenCols);
        } else if (r == screenRows / 3 && numRows == 0) {
            // Welcome message
            int padding = (screenCols - welcome.length()) / 2;
            if (padding) {
                std::cout << "~";
                padding--;
            }
            while (padding--) std::cout << " ";
            std::cout << "Text editor Kilo - C++ version.";
        } else {
            std::cout << "~";
        }
            
        std::cout << "\r\n";
    }
}

void Kilo::drawStatusBar() {
    std::cout << INVERT_COLOR;

    // draw status bar
    std::string status(filename);
    if (status.length() == 0) 
        status += "[No Name]";
    status += " - " + std::to_string(numRows) + " lines " + 
              std::to_string(cy+1) + '/' + std::to_string(numRows);

    // trim the status text or pad with white spaces
    if (status.length() > (unsigned) screenCols) {
        status = status.substr(0, screenCols);
    } else {
        status += std::string(screenCols - status.length(), ' ');
    }
    
    std::cout << status;
    std::cout << RESUME_COLOR << "\r\n";
}

void Kilo::drawMessageBar() {
    std::cout << CLEAR_LINE;
    // print status message that is in 5 sec
    if (time(NULL) - statusMsgTime < 5) {
        std::cout << statusMessage.substr(0, screenCols);
    }
}

void Kilo::refreshScreen() {
    hideCursor();
    clearScreen();
    reposCursor();
    drawRows();
    drawStatusBar();
    drawMessageBar();
    reposCursor(rx, cy);
    displayCursor();
}


/***  Row Operations  ***/
std::string Kilo::renderRow(std::string& row) {
    std::string render;
    for (char c : row) {
        if (c == '\t') {    // render tabs as multiple space chars
            render += std::string(TAB_SIZE-(render.length() % TAB_SIZE), ' ');
        } else {
            render += c;
        }
    }

    return render;
}


/***  File IO  ***/
void Kilo::openFile(std::string& fileName) {
    this->filename = fileName;
    std::ifstream infile(fileName);
    if (!infile.is_open()) { die("file open failed"); }

    std::string row;
    while (std::getline(infile, row)) {
        rows.push_back(row);
        renders.push_back(renderRow(row));
        numRows++;
    }
    infile.close();
}


/***  Public interface  ***/
Kilo::Kilo(std::string& file) {
    cx = cy = rx = 0;
    numRows = 0;
    rowOffset = colOffset = 0;
    statusMsgTime = 0;
    enableRawMode();
    if (setWindowSize() == -1) die("setWindowSize");
    
    if (file.length() > 0)
        openFile(file);
}

void Kilo::run() {
    setStatusMessage("HELP: Ctrl-Q = quit");

    do {
        refreshScreen();
    } while ( processKeypress() );
}
