#include <chrono>
#include <iostream>
#include <vector>
#include <optional>
#include <random>
#include <string>
#include <type_traits>
#include "../include/optional.hpp"

using namespace std::chrono;

// Benchmark helper
template <typename F>
auto benchmark(F &&func, const std::string &name, int iterations = 1000000)
{
    auto start = high_resolution_clock::now();
    func(iterations);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << name << ": " << duration.count() << " μs\n";
    return duration.count();
}

struct TrivialType
{
    int value;
    double data;
    constexpr TrivialType(int v = 0, double d = 0.0) : value(v), data(d) {}
};

struct NonTrivialType
{
    std::string name;
    std::vector<int> data;

    NonTrivialType(const std::string &n = "default") : name(n), data{1, 2, 3, 4, 5} {}
    NonTrivialType(const NonTrivialType &) = default;
    NonTrivialType(NonTrivialType &&) = default;
    NonTrivialType &operator=(const NonTrivialType &) = default;
    NonTrivialType &operator=(NonTrivialType &&) = default;
    ~NonTrivialType() = default;
};

// Custom Optional benchmarks - no templates needed
void custom_trivial_construction(int iterations)
{
    std::vector<Optional<TrivialType>> optionals;
    optionals.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        optionals.emplace_back(TrivialType{i, i * 1.5});
    }
}

void custom_nontrivial_construction(int iterations)
{
    std::vector<Optional<NonTrivialType>> optionals;
    optionals.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        optionals.emplace_back(NonTrivialType{"item" + std::to_string(i)});
    }
}

void custom_trivial_copy(int iterations)
{
    Optional<TrivialType> source{TrivialType{42, 3.14}};
    std::vector<Optional<TrivialType>> copies;
    copies.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        copies.push_back(source);
    }
}

void custom_nontrivial_copy(int iterations)
{
    Optional<NonTrivialType> source{NonTrivialType{"source"}};
    std::vector<Optional<NonTrivialType>> copies;
    copies.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        copies.push_back(source);
    }
}

void custom_trivial_move(int iterations)
{
    std::vector<Optional<TrivialType>> sources;
    std::vector<Optional<TrivialType>> destinations;
    sources.reserve(iterations);
    destinations.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        sources.emplace_back(TrivialType{i, i * 1.5});
    }

    for (int i = 0; i < iterations; ++i)
    {
        destinations.push_back(std::move(sources[i]));
    }
}

void custom_trivial_assignment(int iterations)
{
    Optional<TrivialType> target;
    Optional<TrivialType> source{TrivialType{42, 3.14}};

    for (int i = 0; i < iterations; ++i)
    {
        target = source;
        if (i % 2 == 0)
        {
            target.reset();
        }
    }
}

void custom_trivial_destruction(int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        Optional<TrivialType> opt{TrivialType{i, i * 1.5}};
    }
}

void custom_nontrivial_destruction(int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        Optional<NonTrivialType> opt{NonTrivialType{"item" + std::to_string(i)}};
    }
}

// std::optional benchmarks - separate functions
void std_trivial_construction(int iterations)
{
    std::vector<std::optional<TrivialType>> optionals;
    optionals.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        optionals.emplace_back(TrivialType{i, i * 1.5});
    }
}

void std_nontrivial_construction(int iterations)
{
    std::vector<std::optional<NonTrivialType>> optionals;
    optionals.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        optionals.emplace_back(NonTrivialType{"item" + std::to_string(i)});
    }
}

void std_trivial_copy(int iterations)
{
    std::optional<TrivialType> source{TrivialType{42, 3.14}};
    std::vector<std::optional<TrivialType>> copies;
    copies.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        copies.push_back(source);
    }
}

void std_nontrivial_copy(int iterations)
{
    std::optional<NonTrivialType> source{NonTrivialType{"source"}};
    std::vector<std::optional<NonTrivialType>> copies;
    copies.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        copies.push_back(source);
    }
}

void std_trivial_move(int iterations)
{
    std::vector<std::optional<TrivialType>> sources;
    std::vector<std::optional<TrivialType>> destinations;
    sources.reserve(iterations);
    destinations.reserve(iterations);

    for (int i = 0; i < iterations; ++i)
    {
        sources.emplace_back(TrivialType{i, i * 1.5});
    }

    for (int i = 0; i < iterations; ++i)
    {
        destinations.push_back(std::move(sources[i]));
    }
}

void std_trivial_assignment(int iterations)
{
    std::optional<TrivialType> target;
    std::optional<TrivialType> source{TrivialType{42, 3.14}};

    for (int i = 0; i < iterations; ++i)
    {
        target = source;
        if (i % 2 == 0)
        {
            target.reset();
        }
    }
}

void std_trivial_destruction(int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        std::optional<TrivialType> opt{TrivialType{i, i * 1.5}};
    }
}

void std_nontrivial_destruction(int iterations)
{
    for (int i = 0; i < iterations; ++i)
    {
        std::optional<NonTrivialType> opt{NonTrivialType{"item" + std::to_string(i)}};
    }
}

