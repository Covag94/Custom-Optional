# Optional<T> – A Lightweight Optional Value Type in C++

This project provides a C++17-compatible implementation of a lightweight `Optional<T>` class, similar in behavior to `std::optional<T>`. It allows representing objects that may or may not contain a value.

## Features

- Stores objects in-place without heap allocation
- Fully supports copy, move, and assignment semantics
- Provides safe access to contained values with value-checking
- Offers `value_or`, `operator*`, `operator->`, and more
- Implements equality comparisons
- Respects noexcept and type traits for safe and optimized usage
- Compatible with modern C++ idioms and practices

## Files

- `Optional.h`: The main header file containing the `Optional<T>` implementation

## Usage

### Basic Example

```cpp
#include "Optional.h"
#include <iostream>

int main() {
    Optional<int> opt1;                   // empty
    Optional<int> opt2 = 42;              // initialized
    Optional<int> opt3 = nullopt;         // explicitly empty

    if (opt2.has_value()) {
        std::cout << "opt2 contains: " << *opt2 << '\n';
    }

    int x = opt1.value_or(100);           // x = 100, fallback value
    std::cout << "opt1 fallback: " << x << '\n';

    opt1 = 7;
    std::cout << "opt1 now has: " << opt1.value() << '\n';
}
