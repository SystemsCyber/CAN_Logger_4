# FDCAN Communication on STM32H745 (CM7 Core)

## Overview
This document provides an overview of the implementation of FDCAN communication using the STM32H745's Cortex-M7 (CM7) core. The Flexible Data-rate Controller Area Network (FDCAN) is a high-performance communication protocol used in automotive and industrial applications for efficient data exchange.

## System Initialization
Cache and MPU Configuration
Initially, the CM7 core configures the cache and Memory Protection Unit (MPU) for efficient and safe operation.
```
/* Enable the CPU Cache */
CPU_CACHE_Enable();

/* Configure the MPU attributes */
MPU_Config();
```
## System Clock and HAL Initialization
The system clock is configured to the required frequency, and the HAL (Hardware Abstraction Layer) is initialized to provide low-level hardware interfacing.
```
/* Configure the system clock to 400 MHz */
SystemClock_Config();

/* STM32H7xx HAL library initialization */
HAL_Init();
```

## FDCAN Peripheral Configuration
### FDCAN Initialization
The FDCAN peripheral is initialized with specific settings for bit timing, frame format, and mode of operation. It involves setting up various parameters like prescalers, time segments, and message RAM.
```
/* Configure the FDCAN peripheral */
FDCAN_Config();

static void FDCAN_Config(void)
{
  /* FDCAN Configuration Code */
}
```
### FDCAN Filter Configuration
Filters are configured to specify which messages should be received by the FDCAN peripheral. This step determines the filtering of incoming messages based on identifiers.
```
sFilterConfig.IdType = FDCAN_STANDARD_ID;
sFilterConfig.FilterIndex = 0;
/* Other filter configurations */
HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig);
```
### FDCAN Start and Notification Activation
After configuration, the FDCAN module is started, and notifications for specific events like receiving a new message are activated.
```
if (HAL_FDCAN_Start(&hfdcan) != HAL_OK)
{
  /* Start Error */
  Error_Handler();
}

if (HAL_FDCAN_ActivateNotification(&hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
{
  /* Notification Error */
  Error_Handler();
}
```

## Message Transmission and Reception
### Transmitting Messages
When a specific event (like a button press) occurs, a message is formatted and added to the FDCAN transmit FIFO queue.
```
/* Set the data to be transmitted */
TxData[0] = ubKeyNumber;
/* Other transmission setup */
HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, TxData);
```
### Receiving Messages
When a new message is received in the FDCAN's RX FIFO, a callback function is invoked to process the message.
```
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  /* Message processing code */
}
```
