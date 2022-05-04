
#ifndef REXSAPI_EXCEPTION_HXX
#define REXSAPI_EXCEPTION_HXX

#include <stdexcept>

namespace rexsapi
{
  class TException : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };
}

#endif
