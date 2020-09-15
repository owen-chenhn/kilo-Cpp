/* The main file of kilo in C++ implementation. */
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>

using namespace std;


static struct termios orig_termios;

/* Error handling function. */
void die(const char *str) {
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




int main() {
    std::cout << "Text editor Kilo - C++ version.\n";
    enableRawMode();

    // Read in char one by one until see 'q'
    char c;
    // Read input every 0.1 sec. Return -1 (EOF) to c if no char is read. 
    while ( (c = getchar()) != 'q' ) {
        if (iscntrl(c)) {
            cout << (int) c << "\r\n";
        }
        else if (c >= 0) {
            // Ignore EOF (-1, no char read)
            cout << c << "\r\n";    // must use "\r\n" as new line now because OPOST flag is disabled.
        }
    };
    
    return 0;
}