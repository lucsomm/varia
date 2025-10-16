#include "include/varia/using_varia.h"

struct Person {
    virtual ~Person() = default;

    explicit Person(String name) {
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
    println("Hello World!");
    println(7);

    var a = "4";
    var b = Num(a);
    a += 6;
    b + a;
    a + b;

    a + "";
    Bool(false) + "" + a;

    println("a: " + a + " b: " + b);
    println("a == b? " + (Num(a) == b));

    var<Person> e = Employee("Bob", "Contributor");
    var<Person> e2 = Employee("Bob", "Contributor");
    println("Are they the same? " + (id(e) == id(e2)) + " Are they the same value? " + (e == e2));

    println("Test {} {} {} {}", Num(5), 4, "2", String("Testing"));
    println(Bool(false) == none);
    println(Bool() == none);

    println(Num(Num() == none));
}
