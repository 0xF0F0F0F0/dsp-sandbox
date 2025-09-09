cmake_minimum_required(VERSION 3.22)
project(dsp_sandbox C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# External
add_subdirectory(submodules/lvgl)
add_subdirectory(submodules/CMSIS-DSP)
add_subdirectory(submodules/portaudio)

# Library sources
file(GLOB_RECURSE LIB_SOURCES "lib/**/*.c")

# Core
add_library(core
    src/main.c
    src/audio/audio.c
    src/ui/ui.c
    ${LIB_SOURCES}
)

target_include_directories(core PUBLIC
    src/audio
    src/ui
    lib/audio
    lib/dsp
    lib/sequencer
    lib/platform
    submodules/CMSIS-DSP/Include
    submodules/CMSIS-DSP/Include/dsp
)

target_include_directories(CMSISDSP PUBLIC
    submodules/CMSIS_5/CMSIS/Core/Include
)

target_link_libraries(core PUBLIC lvgl CMSISDSP)

# Platform
if(NOT DEFINED PLATFORM)
    message(FATAL_ERROR "PLATFORM not defined! Use -DPLATFORM=PC or STM32")
endif()

if(PLATFORM STREQUAL "PC")
    add_subdirectory(port/pc)
elseif(PLATFORM STREQUAL "STM32")
    if(NOT DEFINED STM32_CHIP)
        message(FATAL_ERROR "STM32_CHIP not set! Use -DSTM32_CHIP=...")
    endif()
    add_subdirectory(port/stm32)
else()
    message(FATAL_ERROR "Unknown PLATFORM: ${PLATFORM}")
endif()

# Core library
target_link_libraries(core PUBLIC port)

# Executable
add_executable(dsp_sandbox src/main.c)

# Link core and dependencies
target_link_libraries(dsp_sandbox PRIVATE core port portaudio)

# PC-specific: find SDL2 and link to the library that uses it
if(PLATFORM STREQUAL "PC")
    find_package(SDL2 REQUIRED)

    # Include SDL headers for both port library and executable
    target_include_directories(port PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(dsp_sandbox PRIVATE ${SDL2_INCLUDE_DIRS})

    # Link SDL2 to port library (so all SDL calls are resolved)
    target_link_libraries(port PRIVATE ${SDL2_LIBRARIES})
endif()
