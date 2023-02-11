// com_monolithic.c
// TAB C Example COM Monolithic
//
// Written by Bradley Denby
// Other contributors: Abhishek Anand
//
// See the top-level LICENSE file for the license.

// Standard library headers
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t, uint32_t

// Board-specific header
#include <com.h>    // COM header

// TAB header
#include <tab.h>    // TAB header

// libopencm3 library

// Main
int main(void) {
  init_clock();

  // Should never reach this point
  return 0;
}
