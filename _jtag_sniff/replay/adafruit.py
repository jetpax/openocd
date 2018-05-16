#!/usr/bin/env python


# Import standard Python time library.
import time

# Import GPIO and FT232H modules.
import Adafruit_GPIO as GPIO
import Adafruit_GPIO.FT232H as FT232H

print "A"

# Temporarily disable the built-in FTDI serial driver on Mac & Linux platforms.
FT232H.use_FT232H()

print "B"

# Create an FT232H object that grabs the first available FT232H device found.
ft232h = FT232H.FT232H(0x263d, 0x4001)

dir = {
 0 : GPIO.OUT,
 1 : GPIO.OUT,
 2 : GPIO.OUT,
 3 : GPIO.OUT,
 4 : GPIO.OUT,
 5 : GPIO.OUT,
 6 : GPIO.OUT,
 7 : GPIO.OUT,
}

val0 = {
 0 : 0,
 1 : 0,
 2 : 0,
 3 : 0,
 4 : 0,
 5 : 0,
 6 : 0,
 7 : 0,
}

val1 = {
 0 : 1,
 1 : 1,
 2 : 1,
 3 : 1,
 4 : 1,
 5 : 1,
 6 : 1,
 7 : 1,
}

#ft232h.setup_pins(dir, val1)   # Make pin D7 a digital input.


print hex(ft232h.mpsse_read_gpio())
