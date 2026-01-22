module;

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <span>
#include <print>
#include <ranges>

export module span_examples;

// Internal helper functions

// Demonstrates the use of std::span (C++20).
// BEST PRACTICE: Pass std::span by value.
export void demo_span(const std::span<const int> data) {
    // C++23: Using std::print for cleaner I/O without a stream state
    std::print("  Elements ({}): ", data.size());
    for (const int n : data) {
        std::print("{} ", n);
    }
    std::println();
}

int sum_elements(std::span<const int> data) {
    return std::accumulate(data.begin(), data.end(), 0);
}

// Modifies elements of a span (non-const).
void multiply_by_two(std::span<int> data) {
    for (int& n : data) {
        n *= 2;
    }
}

void process_window(std::span<const int> window) {
    // C++20 ranges::minmax_element
    auto [min_it, max_it] = std::ranges::minmax_element(window);
    
    if (min_it != window.end()) {
        std::println("  Window of {} elements: min={}, max={}", 
            window.size(), *min_it, *max_it);
    }
}

void demo_fixed_span(const std::span<int, 3> fixed) {
    std::print("  Fixed-size span (3 elements): ");
    for (int n : fixed) {
        std::print("{} ", n);
    }
    std::println("");
}

// Performance Comparison (internal)
volatile int span_sink = 0;
void process_span_for_perf(std::span<const int> s) {
    span_sink += s.size();
}

void compare_span_performance() {
    using clock = std::chrono::high_resolution_clock;
    constexpr int iterations = 100000;
    std::vector<int> large_vector(iterations);
    std::iota(large_vector.begin(), large_vector.end(), 0);

    auto measure = [&](auto task) {
        const auto start = clock::now();
        for (int i = 0; i < iterations; ++i) {
            task();
        }
        return std::chrono::duration<double, std::milli>(clock::now() - start).count();
    };

    double t1 = measure([&]() {
        process_span_for_perf(std::vector<int>(large_vector.begin() + 100, large_vector.begin() + 200));
    });

    std::span<const int> full_span(large_vector);
    double t2 = measure([&]() {
        process_span_for_perf(full_span.subspan(100, 100));
    });

    std::println("  Creating a sub-view of 100 elements {} times:", iterations);
    std::println("    Creating std::vector: {:>8.4f} ms (Allocation + Copy)", t1);
    std::println("    Creating std::span:   {:>8.4f} ms (Zero Copy)", t2);
}

// Public API
export void run_span_examples() {
    std::println("\n\n=== std::span EXAMPLES (C++20/23) ===");
    std::println("Key Principle: Use for contiguous sequence parameters. Decouples function from container type.\n");

    std::println("1. Basic usage with different containers:");
    std::vector vec = {1, 2, 3, 4, 5};
    std::array<int, 3> arr = {10, 20, 30};
    constexpr int c_arr[] = {100, 200, 300, 400};

    demo_span(vec);
    demo_span(arr);
    demo_span(c_arr);

    std::println("\n2. Operations with spans:");
    std::println("  Sum of vector elements: {}", sum_elements(vec));
    std::println("  Sum of array elements: {}", sum_elements(arr));
    
    std::println("\n3. Data modification (non-const span):");
    std::vector nums = {1, 2, 3, 4, 5};
    std::print("  Before: ");
    demo_span(nums);
    multiply_by_two(nums);
    std::print("  After multiplying by 2: ");
    demo_span(nums);

    std::println("\n4. Subspans & Sliding Window (C++20 ranges::minmax_element):");
    std::vector<int> data = {10, 20, 30, 40, 50, 60, 70, 80};
    
    for (size_t i = 0; i + 3 <= data.size(); i += 2) {
        std::print("  Window at position {}: ", i);
        process_window(std::span(data).subspan(i, 3));
    }

    std::println("\n5. Fixed-size span (compile-time size):");
    std::array<int, 3> fixed_arr = {7, 8, 9};
    demo_fixed_span(fixed_arr);

    std::println("\n6. first() and last() methods:");
    std::span<int> sp(data);
    std::print("  First 4: ");
    demo_span(sp.first(4));
    std::print("  Last 4: ");
    demo_span(sp.last(4));

    std::println("\n7. Performance comparison (indicative):");
    compare_span_performance();
}
