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


    var a = A(4);
    A b = B();

    a->greet();
    b->greet();

    var t = i;
    var h = false;
    var s = "A";
    var s1 = s;
    s1->append(" & B");
    std::cout << s->c_str() << " " << s1->c_str() << '\n';

    var x = "5";
    var y = "6";
    std::cout << (x + y)->c_str() << '\n';
    std::cout << (y + "7")->c_str() << '\n';
    std::cout << ("7" + y)->c_str() << '\n';

    y += x;
    y += "b";
    std::cout << y->c_str() << '\n';

    String conv = 3;
    conv += 5;
    var bo = false;
    String conv2 = conv + bo + true;

    std::cout << conv2->c_str() << '\n';
}
