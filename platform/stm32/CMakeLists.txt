add_library(platform_lib
    audio_driver.c
    ui_backend.c
)

target_include_directories(platform_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/chips
)

# Add chip-specific source
set(CHIP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/chips/${STM32_CHIP}.c")
if(EXISTS ${CHIP_SRC})
    target_sources(platform_lib PRIVATE ${CHIP_SRC})
endif()

target_compile_definitions(platform_lib PUBLIC ${STM32_CHIP})
