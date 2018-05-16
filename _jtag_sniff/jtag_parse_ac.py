#!/usr/bin/env python

import sys
import argparse
import textwrap
import binascii
import bitarray
import dpkt
import struct

ANSI = lambda x: x
tss = lambda: "%10f" % ts
jtagc_init = 'master'
#fromlinux = 0
while sys.argv[1:]:
	if sys.argv[1] == '-r':
		ANSI = lambda x: ''
		tss = lambda: ''
	elif sys.argv[1] == '-l':
		#fromlinux = 1
		pass
	elif sys.argv[1] == '-j': # master/once/npc
		jtagc_init = sys.argv[2]
		del sys.argv[2]
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



class xPC56x:
	def __init__(self, jtagc_init):
		self.jtagc = jtagc_init
		self.ir = "init"
		self.irf = ""

	def decode_ctl(self, data):
		info = 'VLE ' if BIT(1) & data else ''
		info += 'FFRA ' if BIT(10) & data else ''
		info += 'TLBM ' if BIT(8) & data else ''
		info += 'WAIT ' if BIT(16) & data else ''
		assert data & ~(BIT(10) | BIT(16) | BIT(1)) == 0
		return info

	def decode_vle(self, data):
		ffra = 0
		def neg(val, bitsize):
			if val & (1 << (bitsize - 1)) != 0: # if sign bit is set e.g., 8bit: 128-255
				val -= 1 << bitsize         # compute negative value
			return val
		def reg(val):
			return 'r' + str(val)
		def imm(val):
			return str(val) + '/' + hex(val)
		def spreg(val):
			return 'spr' + str(((val & 0x1F) << 5) | ((0x3E0 & val) >> 5))

		def sd4_form(data):
			return RED + "mcu: ERROR TODO1" + OFF
		def se_lbz():
			pass

		def bd8_bo16_form(data):
			bo16 = (data >> 26) & 1
			bi16 = (data >> 24) & 3
			bd8  = (data >> 16) & 0xFF
			return (bo16, bi16, bd8)
		def se_bc(bo16, bi16, bd8):
			return 'se_bc ' + str(bo16) + ',' + str(bi16) + ',' + '+' + str(neg(bd8 << 2, 9))

		def d_form(data):
			rs = (data >> 21) & 0x1F
			ra = (data >> 16) & 0x1F
			d = (data >> 0) & 0xFFFF
			d = neg(d, 16)
			rs = 'r' + str(rs)
			if ffra:
				ra = 'wbbrlo({:08x})'.format(self.wbbrlo)
			else:
				ra = 'r' + str(ra)
			return (rs, ra, d)
		def e_stb(rs, ra, d):
			return 'e_stb ' + rs + ', ' + str(d) + '(' + ra + ')'
		def e_lwz(rs, ra, d):
			return 'e_lwz ' + rs + ', ' + str(d) + '(' + ra + ')'

		def bd24_form(data):
			bd24 = (data & 0x1fffffe) >> 1
			lk = data & 1
			return bd24, lk
		def e_b(bd24, lk):
			return 'e_b' + ('l' if lk else '') + ' ' + imm(bd24<<1)

		def i16l_form(data):
			rt = (data & 0x3E00000) >> 21
			ui = ((data & 0x1F0000) >> 5) | (data & 0x7FF)
			return rt, ui
		def e_lis(rt, ui):
			return 'e_lis ' + reg(rt) + ', ' + imm(ui)

		def sci8_rc_form(data):
			rs = (data >> 21) & 0x1F
			ra = (data >> 16) & 0x1F
			rc  = (data >> 11) & 1
			f = (data >> 10) & 1
			scl = (data >> 8) & 3
			ui8 = (data >> 0) & 0xFF
			sci8 = 0
			if f == 1:
				sci8 = 2**64 - 1
				sci8 &= ~(0xFF << (scl * 8))
			sci8 |= ui8 << (scl * 8)
			ra = 'r' + str(ra)
			if ffra:
				rs = 'wbbrlo({:08x})'.format(self.wbbrlo)
			else:
				rs = 'r' + str(rs)
			return (rs, ra, rc, sci8)
		def e_ori(rs, ra, rc, sci8):
			s = 'ori'
			if rc:
				s += '.'
			s += ' ' + ', '.join((ra, rs, str(sci8)))
			return s

		def d8_form(data):
			rt = (data & 0x3E00000) >> 21
			ra = (data & 0x1f0000) >> 16
			d8 = data & 0xff
			return rt, ra, d8
		def e_stmw(rs, ra, d8):
			return 'e_stmw ' + reg(rs) + ', ' + imm(d8) + '(' + reg(ra) + ')'

		def x_form(data):
			f0 = (data & 0x3E00000) >> 21
			f1 = (data & 0x1f0000) >> 16
			f2 = (data & 0xf800) >> 11
			f3 = data & 1
			return f0, f1, f2, f3
		def dcbf(f0, f1, f2, f3):
			label = { 0:"dcbf", 1:"dcbfl", 3:"dcbflp"}
			return "%s r%d, r%d" % (label[f0], reg(f1), reg(f2))
		def sync(f0, f1, f2, f3):
			return "sync EE={:04b} L={}".format(f0, f1)
		def lwbrx(f0, f1, f2, f3):
			if f1 != 0:
				return "lwbrx %s, %s, %s" % (reg(f0), reg(f1), reg(f2))
			else:
				return "lwbrx %s, %s" % (reg(f0), reg(f2))
		def stwbrx(f0, f1, f2, f3):
			if f1 != 0:
				return "stwbrx %s, %s, %s" % (reg(f0), reg(f1), reg(f2))
			else:
				return "stwbrx %s, %s" % (reg(f0), reg(f2))
		def lhbrx(f0, f1, f2, f3):
			if f1 != 0:
				return "lhbrx %s, %s, %s" % (reg(f0), reg(f1), reg(f2))
			else:
				return "lhbrx %s, %s" % (reg(f0), reg(f2))

		def xfx_form(data):
			f0 = (data >> 21) & 0x1F
			f1 = (data >> 11) & 0x3FF
			return f0, f1
		def mfcr(rt, dummy):
			return 'mfcr ' + reg(rt)
		def mtcrf(rs, fxm):
			assert (fxm & 0x200) == 0, 'The mtcrf 11th bit is not 0 : ' + hex(fxm)
			fxm = fxm >> 1
			return 'mtcrf ' + hex(fxm & 0x1FF) + ', ' + reg(rs)
		def mfspr(rs, spr):
			return 'mfspr ' + spreg(spr) + ', ' + reg(rs)
		def mtspr(rs, spr):
			return 'mtspr ' + spreg(spr) + ', ' + reg(rs)


		filters = (
			(0xF0000000, sd4_form,		{0x80000000: se_lbz}),
			(0xF8000000, bd8_bo16_form,	{0xE0000000: se_bc}),
			(0xFC000000, d_form,		{0x34000000: e_stb,
							 0x50000000: e_lwz}),
			(0xFC000000, bd24_form,		{0x78000000: e_b}),
			(0xFC00F800, i16l_form,		{0x7000E000: e_lis}),
			(0xFC00F000, sci8_rc_form,	{0x1800D000: e_ori}),
			(0xFC00FF00, d8_form,		{0x18000900: e_stmw}),
			(0xFC0007FE, x_form, 		{0x7C0000AC: dcbf,
							 0x7C0004AC: sync,
							 0x7C00042C: lwbrx,
							 0x7C00052C: stwbrx,
							 0x7C00062c: lhbrx}),
			(0xFC0007FE, xfx_form, 		{0x7C000026: mfcr,
							 0x7C000120: mtcrf,
							 0x7C0002A6: mfspr,
							 0x7C0003A6: mtspr}),
			)



		for mask, form, opcodes in filters:
			#print hex(data), hex(mask), hex(data), hex(data & mask), opcodes.keys()
			inst = opcodes.get(data & mask)
			if inst:
				return inst(*form(data))
		return "TODO"

	def jtagc_reset(self):
		print BLUE + tss(), "mcu TMS: jtagc reset", OFF
		self.jtagc = "master"

	def update_ir(self, cnt, r, w):
		print CYAN + tss(), "update IR cnt:{}  R:{:x}  W:{:x}".format(cnt, r, w) , OFF
		# JTAG 1149.1 requires the 2 lsb of IR to be '01'... but NPC TAP seems to disagree
		if self.jtagc != 'npc' and ((0 < cnt and not (r & BIT(0))) or (1 < cnt and (r & BIT(1)))):
			print RED + "mcu: ERROR IR prefix", cnt, self.jtagc, hex(r), hex(w), OFF

		if self.jtagc == "master":
			self.irf = ""
			if cnt != 5:
				print RED + "mcu: ERROR IR cnt", cnt, self.jtagc, hex(r), hex(w), OFF
			elif w == 1:
				self.ir = "IDCODE"
			elif w == 7:
				self.ir = "PASSWORD"
			# WARNING: ACCESS_AUX_TAP_xxx values may differ depending on xPC56 subfamilies
			elif w == 0x10:
				self.ir = "TAP_NPC"
				self.jtagc = "npc"
			elif w == 0x11:
				self.ir = "TAP_ONCE"
				self.jtagc = "once"
			elif w == 0x1F:
				self.ir = "BYPASS"
			else:
				print RED + "mcu: ERROR UNKNOWN JTAGC IR", cnt, hex(r), hex(w), OFF
				self.ir = "?"
				self.jtagc = "?"
			print MAGENTA + tss(), "mcu JTAGC IR: ", self.ir

		elif self.jtagc == "npc":
			if cnt != 4:
				print RED + "mcu: ERROR IR npc cnt", cnt, self.jtagc, hex(r), hex(w), OFF
			elif w == 0:
				self.ir = "NEXUS_ENABLE"
			elif w == 0xf:
				self.ir = "BYPASS"
			else:
				self.ir = "ERROR TODO"

			print MAGENTA + tss(), "mcu IR npc: ", self.ir

		elif self.jtagc == "once":
			if cnt != 10:
				print MAGENTA2+"mcu: ERROR IR cnt", cnt, self.jtagc, OFF


			str_st = ""
			label = ("?", "?", "WAIT", "DEBUG", "STOP", "HALT", "RESET", "CHKST", "ERR", "MCLK")
			for i in reversed(xrange(2, 10)):
				if r & BIT(i):
					str_st += "%s " % label[i]

			str_cmd = ""
			label = ("EX", "GO", "RW")
			self.irf = ""
			for i in reversed(xrange(7, 10)):
				if w & BIT(i):
					str_cmd += " %s" % label[i - 7]
			self.irf = str_cmd
			label = { 0x2	: "JTAG ID",	0x10	: "CPUSCR",	0x11	: "NOREG",	0x12	: "OCR",
				0x20	: "IAC1",	0x21	: "IAC2",	0x22	: "IAC3",	0x23	: "IAC4",
				0x24	: "DAC1",	0x25	: "DAC2",	0x30	: "DBSR",	0x31	: "DBCR0",
				0x32	: "DBCR1",	0x33	: "DBCR2",	0x6f	: "NEXUS_CTRL",	0x70	: "GPR0",
				0x79	: "GPR9",	0x7a	: "CDACNTL",	0x7b	: "CDADATA",	0x7c	: "NEXUS",
				0x7e	: "EOnCE",	0x7f	: "Bypass",
				}
			self.ir = label.get(w & 0x7F, "rsv")
			print MAGENTA + tss(), "mcu IR: " + str_st + "| %6s |" % (self.ir) + str_cmd

		else:
			print RED + "mcu: ERROR IR state", cnt, self.jtagc, hex(r), hex(w), OFF

	nexr = ""
	nexrf = ""
	nexrwa = ""
	nexrwcs = ""

	def update_dr(self, cnt, r, w):
		#print 'FFFFUUUCCCKKK', cnt, self.ir , self.irf, '<', self.nexr, '>', hex(r), hex(w)
		if self.ir == "CPUSCR":
			regs = (("WBBRl", None), ("WBBRh", None), ("MSR", None), ("PC", None), ("IR", xPC56x.decode_vle), ("CTL", xPC56x.decode_ctl))
			if 'RW' in self.irf:
				v = r
				fstr = "mcu:     %5s r:%04x %04x  w:---- ----  %s"
			else:
				v = w
				fstr = "mcu:     %5s r:---- ----  w:%04x %04x  %s"
			for l, f in regs:
				info = f(self, v & 0xFFFFFFFF) if f else ''
				print GREEN + tss(), fstr % (l, (v>>16) & 0xFFFF, v & 0xFFFF, info)
				v >>= 32



			if cnt < 192:
				print "ERROR LEN CPUSCR", cnt
		elif self.ir == "NEXUS":
			if self.nexr == "":
				if cnt != 8:
					raise
				label = { 0x2 : "DC1", 0x3 : "DC2", 0x7 : "RWCS", 0x9 : "RWA", 0xA : "RWD" }
				self.nexr = label[w>>1]
				self.nexrf = "W" if (w & 1) else "R"
				print WHITE + tss(), "mcu: NEXUS reg", self.nexr, self.nexrf
			else:
				if cnt != 32:
					raise

				if self.nexr == "RWA" and self.nexrf == "W":
					self.nexrwa = w
					print WHITE + tss(), "mcu: NEXUS dat RWA W %08x" % (w)
				elif self.nexr == "RWA" and self.nexrf == "R":
					print WHITE + tss(), "mcu: NEXUS dat RWA R %08x " % (r)
				elif self.nexr == "RWCS" and self.nexrf == "R":
					print WHITE + tss(), "mcu: NEXUS dat RWCS R %08x " % (r)
				elif self.nexr == "RWCS" and self.nexrf == "W":
					self.nexrwcs = w
					assert w & BIT(31)
					self.nexcnt = (w & 0xFFFC) >> 2
					print WHITE + tss(), "mcu: NEXUS dat RWCS W %08x " % (r)
					#print RED, "ERROR mcu strange RWCS", hex(w), OFF
				elif self.nexr == "RWD":
					sz = 1 << ((self.nexrwcs >> 27) & 3)
					assert sz == 4
					if self.nexrf == "W" and (self.nexrwcs & BIT(30)):
						print WHITE + tss(), "mcux: NEXUS W @%08x <- %08x " % (self.nexrwa, w),
					elif self.nexrf == "R" and not (self.nexrwcs & BIT(30)):
						print WHITE + tss(), "mcux: NEXUS R @%08x -> %08x " % (self.nexrwa, r),
					else:
						raise
					print "  (sz:%d  cnt:%d)" % (sz, self.nexcnt)+ OFF
					self.nexcnt -= 1
					self.nexrwa += sz

				elif self.nexr == "DC1" and self.nexrf == "R":
					print WHITE + tss(), "mcux: NEXUS READ DC1 %08x " % (r)
				elif self.nexr == "DC1" and self.nexrf == "W":
					print WHITE + tss(), "mcux: NEXUS WRITE??? DC1 %08x %08x " % (r, w)
				else:
					print self.nexr, self.nexrf, self.nexrwa, self.nexrwcs, cnt, r, w
					raise

				self.nexr = ""
				self.nexrf = ""

		else:
			if self.ir == "PASSWORD":
				if cnt not in (64, 65):
					print "ERROR LEN DR"
			else:
				if cnt != 32 and cnt != 0:
					print "ERROR LEN DR", cnt

			dictcol = { "CPUSCR":GREEN, "NEXUS":WHITE, "Bypass":YELLOW, "DBSR":YELLOW}
			print dictcol.get(self.ir, CYAN) + tss(), "mcu: update DR cnt:{:3}  R:{:8x}  W:{:8x}  ({}{})".format(cnt, r, w, self.ir, self.irf) , OFF

