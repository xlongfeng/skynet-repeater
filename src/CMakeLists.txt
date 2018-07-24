
set(LINK_SCRIPT "${PROJECT_SOURCE_DIR}/stm32_flash.ld")

set(EXEC_LINK_FLAGS "-lc -lm -Wl,--gc-sections -T${LINK_SCRIPT} -static -Wl,--start-group -Wl,--end-group -specs=nosys.specs -Wl,-cref -Wl,--defsym=malloc_getpagesize_P=0x1000")

add_executable(repeater
	startup_stm32f10x_md.s
	system_stm32f10x.c
	main.c
	syscalls.c
	tiny_printf.c
	fullduplexuart.c
	halfduplexuart.c
	)

target_link_libraries(repeater stdperiph freertos)

set_target_properties(
    repeater PROPERTIES
    SUFFIX ".elf"
    LINK_FLAGS "${EXEC_LINK_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/repeater.map"
    )

add_custom_command(
	TARGET repeater
	COMMAND "C:/Program Files (x86)/Atollic/TrueSTUDIO for STM32 9.0.1/ide/jre/bin/java"
	ARGS -jar "C:/Program Files (x86)/Atollic/TrueSTUDIO for STM32 9.0.1/Tools/arm-atollic-reports.jar" sizeinfo list repeater.elf
	)