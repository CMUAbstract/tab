// com.c
// COM board support implementation file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand, Chad Taylor, Alexey Tatarinov
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stdint.h>                     // uint8_t

// libopencm3 library
#include <libopencm3/nrf/51/clock.h>    // clock_set_xtal_freq
#include <libopencm3/nrf/common/gpio.h> // gpio_mode_setup
#include <libopencm3/nrf/common/uart.h> // uart functions

// Board-specific header
#include <com.h>                        // COM header

// TAB header
#include <tab.h>                        // TAB header

// Functions required by TAB

// This example implementation of handle_common_data checks whether the bytes
// are strictly increasing, i.e. each subsequent byte is strictly greater than
// the previous byte
int handle_common_data(common_data_t common_data_buff_i) {
  int strictly_increasing = 1;
  uint8_t prev_byte = common_data_buff_i.data[0];
  for(size_t i=1; i<common_data_buff_i.end_index; i++) {
    if(prev_byte>=common_data_buff_i.data[i]) {
      strictly_increasing = 0;
      i = common_data_buff_i.end_index;
    } else {
      prev_byte = common_data_buff_i.data[i];
    }
  }
  return strictly_increasing;
}

// This example implementation of handle_bootloader_erase erases all application
// programs
int handle_bootloader_erase(void){
  // TODO
  return 0;
}

// This example implementation of bootloader_active always returns true
int bootloader_active(void) {
  return 1;
}

// Board initialization functions

void init_clock(void) {
  clock_set_xtal_freq(CLOCK_XTAL_FREQ_16MHZ);
  clock_start_hfclk(true);
}

void init_led(void) {
  gpio_mode_setup(GPIO0, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
  gpio_mode_setup(GPIO0, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

void init_uart(void) {
  gpio_mode_setup(GPIO0,GPIO_MODE_INPUT,GPIO_PUPD_NONE,RXD|CTS);
  gpio_mode_setup(GPIO0,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,TXD|RTS);
  gpio_set(GPIO0,RTS|TXD);
  uart_set_pins(UART0,RXD,TXD,CTS,RTS);
  uart_set_baudrate(UART0,UART_BAUD_115200);
  uart_set_parity(UART0,0);
  uart_set_flow_control(UART0,0);
  uart_enable(UART0);
  uart_start_rx(UART0);
}

// Feature functions

void rx_uart0(rx_cmd_buff_t* rx_cmd_buff_o) {
  while(                                             // while
   UART_EVENT_RXDRDY(UART0) &&                       //  UART0 RX has byte AND
   rx_cmd_buff_o->state!=RX_CMD_BUFF_STATE_COMPLETE  //  Command not complete
  ) {                                                //
    UART_EVENT_RXDRDY(UART0) = 0;                    // Reset RXDRDY event
    uint8_t b = (uint8_t)uart_recv(UART0);           // Receive byte from RX pin
    push_rx_cmd_buff(rx_cmd_buff_o, b);              // Push byte to buffer
  }                                                  //
}

void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
  if(                                                  // if
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE && // rx_cmd is valid AND
   tx_cmd_buff_o->empty                                // tx_cmd is empty
  ) {                                                  //
    write_reply(rx_cmd_buff_o, tx_cmd_buff_o);         // Execute cmd and reply
    uint8_t b = pop_tx_cmd_buff(tx_cmd_buff_o);        // Pop 1st TX buffer byte
    uart_send(UART0,b);                                // Generate TXDRDY event
    uart_start_tx(UART0);                              // Start TX session
  }                                                    //
}

void tx_uart0(tx_cmd_buff_t* tx_cmd_buff_o) {
  while(                                             // while
   UART_EVENT_TXDRDY(UART0) &&                       //  UART0 TX empty AND
   !(tx_cmd_buff_o->empty)                           //  TX buffer not empty
  ) {                                                //
    UART_EVENT_TXDRDY(UART0) = 0;                    // Reset TXDRDY event
    uint8_t b = pop_tx_cmd_buff(tx_cmd_buff_o);      // Pop byte from TX buffer
    uart_send(UART0,b);                              // Send byte to TX pin
  }                                                  //
  if(                                                // if
   UART_EVENT_TXDRDY(UART0) &&                       //  UART0 TX empty AND
   tx_cmd_buff_o->empty                              //  TX buffer empty
  ) {                                                //
    UART_EVENT_TXDRDY(UART0) = 0;                    // Reset TXDRDY event
    uart_stop_tx(UART0);                             // Stop TX session
  }                                                  //
}
