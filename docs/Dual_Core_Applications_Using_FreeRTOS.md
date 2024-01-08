# Dual-Core Application on STM32H745 Using FreeRTOS

## Overview
This document explains the implementation of an asymmetric multiprocessing (AMP) system on the STM32H745 MCU, which features dual cores: Cortex-M7 (CM7) and Cortex-M4 (CM4). The application utilizes FreeRTOS for task management and inter-core communication.

## System Initialization
The CM7 core starts first and performs the initial system setup, including enabling caches and configuring the Memory Protection Unit (MPU).

```
/* MPU Configuration */
MPU_Config();

/* Enable the CPU Cache */
CPU_CACHE_Enable();

/* Wait for CM4 core to enter stop mode */
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
```

Both cores initialize the Hardware Abstraction Layer (HAL) and set up the system clock.
### CM7:
```
HAL_Init();
SystemClock_Config();
```
### CM4:
```
HAL_Init();
```

## Inter-Core Communication Setup
### Message Buffers Creation (CM7)
The CM7 core creates message buffers for sending data and control signals to the CM4 core.
```
/* Create message buffers */
xControlMessageBuffer = xMessageBufferCreateStatic(...);
for( x = 0; x < mbaNUMBER_OF_CORE_2_TASKS; x++ ) {
    xDataMessageBuffers[x] = xMessageBufferCreateStatic(...);
}
```
## Task Creation and Management
### CM7 Core Tasks
The CM7 core creates tasks for sending data and monitoring application health.

Data Sending Task (prvCore1Task):
```
xTaskCreate(prvCore1Task, "AMPCore1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
```
Health Check Task (prvCheckTask):
```
xTaskCreate(prvCheckTask, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL);
```
### CM4 Core Tasks
The CM4 core creates tasks to receive and process data from the CM7 core.

Data Receiving Tasks (prvCore2Tasks):
```
for( x = 0; x < mbaNUMBER_OF_CORE_2_TASKS; x++ ) {    
    xTaskCreate(prvCore2Tasks, "AMPCore2", configMINIMAL_STACK_SIZE, (void *) x, tskIDLE_PRIORITY + 1, NULL);
}
```

## Inter-Core Synchronization
### CM7 to CM4 Core Wake-Up
The CM7 core releases the CM4 core from stop mode using a hardware semaphore.
```
HAL_HSEM_Release(HSEM_ID_0, 0);
```

## Interrupt Handling for Communication (CM4)
The CM4 core uses interrupts to handle incoming data from the CM7 core.
```
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    prvCore2InterruptHandler();
}
```
## Conclusion
This application demonstrates the use of FreeRTOS in a dual-core STM32H745 MCU for task sharing and inter-core communication. The CM7 core manages initial setup and data-sending tasks, while the CM4 core handles data reception and processing.
