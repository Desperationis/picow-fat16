# How to run this commit

`sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib`

`git submodule update --init`
cd pico-sdk
`git submodule update --init`

`mkdir build && cd build`
`cmake ..`
`make`

`minicom -b 115200 -o -D /dev/ttyACM0`
