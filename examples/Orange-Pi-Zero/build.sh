#!/bin/bash

gcc -std=gnu17 -O2 "-I../../src/" "../../src/w25qxx.c" -DW25QXX_DYNAMIC_OVERWRITE_BUFFER -DW25QXX_DEBUG intf_orange_pi_zero.c main.c -o opi_spi_flash_test