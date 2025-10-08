#include "include/varia/using_varia.h"

int main() {
    var a = 6.657685768;
    float b = a;
    std::cout << b << '\n';
    var c = b;
    a = c;

    Num bajs = Num{"7"};

    var n = none;
    var s = "Hello!" + String(" Yello!");
    std::cout << (s == none) << '\n';
    s = n;
    std::cout << (s == none) << '\n';
    std::cout << s << '\n';
    s = String(a);
    s->shrink_to_fit();
    std::cout << s << '\n';

    var q = "2 + 2.5 = ";
    var ans = Num(2) + 2.5;
    q += String(ans) + String(5);

    String oof = 6;

    std::cout << q;
}
