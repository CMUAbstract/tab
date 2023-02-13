// com.c
// COM board support implementation file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stdint.h>                 // uint8_t

// libopencm3 library
//#include <libopencm3/stm32/flash.h> // used in init_clock
//#include <libopencm3/stm32/gpio.h>  // used in init_gpio
//#include <libopencm3/stm32/rcc.h>   // used in init_clock, init_rtc
//#include <libopencm3/stm32/usart.h> // used in init_uart
// nrf
#include <libopencm3/nrf/51/clock.h>  // used in init_clock
#include <libopencm3/nrf/gpio.h>	  // used in init_gpio

// Board-specific header
#include <com.h>                    // COM header

// TAB header
#include <tab.h>                    // TAB header

// Functions required by TAB

int handle_common_data(common_data_t common_data_buff_i) {
  int strictly_increasing = 1;
  uint8_t prev_byte = common_data_buff_i.data[0];
  for(size_t i=1; i<common_data_buff_i.end_index; i++) {
    if(prev_byte>=common_data_buff_i.data[i]) {
      strictly_increasing = 0;
      i = common_data_buff_i.end_index;
    }
  }
  return strictly_increasing;
}

// Board initialization functions

void init_clock(void) {
  clock_set_xtal_freq(CLOCK_XTAL_FREQ_16MHZ);
  clock_start_lfclk (true);
}

void init_led(void) {
  gpio_mode_setup(GPIO0, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
  gpio_mode_setup(GPIO0, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
}

void init_uart(void) {
  // TODO
}

// Feature functions

void rx_usart1(rx_cmd_buff_t* rx_cmd_buff_o) {
/*while(                                             // while
   usart_get_flag(USART1,USART_ISR_RXNE) &&          //  USART1 RX not empty AND
   rx_cmd_buff_o->state!=RX_CMD_BUFF_STATE_COMPLETE  //  Command not complete
  ) {                                                //
    uint8_t b = usart_recv(USART1);                  // Receive byte from RX pin
    push_rx_cmd_buff(rx_cmd_buff_o, b);              // Push byte to buffer
  }*/                                                  //
}

void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
/*if(                                                  // if
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE && // rx_cmd is valid AND
   tx_cmd_buff_o->empty                                // tx_cmd is empty
  ) {                                                  //
    write_reply(rx_cmd_buff_o, tx_cmd_buff_o);         // execute cmd and reply
  }*/                                                    //
}

void tx_usart1(tx_cmd_buff_t* tx_cmd_buff_o) {
/*while(                                             // while
   usart_get_flag(USART1,USART_ISR_TXE) &&           //  USART1 TX empty AND
   !(tx_cmd_buff_o->empty)                           //  TX buffer not empty
  ) {                                                //
    uint8_t b = pop_tx_cmd_buff(tx_cmd_buff_o);      // Pop byte from TX buffer
    usart_send(USART1,b);                            // Send byte to TX pin
  }*/                                                  //
}
