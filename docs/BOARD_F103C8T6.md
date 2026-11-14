# F103C8T6 CubeMX 工程接入记录

目录 testwork.stm 是 CubeMX 生成的 STM32F103C8T6 工程。读取 .ioc 和生成代码确认：72 MHz 系统时钟（HSE+PLL），USART1 使用 PA9/PA10、115200、8-N-1；PC13 配成 LED 输出；TIM4 被 Cube HAL 用作 1 ms 的 HAL tick，SWD 保留。

## 已接上的代码

- testwork.stm/CMakeLists.txt 将 C++ 编译标准设为 C++17，并把 BSP、Module、Application 源文件加入固件目标。
- Core/Src/main.c 在 CubeMX USER CODE 块调用 FW_Board_Init，并把 TIM period callback 转给 BSP。
- BSP/boards/stm32f103c8t6/board.cpp 用 USART1 输出启动和日志消息；将应用心跳挂到已运行的 TIM4，每 500 个 1 ms tick 翻转一次 PC13。定时器适配器识别并复用这个已运行的 HAL tick，不会停掉 HAL 时基。
- BSP/src/stm32_hal_ports.cpp 对未启用的 HAL CAN / I2C 模块做条件编译；F103 当前工程没有生成它们，因此现在不会编译出 CAN/I2C 硬件驱动。接口和模块包装仍在框架里，需在 CubeMX 勾选对应 IP、生成驱动后再接入。

## 构建与 Ozone 烧录

当前 build/Debug/CMakeCache.txt 记录了旧工程路径 d:/STM32/testwork.stm，不能复用。建议先安装并加入 PATH：Arm GNU Toolchain、CMake、Ninja、SEGGER J-Link 软件包。然后在 PowerShell：

```powershell
cd D:\testwork1\testwork.stm
cmake -S . -B build/f103-debug -G Ninja "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=cmake/gcc-arm-none-eabi.cmake" -DCMAKE_BUILD_TYPE=Debug
cmake --build build/f103-debug
```

编译成功后，在 Ozone 新建项目，目标选 STM32F103C8，调试探针选 J-Link / SWD，程序文件选 build/f103-debug/testwork.stm.elf，执行下载并调试。观察 main、FW_Board_Init 和 HAL_TIM_PeriodElapsedCallback；串口终端设 115200 8-N-1，预期看到 “framework initialized” 和 “STM32F103C8T6 ready”，PC13 LED 约每 0.5 秒变化一次。具体 ELF 名称以 build 目录产物为准。

## 当前限制

这个 .ioc 只启用 USART1、GPIO、RCC、SYS；TIM4 只作为 HAL 时基。没有 CAN、I2C、FreeRTOS task 配置，也没有采购器件的具体型号。不要把当前阶段描述成 CAN/I2C/RTOS 或实板验收已完成。

F411CEU6 尚未放入对应 CubeMX 工程。迁移时必须为 F411 重新生成时钟和引脚配置，再把 F411 的 UART/TIM 句柄传给同一 HAL port 接口；具体外设实例须以你的 F411 .ioc 为准。
