#pragma once
#include "framework/ports.hpp"
namespace fw::module{using CanFrame=fw::bsp::CanPacket;class CanNode{public:explicit CanNode(fw::bsp::ICanBus&b):bus_(b){}fw::Result<void>send(const CanFrame&f,fw::bsp::TransferMode m=fw::bsp::TransferMode::Interrupt,std::uint32_t t=10){if(f.size>8)return fw::Result<void>::failure(fw::Error::InvalidArgument);return bus_.transmit(f.id,f.extended,f.data,f.size,m,t);}fw::Result<CanFrame>receive(std::uint32_t timeoutMs=10){return bus_.receive(timeoutMs);}private:fw::bsp::ICanBus&bus_;};}
