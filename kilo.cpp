/* The implementation file of kilo-cpp. */
#include "kilo.h"
#include <string>

#define CTRL_KEY(c) ((c) & 0x1f)

// escape sequences to control screen and cursor
#define CLEAR_SCREEN "\x1b[2J"
#define REPOS_CURSOR "\x1b[H"
#define HIDE_CURSOR "\x1b[?25l"
#define DISPLAY_CURSOR "\x1b[?25h"

// Clear the screen, hide and display cursor, and reposition the cursor.
static void clearScreen() { std::cout << CLEAR_SCREEN; }
static void reposCursor() { std::cout << REPOS_CURSOR; }
// position the cursor to terminal location (x, y)
static void reposCursor(int x, int y) { 
    char seqBuf[32];
    sprintf(seqBuf, "\x1b[%d;%dH", y, x);
    std::cout << seqBuf;
}
static void hideCursor() { std::cout << HIDE_CURSOR; }
static void displayCursor() { std::cout << DISPLAY_CURSOR; }

// welcome message
static const std::string welcome = "Text editor Kilo - C++ version.";

/***  Error handling  ***/
void Kilo::die(const char *str) {
    clearScreen();
    reposCursor();
    perror(str);
    exit(1);
}

/***  Terminal setup  ***/
int Kilo::setWindowSize() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) 
        return -1;
    screenRows = ws.ws_row;
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
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) == EOF ) {}
    if (c == '\x1b') {
        if (getchar() == '[') {
            switch (getchar()) {
                case 'A': return keyType::ARROW_UP;
                case 'B': return keyType::ARROW_DOWN;
                case 'C': return keyType::ARROW_RIGHT;
                case 'D': return keyType::ARROW_LEFT;
                default : return c;
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
            reposCursor();
            flag = false;
            break;

        case ARROW_UP:
        case ARROW_LEFT:
        case ARROW_DOWN:
        case ARROW_RIGHT:
            moveCursor(c);
            break;

        default:
            //TODO
            if (iscntrl(c)) {
                std::cout << c << " ";
            }
            else {
                std::cout << (char) c;
                cx++;
            }
    }
    return flag;
}


/***  Output Handling  ***/
void Kilo::drawRows() {
    // Draw tilds at the beginning of each row.
    for (int r = 0; r < screenRows; r++) {
        if (r == screenRows / 3) {
            // Welcome message
            int padding = (screenCols - welcome.length()) / 2;
            if (padding) {
                std::cout << "~";
                padding--;
            }
            while (padding--) std::cout << " ";
            std::cout << "Text editor Kilo - C++ version.";
        }
        else 
            std::cout << "~";
        if (r < screenRows - 1) std::cout << "\r\n";
    }
}

void Kilo::refreshScreen() {
    hideCursor();
    clearScreen();
    reposCursor();
    drawRows();
    reposCursor();
    displayCursor();
}

void Kilo::moveCursor(int direction) {
    switch (direction) {
        case ARROW_UP: cy--; break;
        case ARROW_LEFT: cx--; break;
        case ARROW_DOWN: cy++; break;
        case ARROW_RIGHT: cx++; break;
    }
    reposCursor(cx, cy);
}


/***  Public interface  ***/
void Kilo::run() {
    while ( processKeypress() ) ;
}
