# Optional<T> - Custom Optional Type in C++

This project implements a minimal version of `std::optional` in C++ as a templated class `Optional<T>`.

## Features

- Stores an object of type `T` in aligned raw storage
- Supports:
  - Default construction (no value)
  - Copy and move construction
  - Copy-and-swap assignment (exception safe)
  - Emplacement of values
  - Access to value via dereference and `value()`
- Exception-safe construction and assignment
- Lightweight and header-only

## Example Usage

```cpp
#include "Optional.h"
#include <iostream>

int main() {
    Optional<int> opt1;
    Optional<int> opt2(42);

    if (opt2) {
        std::cout << "Value: " << *opt2 << std::endl;
    }

    opt1 = opt2;

    if (opt1.has_value()) {
        std::cout << "Copied value: " << opt1.value() << std::endl;
    }

    return 0;
}
