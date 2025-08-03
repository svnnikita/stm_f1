#================================================================================
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE cmake/arm-none-eabi-gcc.cmake)


#================================================================================
project(stm_f1 LANGUAGES CXX C ASM)
set(DEVICE STM32F103C8T6)


#================================================================================

add_subdirectory(src)   

add_subdirectory(libs)

add_subdirectory(flash_n_debug)

add_subdirectory(doc)
