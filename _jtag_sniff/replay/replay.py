#!/usr/bin/env python

import time
import sys
import binascii

import ftdi1 as ftdi

ANSI = lambda x: x
#fromlinux = 0
while sys.argv[1:]:
	if sys.argv[1] == '-r':
		ANSI = lambda x: ''
	elif sys.argv[1] == '-l':
		#fromlinux = 1
		pass
	else:
		infile = sys.argv[1]
	del sys.argv[1]

TAB=ANSI('\033[40G')		; OFF=ANSI('\033[0m')		; BG_OFF=ANSI('\033[0;49m')
RED=ANSI('\033[0;31m')		; RED2=ANSI('\033[1;31m')	; BG_RED=ANSI('\033[0;41m')
GREEN=ANSI('\033[0;32m')	; GREEN2=ANSI('\033[1;32m')
YELLOW=ANSI('\033[0;33m')	; YELLOW2=ANSI('\033[1;33m')
BLUE=ANSI('\033[0;34m')		; BLUE2=ANSI('\033[1;34m')	; BG_BLUE=ANSI('\033[0;44m')
MAGENTA=ANSI('\033[0;35m')	; MAGENTA2=ANSI('\033[1;35m')
CYAN=ANSI('\033[0;36m')		; CYAN2=ANSI('\033[1;36m')
WHITE=ANSI('\033[0;37m')	; WHITE2=ANSI('\033[1;37m')

def BIT(x):
	return 1 << x

# Initialize FTDI device connection.
ctx = ftdi.new()
ftdi.usb_open(ctx, 0x263d, 0x4001)
ftdi.usb_reset(ctx)


# Disable flow control. Commented out because it is unclear if this is necessary.
#self._check(ftdi.setflowctrl, ftdi.SIO_DISABLE_FLOW_CTRL)

# Change read & write buffers to maximum size, 65535 bytes.
ftdi.read_data_set_chunksize(ctx, 65535)
ftdi.write_data_set_chunksize(ctx, 65535)

# Clear pending read data & write buffers.
ftdi.usb_purge_buffers(ctx)
# Reset MPSSE
ftdi.set_bitmode(ctx, 0, 0)
# Enable MPSSE
ftdi.set_bitmode(ctx, 0, 2)

def ftwrite(string):
	length = len(string)
	ret = ftdi.write_data(ctx, string, length)
	if ret < 0:
		raise RuntimeError('ftdi_write_data failed with error {0}: {1}'.format(ret, ftdi.get_error_string(self._ctx)))
	if ret != length:
		raise RuntimeError('ftdi_write_data expected to write {0} bytes but actually wrote {1}!'.format(length, ret))

def ftread():
	ret, data = ftdi.read_data(ctx, 200)
	if ret < 0:
		raise RuntimeError('ftdi_read_data failed with error code {0}.'.format(ret))
	return data[0:ret]


# Enable MPSSE and syncronize communication with device.
ftwrite('\xAB')
# Keep reading until bad command response (0xFA 0xAB) is returned.
# Fail if too many read attempts are made to prevent sticking in a loop.
tries = 0
sync = False
while not sync:
	data = ftread()
	if data == '\xFA\xAB':
		sync = True
	tries += 1
	if tries >= 10:
		raise RuntimeError('Could not synchronize with FT232H!')



epval = ""

with open(infile, 'rb') as f:
	for l in f:
		s = l.split()
		if len(s) != 5 or s[1] != '(processing':
			print BLUE, l, OFF,
			continue
		print l,

		val = ""
		for x in s[3].split(':'):
			val += chr(int(x, 16))

		#print binascii.hexlify(val)

		if s[2] == '>':
			ftwrite(val)
		else:
			val = val[2:]
			if len(val) == 0:
				continue

			while len(epval) < len(val):
				epval += ftread()

			print "Expected:", WHITE, binascii.hexlify(val), OFF
			print "Received:", WHITE, binascii.hexlify(epval), OFF
			if val == epval[0:len(val)]:
				print CYAN, "... ok", OFF
			else:
				print RED, "... BBBBBAAAAAADDDDDD!!!!!", OFF


			epval = epval[len(val):]










#while True:
#	pass

#

# write the data
#ep.write('test')


