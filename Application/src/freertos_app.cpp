#include "application/freertos_api.h"
#include "application/tasks.hpp"
#include "framework/error.hpp"
#include "framework/log.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"

namespace {
struct IoEvent { uint16_t id; uint32_t value; };
QueueHandle_t ioQueue = nullptr;
EventGroupHandle_t healthEvents = nullptr;
SemaphoreHandle_t dmaDone = nullptr;
constexpr EventBits_t kControlAlive = 1U << 0;
constexpr EventBits_t kIoAlive = 1U << 1;
void controlTask(void*) {
    TickType_t wake = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(1));
        xEventGroupSetBits(healthEvents, kControlAlive);
    }
}
void ioTask(void*) {
    TickType_t wake = xTaskGetTickCount();
    IoEvent event{};
    for (;;) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(2));
        if (xQueueReceive(ioQueue, &event, 0) == pdPASS) {
            (void)event; // Dispatch to the registered Module device handler here.
            xEventGroupSetBits(healthEvents, kIoAlive);
        }
        if (xSemaphoreTake(dmaDone, 0) == pdTRUE) xEventGroupSetBits(healthEvents, kIoAlive);
    }
}
void monitorTask(void*) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        const EventBits_t bits = xEventGroupClearBits(healthEvents, kControlAlive | kIoAlive);
        if ((bits & kControlAlive) == 0U) fw::log(fw::LogLevel::Error, "monitor", "control task missed heartbeat");
        else fw::log(fw::LogLevel::Info, "monitor", "system alive");
    }
}
}

extern "C" int FW_App_StartScheduler(void) {
    ioQueue = xQueueCreate(8, sizeof(IoEvent));
    healthEvents = xEventGroupCreate();
    dmaDone = xSemaphoreCreateBinary();
    if (ioQueue == nullptr || healthEvents == nullptr || dmaDone == nullptr) return static_cast<int>(fw::Error::QueueFull);
    TaskFunction_t entries[] = {controlTask, ioTask, monitorTask};
    for (std::size_t i = 0; i < (sizeof(fw::app::kTasks) / sizeof(fw::app::kTasks[0])); ++i) {
        const auto& spec = fw::app::kTasks[i];
        if (xTaskCreate(entries[i], spec.name, spec.stackWords, nullptr, spec.priority, nullptr) != pdPASS)
            return static_cast<int>(fw::Error::Internal);
    }
    vTaskStartScheduler();
    return static_cast<int>(fw::Error::Internal); // Scheduler returns only if task memory setup failed.
}
extern "C" int FW_App_PostIoEvent(uint16_t id, uint32_t value) {
    if (ioQueue == nullptr) return static_cast<int>(fw::Error::NotInitialized);
    const IoEvent event{id, value};
    return xQueueSend(ioQueue, &event, 0) == pdPASS ? 0 : static_cast<int>(fw::Error::QueueFull);
}
extern "C" int FW_App_PostIoEventFromISR(uint16_t id, uint32_t value) {
    if (ioQueue == nullptr) return static_cast<int>(fw::Error::NotInitialized);
    const IoEvent event{id, value};
    BaseType_t woken = pdFALSE;
    const BaseType_t ok = xQueueSendFromISR(ioQueue, &event, &woken);
    portYIELD_FROM_ISR(woken);
    return ok == pdPASS ? 0 : static_cast<int>(fw::Error::QueueFull);
}
extern "C" void FW_App_DmaTransferCompleteFromISR(void) {
    if (dmaDone == nullptr) return;
    BaseType_t woken = pdFALSE;
    xSemaphoreGiveFromISR(dmaDone, &woken);
    portYIELD_FROM_ISR(woken);
}
extern "C" void vApplicationMallocFailedHook(void) { fw::log(fw::LogLevel::Error, "rtos", "heap exhausted"); taskDISABLE_INTERRUPTS(); for (;;) {} }
extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char* name) { fw::log(fw::LogLevel::Error, "rtos", name); taskDISABLE_INTERRUPTS(); for (;;) {} }
