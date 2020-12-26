#include "kilo.h"
#include <string>

int main(int argc, char *argv[]) {
    std::string file = (argc > 1) ? argv[1] : "";
    Kilo kilo(file);
    kilo.run();

    return 0;
}
