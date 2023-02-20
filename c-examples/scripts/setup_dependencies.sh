#!/bin/bash
#
# setup_dependencies.sh
# A bash script that sets up dependencies
#
# Usage: ./setup_dependencies.sh
# Assumptions:
#  - The dependencies outlined in the C examples README have been installed
# Arguments:
#  - None; this script assumes the git repo directory structure. Note that, due
#    to this assumption, this script should be executed from its containing
#    directory (i.e. tab/c-examples/scripts/)
# Results:
#  - Sets up dependencies for the TAB C examples
#
# Written by Bradley Denby
# Other contributors: None
#
# See the top-level LICENSE file for the license.

# print prerequisite message
echo ""
echo "IMPORTANT: manually install dependencies in the C examples README first"
echo ""

# stlink
cd ../utilities/
git clone https://github.com/stlink-org/stlink.git
cd stlink/
git checkout --quiet v1.7.0
make clean
make release

# GNU Arm Embedded Toolchain
cd ../
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
tar xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
echo "GNU Arm Embedded Toolchain ready"
echo "Don't forget to source the sourcefile:"
echo "  $ source sourcefile.txt"
echo "Be sure to source from the tab/c-examples/scripts directory"
cd ../scripts/
source sourcefile.txt

# libopencm3
cd ../
git clone https://github.com/libopencm3/libopencm3.git
cd libopencm3/
git checkout --quiet 8bc4837
make TARGETS="stm32/l4 nrf/51"
