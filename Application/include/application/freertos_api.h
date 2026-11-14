#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
int FW_App_StartScheduler(void);
int FW_App_PostIoEvent(uint16_t eventId, uint32_t value);
int FW_App_PostIoEventFromISR(uint16_t eventId, uint32_t value);
void FW_App_DmaTransferCompleteFromISR(void);
#ifdef __cplusplus
}
#endif
