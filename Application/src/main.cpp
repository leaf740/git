#include "application/robot.hpp"
#include "framework/log.hpp"
#include <optional>
namespace fw::app {
namespace { std::optional<fw::module::Mpu6050> imu; std::optional<fw::module::Tja1050Link> can; std::optional<fw::module::Sg90Servo> servo; }
fw::Result<void> initialize(fw::bsp::II2cBus& i2c, fw::bsp::ICanBus& canBus, fw::bsp::IPwmOutput& pwm) {
    imu.emplace(i2c); can.emplace(canBus); servo.emplace(pwm);
    auto result = imu->initialize();
    if (!result) { fw::log(fw::LogLevel::Error, "mpu6050", "WHO_AM_I/configuration failed"); return result; }
    result = servo->initialize();
    if (!result) { fw::log(fw::LogLevel::Error, "sg90", "PWM start/center command failed"); return result; }
    fw::log(fw::LogLevel::Info, "app", "MPU6050, TJA1050 CAN link and SG90 initialized");
    return fw::Result<void>::success();
}
fw::Result<fw::module::Mpu6050Sample> readImu() { return imu ? imu->read() : fw::Result<fw::module::Mpu6050Sample>::failure(fw::Error::NotInitialized); }
fw::Result<void> setServoAngle(std::uint16_t degrees) { return servo ? servo->setAngle(degrees) : fw::Result<void>::failure(fw::Error::NotInitialized); }
fw::Result<void> sendCan(const fw::module::CanFrame& frame, fw::bsp::TransferMode mode) { return can ? can->send(frame, mode) : fw::Result<void>::failure(fw::Error::NotInitialized); }
}
extern "C" int FW_App_Init(void* i2cBus, void* canBus, void* servoPwm) {
    if (!i2cBus || !canBus || !servoPwm) return static_cast<int>(fw::Error::InvalidArgument);
    auto result = fw::app::initialize(*static_cast<fw::bsp::II2cBus*>(i2cBus), *static_cast<fw::bsp::ICanBus*>(canBus), *static_cast<fw::bsp::IPwmOutput*>(servoPwm));
    return result ? 0 : static_cast<int>(result.error());
}
