#include "config.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usb.h"

void init(void){
    usb_init();
    sei();                     //Enable Global Interrupts

    while (!usb_configured());
    _delay_ms(1000);
}

int main() {
    DDRC |= (1 << PC7); 
    DDRB |= (1 << PB0); 
    init();
    
    // Main loop
    int c = 0;
    while (1) {
        // Main loop does nothing, just wait for interrupts
        // wait for the user to run their terminal emulator program
		// which sets DTR to indicate it is ready to receive.
		while (!(usb_serial_get_control() & USB_SERIAL_DTR)) /* wait */ ;

		// discard anything that was received prior.  Sometimes the
		// operating system or other software will send a modema
		// "AT command", which can still be buffered.
		// usb_serial_flush_input();

        //run
        serialPrint("Hello World: ");
        serialPrintInt(c);
        serialPrint("\n");
        c++;

        _delay_ms(1000);
    }

    return 0;
}
