#include "include/varia/using_varia.h"

int main() {
    var a = 6;
    int b = a;
    std::cout << b << '\n';
    var c = b;
    a = c;

    var n = none;
    var s = "Hello!";
    std::cout << (s == none) << '\n';
    s = n;
    std::string& test = get(s);
    test += "1";
    std::cout << test << " : " << s << "\n";
    std::cout << (s == none) << '\n';
    std::cout << s << '\n';
    s = a;
    s->shrink_to_fit();
    std::cout << s << '\n';

    var q = "2+2=";
    var ans = 2 + 2;
    q += ans;

    std::cout << q;
}
