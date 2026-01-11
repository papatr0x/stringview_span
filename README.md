# Modern C++ Example: `std::string_view`, `std::span` & C++20 Modules

[![C++23](https://img.shields.io/badge/Standard-C%2B%2B23-blue.svg)](https://en.cppreference.com/w/cpp/23) [![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## Why this project?

This repository documents my journey exploring the "modern" parts of C++ that we often skip. I wanted to move beyond the theory and build something that applies **strictly** modern standards to understand the trade-offs.

This is not just a code dump; it's a guide to:
- **Efficiency**: Zero-copy operations with `std::string_view` (C++17) and `std::span` (C++20).
- **Modern I/O**: Using `std::print` (C++23) for cleaner output.
- **Architecture**: Organizing code with **C++20 Modules**.

---

## 🧠 Key Learnings & "Gotchas"

### 1. The Performance Rule: Pass by Value ⚡
I found that `std::string_view` and `std::span` should almost always be passed by **value**, not by `const&`.

*   **Observation**: These views are just a pointer + size (16 bytes).
*   **The Win**: Passing by value puts them in CPU registers, avoiding memory indirection overhead.

```cpp
// ✅ Correct: Fast, register-friendly
void process(std::string_view sv);

// ❌ Avoid: Adds unnecessary pointer dereference
void process(const std::string_view& sv);
```

### 2. The Safety Trap: Lifetime Management 🛡️
The biggest risk I encountered was **dangling references**. Since views don't own data, returning a view to a local variable is a guaranteed crash (or worse, silent corruption).

```cpp
// ☠️ DANGER: Returning view to a destroyed object
std::string_view get_name() {
    std::string s = "Temporary";
    return s;
}
```

### 3. C++ Modules: Theory vs. Reality 🏗️
Migrating to `.cppm` files was cleaner but challenging.
*   **Pro**: No more header/source split for small modules. `export` makes APIs crystal clear.
*   **Con**: Toolchain support is still maturing. I had to use **LLVM Clang 17+** and **Ninja**, as standard AppleClang (Xcode) struggled with some module features.

### 4. CMake Configuration for Modules 🛠️
To support C++20 Modules properly, the `CMakeLists.txt` requires a specific setup:

- **Version 3.28+**: Required for robust `FILE_SET CXX_MODULES` support.
- **`FILE_SET` API**: Instead of just adding files to `add_library`, we use the new `FILE_SET` syntax to let CMake manage module dependencies and visibility.

```cmake
add_library(my_module)
target_sources(my_module
  PUBLIC
    FILE_SET CXX_MODULES FILES
    src/my_module.cppm
)
```

---

## Features

### `std::string_view` (C++17)
A non-owning view of a character string.
- **O(1) Substrings**: Operations like `substr()` are instantaneous.
- **Flexibility**: Works with `std::string`, string literals, and `char` arrays.

### `std::span` (C++20)
A view over a contiguous block of memory (the "array view").
- **Unified API**: Accepts `std::vector`, `std::array`, or C-style arrays.
- **Safety**: Knows its size, preventing buffer overflows common with raw pointers.

### `std::print` (C++23)
- **Cleaner Syntax**: No more `<<` operator chaining.
- **Type Safety**: Compile-time format string checks.

---

## How to Build

**Prerequisites:**
- **CMake**: 3.28+
- **Compiler**: LLVM Clang 17+ (Homebrew version recommended on macOS)
- **Generator**: Ninja

### macOS (Homebrew) Setup

1.  **Install dependencies:**
    ```bash
    brew install llvm ninja cmake
    ```

2.  **Configure and Build:**
    ```bash
    # Configure using Homebrew's Clang and Ninja
    cmake -B build -S . -G Ninja \
          -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
          -DCMAKE_MAKE_PROGRAM=/opt/homebrew/opt/ninja/bin/ninja

    # Build
    cmake --build build
    ```

3.  **Run:**
    ```bash
    ./build/stringview_span
    ```

## Project Structure

```
Project/
├── CMakeLists.txt              # CMake with C++20 Module support
├── README.md                   # This file
└── src/
    ├── main.cpp                # Main entry point (imports modules)
    ├── string_view_examples.cppm # String view module
    └── span_examples.cppm      # Span module
```
