# FreeRTOS 任务划分建议

优先级数字越大越高；stackWords 是 StackType_t 个数，通常 384 words=1536 bytes。下列是可开始调试的建议，跑起来后用 high-water mark 调栈。Application/include/application/tasks.hpp 中保留同一份配置表。

|任务|优先级|周期/触发|栈(words)|通信|为什么|
|---|---:|---|---:|---|---|
|control|4|1 ms|384|Queue 接收最新 MPU6050 样本|控制时间紧，不被慢总线和日志阻塞|
|io|3|8 ms（125 Hz）|512|Queue + DMA 完成二值信号量|以 MPU6050 采样率读传感器，把可能超时的总线操作隔离出控制任务|
|monitor|1|1000 ms|256|EventGroup 健康标志|诊断低频运行，不抢控制时间|

`freertos_app.cpp` 实际创建三项任务、样本队列、事件队列、健康事件组和 DMA 完成二值信号量；`main.c` 在启动调度器前初始化设备。ISR 中只使用 FromISR API；DMA buffer 在完成回调前必须保持有效。共享数据优先消息队列，禁止在高优先级控制任务里打印。控制任务当前消费最新样本但没有加入具体控制算法，应用层可在此扩展。

F103 工程现已在 CMake 中手动集成 FreeRTOS Cortex-M3 GCC 内核和实际任务入口；CubeMX `.ioc` 文件本身仍未记录 FreeRTOS middleware。CAN1、I2C1、TIM2 的初始化也由 BSP 手写实现，尚未写回 `.ioc`；若用 CubeMX 重生成，需要保留 `FreeRTOSConfig.h`、CMake 源列表及 IRQ handler 交接修改。任务可以编译链接，需上板确认调度节拍、传感器响应和栈余量。
