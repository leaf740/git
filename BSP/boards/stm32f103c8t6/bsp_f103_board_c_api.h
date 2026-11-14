#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int FW_Board_Init(void);
void FW_Board_TimerElapsed(void* timerHandle);
#ifdef __cplusplus
}
#endif
