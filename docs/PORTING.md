# CubeMX 与 F103 → F411 移植

STM32F103C8T6 与 STM32F411CEU6 都支持 bxCAN 和常见 UART HAL 操作。时钟树、GPIO 复用、DMA stream/channel、中断和启动文件必须由目标芯片 CubeMX 工程重新生成。

1. CubeMX 新建 STM32F103C8Tx 工程，设置实际晶振/时钟、SWD、USART、TIM；按真实接线选 CAN/I2C。
2. 生成 HAL 工程并保留 USER CODE 区。句柄在 BSP 适配器构造时传入；Application 不引用 HAL handle。
3. 加入 BSP/include、Module/include、Application/include；把 BSP/src/log.cpp、Module/src/i2c_register_device.cpp 加入编译。链接 HAL 适配器时还要加入 BSP/src/stm32_hal_ports.cpp，并定义 FW_STM32F1。
4. CubeMX 新建 STM32F411CEUx 工程，重新生成时钟、引脚、USART、TIM，定义 FW_STM32F4。新句柄传给 Stm32UartPort 和 Stm32TimerPort，核心接口与业务不变。
5. 先验证阻塞 UART，再开定时器中断。CubeMX 的 HAL_TIM_PeriodElapsedCallback 中，检查 htim->Instance 是否匹配，再调用对应 adapter.onPeriodElapsed(htim)。periodUs 参数是期望周期；需在 CubeMX 定时器 prescaler/period 中配成相同值。
6. TX/RX 交叉、共地、波特率相同；J-Link 连 SWDIO/SWCLK/GND。用 Ozone 打开 CMake 生成的带符号 ELF，设置目标型号和 SWD 连接后下载；在 main 和定时器回调打断点，观察变量/寄存器，并用串口终端确认启动消息。J-Link 连 SWDIO/SWCLK/GND。保存 CubeMX 配置、Ozone 下载/调试记录和串口输出作为最小移植证据。

CubeMX 负责初始化代码；VS Code 配 C/C++ 和 CMake Tools；安装 Arm GNU Toolchain、CMake、Ninja 和 STM32CubeProgrammer；ST-Link 用于烧录/单步调试。个人工具路径放本机配置，不要提交绝对路径。Ozone 的工程配置保存为本机 .jdebug 时，不要提交含本机绝对路径的个人设置；可提供不含个人路径的模板。当前仓库没有 .ioc、启动文件、链接脚本和 HAL 包，顶层 CMake 只构建核心，不能独自生成可烧录 ELF。

F103 的 CAN 引脚可能与其他功能复用；核对板卡原理图和 AFIO remap。F411 的 DMA stream/IRQ 必须重新由 CubeMX 配置。若目标 MCU 使用 FDCAN，应新增同接口 adapter。
