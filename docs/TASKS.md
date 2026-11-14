# FreeRTOS 任务划分建议

优先级数字越大越高；stackWords 是 StackType_t 个数，通常 384 words=1536 bytes。下列是可开始调试的建议，跑起来后用 high-water mark 调栈。Application/include/application/tasks.hpp 中保留同一份配置表。

|任务|优先级|周期/触发|栈(words)|通信|为什么|
|---|---:|---|---:|---|---|
|control|4|1 ms|384|Queue 接收控制命令|控制时间紧，不被慢总线和日志阻塞|
|io|3|队列事件/2 ms|384|Queue + DMA 完成二值信号量|把可能超时的总线操作隔离出控制任务|
|monitor|1|1000 ms|256|EventGroup 健康标志|诊断低频运行，不抢控制时间|

任务表还需在 CubeMX 的 FreeRTOS 配置里真正创建任务和队列，框架骨架没有假装任务已经启动。ISR 中只使用 FromISR API；DMA buffer 在完成回调前必须保持有效。共享数据优先消息队列，禁止在高优先级控制任务里打印。

现有 F103 .ioc 尚未启用 FreeRTOS，任务配置目前是设计表，尚未创建/运行。等 CubeMX 打开 FreeRTOS middleware 后，按上表把任务与队列落到具体工程。
