#pragma once
#include "framework/error.hpp"
#include <cstddef>
#include <cstdint>
namespace fw::bsp{enum class TransferMode:std::uint8_t{Blocking,Interrupt,Dma};struct IByteStream{virtual~IByteStream()=default;virtual Result<std::size_t>write(const std::uint8_t*,std::size_t,TransferMode,std::uint32_t)=0;};struct ICanBus{virtual~ICanBus()=default;virtual Result<void>transmit(std::uint32_t,bool,const std::uint8_t*,std::uint8_t,TransferMode,std::uint32_t)=0;};struct II2cBus{virtual~II2cBus()=default;virtual Result<void>write(std::uint16_t,const std::uint8_t*,std::size_t,TransferMode,std::uint32_t)=0;virtual Result<void>read(std::uint16_t,std::uint8_t*,std::size_t,TransferMode,std::uint32_t)=0;};struct ITimer{virtual~ITimer()=default;virtual Result<void>startPeriodic(std::uint32_t,void(*)(void*),void*)=0;virtual void stop()=0;};}
