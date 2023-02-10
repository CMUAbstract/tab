// cdh_monolithic.c
// TAB C Example CDH Monolithic
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Board-specific header
#include <cdh.h>    // CDH header

// TAB header
#include <tab.h>    // TAB header

// libopencm3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// Main
int main(void) {
	
   init_clock_hse();

  //HSE Code test for MCO output /////////////////
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Output clock on MCO pin ---------------------------------------------*/
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
  gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);
  gpio_set(GPIOA, GPIO8);
 
  rcc_set_mco(RCC_CFGR_MCO_PLL); //for PLL clk test : 100 MHz
  //rcc_set_mco(RCC_CFGR_MCO_HSE);  // for checking HSE output : 16 MHz

  // LED pin enable for debugging
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_set(GPIOC, GPIO10);
  gpio_clear(GPIOC, GPIO12);
  while(1) {
    for(int i=0; i<1600000; i++) {
      __asm__("nop");
    }
    gpio_toggle(GPIOC, GPIO10);
    gpio_toggle(GPIOC, GPIO12);
  }
  //HSE Code test for MCO output /////////////////
  
  
  // MCU initialization
  //init_clock();
  init_uart();
  // TAB initialization
  rx_cmd_buff_t rx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_rx_cmd_buff(&rx_cmd_buff);
  tx_cmd_buff_t tx_cmd_buff = {.size=CMD_MAX_LEN};
  clear_tx_cmd_buff(&tx_cmd_buff);
  // TAB loop
  while(1) {
    rx_usart1(&rx_cmd_buff);           // Collect command bytes
    reply(&rx_cmd_buff, &tx_cmd_buff); // Command reply logic
    tx_usart1(&tx_cmd_buff);           // Send a response if any
  }
  // Should never reach this point
  return 0;
}
