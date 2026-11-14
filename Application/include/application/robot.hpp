#pragma once
#include "framework/ports.hpp"
#include "module/mpu6050.hpp"
#include "module/tja1050_link.hpp"
#include "module/sg90_servo.hpp"
namespace fw::app {
fw::Result<void> initialize(fw::bsp::II2cBus&, fw::bsp::ICanBus&, fw::bsp::IPwmOutput&);
fw::Result<fw::module::Mpu6050Sample> readImu();
fw::Result<void> setServoAngle(std::uint16_t degrees);
fw::Result<void> sendCan(const fw::module::CanFrame&, fw::bsp::TransferMode mode = fw::bsp::TransferMode::Interrupt);
}