class TapState:
	TEST_LOGIC_RESET = 1
	RUN_TEST_IDLE = 2
	SELECT_DR_SCAN = 3
	CAPTURE_DR = 4
	SHIFT_DR = 5
	EXIT1_DR = 6
	PAUSE_DR = 7
	EXIT2_DR = 8
	UPDATE_DR = 9
	SELECT_IR_SCAN = 10
	CAPTURE_IR = 11
	SHIFT_IR = 12
	EXIT1_IR = 13
	PAUSE_IR = 14
	EXIT2_IR = 15
	UPDATE_IR = 16

	NEXT = ((RUN_TEST_IDLE, TEST_LOGIC_RESET),  # TEST_LOGIC_RESET
		(RUN_TEST_IDLE, SELECT_DR_SCAN),    # RUN_TEST_IDLE
		(CAPTURE_DR, SELECT_IR_SCAN),       # SELECT_DR_SCAN
		(SHIFT_DR, EXIT1_DR),               # CAPTURE_DR
		(SHIFT_DR, EXIT1_DR),               # SHIFT_DR
		(PAUSE_DR, UPDATE_DR),              # EXIT1_DR
		(PAUSE_DR, EXIT2_DR),               # PAUSE_DR
		(SHIFT_DR, UPDATE_DR),              # EXIT2_DR
		(RUN_TEST_IDLE, SELECT_DR_SCAN),    # UPDATE_DR
		(CAPTURE_IR, TEST_LOGIC_RESET),     # SELECT_IR_SCAN
		(SHIFT_IR, EXIT1_IR),               # CAPTURE_IR
		(SHIFT_IR, EXIT1_IR),               # SHIFT_IR
		(PAUSE_IR, UPDATE_IR),              # EXIT1_IR
		(PAUSE_IR, EXIT2_IR),               # PAUSE_IR
		(SHIFT_IR, UPDATE_IR),              # EXIT2_IR
		(RUN_TEST_IDLE, SELECT_DR_SCAN))    # UPDATE_IR

	current = TEST_LOGIC_RESET

	def next(self, tms):
		self.current = self.NEXT[self.current - self.TEST_LOGIC_RESET][tms]

	def name(self,var = None):
		if var is None:
			var = self.current
		return [ k for k,v in TapState.__dict__.iteritems() if v is var and k.isupper() ][0]


