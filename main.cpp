#include "include/varia/using_varia.h"

struct Person {
    Person(String name) {
        this->name = name;
    }

    virtual void greet() {
        std::cout << "Hi. My name is " << name << ".\n";
    }

    static Int id_counter;
    String name;
};

struct Employee : Person {
    Employee(String name, String role) : Person(name) {
        this->role = role;
    }

    void greet() override {
        Person::greet();
        std::cout << "My role is: " << role << " and my ID is: " << id;
    }

    String role;
    Int id;
};

int main() {
    var a = 6.657685768;
    float b = a;
    std::cout << b << '\n';
    var c = b;
    a = c;

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
    oof = 7;
    Num bar = "3";
    bar = "56";

    std::cout << q << '\n';

    // Immutable ref example
    var one = "one";
    var two = one;
    two += "two";
    std::cout << one << '\n';

    // Shared ref example
    var p = Person("Kalle");
    var p2 = p;
    p2->name += " Jr.";
    std::cout << p->name << '\n';

    String no = none;
    std::cout << no << '\n';
    std::cout << (no == none) << '\n';

    // Polymorphism example
    var<Person> e = Employee("Olle", "Janitor");
    var e2 = e;
    e2->name = "Pelle";
    e->greet();

    // Type ids and object ids
    std::cout << "Are they the same type? " << type(e) << " " << type(e2) << '\n';
    std::cout << "Do they have the same id? " << id(e) << " " << id(e2) << '\n';
}
