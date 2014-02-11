#!/usr/bin/env python
# Generate a C header file with a sine wave lookup table

import pylab
import numpy as np
LEN=32
BITS=12
FULL_SCALE=3.0

def make_base_table():
    # for 0:2*pi, take LEN steps and calculate sine, exploded to 2^BITS range, centered about 0 (-2^BITS-1:2^BITS-1)
    nu = np.linspace(0, 2*np.pi, LEN+1)[:LEN]
    print(nu)
    sins = np.sin(nu)
    print(sins)
    sinsbin = sins * pow(2, BITS-1)
    print(sinsbin)
    outp = sinsbin + pow(2, BITS-1) - 1
    print(np.around(outp))
    fig2 = pylab.figure()
    ax2 = fig2.add_subplot(111)
    ax2.plot(nu, np.around(outp), label="mymodel")
    #ax2.plot(nu, np.around(sinsbin))
    st_data = [	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647]
    #ax2.plot(nu, st_data, label="st data")
    print(len(st_data))
    print(len(outp))
    pylab.legend()
    #pylab.show()
    return np.around(sinsbin)

def scale_offset(vals, ampl, offset_volts):
    assert(ampl <= FULL_SCALE)
    offset_bits = offset_volts / FULL_SCALE * pow(2, BITS) - 1
    scaled = [x * ampl/FULL_SCALE + offset_bits for x in vals]
    return scaled


def gomainzz():
    vals = make_base_table()
    # Unmodified, we're centered around 0, which makes it easier to do the maths for amplitude scaling
    print("raw input sine table", vals)
    print("fullscale at 1.5V", scale_offset(vals, 3.0, 1.5))
    print("half scale at 1.5V", scale_offset(vals, 1.5, 1.5))


def gomain():
    st_data = [	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647]

    # Want to interpolate to 1024 values
    output = []
    for oi in range(0, 64):
        















if __name__ == "__main__":
    gomain()

