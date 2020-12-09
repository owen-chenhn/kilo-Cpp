/* The implementation file of kilo-cpp. */
#include "kilo.h"

/***  Global data  ***/


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
char Kilo::readKey() {
    char c;
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) == EOF ) {}
    return c;
}

bool Kilo::processKeypress() {
    char c = readKey();
    bool flag = true;

    switch (c) {
        case CTRL_KEY('q'):    // Ctrl-Q
            clearScreen();
            reposCursor();
            flag = false;
            break;

        default:
            //TODO
            if (iscntrl(c)) {
                std::cout << (int) c << " ";
            }
            else {
                std::cout << c;
            }
    }
    return flag;
}


/***  Output Handling  ***/
void Kilo::drawRows() {
    // Draw tilds at the beginning of each row.
    for (int r = 0; r < screenRows - 1; r++) {
        std::cout << "~\r\n";
    }
    std::cout << "~";
}


/***  Public interface  ***/
void Kilo::run() {
    while ( processKeypress() ) ;
}
