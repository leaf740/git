#pragma once
#include "framework/ports.hpp"
#if defined(FW_STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(FW_STM32F4)
#include "stm32f4xx_hal.h"
#else
#error Define_FW_STM32F1_or_FW_STM32F4
#endif
namespace fw::bsp{class Stm32UartPort:public IByteStream{public:explicit Stm32UartPort(UART_HandleTypeDef&h):h_(h){}Result<std::size_t>write(const std::uint8_t*,std::size_t,TransferMode,std::uint32_t)override;private:UART_HandleTypeDef&h_;};class Stm32I2cPort:public II2cBus{public:explicit Stm32I2cPort(I2C_HandleTypeDef&h):h_(h){}Result<void>write(std::uint16_t,const std::uint8_t*,std::size_t,TransferMode,std::uint32_t)override;Result<void>read(std::uint16_t,std::uint8_t*,std::size_t,TransferMode,std::uint32_t)override;private:I2C_HandleTypeDef&h_;};class Stm32CanPort:public ICanBus{public:explicit Stm32CanPort(CAN_HandleTypeDef&h):h_(h){}Result<void>transmit(std::uint32_t,bool,const std::uint8_t*,std::uint8_t,TransferMode,std::uint32_t)override;private:CAN_HandleTypeDef&h_;};class Stm32TimerPort:public ITimer{public:explicit Stm32TimerPort(TIM_HandleTypeDef&h):h_(h){}Result<void>startPeriodic(std::uint32_t,void(*)(void*),void*)override;void stop()override;void onPeriodElapsed(TIM_HandleTypeDef*);private:TIM_HandleTypeDef&h_;void(*callback_)(void*){nullptr};void*context_{nullptr};};}
