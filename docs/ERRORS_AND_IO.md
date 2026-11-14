# 错误码、日志和传输方式

统一返回 fw::Result<T>/fw::Error。初始化失败、参数错误、超时、忙、总线错误、校验错误分别对应 NotInitialized、InvalidArgument、Timeout、Busy、Io、Checksum。每个调用点要检查结果；日志通过 fw::ILogger 注入输出，不能吞掉错误。HAL_TIMEOUT 映射 Timeout，HAL_BUSY 映射 Busy，HAL_ERROR 映射 Io。

|外设|方式|适合什么时候|
|---|---|---|
|UART/USART|阻塞/中断/DMA|启动时短消息用阻塞；短异步帧用中断；连续高吞吐数据用 DMA|
|CAN bxCAN|轮询/中断|初始化低频消息用轮询；周期控制帧用中断；邮箱满明确返回 Busy 或 Timeout|
|I2C|阻塞/DMA|配置和低频读数用阻塞；连续采样/较长数据用 DMA|

异步返回只代表提交成功，不代表传输完成；完成状态要由 HAL callback 通知队列、信号量或事件组。设备协议校验失败由具体 Module 返回 Checksum。I2C register helper 的寄存器读取先写寄存器再读数据；如设备要求 repeated-start，需在 BSP 另加对应 HAL 调用。
