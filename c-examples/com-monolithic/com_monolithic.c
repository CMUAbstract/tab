// com_monolithic.c
// TAB C Example COM Monolithic
//
// Written by Bradley Denby
// Other contributors: None
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Board-specific header
#include <com.h>    // COM header

// TAB header
#include <tab.h>    // TAB header

// libopencm3
#include <libopencm3/nrf/rtc.h>
#include <libopencm3/nrf/gpio.h>
#include <libopencm3/nrf/51/clock.h>

// Main
int main(void) {
  init_clock();
	
	
  gpio_mode_setup(GPIO0, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
  //gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_set(GPIO0, GPIO13);
  //gpio_clear(GPIOC, GPIO12);
  while(1) {
	for(int i=0; i<1600000; i++) {
	  __asm__("nop");
	}
	gpio_toggle(GPIO0, GPIO13);
	//gpio_toggle(GPIOC, GPIO12);
  }
  // Should never reach this point
  return 0;
}
