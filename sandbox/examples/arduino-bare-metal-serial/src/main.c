#include "config.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usb.h"


void init(void){ 
    serial.begin();
}

int main() {
    init();
    int c = 0;

    //Main Loop
    while (1) {
        
        serial.print("Hello World: ");
        serial.printInt(c);
        serial.println();
        serial.print("USB Register Binary: ");
        serial.printBinary(USBCON);
        serial.println();
        serial.print("USB Register Hex: ");
        serial.printHex(USBCON);
        serial.println();
        c++;

        _delay_ms(1000);
    }

    return 0;
}
