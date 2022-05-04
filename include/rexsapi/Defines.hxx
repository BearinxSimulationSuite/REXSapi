
#ifndef REXSAPI_DEFINES_HXX
#define REXSAPI_DEFINES_HXX

#include <type_traits>

#if defined(_MSC_VER)
using ssize_t = std::make_signed_t<size_t>;
#endif

#endif
