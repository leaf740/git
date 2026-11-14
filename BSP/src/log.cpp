#include "framework/log.hpp"
namespace fw{namespace{ILogger*g=nullptr;}void setLogger(ILogger*p)noexcept{g=p;}Result<void>log(LogLevel l,const char*t,const char*m)noexcept{if(!g)return Result<void>::failure(Error::NotInitialized);if(!t||!m)return Result<void>::failure(Error::InvalidArgument);return g->write(l,t,m);}}
