################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ws2812b/ws2812b_base.c 

OBJS += \
./ws2812b/ws2812b_base.o 

C_DEPS += \
./ws2812b/ws2812b_base.d 


# Each subdirectory must supply rules for building sources it contributes
ws2812b/%.o: ../ws2812b/%.c ws2812b/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F042x6 -c -I../Core/Inc -I/home/termy/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.3/Drivers/STM32F0xx_HAL_Driver/Inc -I/home/termy/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.3/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I/home/termy/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.3/Drivers/CMSIS/Device/ST/STM32F0xx/Include -I/home/termy/STM32Cube/Repository/STM32Cube_FW_F0_V1.11.3/Drivers/CMSIS/Include -I../ws2812b -I../test -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

