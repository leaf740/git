#pragma once
#include "framework/error.hpp"
#include <cstdint>
namespace fw{enum class LogLevel:std::uint8_t{Debug,Info,Warn,Error};class ILogger{public:virtual~ILogger()=default;virtual Result<void>write(LogLevel,const char*,const char*)=0;};void setLogger(ILogger*)noexcept;Result<void>log(LogLevel,const char*,const char*)noexcept;}
