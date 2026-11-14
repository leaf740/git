# STM32 C++17 框架：F103C8T6 到 F411CEU6

三层：BSP 封装 UART/USART、bxCAN、I2C、定时器；Module 封装器件协议；Application 写任务和业务。架构参考湖南大学跃鹿战队 basic_framework 的分层思想，本项目代码独立实现。CubeMX 生成芯片初始化，HAL 只允许出现在 BSP。

大白话：CubeMX 负责配置芯片和引脚，BSP 是统一插座，Module 是设备说明书，Application 决定做什么。迁移 F411 时用 CubeMX 重新生成时钟/引脚/DMA/IRQ，再复用模块与业务层。

VS Code、CMake、Ninja、Arm GCC、J-Link + Ozone 工作流见 docs/PORTING.md。FreeRTOS 任务见 docs/TASKS.md，十天步骤见 docs/TEN_DAY_PLAN.md，采购器件登记见 docs/MATERIALS.md。当前没有 .ioc、接线清单、采购型号或 Cube HAL，因此不是可直接烧录的固件。

参考：https://github.com/HNUYueLuRM/basic_framework

## Commit 约定

每日完成一项后按 docs/TEN_DAY_PLAN.md 建立提交；当前实现的接口骨架单独提交，所有提交使用实际创建时间。

当前已接入的 CubeMX 工程和 USART1/TIM4 验收步骤见 [F103C8T6 工程记录](docs/BOARD_F103C8T6.md)。
