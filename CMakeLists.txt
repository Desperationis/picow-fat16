cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} ${CMAKE_SOURCE_DIR}/pico-sdk) # For cmake files
set(PICO_BOARD pico_w) # For pico-sdk
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For nvim

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(picow-remote C CXX)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of the project
add_executable(main
    src/main.cpp
	src/usb_descriptors.cpp
	src/msc_disk.cpp
)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/)
target_link_libraries(main PUBLIC pico_stdlib tinyusb_device tinyusb_board pico_cyw43_arch_none)

pico_enable_stdio_usb(main 0)
pico_enable_stdio_uart(main 0)

# Generate U2F file
pico_add_extra_outputs(main)

