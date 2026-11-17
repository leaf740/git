# 错误码、日志和传输方式

## `error.hpp`：怎么表示成功或失败

`BSP/include/framework/error.hpp` 定义统一错误枚举 `fw::Error` 和结果包装 `fw::Result<T>`。大白话说：函数除了告诉调用者“做完了”，还会把失败原因一起带回来。不要只看串口有没有打印，也不要忽略返回值。

当前错误码数值如下。数值由枚举顺序决定，若调整顺序或插入新项，要同步更新本文档；对外协议不应直接依赖这些数值。

|数值|错误码|含义|常见场景|
|---:|---|---|---|
|0|`Ok`|成功|调用完成|
|1|`NotInitialized`|尚未初始化|还没启动设备或 logger|
|2|`InvalidArgument`|参数错误|空指针、地址或数据长度超范围|
|3|`Timeout`|等待超时|UART/I2C/CAN 等操作超过设定时间|
|4|`Busy`|资源忙|外设正在传输或 CAN 邮箱不可用|
|5|`Io`|硬件或总线错误|HAL 返回一般错误|
|6|`Checksum`|数据校验失败|模块协议校验失败；当前 MPU6050 原始读数没有校验和，因此不会报告此码|
|7|`Unsupported`|不支持该操作|例如请求未实现的传输方式|
|8|`QueueFull`|队列满|FreeRTOS 队列暂时无法接收新消息|
|9|`DeviceMismatch`|设备不匹配|MPU6050 的 WHO_AM_I 值不符合预期|
|10|`Internal`|内部错误|任务创建失败等框架内部问题|

调用者用 `if (!result)` 判断失败，再用 `result.error()` 取得错误原因：

```cpp
auto result = imu.initialize();
if (!result) {
    fw::log(fw::LogLevel::Error, "mpu6050", "initialize failed");
    return result; // 把原错误交给上一层处理，不能假装成功
}
```

`Result<void>` 用于不返回数据的函数；`Result<T>` 还可以在成功时用 `value()` 取回数据。C 接口需要整数时，当前代码把 `fw::Error` 转成 `int` 返回。

## `log.hpp`：怎么记下发生了什么

`BSP/include/framework/log.hpp` 定义日志等级、日志接口和 `fw::log()`：

- `Debug`：调试细节。
- `Info`：正常启动或状态信息。
- `Warn`：有异常但程序还能继续。
- `Error`：初始化、通信等操作失败。

`ILogger` 是输出口；BSP 在启动时用 `fw::setLogger(&logger)` 注册真正的输出设备。业务代码通过 `fw::log(level, tag, message)` 记录信息，不直接操作 UART 或 HAL。例如：

```cpp
fw::log(fw::LogLevel::Info, "app", "framework ready");
fw::log(fw::LogLevel::Error, "i2c", "MPU6050 read failed");
```

当前 F103 的 UART logger 输出 `[tag] message`。它暂时没有把日志等级或错误码数字一起打印；`fw::log()` 自身会返回 `Result<void>`，如果没有注册 logger 会返回 `NotInitialized`，参数为空会返回 `InvalidArgument`。调用者也应检查日志调用结果，避免日志后端未就绪时静默失败。

## HAL 错误如何变成统一错误码

HAL 状态由 BSP 转换：`HAL_TIMEOUT` → `Timeout`，`HAL_BUSY` → `Busy`，其他 HAL 错误 → `Io`。模块再把 BSP 返回值原样向上传递。应用层负责检查结果，选择重试、记录日志或停止初始化。

|外设|方式|适合什么时候|
|---|---|---|
|UART/USART|阻塞/中断/DMA|启动时短消息用阻塞；短异步帧用中断；连续高吞吐数据用 DMA|
|CAN bxCAN|轮询/中断|初始化低频消息用轮询；周期控制帧用中断；邮箱满明确返回 Busy 或 Timeout|
|I2C|阻塞/DMA|配置和低频读数用阻塞；连续采样/较长数据用 DMA|

异步调用返回成功只代表传输已提交，不代表数据已经发完；完成状态要由 HAL callback 通知队列、信号量或事件组。I2C register helper 的寄存器读取先写寄存器再读数据；如设备要求 repeated-start，需在 BSP 另加对应 HAL 调用。

|外设|方式|适合什么时候|
|---|---|---|
|UART/USART|阻塞/中断/DMA|启动时短消息用阻塞；短异步帧用中断；连续高吞吐数据用 DMA|
|CAN bxCAN|轮询/中断|初始化低频消息用轮询；周期控制帧用中断；邮箱满明确返回 Busy 或 Timeout|
|I2C|阻塞/DMA|配置和低频读数用阻塞；连续采样/较长数据用 DMA|

异步返回只代表提交成功，不代表传输完成；完成状态要由 HAL callback 通知队列、信号量或事件组。设备协议校验失败由具体 Module 返回 Checksum。I2C register helper 的寄存器读取先写寄存器再读数据；如设备要求 repeated-start，需在 BSP 另加对应 HAL 调用。
