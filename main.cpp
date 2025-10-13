#include "include/varia/using_varia.h"

struct Person {
    Person(String name) {
        this->name = name;
    }

    virtual void greet() {
        println("Hi. My name is " + name + ".");
    }

    auto operator<=>(const Person&) const = default;

    static Int id_counter;
    String name;
};

struct Employee : Person {
    Employee(String name, String role) : Person(name) {
        this->role = role;
    }

    void greet() override {
        Person::greet();
        println("My role is: " + role + " and my ID is: " + id);
    }

    String role;
    Int id;
};

int main() {
    var a = 6.657685768;
    float b = a;
    println(b);
    var c = b;
    a = c;

    var n = none;
    var s = "Hello!" + String(" Yello!");
    println(s == none);
    s = n;
    println(s == none);
    println(s);
    s = String(a);
    s->shrink_to_fit();
    println(s);

    var q = "2 + 2.5 = ";
    var ans = Num(2) + 2.5;
    q += String(ans) + String(5);

    String oof = 6;
    oof = 7;
    Num bar = "3";
    bar = "56";

    println(q);

    // Immutable ref example
    var one = "one";
    var two = one;
    two += "two";
    println(one);

    // Shared ref example
    var p = Person("Kalle");
    var p2 = p;
    p2->name += " Jr.";
    println(p->name);

    String no = none;
    println(no);
    println(no == none);

    // Polymorphism example
    var<Person> e = Employee("Olle", "Janitor");
    var e2 = e;
    e2->name = "Pelle";
    e->greet();
    var<Person> e3 = Employee("Pelle", "Janitor");

    // Type ids and object ids
    std::cout << "Do they have the same id (id(e) == id(e3))? " << (id(e) == id(e3)) << '\n';
    std::cout << "Are they the same (e == e3)? " << (e == e3) << '\n';
    println(String("Do they have the same id (id(e) == id(e3))? ") + (id(e) == id(e3)));

    var testo = false;
    println(testo);
}
