################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32f7xx.c 

S_UPPER_SRCS += \
../system/src/cmsis/startup_stm32f769xx.S 

OBJS += \
./system/src/cmsis/startup_stm32f769xx.o \
./system/src/cmsis/system_stm32f7xx.o 

S_UPPER_DEPS += \
./system/src/cmsis/startup_stm32f769xx.d 

C_DEPS += \
./system/src/cmsis/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -x assembler-with-cpp -DDEBUG -DSTM32F769xx -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -I"../include" -I../system/include/BSP -I../system/include/Components -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f7xx" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DSTM32F769xx -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -I../system/include/BSP -I../system/include/Components -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f7xx" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


