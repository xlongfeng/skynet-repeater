cmake_minimum_required(VERSION 2.8)

project(skynet-repeater ASM C)

set(CMAKE_ASM_FLAGS "-g -O0 -mcpu=cortex-m3 -mthumb -specs=nano.specs -x assembler-with-cpp")
set(CMAKE_C_FLAGS "-g -mcpu=cortex-m3 -std=gnu11 -O0 -Wall -ffunction-sections -fdata-sections -mthumb -specs=nano.specs")

add_definitions(-D"STM32F10X_MD" -D"USE_STDPERIPH_DRIVER")

include_directories(
	Libraries/CMSIS/Include
	Libraries/CMSIS/Device/ST/STM32F10x/Include
	Libraries/STM32F10x_StdPeriph_Driver/inc
	FreeRTOS/portable/GCC/ARM_CM3
	FreeRTOS/include
	src)

add_subdirectory(Libraries)
add_subdirectory(FreeRTOS)
add_subdirectory(src)