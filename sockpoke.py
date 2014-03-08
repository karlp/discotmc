#!/usr/bin/env python

import socket
import errno
import time


class Instrument():
    def __init__(self, host, port=5025):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.conn_tuple = (host, port)

    def __enter__(self):
        self.s.connect(self.conn_tuple)
        self.s.setblocking(False)
        return self

    def __exit__(self, type, value, traceback):
        self.s.close()

    def tell(self, q):
        self.s.send(q)

    def ask(self, q):
        self.s.send(q)
        self.s.send("\r\n")
        data = ''
        waiting = True
        while waiting:
            try:
                data += self.s.recv(1024)
            except socket.error, e:
                if e.errno != errno.EWOULDBLOCK:
                    print("weird unexpected error: %r" % e)
                else:
                    print("would block, need to wait a bit longer I think?: %r" % e)
                    time.sleep(0.1)
            if data[-2:] == "\r\n":
                print("Found the end of a reply, we're good now!")
                waiting = False

        return data


with Instrument("localhost") as instr:
    print("idn:", instr.ask("*IDN?"))
    print("output1 state:", instr.ask("OUTP1?"))
    print("idn3:", instr.ask("*IDN?"))
    print(instr.tell("SOUR1:APPLy:SINe"))
    print("idn4:", instr.ask("*IDN?"))
    print(instr.ask("SOUR1:APPLy?"))
    #print("*rst:", instr.ask("*RST"))
    #print(instr.ask("OUTP1 ON"))
    #print("hoho", instr.ask("SOUR1:ARB:OUTPut"))

