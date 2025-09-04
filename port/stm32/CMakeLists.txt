add_library(port
    audio_driver.c
    ui_backend.c
    audio_callback.c
)

target_include_directories(port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/chip
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/dsp
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/submodules/CMSIS-DSP/Include
    ${CMAKE_SOURCE_DIR}/submodules/lvgl
)

# Add chip-specific source
set(CHIP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/chip/${STM32_CHIP}.c")
if(EXISTS ${CHIP_SRC})
    target_sources(port PRIVATE ${CHIP_SRC})
endif()

target_compile_definitions(port PUBLIC ${STM32_CHIP})
