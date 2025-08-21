#!/bin/bash

cmake -B build/stm32 -S . -DPLATFORM=STM32 \
      -DCMAKE_TOOLCHAIN_FILE=cmake/stm32_toolchain.cmake \
      -DSTM32_CHIP=stm32f429
cmake --build build/stm32
