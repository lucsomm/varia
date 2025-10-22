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
    println("Hello, World!");
    var i = 5;
    var j = i + 5;
    i += j + 2;
    println(i);


    var a = A(4);
    A b = B();

    a->greet();
    b->greet();

    var t = i;
    var h = false;
    var s = "A";
    var s1 = s;
    s1->append(" & B");
    println(s + " " + s1);

    var x = "5";
    var y = "6";
    println(x + y);
    println(y + "7");
    println("7" + y);

    y += x;
    y += "b";
    println(y);

    String conv = 3;
    conv += 5;
    var bo = false;
    String conv2 = false + conv + bo + true;

    println(conv2);
}
