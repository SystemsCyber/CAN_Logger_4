################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c 

OBJS += \
./Drivers/BSP/Components/mfxstm32l152.o 

C_DEPS += \
./Drivers/BSP/Components/mfxstm32l152.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mfxstm32l152.o: C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/Drivers/BSP/Components/mfxstm32l152/mfxstm32l152.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32H745xx -DCORE_CM4 -DUSE_HAL_DRIVER -DUSE_STM32H745I_DISCO -DUSE_IOEXPANDER -c -I../../../CM4/Inc -I../../../Common/Inc -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM32H745I-DISCO -I../../../Drivers/BSP/Components/Common -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Utilities/Fonts -I../../../Utilities/CPU -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/mfxstm32l152.cyclo ./Drivers/BSP/Components/mfxstm32l152.d ./Drivers/BSP/Components/mfxstm32l152.o ./Drivers/BSP/Components/mfxstm32l152.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

