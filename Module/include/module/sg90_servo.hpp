#pragma once
#include "framework/ports.hpp"
namespace fw::module{class Sg90Servo{public:explicit Sg90Servo(fw::bsp::IPwmOutput&p,std::uint16_t minPulseUs=1000,std::uint16_t maxPulseUs=2000):pwm_(p),min_(minPulseUs),max_(maxPulseUs){}fw::Result<void>initialize();fw::Result<void>setAngle(std::uint16_t degrees);fw::Result<void>setPulseWidthUs(std::uint16_t pulseUs);private:fw::bsp::IPwmOutput&pwm_;std::uint16_t min_,max_;};}
