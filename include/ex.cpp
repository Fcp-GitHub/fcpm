#include <type_traits>

template <typename D>
struct Traits {};




template <typename T, typename = void>
struct has_ext : std::false_type{};

template <typename T>
struct has_ext<T, std::void_t<typename Traits<T>::ext>> : std::true_type {};

template <typename T>
constexpr inline bool has_ext_v{ has_ext<T>::value };

struct NoExtension {};

template <typename Derived>
struct Base : std::conditional_t<
								has_ext_v<Derived>, 
								typename Traits<Derived>::ext, 
								NoExtension
							>
{
	void base_common_method() {};
};

struct ExtensionForActual
{
	void extension_for_actual_method() {}
};

struct Actual;

template <>
struct Traits<Actual>
{
	using ext = ExtensionForActual;
};

struct Actual : Base<Actual>
{
	void actual_class_method() {}
};


int main()
{
	Actual obj;	
	obj.extension_for_actual_method();	

}
