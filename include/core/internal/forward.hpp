#ifndef FCP_MATH_CORE_FORWARD_HPP
#define FCP_MATH_CORE_FORWARD_HPP

//----------------------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------------------

namespace fcp {
namespace math {
namespace internal {

template <typename T>
struct Traits;

template <typename T, int N, int M, typename Layout>
struct StaticStorageBase;

template <typename Expr, typename T, typename Derived>
struct UnaryExpressionBase;

template <typename Expr, typename T, typename UnaryOp>
struct ElementwiseUnaryExpression; 

template <typename Expr, typename T, typename UnaryOp>
struct MemoizedUnaryExpression; 

template <typename LeftExpr, typename RightExpr, typename T, typename Derived>
struct BinaryExpressionBase;

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct ElementwiseBinaryExpression;

template <typename LeftExpr, typename RightExpr, typename T, typename BinaryOp>
struct MemoizedBinaryExpression;

template <typename Expr, typename T>
struct TransposeExpr;

template <typename Expr, typename T>
struct ConjugateExpr;

template <typename Expr>
struct PermutationExpr;

template <typename LeftExpr, typename RightExpr, typename T>
struct GemmExpr;

template <typename Expr, typename T>
struct InverseExpr;

struct NoPlugin;

template <typename Derived>
struct QuaternionPlugin;

template <typename Derived>
class QuaternionBase;

}	// internal
	
struct RowMajorTag;
struct ColumnMajorTag;

template <typename T, int NumRows, int NumColumns, int Flags>
class Matrix;

template <typename Parent, int NumRows, int NumColumns>
class BlockView;

template <typename T>
struct Scalar;

template <typename T>
class Quaternion;

template <typename T>
class UnitQuaternion;

}	// math
}	// fcp

#endif	//FCP_MATH_CORE_FORWARD_HPP
