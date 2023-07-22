#include <stdio.h>
#include "pico/stdlib.h"

int main(){
    //Initialise I/O
    stdio_init_all(); 

    //Main Loop 
    while(1){
        printf("hello, world\n");
        sleep_ms(1000); // 0.5s delay
    }
}
