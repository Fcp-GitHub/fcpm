#ifndef FCP_MATH_CORE_HARDWARE_HPP
#define FCP_MATH_CORE_HARDWARE_HPP

#ifndef FCPM_MATH_L1_SIZE
#define FCPM_MATH_L1_SIZE 32768
#endif

#ifndef FCPM_ENGINE_SOO_THRESHOLD
#define FCPM_ENGINE_SOO_THRESHOLD 16
#endif

#ifndef FCPM_CMP_MAX_ULP
#define FCPM_CMP_MAX_ULP 6
#endif

// Epsilon used for fast comparison
#ifndef FCPM_CMP_EPSILON
#define FCPM_CMP_EPSILON 1.e-3L
#endif

// Fast comparison method
#ifndef FCPM_CMP_FAST 
#define FCPM_CMP_FAST 0
#endif

// Fully accurate comparison method
#ifndef FCPM_CMP_ACCURATE 
#define FCPM_CMP_ACCURATE 1
#endif

// Internal comparison method
// Possible values:
// - FCPM_CMP_FAST: fast comparison method (less accurate)
// - FCPM_CMP_ACCURATE: accurate comparison method (slower)
#ifndef FCPM_CMP_INTERNAL
#define FCPM_CMP_INTERNAL FCPM_CMP_FAST
#endif

// Row/Column-major default storage layout for the graphics API
// Possible values:
// - true: use row-major format
// - false: use column-major format
#ifndef FCPM_GRAPHICS_USE_ROW_MAJOR
#define FCPM_GRAPHICS_USE_ROW_MAJOR false
#endif

// Row/Column-vector convention for the graphics API
// Possible values:
// - true: use column-vector convention
// - false: use row-vector convention
#ifndef FCPM_GRAPHICS_USE_CVECTOR_FORMAT 
#define FCPM_GRAPHICS_USE_CVECTOR_FORMAT true
#endif

// Handedness convention
// Possible values:
// - true: use LHS convention
// - false: use RHS convention
#ifndef FCPM_GRAPHICS_USE_LHS_SYSTEM 
#define FCPM_GRAPHICS_USE_LHS_SYSTEM false
#endif

// Depth interval convention
// Possible values:
// - true: use interval [-1, +1]
// - false: use interval [0, +1]
#ifndef FCPM_GRAPHICS_USE_NOTO_DEPTH_INTERVAL
#define FCPM_GRAPHICS_USE_NOTO_DEPTH_INTERVAL true
#endif

#endif	//FCP_MATH_CORE_HARDWARE_HPP
