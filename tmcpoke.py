#!/usr/bin/env python

import usbtmc

instr =  usbtmc.Instrument(0xc03e, 0xb007)
instr.pulse()
print(instr.ask("*IDN?"))
#print(instr.ask("OUTP1?"))
#print(instr.ask("OUTP1 ON"))
#print(instr.ask("SOUR1:APPLy:SIN 7000 Hz"))
#print(instr.ask("OUTP1 OFF"))
#print(instr.ask("*RST"))
