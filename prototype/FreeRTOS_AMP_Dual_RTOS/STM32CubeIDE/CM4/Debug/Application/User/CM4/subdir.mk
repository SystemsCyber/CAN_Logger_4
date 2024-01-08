################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/CM4/Src/main.c \
C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/CM4/Src/stm32h7xx_it.c \
../Application/User/CM4/syscalls.c \
../Application/User/CM4/sysmem.c 

OBJS += \
./Application/User/CM4/main.o \
./Application/User/CM4/stm32h7xx_it.o \
./Application/User/CM4/syscalls.o \
./Application/User/CM4/sysmem.o 

C_DEPS += \
./Application/User/CM4/main.d \
./Application/User/CM4/stm32h7xx_it.d \
./Application/User/CM4/syscalls.d \
./Application/User/CM4/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/CM4/main.o: C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/CM4/Src/main.c Application/User/CM4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32H745xx -DCORE_CM4 -DUSE_HAL_DRIVER -DUSE_STM32H745I_DISCO -DUSE_IOEXPANDER -c -I../../../CM4/Inc -I../../../Common/Inc -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM32H745I-DISCO -I../../../Drivers/BSP/Components/Common -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Utilities/Fonts -I../../../Utilities/CPU -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/CM4/stm32h7xx_it.o: C:/Users/chatt/STM32Cube/Example/FreeRTOS_AMP_Dual_RTOS/CM4/Src/stm32h7xx_it.c Application/User/CM4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32H745xx -DCORE_CM4 -DUSE_HAL_DRIVER -DUSE_STM32H745I_DISCO -DUSE_IOEXPANDER -c -I../../../CM4/Inc -I../../../Common/Inc -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM32H745I-DISCO -I../../../Drivers/BSP/Components/Common -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Utilities/Fonts -I../../../Utilities/CPU -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/CM4/%.o Application/User/CM4/%.su Application/User/CM4/%.cyclo: ../Application/User/CM4/%.c Application/User/CM4/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32H745xx -DCORE_CM4 -DUSE_HAL_DRIVER -DUSE_STM32H745I_DISCO -DUSE_IOEXPANDER -c -I../../../CM4/Inc -I../../../Common/Inc -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM32H745I-DISCO -I../../../Drivers/BSP/Components/Common -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Utilities/Fonts -I../../../Utilities/CPU -O0 -ffunction-sections -fdata-sections -Wall -Wno-unused-variable -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-CM4

clean-Application-2f-User-2f-CM4:
	-$(RM) ./Application/User/CM4/main.cyclo ./Application/User/CM4/main.d ./Application/User/CM4/main.o ./Application/User/CM4/main.su ./Application/User/CM4/stm32h7xx_it.cyclo ./Application/User/CM4/stm32h7xx_it.d ./Application/User/CM4/stm32h7xx_it.o ./Application/User/CM4/stm32h7xx_it.su ./Application/User/CM4/syscalls.cyclo ./Application/User/CM4/syscalls.d ./Application/User/CM4/syscalls.o ./Application/User/CM4/syscalls.su ./Application/User/CM4/sysmem.cyclo ./Application/User/CM4/sysmem.d ./Application/User/CM4/sysmem.o ./Application/User/CM4/sysmem.su

.PHONY: clean-Application-2f-User-2f-CM4

