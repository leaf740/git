#include "application/tasks.hpp"
#include "framework/log.hpp"
namespace fw::app{fw::Result<void>initialize(){return fw::log(fw::LogLevel::Info,"app","framework initialized");}}
