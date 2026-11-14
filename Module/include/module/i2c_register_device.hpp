#pragma once
#include "framework/ports.hpp"
namespace fw::module{class I2cRegisterDevice{public:I2cRegisterDevice(fw::bsp::II2cBus&b,std::uint16_t a):bus_(b),address_(a){}fw::Result<void>read(std::uint8_t,std::uint8_t*,std::size_t,fw::bsp::TransferMode=fw::bsp::TransferMode::Blocking,std::uint32_t=20);fw::Result<void>write(std::uint8_t,const std::uint8_t*,std::size_t,std::uint32_t=20);private:fw::bsp::II2cBus&bus_;std::uint16_t address_;};}
