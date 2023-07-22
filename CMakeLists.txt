cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} ${CMAKE_SOURCE_DIR}/pico-sdk) # For cmake files
set(PICO_BOARD pico_w) # For pico-sdk
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For nvim

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(picow-remote)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of the project
add_executable(main
    main.cpp
)

target_link_libraries(main pico_stdlib)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# Generate U2F file
pico_add_extra_outputs(main)

