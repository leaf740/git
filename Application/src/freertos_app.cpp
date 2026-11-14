#include "application/freertos_api.h"
#include "application/robot.hpp"
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
QueueHandle_t ioQueue=nullptr, sampleQueue=nullptr;
EventGroupHandle_t healthEvents=nullptr;
SemaphoreHandle_t dmaDone=nullptr;
constexpr EventBits_t kControlAlive=1U, kIoAlive=2U, kImuFault=4U;
void controlTask(void*) { TickType_t wake=xTaskGetTickCount(); fw::module::Mpu6050Sample sample{}; for(;;){vTaskDelayUntil(&wake,pdMS_TO_TICKS(1)); if(xQueueReceive(sampleQueue,&sample,0)==pdPASS){(void)sample;} xEventGroupSetBits(healthEvents,kControlAlive);} }
void ioTask(void*) { TickType_t wake=xTaskGetTickCount(); IoEvent event{}; for(;;){vTaskDelayUntil(&wake,pdMS_TO_TICKS(8)); while(xQueueReceive(ioQueue,&event,0)==pdPASS){(void)event;} auto sample=fw::app::readImu(); if(sample){xQueueOverwrite(sampleQueue,&sample.value());xEventGroupSetBits(healthEvents,kIoAlive);xEventGroupClearBits(healthEvents,kImuFault);}else{xEventGroupSetBits(healthEvents,kImuFault);} if(xSemaphoreTake(dmaDone,0)==pdTRUE)xEventGroupSetBits(healthEvents,kIoAlive);} }
void monitorTask(void*) { for(;;){vTaskDelay(pdMS_TO_TICKS(1000));const EventBits_t bits=xEventGroupClearBits(healthEvents,kControlAlive|kIoAlive);if(!(bits&kControlAlive))fw::log(fw::LogLevel::Error,"monitor","control task missed heartbeat");if(!(bits&kIoAlive))fw::log(fw::LogLevel::Error,"monitor","I/O task missed heartbeat");if(xEventGroupGetBits(healthEvents)&kImuFault)fw::log(fw::LogLevel::Error,"monitor","MPU6050 communication error");} }
void dmaCompleteHook(void*) { FW_App_DmaTransferCompleteFromISR(); }
}
extern "C" int FW_App_StartScheduler(void) {
 ioQueue=xQueueCreate(8,sizeof(IoEvent));sampleQueue=xQueueCreate(1,sizeof(fw::module::Mpu6050Sample));healthEvents=xEventGroupCreate();dmaDone=xSemaphoreCreateBinary();
 if(!ioQueue||!sampleQueue||!healthEvents||!dmaDone)return static_cast<int>(fw::Error::QueueFull);
 fw::bsp::setAsyncCompletionHook(dmaCompleteHook,nullptr);
 TaskFunction_t entries[]={controlTask,ioTask,monitorTask};
 for(std::size_t i=0;i<sizeof(fw::app::kTasks)/sizeof(fw::app::kTasks[0]);++i){const auto&s=fw::app::kTasks[i];if(xTaskCreate(entries[i],s.name,s.stackWords,nullptr,s.priority,nullptr)!=pdPASS)return static_cast<int>(fw::Error::Internal);}
 vTaskStartScheduler();return static_cast<int>(fw::Error::Internal);
}
extern "C" int FW_App_PostIoEvent(uint16_t id,uint32_t value){if(!ioQueue)return static_cast<int>(fw::Error::NotInitialized);IoEvent e{id,value};return xQueueSend(ioQueue,&e,0)==pdPASS?0:static_cast<int>(fw::Error::QueueFull);}
extern "C" int FW_App_PostIoEventFromISR(uint16_t id,uint32_t value){if(!ioQueue)return static_cast<int>(fw::Error::NotInitialized);IoEvent e{id,value};BaseType_t w=pdFALSE;const BaseType_t ok=xQueueSendFromISR(ioQueue,&e,&w);portYIELD_FROM_ISR(w);return ok==pdPASS?0:static_cast<int>(fw::Error::QueueFull);}
extern "C" void FW_App_DmaTransferCompleteFromISR(void){if(!dmaDone)return;BaseType_t w=pdFALSE;xSemaphoreGiveFromISR(dmaDone,&w);portYIELD_FROM_ISR(w);}
extern "C" void vApplicationMallocFailedHook(void){fw::log(fw::LogLevel::Error,"rtos","heap exhausted");taskDISABLE_INTERRUPTS();for(;;){}}
extern "C" void vApplicationStackOverflowHook(TaskHandle_t,char*name){fw::log(fw::LogLevel::Error,"rtos",name);taskDISABLE_INTERRUPTS();for(;;){}}