// Memory layout verification
void verify_memory_layout()
{
    std::cout << "\n=== Memory Layout Analysis ===\n";
    std::cout << "Optional<int> size: " << sizeof(Optional<int>) << " bytes\n";
    std::cout << " Optional<TrivialType> size: " << sizeof(Optional<TrivialType>) << " bytes\n";
    std::cout << "Optional<NonTrivialType> size: " << sizeof(Optional<NonTrivialType>) << " bytes\n";

    std::cout << "std::optional<int> size: " << sizeof(std::optional<int>) << " bytes\n";
    std::cout << "std::optional<TrivialType> size: " << sizeof(std::optional<TrivialType>) << " bytes\n";
    std::cout << "std::optional<NonTrivialType> size: " << sizeof(std::optional<NonTrivialType>) << " bytes\n";
}

// Triviality verification
template <typename T>
void verify_triviality(const std::string &type_name)
{
    std::cout << "\n=== Triviality Analysis for " << type_name << " ===\n";
    std::cout << "is_trivially_destructible: " << std::is_trivially_destructible_v<T> << "\n";
    std::cout << " is_trivially_copy_constructible: " << std::is_trivially_copy_constructible_v<T> << "\n";
    std::cout << "is_trivially_copy_assignable: " << std::is_trivially_copy_assignable_v<T> << "\n";
    std::cout << "is_trivially_move_constructible: " << std::is_trivially_move_constructible_v<T> << "\n";
    std::cout << " is_trivially_move_assignable: " << std::is_trivially_move_assignable_v<T> << "\n";
}

int main()
{
    const int iterations = 1000000;

    std::cout << "=== Optional Performance Benchmarks ===\n\n";

    // Memory layout comparison
    verify_memory_layout();

    // Triviality verification
    verify_triviality<Optional<TrivialType>>("Optional<TrivialType>");
    verify_triviality<Optional<NonTrivialType>>("Optional<NonTrivialType>");
    verify_triviality<std::optional<TrivialType>>("std::optional<TrivialType>");
    verify_triviality<std::optional<NonTrivialType>>("std::optional<NonTrivialType>");

    std::cout << "\n=== Construction Benchmarks ===\n";

    auto custom_trivial_construction_time = benchmark(custom_trivial_construction, "Custom Optional<TrivialType> construction", iterations);
    auto std_trivial_construction_time = benchmark(std_trivial_construction, "std::optional<TrivialType> construction", iterations);

    auto custom_nontrivial_construction_time = benchmark(custom_nontrivial_construction, "Custom Optional<NonTrivialType> construction", iterations);
    auto std_nontrivial_construction_time = benchmark(std_nontrivial_construction, "std::optional<NonTrivialType> construction", iterations);

    std::cout << "\n=== Copy Benchmarks ===\n";

    auto custom_trivial_copy_time = benchmark(custom_trivial_copy, "Custom Optional<TrivialType> copy", iterations);
    auto std_trivial_copy_time = benchmark(std_trivial_copy, "std::optional<TrivialType> copy", iterations);

    std::cout << "\n=== Assignment Benchmarks ===\n";

    auto custom_trivial_assignment_time = benchmark(custom_trivial_assignment, "Custom Optional<TrivialType> assignment", iterations);
    auto std_trivial_assignment_time = benchmark(std_trivial_assignment, "std::optional<TrivialType> assignment", iterations);

    std::cout << "\n=== Move Benchmarks ===\n";

    auto custom_trivial_move_time = benchmark(custom_trivial_move, "Custom Optional<TrivialType> move", iterations);
    auto std_trivial_move_time = benchmark(std_trivial_move, "std::optional<TrivialType> move", iterations);

    std::cout << "\n=== Destruction Benchmarks ===\n";

    auto custom_trivial_destruction_time = benchmark(custom_trivial_destruction, "Custom Optional<TrivialType> destruction", iterations);
    auto std_trivial_destruction_time = benchmark(std_trivial_destruction, "std::optional<TrivialType> destruction", iterations);

    auto custom_nontrivial_destruction_time = benchmark(custom_nontrivial_destruction, "Custom Optional<NonTrivialType> destruction", iterations);
    auto std_nontrivial_destruction_time = benchmark(std_nontrivial_destruction, "std::optional<NonTrivialType> destruction", iterations);

    std::cout << "\n=== Performance Summary ===\n";
    std::cout << "Trivial construction ratio (std/custom): " << (double)std_trivial_construction_time / custom_trivial_construction_time << "x\n";
    std::cout << "Trivial copy ratio (std/custom): " << (double)std_trivial_copy_time / custom_trivial_copy_time << "x\n";
    std::cout << "Trivial assignment ratio (std/custom): " << (double)std_trivial_assignment_time / custom_trivial_assignment_time << "x\n";
    std::cout << "Trivial move ratio (std/custom): " << (double)std_trivial_move_time / custom_trivial_move_time << "x\n";
    std::cout << "Trivial destruction ratio (std/custom): " << (double)std_trivial_destruction_time / custom_trivial_destruction_time << "x\n";
}