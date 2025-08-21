add_library(port
    audio_driver.c
    ui_backend.c
)

target_include_directories(port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/submodules/portaudio/include
    ${CMAKE_SOURCE_DIR}/submodules/lvgl
)
