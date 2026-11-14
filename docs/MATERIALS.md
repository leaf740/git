# 采购物资驱动登记

收到的物资清单：MPU6050、TJA1050、SG90 9g 舵机。当前固件按 F103C8T6 的默认复用脚分配如下；插线前核对开发板引脚排针和模块丝印。

|物资|模块/总线|建议 F103 引脚|配置|当前状态|
|---|---|---|---|---|
|MPU6050|Module/Mpu6050，I2C1|PB6=SCL、PB7=SDA|7-bit 地址默认 0x68（AD0 高时构造参数设 0x69）；WHO_AM_I 校验；±2g/±250 dps；125 Hz|驱动已编写，板级初始化/实测待完成|
|TJA1050|Module/Tja1050Link + BSP bxCAN|PA12=CAN_TX→TXD；RXD→PA11=CAN_RX|CAN1 500 kbit/s 建议值；标准/扩展帧|链路包装已编写，板级初始化/对端收发待完成|
|SG90 9g|Module/Sg90Servo，TIM PWM|PA0=TIM2_CH1|50 Hz，默认脉宽 1000–2000 us 可配置，初始化置中|模块已编写，TIM2 初始化/实测待完成|

## 接线和供电要点

- MPU6050 芯片 I2C 地址由 AD0 决定；WHO_AM_I 寄存器 0x75 默认返回 0x68。驱动在 WHO_AM_I 不匹配时返回 DeviceMismatch。测量结果保留原始有符号计数，同时提供 ±2g、±250 dps 的换算。批量数据使用 14-byte burst read。参考：[TDK/InvenSense register map](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map.pdf)。
- I2C 需要 SDA/SCL 上拉到 3.3 V；有些 MPU6050 breakout 已焊上拉，先检查板子，避免并太多或拉到 5 V。裸芯片与不同 breakout 的供电范围不同，以你手中模块原理图/丝印为准。
- TJA1050 是 CAN 物理层收发器，不会自己产生 CAN 帧。VCC 接 5 V，TXD/RXD 接 MCU CAN_TX/CAN_RX，S 引脚保持低电平为高速模式；CANH/CANL 接总线，整条总线两端各一个 120 Ω 终端，不能每个节点都额外并一个。真实收发至少还要另一 CAN 节点。PA11/PA12 在 F103C8T6 上是 5 V tolerant 引脚；仍需共地并检查实际 TJA1050 模块是否把 S/终端电阻引出。[NXP TJA1050 datasheet](https://www.nxp.com/docs/en/data-sheet/TJA1050.pdf), [ST STM32F103 datasheet](https://www.st.com/resource/en/datasheet/stm32f103r8.pdf)。
- SG90 电机不要由 MCU 3.3 V 引脚供电。按原厂规格用 4.8 V 伺服电源，伺服电源与 STM32 共地，信号线接 PA0。先让机械连杆脱开或舵盘置中，再测试端点；不同 SG90 仿品端点脉宽可能不同，驱动允许调整区间。[TowerPro SG90](https://towerpro.com.tw/product/sg90-analog/)。

具体供电模块/接插件未列在清单中，所以电源板、模块电平、CAN 总线对端与终端位置仍需按实物确认。
