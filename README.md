# picow-fat16
Here is a complete working version of FAT16 on the USB side. Think of this project as a driver to **fool** the operating system to believe this is a USB you can put data onto. If you've wanted to hide secret messages on the pico or use this project as a base for other projects, be my guest. 

## How to Build and Install to Pico W
First, install the cross-compiler `sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib`. Then run `git submodule update --init`, `cd pico-sdk`, and another `git submodule update --init`. Then go back to the root directory and run these commands or `compile.bash`:

    mkdir build && cd build
    cmake ..
    make

## How to Build and Install to Regular Pico
TODO

## Max Storage Size
While the pseudo-USB reports it is 128mb big, in reality you can only write about 1mb to the pico in total. This is because how your computer determines if a FAT filesystem is FAT12, FAT16, or FAT32 is determined by the amount of **clusters** that the data section can hold ([Microsoft's FAT Whitepaper](https://academy.cba.mit.edu/classes/networking_communications/SD/FAT.pdf)), and the 2mb of flash memory that pico has will not cut it. 
