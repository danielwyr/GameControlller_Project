################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/include/Components/st7789h2/st7789h2.c 

OBJS += \
./system/include/Components/st7789h2/st7789h2.o 

C_DEPS += \
./system/include/Components/st7789h2/st7789h2.d 


# Each subdirectory must supply rules for building sources it contributes
system/include/Components/st7789h2/%.o: ../system/include/Components/st7789h2/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DSTM32F769xx -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -I../system/include/BSP -I../system/include/Components -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f7xx" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


