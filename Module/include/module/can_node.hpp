#pragma once
#include "framework/ports.hpp"
namespace fw::module{struct CanFrame{std::uint32_t id{};bool extended{};std::uint8_t size{};std::uint8_t data[8]{};};class CanNode{public:explicit CanNode(fw::bsp::ICanBus&b):bus_(b){}fw::Result<void>send(const CanFrame&f,fw::bsp::TransferMode m=fw::bsp::TransferMode::Interrupt,std::uint32_t t=10){if(f.size>8)return fw::Result<void>::failure(fw::Error::InvalidArgument);return bus_.transmit(f.id,f.extended,f.data,f.size,m,t);}private:fw::bsp::ICanBus&bus_;};}
