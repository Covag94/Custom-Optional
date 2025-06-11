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

## Benchmarking

This implementation of `Optional<T>` has been benchmarked against `std::optional<T>` using both trivial and non-trivial types. The evaluation covers construction, assignment, move, copy, and destruction performance.

### Summary

| Operation     | Type            | Custom `Optional` | `std::optional` | Ratio (std/custom) |
|--------------|------------------|-------------------|------------------|--------------------|
| Construction | TrivialType       | 39,093 μs         | 39,209 μs        | 1.00×              |
| Construction | NonTrivialType    | 398,721 μs        | 437,712 μs       | 1.10×              |
| Copy         | TrivialType       | 22,258 μs         | 22,761 μs        | 1.02×              |
| Assignment   | TrivialType       | 2,978 μs          | 2,351 μs         | 0.79×              |
| Move         | TrivialType       | 48,333 μs         | 53,648 μs        | 1.11×              |
| Destruction  | TrivialType       | 5,311 μs          | 11,296 μs        | 2.13×              |
| Destruction  | NonTrivialType    | 330,172 μs        | 353,625 μs       | 1.07×              |

### Benchmark Setup

Benchmarks were conducted using custom micro-benchmarking tools with tight loops and instrumented measurement of performance-critical operations. Both `TrivialType` and `NonTrivialType` scenarios were tested to simulate real-world usage under different trait conditions.

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
```