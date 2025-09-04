# STM32 Toolchain file placeholder
# This is a stub toolchain file for STM32 cross-compilation

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain paths - these would need to be set to actual ARM toolchain
# set(CMAKE_C_COMPILER arm-none-eabi-gcc)
# set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
# set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# For now, use regular GCC to test compilation (will fail at link stage)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# STM32F429 specific flags would go here
# set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# Prevent CMake from testing compiler (which would fail without real ARM toolchain)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)