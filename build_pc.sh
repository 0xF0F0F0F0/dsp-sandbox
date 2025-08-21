#!/bin/bash

cmake -B build/pc -S . -DPLATFORM=PC
cmake --build build/pc
