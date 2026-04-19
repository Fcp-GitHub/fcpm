#ifndef FCP_MATH_CORE_ARCH_HPP
#define FCP_MATH_CORE_ARCH_HPP

//----------------------------------------------------------------------------------
// Compiler detection
//----------------------------------------------------------------------------------

#ifndef FCPM_USE_COMPILER_SPECIFIC_FEATURES
#define FCPM_USE_COMPILER_SPECIFIC_FEATURES true
#endif

// Detect any compiler implementing the GNU 
// compiler extensions
#if defined(__GNUC__) && FCPM_USE_COMPILER_SPECIFIC_FEATURES
#define FCPM_COMPILER_GNU (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#define FCPM_COMPILER_GNU 0
#endif

// Detect Clang
#if defined(__clang__) && FCPM_USE_COMPILER_SPECIFIC_FEATURES
#define FCPM_COMPILER_CLANG 1
#else
#define FCPM_COMPILER_CLANG 0
#endif

// Detect GCC
#if FCPM_COMPILER_GNU && !FCPM_COMPILER_CLANG
#define FCPM_COMPILER_GCC FCPM_COMPILER_GNU
#else
#define FCPM_COMPILER_GCC 0
#endif

//TODO: only if extensions are required
// There must be at least one supported compiler
#if FCPM_USE_COMPILER_SPECIFIC_FEATURES
	#if 0 == (FCPM_COMPILER_GCC + FCPM_COMPILER_CLANG)
	#error "Unsupported compiler"
	#endif
#endif

//----------------------------------------------------------------------------------
// Attributes
//----------------------------------------------------------------------------------

#if FCPM_COMPILER_GCC | FCPM_COMPILER_CLANG
#define FCPM_ALWAYS_INLINE [[gnu::always_inline]]
#else
#define FCPM_ALWAYS_INLINE
#endif

#if FCPM_COMPILER_GCC | FCPM_COMPILER_CLANG
#define FCPM_FLATTEN [[gnu::flatten]]
#else
#define FCPM_FLATTEN
#endif

#if FCPM_COMPILER_GCC | FCPM_COMPILER_CLANG
#define FCPM_NOINLINE [[gnu::noinline]]
#else
#define FCPM_NOINLINE
#endif

#if FCPM_COMPILER_GCC | FCPM_COMPILER_CLANG
#define FCPM_CONST [[gnu::const]]
#else
#define FCPM_CONST
#endif

#if FCPM_COMPILER_GCC | FCPM_COMPILER_CLANG
#define FCPM_PURE [[gnu::pure]]
#else
#define FCPM_PURE
#endif

#endif	//FCP_MATH_CORE_ARCH_HPP