def bitapp(dst, srcb, c, msb = False):
	assert c < 8 or c % 8 == 0
	if msb and c < 8:
		srcb[0] >>= 8 - c
	for i in xrange(c):
		dst += [ (srcb[i/8] >> (i&7)) & 1]

print BG_BLUE+"START"+OFF

cnt = 0
nop = 0

ltms = []
ltdi = []
ltdo = []
rtdo = []

reg_r = 0
reg_w = 0
reg_cnt = 0

tapst = TapState()
mcu = xPC56x(jtagc_init)

fromlinux = None
device = None

ts_first = None

while True:
	f = open(infile, 'rb')

	for ts, pkt in dpkt.pcap.Reader(f):

		#print binascii.hexlify(pkt)

		if ts_first is None:
			ts_first = ts
		ts -= ts_first

		if fromlinux is None:
			fromlinux = ((ord(pkt[1])<<8) | ord(pkt[0])) != 27

		if fromlinux:
			id, type, xtype, endp, dev, bus, fs, fd, ts_s, ts_u, st, dlen2, dlen, err, desc, intv, start, xf, ndesc = struct.unpack_from("<QBBBBHBBqllLLllllLL", pkt)
			assert dlen == len(pkt) - 64
			if dlen == 0:
				continue

			seq = pkt[64:]
		else:
			hlen, id, st, func, info, bus, dev, endp, tran, dlen = struct.unpack_from("<HQLHBHHBBL", pkt)
			if hlen != 27:
				continue
			assert dlen == len(pkt) - hlen
			seq = pkt[hlen:]

		if endp in (0, 128, 131):
			continue

		#print "   SO:", bus, dev, endp, fromlinux

		if device is None:
			if ord(seq[0]) == 0x32 and ord(seq[1]) in (0x00, 0x08, 0x60, 0x61, 0x68, 0x78):
				break
			continue
		if dev != device:
			continue


		if endp == 0x81 and seq in ("\x32\x60", "\x32\x68", "\x32\x78"):
			nop += 1
			continue

		if nop:
			print WHITE + str(cnt), "(skipping", nop, "nop)" + OFF
			cnt += nop
			nop = 0

		seq = map(ord, seq)

		print WHITE+str(cnt), "(processing", '<' if endp == 0x81 else '>', ':'.join("%02x" % v for v in seq), ")", "@%10f" % ts, OFF
		cnt += 1

		while seq:
			if endp == 2:
				if seq[0] == 0x80:
					print CYAN + "- ADbus val:{:08b}  dir:{:08b}".format(seq[1], seq[2]), OFF
					seq = seq[3:]

				elif seq[0] == 0x82:
					print CYAN + "- ACbus val:{:08b}  dir:{:08b}".format(seq[1], seq[2]), OFF
					if seq[2] & 2:
						print RED + tss(), "mcu: RESET", "off" if (seq[1] & 2) else "ON", OFF
					seq = seq[3:]
				elif seq[0] == 0x84:
					print CYAN + "- enable loopback", OFF
					seq = seq[1:]
				elif seq[0] == 0x85:
					print CYAN + "- disable loopback", OFF
					seq = seq[1:]

				elif seq[0] == 0x86:
					reg = (seq[2] << 8) | seq[1]
					clk = 6000.0 / (reg+1)
					print CYAN + "- set TCK/SK divisor reg:0x{:04x}  clk:{} kHz".format(reg, clk), OFF
					seq = seq[3:]

				elif seq[0] == 0x87:
					print CYAN + "- execute queue", rtdo, OFF
					seq = seq[1:]

				elif seq[0] == 0x8a:
					print CYAN + "- disable clock 5 divide", OFF
					seq = seq[1:]
				elif seq[0] == 0x96:
					print CYAN + "- enable adaptive clk", OFF
					seq = seq[1:]
				elif seq[0] == 0x97:
					print CYAN + "- disable adaptive clk", OFF
					seq = seq[1:]

				elif seq[0] & BIT(7) == 0:
					# generic r/w command
					efw = seq[0] & BIT(0)
					bit = seq[0] & BIT(1)
					efr = seq[0] & BIT(2)
					lsb = seq[0] & BIT(3)
					tdi = seq[0] & BIT(4)
					tdo = seq[0] & BIT(5)
					tms = seq[0] & BIT(6)

					assert tdi or tms or tdo
					assert not efw or tdi or tms
					assert not efr or tdo

					if bit:
						l = seq[1] + 1
						d = [ seq[2] ]
					else:
						l = 8 * ((seq[2] << 8) + seq[1] + 1)
						d = seq[3:3 + l/8]

					#print CYAN + "- {:02x}/{:03b} {:04b} [{}]".format(seq[0], seq[0]>>4,seq[0]&15, l), OFF

					assert lsb

					if tms:
						assert l <= 7 and bit and lsb and not tdi
						assert efw
						bitapp(ltms, d, l)
						ltdi += [ d[0] >> 7 ] * l

						# bug mpsse
						# http://developer.intra2net.com/mailarchive/html/libftdi/2009/msg00292.html
						if l == 7:
							if False:
								ltms += [ d[0] >> 7 ]
								ltdi += [ d[0] >> 7 ]
								l += 1
							else:
								print RED + "   upd bug FTDI?" + OFF
					else:
						ltms += [0] * l
						if tdi:
							assert efw
							bitapp(ltdi, d, l)
						else:
							ltdi += [2] * l

					if tdo:
						rtdo += [l]
					else:
						rtdo += [-l]

					if tms or tdi:
						if bit:
							seq = seq[3:]
						else:
							seq = seq[3 + l/8:]
					else:
						if bit:
							seq = seq[2:]
						else:
							seq = seq[3:]

				elif seq[0] == 0xab:
					print "(fake cmd)"
					seq = ""
				else:
					print RED+"todo"+OFF
					print '>', ':'.join("%02x" % v for v in seq)
					seq=""

			else:
				if seq[0] == 0x32:
					if seq[1] not in (0x00, 0x08, 0x60, 0x61, 0x68):
						print RED + "ERROR Rx" + OFF

					seq = seq[2:]
					while rtdo and seq:
						if 0 < rtdo[0]:
							avail = len(seq) * 8
							if avail < rtdo[0]:
								rtdo = [ avail, rtdo[0] - avail ] + rtdo[1:]

							bitapp(ltdo, seq, rtdo[0], True)
							del seq[:(rtdo[0] + 7) / 8]

						if rtdo[0] < 0:
							ltdo += [ 2 ] * (-rtdo[0])
						del rtdo[0]
				elif seq[0] == 0xfa:
					print "bad cmd ", ':'.join("%02x" % v for v in seq)
					seq = ""
				else:
					print RED + "ERROR RX", ':'.join("%02x" % v for v in seq), OFF
					seq = ""


			while rtdo and rtdo[0] < 0:
				ltdo += [ 2 ] * (-rtdo[0])
				del rtdo[0]

			#print "L", len(ltms), len(ltdi), len(ltdo)
			if ltms and len(ltms) == len(ltdi) == len(ltdo):

				print 'tms', ltms
				print 'tdi', ltdi
				print 'tdo', ltdo, rtdo

				#print "reg cnt:{}  R:{:x}  W:{:x}".format(reg_cnt, reg_r, reg_w) , OFF

				while ltms:

					if tapst.current in (tapst.SHIFT_DR, tapst.SHIFT_IR):
						n = tapst.name()
						for i, v in enumerate(ltms):
							if v:
								tapst.next(v)
								break
						cnt = i + 1
						print n, cnt
						r = w = 0
						# Note: unkown level sample (2) is converted to 0 (0) to make subsequent processing easier
						for i in reversed(xrange(cnt)):
							r = 2*r + (ltdo[i] & 1)
							w = 2*w + (ltdi[i] & 1)

						reg_r = (r<<reg_cnt) + reg_r
						reg_w = (w<<reg_cnt) + reg_w
						reg_cnt += cnt

						del ltdo[:cnt]
						del ltdi[:cnt]
						del ltms[:cnt]

					else:
						print tapst.name()
						if tapst.current == tapst.PAUSE_DR:
							mcu.jtagc_reset()

						elif tapst.current == tapst.UPDATE_DR:
							#if reg_cnt:
							mcu.update_dr(reg_cnt, reg_r, reg_w)
							reg_cnt = reg_r = reg_w = 0

						elif tapst.current == tapst.UPDATE_IR:
							#if reg_cnt:
							mcu.update_ir(reg_cnt, reg_r, reg_w)
							reg_cnt = reg_r = reg_w = 0

						tapst.next(ltms[0])

						del ltdo[0]
						del ltdi[0]
						del ltms[0]
	else:
		if device is None:
			print RED + "ERROR: NO DEVICE FOUND MATCHING 32xx" + OFF
		else:
			print BG_BLUE+"END"+OFF
		f.close()
		break

	print YELLOW + "Device found:", dev, OFF
	device = dev
	f.close()


