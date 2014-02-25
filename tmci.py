#!/usr/bin/env python
# Basic command line tool for sending commands to the device...

import sys
import usbtmc

instr =  usbtmc.Instrument(0xc03e, 0xb007)
instr.pulse()

def do_one(args):
    for a in args:
        r = instr.ask(a)
        print(">>Requested: %s" % a)
        print("<<Response was: %s" % r)

if __name__ == "__main__":
    print("Operating with instrument: %s" % instr.ask("*IDN?"))
    do_one(sys.argv[1:])
