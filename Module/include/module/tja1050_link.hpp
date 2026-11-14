#pragma once
#include "module/can_node.hpp"
namespace fw::module{// The TJA1050 is a physical-layer transceiver; CAN bit timing/filtering live in the BSP.
class Tja1050Link{public:explicit Tja1050Link(fw::bsp::ICanBus&bus):node_(bus){}fw::Result<void>send(const CanFrame&f,fw::bsp::TransferMode m=fw::bsp::TransferMode::Interrupt,std::uint32_t timeoutMs=10){if((f.extended&&f.id>0x1FFFFFFFU)||(!f.extended&&f.id>0x7FFU))return fw::Result<void>::failure(fw::Error::InvalidArgument);return node_.send(f,m,timeoutMs);}fw::Result<CanFrame>receive(std::uint32_t timeoutMs=10){return node_.receive(timeoutMs);}private:CanNode node_;};}
