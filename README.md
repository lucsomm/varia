# Varia
A modern header-only **C++20 library** that transforms C++ syntax into what resembles a **strongly typed scripting language**. 

## `var` - `auto`++
A transparent value wrapper that:
### Deduces to more sensible, default initialized types
```cpp
  var a = 5; // var<int64_t>
  var b = "Hello"; // var<std::string>
  var c = {3, 4, 5.5}; // var<std::vector<double>>
  String s; // Alias for var<std::string>
```

### Provides safer storage rules
Primitives are trivially copied or shared *copy-on-write* references, while user defined types and sequence containers are shared and reference counted.

You are free to opt out of any Varia default storage behavior for your custom class. Either by letting your custom type inherit `CopiedObject`, 
`ImmutableSharedObject` or `SharedObject`, or at usage `var<Construct_Custom, CopiedStorage>`.

### Supports custom classes with the `varclass` macro
Generates var-wrapped aliases for your type.
```cpp
  varclass(A,
         explicit Construct_A(int a) {
             this->a = a;
         }

         virtual void greet() {
             println("Hello, I am A.");
         }

         Int a = 8;
);

varclass(B,
         void greet() override {
             println("Hello, I am B.");
         }

         , A);

int main() {
    // Runtime polymorphism example
    var a = A(4);
    A b = B();

    a->greet(); // "Hello, I am A."
    b->greet(); // "Hello, I am B."
}
```

## Other varia features
- Constexpr evaluation for copied `var` types
- Integrates with either **std::format** or **fmt** for nicer string conversion, formatting and printing
