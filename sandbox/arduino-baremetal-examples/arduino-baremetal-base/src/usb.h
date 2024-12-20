#ifndef USB_H_
#define USB_H_

#include <stdint.h>
#include <stdlib.h> 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// setup
void usb_init(void);			    // initialize everything
uint8_t usb_configured(void);		// is the USB port configured

// simplified serial
void wait_serial_open(void);
void serial_begin();

void serial_print(const char *s);
void serial_println(void);

void serial_print_int(int num);
void serial_print_binary(uint8_t regValue);
void serial_print_hex(uint8_t regValue);

uint8_t serial_recieve(char *buf, uint8_t size);


// receiving data
int16_t usb_serial_getchar(void);	// receive a character (-1 if timeout/error)
uint8_t usb_serial_available(void);	// number of bytes in receive buffer
void usb_serial_flush_input(void);	// discard any buffered input

// transmitting data
int8_t usb_serial_putchar(uint8_t c);	// transmit a character
int8_t usb_serial_putchar_nowait(uint8_t c);  // transmit a character, do not wait
int8_t usb_serial_write(const uint8_t *buffer, uint16_t size); // transmit a buffer
void usb_serial_flush_output(void);	// immediately transmit any buffered output

// // serial parameters
uint32_t usb_serial_get_baud(void);	// get the baud rate
uint8_t usb_serial_get_stopbits(void);	// get the number of stop bits
uint8_t usb_serial_get_paritytype(void);// get the parity type
uint8_t usb_serial_get_numbits(void);	// get the number of data bits
uint8_t usb_serial_get_control(void);	// get the RTS and DTR signal state
int8_t usb_serial_set_control(uint8_t signals); // set DSR, DCD, RI, etc

#define LSB(n) (n & 0xFF)           //Least Significatn Byte
#define MSB(n) ((n >> 8) & 0xFF)    //Most Significatn Byte

// // This file does not include the HID debug functions, so these empty
// // macros replace them with nothing, so users can compile code that
// // has calls to these functions.
#define usb_debug_putchar(c)
#define usb_debug_flush_output()

// constants corresponding to the various serial parameters
#define USB_SERIAL_DTR			0x01
#define USB_SERIAL_RTS			0x02
#define USB_SERIAL_1_STOP		0
#define USB_SERIAL_1_5_STOP		1
#define USB_SERIAL_2_STOP		2
#define USB_SERIAL_PARITY_NONE	0
#define USB_SERIAL_PARITY_ODD	1
#define USB_SERIAL_PARITY_EVEN	2
#define USB_SERIAL_PARITY_MARK	3
#define USB_SERIAL_PARITY_SPACE	4
#define USB_SERIAL_DCD			0x01
#define USB_SERIAL_DSR			0x02
#define USB_SERIAL_BREAK		0x04
#define USB_SERIAL_RI			0x08
#define USB_SERIAL_FRAME_ERR	0x10
#define USB_SERIAL_PARITY_ERR	0x20
#define USB_SERIAL_OVERRUN_ERR	0x40

#define EP_TYPE_CONTROL			0x00
#define EP_TYPE_BULK_IN			0x81
#define EP_TYPE_BULK_OUT		0x80
#define EP_TYPE_INTERRUPT_IN	0xC1
#define EP_TYPE_INTERRUPT_OUT	0xC0
#define EP_TYPE_ISOCHRONOUS_IN	0x41
#define EP_TYPE_ISOCHRONOUS_OUT	0x40
#define EP_SINGLE_BUFFER		0x02
#define EP_DOUBLE_BUFFER		0x06
 // Maps Size(s) to Register Value (Hex) required for configuration.
#define EP_SIZE(s)  ((s) == 64 ? 0x30 :	\
					((s) == 32 ? 0x20 :	\
					((s) == 16 ? 0x10 :	\
								0x00)))

#define MAX_ENDPOINT			4

// standard control endpoint request types
#define GET_STATUS              0x00
#define CLEAR_FEATURE           0x01
#define SET_FEATURE             0x03
#define SET_ADDRESS             0x05
#define GET_DESCRIPTOR          0x06
#define GET_CONFIGURATION       0x08
#define SET_CONFIGURATION       0x09
#define GET_INTERFACE           0x0A    //10
#define SET_INTERFACE           0x0B    //11
// HID (Human Interface Device)
#define HID_GET_REPORT          0x01
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A    //10
#define HID_SET_PROTOCOL        0x0B    //11
// CDC (communication class device)
#define CDC_SET_LINE_CODING	    0x20
#define CDC_GET_LINE_CODING		0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22


/* Define a struct for the Serial object (Models Arduino Interface)*/

typedef struct {
    // Function pointers to simulate Serial functions
    void (*begin)(void);
    void (*print)(const char *message);
	void (*println)(void);
    void (*printInt)(int value);
    void (*printBinary)(uint8_t value);
    void (*printHex)(uint8_t value);
} Serial_t;

extern Serial_t serial;

#endif /* USB_H_ */
