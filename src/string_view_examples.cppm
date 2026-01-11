module;

#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <utility>
#include <print> // C++23: Modern printing

export module string_view_examples;

// Internal helper functions (not exported)

// Demonstrates the use of std::string_view (C++17).
// BEST PRACTICE: Pass std::string_view by value.
// WHY?
// 1. SIZE: It's small (typically 16 bytes: ptr + size), fitting in 2 CPU registers.
// 2. INDIRECTION: Passing by 'const string_view&' adds a pointer dereference overhead
//    every time you access .size() or .data(). Passing by value avoids this.
// 3. SEMANTICS: It is a "borrowed reference" handle itself. Taking a reference to a reference is redundant.
export void demo_string_view(std::string_view sv) {
    // C++23: std::println is the modern standard for output
    std::println("  Content: \"{}\" | Length: {}", sv, sv.length());
}

bool contains_word(std::string_view text, std::string_view word) {
    return text.find(word) != std::string_view::npos;
}

// Splits a path into directory and filename using string_view.
// LIFETIME SAFETY: The returned string_views point to the memory of 'path'.
// 'path' must outlive the returned views. This is safe here because we process them immediately.
std::pair<std::string_view, std::string_view> parse_path(std::string_view path) {
    auto pos = path.find_last_of('/');
    if (pos != std::string_view::npos) {
        return {path.substr(0, pos), path.substr(pos + 1)};
    }
    return {"", path};
}

void check_prefix_suffix(std::string_view text) {
    std::println("  Text: \"{}\"", text);
    std::println("  Starts with 'Hello'? {}", text.starts_with("Hello"));
    std::println("  Ends with 'world'? {}", text.ends_with("world"));
}

// C++20: CONSTEVAL - Immediate Function
// This function forces execution at compile time.
consteval bool is_valid_identifier(std::string_view s) {
    if (s.empty()) return false;
    if (!((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z'))) return false;
    for (char c : s) {
        bool is_alpha = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        bool is_digit = (c >= '0' && c <= '9');
        if (!is_alpha && !is_digit && c != '_') return false;
    }
    return true;
}

// Performance Comparison (internal)
volatile size_t sink = 0;
void process_string(const std::string& s) {
    sink += s.length();
}

void process_string_view(std::string_view sv) {
    sink += sv.length();
}

void compare_string_performance() {
    constexpr int iterations = 1000000;
    const std::string large_string(1024, 'x');

    auto measure = [](auto func, auto&& arg) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func(arg);
        }
        return std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();
    };

    double t1 = measure(process_string, "this is a test string");
    double t2 = measure(process_string_view, "this is a test string");

    std::println("  Passing a C-string literal {} times:", iterations);
    std::println("    const std::string&: {:>8.4f} ms (Allocation per call)", t1);
    std::println("    std::string_view:   {:>8.4f} ms (Zero allocation)", t2);

    double t3 = measure(process_string, large_string.substr(0, 512));
    double t4 = measure(process_string_view, std::string_view(large_string).substr(0, 512));

    std::println("  Passing a substring {} times:", iterations);
    std::println("    const std::string&: {:>8.4f} ms (Allocation + Copy)", t3);
    std::println("    std::string_view:   {:>8.4f} ms (Zero Copy)", t4);
}

// Public API (exported)
export void run_string_view_examples() {
    std::println("=== std::string_view EXAMPLES (C++20/23) ===");
    std::println("Key Principle: Use for read-only function arguments. Never for storage.\n");

    // 1. Compile-time validation (Zero Overhead)
    std::println("1. Compile-time validation (consteval):");
    constexpr std::string_view id = "variable_123";
    if constexpr (is_valid_identifier(id)) {
        std::println("  '{}' is a valid identifier (Checked at compile-time!)", id);
    } else {
        std::println("  Invalid identifier (Compile-time error)");
    }

    std::println("\n2. Basic usage:");
    std::string str = "I am a std::string";
    const char* literal = "I am a C string literal";

    demo_string_view(str);
    demo_string_view(literal);

    std::println("\n3. Efficient substrings (no copies):");
    std::string_view sv = str;
    demo_string_view(sv.substr(7, 6)); 
    demo_string_view(sv.substr(0, 4));

    std::println("\n4. Path parsing:");
    auto [dir, file] = parse_path("/home/user/documents/file.txt");
    std::println("  Directory: \"{}\"", dir);
    std::println("  Filename: \"{}\"", file);

    std::println("\n5. Prefix and suffix verification:");
    check_prefix_suffix("Hello world");

    std::println("\n6. Performance comparison (indicative):");
    compare_string_performance();
}