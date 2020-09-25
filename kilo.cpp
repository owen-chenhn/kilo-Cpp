/* The main file of kilo in C++ implementation. */
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>


#define CTRL_KEY(c) ((c) & 0x1f)

/***  Global data  ***/
struct editorConfig {
    struct termios orig_termios;
    int screenRows;
    int screenCols;

    int setWindowSize() {
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) 
            return -1;
        screenRows = ws.ws_row;
        screenCols = ws.ws_col;
        return 0;
    }
};
const char *CLEAR_SCREEN = "\x1b[2J";
const char *REPOS_CURSOR = "\x1b[H";


/***  Terminal setup  ***/
static editorConfig config;

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
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig_termios) == -1)
        die("tcsetattr failed");
}

/* Turn on Raw Mode of terminal. */
void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &config.orig_termios) == -1) 
        die("tcgetattr failed");
    atexit(disableRawMode);

    termios raw = config.orig_termios;
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
char readKey() {
    char c;
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) == EOF ) {}
    return c;
}

void editorProcessKeypress() {
    char c = readKey();

    switch (c) {
        case CTRL_KEY('q'):    // Ctrl-Q
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
    for (int r = 0; r < config.screenRows - 1; r++) {
        std::cout << "~\r\n";
    }
    std::cout << "~";
}

void editorRefreshScreen() {
    clearScreen();
    reposCursor();

    editorDrawRows();
    reposCursor();
}


/***  Init  ***/
void initEditor() {
    if (config.setWindowSize() == -1) die("setWindowSize");
}

int main() {
    enableRawMode();
    initEditor();
    editorRefreshScreen();

    std::cout << "Text editor Kilo - C++ version.\r\n";
    while (true) {
        editorProcessKeypress();
    }
    
    return 0;
}
