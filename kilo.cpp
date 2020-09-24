/* The main file of kilo in C++ implementation. */
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>

//using namespace std;


/***  Terminal setup  ***/
static struct termios orig_termios;
const char *CLEAR_SCREEN = "\x1b[2J";
const char *REPOS_CURSOR = "\x1b[H";

// Clear the screen and reposition the cursor.
void clearScreen() {
    std::cout << CLEAR_SCREEN;
}
void reposCursor() {
    std::cout << REPOS_CURSOR;
}


/* Error handling function. */
void die(const char *str) {
    clearScreen();
    reposCursor();

    perror(str);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr failed");
}

/* Turn on Raw Mode of terminal. */
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) 
        die("tcgetattr failed");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
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
/* Map a lower case letter (e.g, 'q') to its ctrl key (e.g, 'Ctrl-Q'). */
inline char ctrl_key(char ch) {
    return ch & 0x1f;
}

char readKey() {
    char c;
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) == EOF ) {}
    return c;
}

void editorProcessKeypress() {
    char c = readKey();

    switch (c) {
        case 27:    // 'ESC'
            clearScreen();
            reposCursor();
            exit(0);
            break;

        default:
            //TODO
            if (iscntrl(c)) {
                std::cout << (int) c << "\r\n";
            }
            else {
                std::cout << c << "\r\n";
            }
    }
}


/***  Output Handling  ***/
void editorDrawRows() {
    // Draw tilds at the beginning of each row.
    int max_row = 24;   // Hard-code the max number of rows to 24.
    for (int r = 0; r < max_row; r++) {
        std::cout << "~\r\n";
    }
}

void editorRefreshScreen() {
    clearScreen();
    reposCursor();

    editorDrawRows();
    reposCursor();
}


/***  Init  ***/
int main() {
    enableRawMode();
    editorRefreshScreen();

    std::cout << "Text editor Kilo - C++ version.\r\n";
    while (true) {
        editorProcessKeypress();
    }
    
    return 0;
}
