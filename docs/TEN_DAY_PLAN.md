# 十天实施计划与 Git commit

Git 时间必须反映真实工作。本次提交只记录当前建好的框架骨架；之后按天完成任务并提交，不回填虚假日期。

|天|工作与验收|建议提交标题|
|---:|---|---|
|1|CubeMX 建 F103C8T6，时钟/SWD/USART/GPIO|chore(cubemx): add stm32f103c8t6 base project|
|2|CMake + Ninja + Arm GCC 构建 F103 固件|build: integrate cmake ninja and arm gcc|
|3|UART BSP 和日志，阻塞/中断/DMA|feat(bsp): add uart abstraction and logging|
|4|CAN BSP、滤波器和收发中断|feat(bsp): add can bus abstraction|
|5|I2C BSP、器件 ID 和错误处理|feat(bsp): add i2c bus abstraction|
|6|按真实采购单封装 Module|feat(module): wrap purchased peripherals|
|7|CubeMX 配 FreeRTOS 任务和队列|feat(app): add task layout and queues|
|8|F103 用 J-Link/Ozone 下载并验证串口、定时器和总线|test(board): validate f103 peripherals|
|9|F411CEU6 用 CubeMX 新工程并用 Ozone 跑 UART+定时器|feat(port): bring up stm32f411ceu6|
|10|整理接线、构建和调试指南并复现|docs: finish bring-up and migration guide|

每天提交：git status → git diff → git add 相关文件 → git commit -m "标题"。提交前检查暂存差异，排除个人绝对路径和临时文件。

## 本次仓库进度

- 第 1 天：已生成并核对 F103C8T6 CubeMX 工程配置。
- 第 2 天：框架 C++17 源文件已接进工程；5 个新增 C++ 单元和 CubeMX main.c 编译通过，随后 Ninja 全量构建/链接 28 步成功。旧 build cache 指向搬动前路径，已使用新的 build/f103-debug。
- 第 3 天：USART1 BSP 已接入启动日志；Ozone 项目文件已创建并打开；尚待目标板下载和串口实测。
- F103 的 CAN/I2C HAL 和 FreeRTOS 内核已编译集成；外设初始化、物资驱动与验证待补。采购清单已收到（MPU6050/TJA1050/SG90）；F411 工程与硬件验证待补。
