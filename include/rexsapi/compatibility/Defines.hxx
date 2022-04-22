
#ifndef REXSAPI_COMPATIBILITY_DEFINES_HXX
#define REXSAPI_COMPATIBILITY_DEFINES_HXX

#if defined(_MSC_VER)
  using ssize_t = std::make_signed_t<size_t>;
#endif

#endif
