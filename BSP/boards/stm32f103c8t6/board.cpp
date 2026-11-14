#include "bsp_f103_board_c_api.h"
#include "framework/stm32_hal_ports.hpp"
#include "framework/log.hpp"
#include "main.h"
#include <cstdio>
#include <cstring>

extern "C" {
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
}

namespace {
fw::bsp::Stm32UartPort uart(huart1);
fw::bsp::Stm32TimerPort halTickTimer(htim4);
fw::bsp::Stm32I2cPort i2cBus(hi2c1);
fw::bsp::Stm32CanPort canBus(hcan1);
fw::bsp::Stm32PwmPort servoPwm(htim2, TIM_CHANNEL_1, 20000U);
class UartLogger final : public fw::ILogger {
public:
    fw::Result<void> write(fw::LogLevel, const char* tag, const char* message) override {
        if (tag == nullptr || message == nullptr) return fw::Result<void>::failure(fw::Error::InvalidArgument);
        char line[128]{};
        const int n = std::snprintf(line, sizeof(line), "[%s] %s\r\n", tag, message);
        if (n < 0 || static_cast<std::size_t>(n) >= sizeof(line)) return fw::Result<void>::failure(fw::Error::InvalidArgument);
        const auto sent = uart.write(reinterpret_cast<const std::uint8_t*>(line), static_cast<std::size_t>(n), fw::bsp::TransferMode::Blocking, 50);
        return sent ? fw::Result<void>::success() : fw::Result<void>::failure(sent.error());
    }
} logger;
std::uint16_t heartbeatTicks = 0;
void heartbeat(void*) {
    if (++heartbeatTicks >= 500U) {
        heartbeatTicks = 0;
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
}
}

extern "C" int FW_Board_Init(void) {
    fw::setLogger(&logger);
    const int peripherals = FW_Board_PeripheralsInit();
    if (peripherals != 0) {
        fw::log(fw::LogLevel::Error, "bsp", "CAN/I2C/TIM2 initialization failed");
        return peripherals;
    }
    // CubeMX already starts TIM4 as the 1 ms HAL tick; attach without stopping/restarting it.
    auto timer = halTickTimer.startPeriodic(1000U, heartbeat, nullptr);
    if (!timer) {
        fw::log(fw::LogLevel::Error, "bsp.timer", "TIM4 attach failed");
        return static_cast<int>(timer.error());
    }
    const char ready[] = "STM32F103C8T6 ready; USART1/CAN1/I2C1/TIM2 initialized\r\n";
    auto sent = uart.write(reinterpret_cast<const std::uint8_t*>(ready), sizeof(ready) - 1U, fw::bsp::TransferMode::Blocking, 100U);
    return sent ? 0 : static_cast<int>(sent.error());
}

extern "C" void* FW_Board_I2cBus(void) { return &i2cBus; }
extern "C" void* FW_Board_CanBus(void) { return &canBus; }
extern "C" void* FW_Board_ServoPwm(void) { return &servoPwm; }

extern "C" void FW_Board_TimerElapsed(void* handle) {
    halTickTimer.onPeriodElapsed(static_cast<TIM_HandleTypeDef*>(handle));
}
