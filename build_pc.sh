#!/bin/bash

cmake -B build/pc -S . -DPLATFORM=PC -DCMAKE_BUILD_TYPE=Debug
cmake --build build/pc
