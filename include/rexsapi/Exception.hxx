
#ifndef REXSCXX_EXCEPTION_HXX
#define REXSCXX_EXCEPTION_HXX

#include <stdexcept>
#include <string>

namespace rexsapi
{
  class Exception : public std::runtime_error
  {
  public:
    using std::runtime_error::runtime_error;
  };
}


#endif
