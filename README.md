`fcpm` is a header-only, C++23 template library for linear algebra and graphics programming.  

# Overview
This project serves as a personal laboratory for experimenting with modern C++ optimization techniques. The core features of the library are:
- **Lazy-evaluation**: `fcpm` avoids unnecessary memory allocations and redundant loops by using expression templates (when useful).
- **Modern C++23**: leverages the latest language features, including multi-dimensional subscript operators, concepts and template metaprogramming to ensure type safety and compile-time optimizations.
- **STL integration**: everything is designed with STL-compatibility in mind.
- **Constexpr-capable**: `fcpm` is designed to be 100% constexpr-capable. Every functionality can be used in a compile-time context.
- **Linear algebra**: custom matrix and vector types, view objects and solvers.
- **Graphics**: custom quaternion and unit quaternion types, geometric transforms and conversions.

# Status
While the core architecture is established, this repository is an active work-in-progress. Here are some of the features that I plan to add in the future:
- [ ] SIMD support
- [ ] `analysis` module for tasks such as differentiation, integration, etc.
- [ ] Library API macros and compiler-specific options (e.g. force-inline).
- [ ] Pretty-print options.
- [ ] Mask operations.
- [ ] More algorithms to further optimize complex expression trees.
- [ ] Better constexpr mathematical functions (the current ones use Taylor expansions, leading to massive errors in some situations).
- [ ] Support for more Euler angles' conventions.
