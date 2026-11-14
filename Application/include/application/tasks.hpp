#pragma once
#include <cstdint>
namespace fw::app{enum class Comm:std::uint8_t{None,Queue,BinarySemaphore,EventGroup};struct TaskSpec{const char*name;std::uint8_t priority;std::uint16_t stackWords;std::uint16_t periodMs;Comm communication;};constexpr TaskSpec kTasks[]={{"control",4,384,1,Comm::Queue},{"io",3,384,2,Comm::Queue},{"monitor",1,256,1000,Comm::EventGroup}};}
