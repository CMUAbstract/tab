// com.h
// COM board support header file
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand
//
// See the top-level LICENSE file for the license.

#ifndef COM_H
#define COM_H

// TAB header
#include <tab.h> 

// Functions required by TAB
int handle_common_data(common_data_t common_data_buff_i);

// Board initialization functions

void init_clock(void);
void init_uart(void);

#endif
