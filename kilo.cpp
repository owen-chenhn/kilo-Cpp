/* The main file of kilo in C++ implementation. */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>

using namespace std;


static struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/* Turn on Raw Mode of terminal. */
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}


int main() {
    std::cout << "Text editor Kilo - C++ version.\n";
    enableRawMode();

    // Read in char one by one until see 'q'
    char c;
    while ( (c = getchar()) != EOF && c != 'q' ) {
        cout << c << endl;
    };
    
    return 0;
}