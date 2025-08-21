cmake_minimum_required(VERSION 3.22)
project(dsp_sandbox C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# External
add_subdirectory(submodules/lvgl)
add_subdirectory(submodules/CMSIS-DSP)
add_subdirectory(submodules/portaudio)

# Core
add_library(core
    src/main.c
    src/audio/audio.c
    src/ui/ui.c
    dsp/dsp.c
)

target_include_directories(core PUBLIC
    src/audio
    src/ui
    dsp
    submodules/lvgl
    submodules/CMSIS-DSP
)

# Platform
if(NOT DEFINED PLATFORM)
    message(FATAL_ERROR "PLATFORM not defined! Use -DPLATFORM=PC or STM32")
endif()

if(PLATFORM STREQUAL "PC")
    add_subdirectory(platform/pc)
elseif(PLATFORM STREQUAL "STM32")
    if(NOT DEFINED STM32_CHIP)
        message(FATAL_ERROR "STM32_CHIP not set! Use -DSTM32_CHIP=...")
    endif()
    add_subdirectory(platform/stm32)
else()
    message(FATAL_ERROR "Unknown PLATFORM: ${PLATFORM}")
endif()

# Executable
add_executable(dsp_sandbox src/main.c)
target_link_libraries(dsp_sandbox PRIVATE core platform_lib portaudio lvgl CMSISDSP)
