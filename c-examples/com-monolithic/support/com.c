// com.c
// COM board support implementation file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stdint.h>                 

// libopencm3 library
#include <libopencm3/nrf/51/clock.h>

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

void init_uart(void) {
  // TODO
}
