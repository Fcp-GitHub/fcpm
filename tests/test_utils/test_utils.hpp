#ifndef FCP_TESTS_UTILITY_UTILS_HPP
#define FCP_TESTS_UTILITY_UTILS_HPP

// Taken from: https://www.reddit.com/r/cpp/comments/ijtokq/an_alternative_to_stdtype_info_and_typeid_with/

#include <memory>
#include <cxxabi.h>

namespace utility
{
	
	template<typename T>
	std::string type_name()
	{
	    int status = 0;
	
	    std::unique_ptr<char, void(*)(void*)> res {
	        abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status),
	        std::free
	    };
	
	    if (status != 0) throw status; // stub
	
	    return res.get();
	}
	
}	// namespace utility

#endif	//FCP_TESTS_UTILITY_UTILS_HPPY
