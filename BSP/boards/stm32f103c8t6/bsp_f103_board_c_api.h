#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int FW_Board_Init(void);
void FW_Board_TimerElapsed(void* timerHandle);
int FW_Board_PeripheralsInit(void);
void* FW_Board_I2cBus(void);
void* FW_Board_CanBus(void);
void* FW_Board_ServoPwm(void);
#ifdef __cplusplus
}
#endif
