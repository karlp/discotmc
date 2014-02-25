#!/usr/bin/env python

import usbtmc

instr =  usbtmc.Instrument(0xc03e, 0xb007)
instr.pulse()
print("idn:", instr.ask("*IDN?"))
print("outp1?:", instr.ask("OUTP1?"))
print("outp1 on:", instr.ask("OUTP1 ON"))
print("outp1?:", instr.ask("OUTP1?"))
print("sour1:apply?", instr.ask("SOUR1:apply?"))
print("sour1:apply:sin 7000 Hz", instr.ask("SOUR1:APPLy:SIN 7000 Hz"))
print("sour1:apply?", instr.ask("SOUR1:apply?"))
print("sour1:apply:sin 2.2 KHz, 456mv, 250mv", instr.ask("SOUR1:APPLy:SIN 2.2 KHz, 456mv, 250mv"))
print("sour1:apply?", instr.ask("SOUR1:apply?"))
print("outp1 off:", instr.ask("OUTP1 off"))
print("outp1?:", instr.ask("OUTP1?"))
#print(instr.ask("*RST"))
