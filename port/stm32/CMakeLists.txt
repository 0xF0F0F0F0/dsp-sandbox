add_library(port
    audio_driver.c
    ui_backend.c
)

target_include_directories(port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/chips
)

# Add chip-specific source
set(CHIP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/chips/${STM32_CHIP}.c")
if(EXISTS ${CHIP_SRC})
    target_sources(port PRIVATE ${CHIP_SRC})
endif()

target_compile_definitions(port PUBLIC ${STM32_CHIP})
