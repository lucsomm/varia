#include <iostream>
#include "include/varia/using_varia.h"

varinterface(Greeter,
             virtual void greet() = 0;
)

varclass(A,
         explicit Construct_A(int a) {
         this->a = a;
         }

         void greet() override {
         println("Hello, I am A.");
         }

         Int a = 8;
         , Greeter)

varinterface(Tester,
             virtual void test() = 0;
)

varclass(B,
         void greet() override {
         println("Hello, I am B.");
         }

         void test() override {
         println("Idk");
         }

         , A, Tester)

varstruct(D,);
varstruct(G,);
varstruct(F,);

varstruct(C,
          Int four{4};

          , D)

template<typename T = Float>
template_varclass(Number,
                  T number;
)

int main() {
    println("Hello, World!");
    Int i = 5;
    var j = 1 + i + 5;
    var k = 2;
    i *= j - Int(k) % i;
    println(i);
    -i;
    +j;

    var arr = {5, 5, 4, 4};
    var arr2 = {"A", "B", "C"};
    var arr3 = {false, true, false};
    println(arr);
    println(arr2);
    println(arr3);
    println(arr2[2]);
    arr->resize(i);
    println("Array size {}", arr->size());

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
    conv += j;
    var bo = false;
    var conv2 = false + conv + bo + true;

    println(conv2);
    println("Test {} {}", conv, Float(5.56));

    var<double, ValueStorage> v1 = 7;
    var<double, ValueStorage> v2 = v1;
    var v3 = v1 + v2;
    println(concepts::Ref<decltype(v3)>);

    constexpr var ce_test = 6.;
    constexpr var ce_test2 = 10;
    constexpr var ce_res = ce_test / ce_test2;
    println(ce_res);

    println(concepts::Value<detail::get_value_t<Array<int> > >);
    println(concepts::Value<detail::get_value_t<Array<std::string> > >);
    println(concepts::Ref<detail::get_value_t<Array<A> > >);

    var f = 8.56758;
    Int ftoi = Int(f);
    f = Float("45.748342356");
    println(f + " " + ftoi);

    var a1 = objects::Array<Int>{3, 6};
    var a2 = {2, 1};

    Number<> fnumber;
}
