#include "include/varia/using_varia.h"

int main() {
    var a = 6;
    int b = a;
    var c = b;
    a = c;

    var n = none;
    var s = "Hej!";
    s->shrink_to_fit();
    std::cout << (s == none) << '\n';
    s = n;
    std::cout << (s == none);
}
