# the name of the target operating system
set (CMAKE_SYSTEM_NAME Generic)

set (CMAKE_MAKE_PROGRAM "C:/Program Files (x86)/Atollic/TrueSTUDIO for STM32 9.0.1/Tools/make.exe")

# which compilers to use for ASM and C
set (CMAKE_ASM_COMPILER "C:/Program Files (x86)/Atollic/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-gcc.exe" )
set (CMAKE_C_COMPILER "C:/Program Files (x86)/Atollic/TrueSTUDIO for STM32 9.0.1/ARMTools/bin/arm-atollic-eabi-gcc.exe" )

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)