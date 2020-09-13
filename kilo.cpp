/* The main file of kilo in C++ implementation. */
#include <iostream>
#include <cstdio>

using namespace std;

int main() {
    std::cout << "Text editor Kilo - C++ version.\n";

    // Read in char one by one until see 'q'
    char c;
    while ( (c = getchar()) != EOF && c != 'q' ) {
        cout << c << endl;
    };
    
    return 0;
}