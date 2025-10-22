#include <iostream>
#include "include/varia/using_varia.h"

varclass(A,
         explicit Construct_A(int a) {
         a_ = a;
         }

         virtual void greet() {
         std::cout << "Hello, I am A. " << '\n';
         }

         int a_{8};
)

varclass(B,
         void greet() override {
         std::cout << "Hello, I am B. " << '\n';
         }

         , A)

int main() {
    std::cout << "Hello, World!" << '\n';
    var i = 5;
    var j = i + 5;
    i += j + 2;
    std::cout << i << '\n';


    var a = A(4);
    A b = B();

    a->greet();
    b->greet();

    var t = i;
    var h = false;
    var s = "A";
    var s1 = s;
    s1->append(" & B");
    std::cout << s << " " << s1 << '\n';

    var x = "5";
    var y = "6";
    std::cout << (x + y) << '\n';
    std::cout << (y + "7") << '\n';
    std::cout << ("7" + y) << '\n';

    y += x;
    y += "b";
    std::cout << y << '\n';

    String conv = 3;
    conv += 5;
    var bo = false;
    String conv2 = false + conv + bo + true;

    std::cout << conv2 << '\n';
}
