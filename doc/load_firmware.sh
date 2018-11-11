#! /bin/sh

# path to hex folder
target_dir="firmware"
# firmware file name
target_file="sketch_may30a.ino"

DIR="$( cd "$( dirname "$0" )" && pwd )"

# Erase chip
avrdude.exe -p m328p -c usbasp -P usb -e

# Set fuse bits (see details on http://www.engbedded.com/fusecalc)
avrdude.exe -p m328p -c usbasp -P usb -U lfuse:w:0xff:m -U hfuse:w:0xdb:m -U efuse:w:0x06:m

# Write firmware
avrdude.exe -p m328p -c usbasp -P usb -U flash:w:"$DIR\${target_dir}\${target_file}.hex":a

# Set lock bits (may be eresed by 'Erase chip command')
avrdude.exe -p m328p -c usbasp -P usb -U lock:w:0x00:m
