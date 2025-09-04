add_library(port
    audio_driver.c
    ui_backend.c
    audio_callback.c
)

target_include_directories(port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/audio
    ${CMAKE_SOURCE_DIR}/dsp
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/submodules/portaudio/include
    ${CMAKE_SOURCE_DIR}/submodules/lvgl
    ${CMAKE_SOURCE_DIR}/submodules/CMSIS-DSP/Include
)
