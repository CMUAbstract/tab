// cdh.c
// CDH board support implementation file
//
// Written by Bradley Denby
// Other contributors: Chad Taylor
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stdint.h>                 // uint8_t

// libopencm3 library
#include <libopencm3/stm32/flash.h> // used in init_clock
#include <libopencm3/stm32/gpio.h>  // used in init_gpio
#include <libopencm3/stm32/rcc.h>   // used in init_clock, init_rtc
#include <libopencm3/stm32/usart.h> // used in init_uart

// Board-specific header
#include <cdh.h>                    // Header file

// TAB header
#include <tab.h>                    // TAOLST protocol macros, typedefs, fnctns

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
  flash_unlock();
  for(size_t subpage_id=0; subpage_id<255; subpage_id++) {
    // subpage_id==0x00 writes to APP_ADDR==0x08008000 i.e. start of page 16
    // So subpage_id==0x10 writes to addr 0x08008800 i.e. start of page 17 etc
    // Need to erase page once before writing inside of it
    if((subpage_id*BYTES_PER_CMD)%BYTES_PER_PAGE==0) {
      flash_erase_page(16+(subpage_id*BYTES_PER_CMD)/BYTES_PER_PAGE);
      flash_clear_status_flags();
    }
  }
  flash_lock();
  return 1;
}

// This example implementation of bootloader_active always returns true
int bootloader_active(void) {
  return 1;
}

// Board initialization functions

void init_clock(void) {
  rcc_osc_on(RCC_HSI16);                    // 16 MHz internal RC oscillator
  rcc_wait_for_osc_ready(RCC_HSI16);        // Wait until oscillator is ready
  rcc_set_sysclk_source(RCC_CFGR_SW_HSI16); // Sets sysclk source for RTOS
  rcc_set_hpre(RCC_CFGR_HPRE_NODIV);        // AHB at 80 MHz (80 MHz max.)
  rcc_set_ppre1(RCC_CFGR_PPRE_DIV2);        // APB1 at 40 MHz (80 MHz max.)
  rcc_set_ppre2(RCC_CFGR_PPRE_NODIV);       // APB2 at 80 MHz (80 MHz max.)
  //flash_prefetch_enable();                  // Enable instr prefetch buffer
  flash_set_ws(FLASH_ACR_LATENCY_4WS);      // RM0351: 4 WS for 80 MHz
  //flash_dcache_enable();                    // Enable data cache
  //flash_icache_enable();                    // Enable instruction cache
  rcc_set_main_pll(                         // Setup 80 MHz clock
   RCC_PLLCFGR_PLLSRC_HSI16,                // PLL clock source
   4,                                       // PLL VCO division factor
   40,                                      // PLL VCO multiplication factor
   0,                                       // PLL P clk output division factor
   0,                                       // PLL Q clk output division factor
   RCC_PLLCFGR_PLLR_DIV2                    // PLL sysclk output division factor
  ); // 16MHz/4 = 4MHz; 4MHz*40 = 160MHz VCO; 160MHz/2 = 80MHz PLL
  rcc_osc_on(RCC_PLL);                      // 80 MHz PLL
  rcc_wait_for_osc_ready(RCC_PLL);          // Wait until PLL is ready
  rcc_set_sysclk_source(RCC_CFGR_SW_PLL);   // Sets sysclk source for RTOS
  rcc_wait_for_sysclk_status(RCC_PLL);
  rcc_ahb_frequency = 80000000;
  rcc_apb1_frequency = 40000000;
  rcc_apb2_frequency = 80000000;
}

void init_uart(void) {
  rcc_periph_reset_pulse(RST_USART1);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART1);
  gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO9|GPIO10);
  gpio_set_af(GPIOA,GPIO_AF7,GPIO9);  // USART1_TX is alternate function 7
  gpio_set_af(GPIOA,GPIO_AF7,GPIO10); // USART1_RX is alternate function 7
  usart_set_baudrate(USART1,115200);
  usart_set_databits(USART1,8);
  usart_set_stopbits(USART1,USART_STOPBITS_1);
  usart_set_mode(USART1,USART_MODE_TX_RX);
  usart_set_parity(USART1,USART_PARITY_NONE);
  usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
  usart_enable(USART1);
}

// Feature functions

void rx_usart1(rx_cmd_buff_t* rx_cmd_buff_o) {
  while(                                             // while
   usart_get_flag(USART1,USART_ISR_RXNE) &&          //  USART1 RX not empty AND
   rx_cmd_buff_o->state!=RX_CMD_BUFF_STATE_COMPLETE  //  Command not complete
  ) {                                                //
    uint8_t b = usart_recv(USART1);                  // Receive byte from RX pin
    push_rx_cmd_buff(rx_cmd_buff_o, b);              // Push byte to buffer
  }                                                  //
}

void reply(rx_cmd_buff_t* rx_cmd_buff_o, tx_cmd_buff_t* tx_cmd_buff_o) {
  if(                                                  // if
   rx_cmd_buff_o->state==RX_CMD_BUFF_STATE_COMPLETE && // rx_cmd is valid AND
   tx_cmd_buff_o->empty                                // tx_cmd is empty
  ) {                                                  //
    write_reply(rx_cmd_buff_o, tx_cmd_buff_o);         // execute cmd and reply
  }                                                    //
}

void tx_usart1(tx_cmd_buff_t* tx_cmd_buff_o) {
  while(                                             // while
   usart_get_flag(USART1,USART_ISR_TXE) &&           //  USART1 TX empty AND
   !(tx_cmd_buff_o->empty)                           //  TX buffer not empty
  ) {                                                //
    uint8_t b = pop_tx_cmd_buff(tx_cmd_buff_o);      // Pop byte from TX buffer
    usart_send(USART1,b);                            // Send byte to TX pin
  }                                                  //
}
