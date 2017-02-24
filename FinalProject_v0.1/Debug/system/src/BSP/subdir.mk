################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/BSP/stm32f769i_discovery.c \
../system/src/BSP/stm32f769i_discovery_audio.c \
../system/src/BSP/stm32f769i_discovery_eeprom.c \
../system/src/BSP/stm32f769i_discovery_lcd.c \
../system/src/BSP/stm32f769i_discovery_qspi.c \
../system/src/BSP/stm32f769i_discovery_sd.c \
../system/src/BSP/stm32f769i_discovery_sdram.c \
../system/src/BSP/stm32f769i_discovery_ts.c 

OBJS += \
./system/src/BSP/stm32f769i_discovery.o \
./system/src/BSP/stm32f769i_discovery_audio.o \
./system/src/BSP/stm32f769i_discovery_eeprom.o \
./system/src/BSP/stm32f769i_discovery_lcd.o \
./system/src/BSP/stm32f769i_discovery_qspi.o \
./system/src/BSP/stm32f769i_discovery_sd.o \
./system/src/BSP/stm32f769i_discovery_sdram.o \
./system/src/BSP/stm32f769i_discovery_ts.o 

C_DEPS += \
./system/src/BSP/stm32f769i_discovery.d \
./system/src/BSP/stm32f769i_discovery_audio.d \
./system/src/BSP/stm32f769i_discovery_eeprom.d \
./system/src/BSP/stm32f769i_discovery_lcd.d \
./system/src/BSP/stm32f769i_discovery_qspi.d \
./system/src/BSP/stm32f769i_discovery_sd.d \
./system/src/BSP/stm32f769i_discovery_sdram.d \
./system/src/BSP/stm32f769i_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/BSP/%.o: ../system/src/BSP/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DSTM32F769xx -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -I../system/include/BSP -I../system/include/Components -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f7xx" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


