
This is a usbtmc 2 channel function generator on an stm32F4 discovery board.
Bandwidth and all that sort of stuff is limited, we're using the onboard DAC,
but the flexibility can't be beat.

Current status:
* dual channel sine wave output, 1hz-20khz.
  (granularity is terrible above ~4kHz, but that's fixable)
* Load arbitrary data and output. (lots of bugs in data bigger than usb frames)
* still bugs in usb comms, need to reset the board far too often.

getting started....

```
git submodule update --init
```

Device firmware portion
=======================
```
make -C libopencm3
make -C scpi-parser/libscpi
make -C device/stm32f4
make -C device/stm32f4 flash  # assumes openocd installed and operational
```

See device/native-tcp-server for a host build of the core functionality. (No
DAC, but all the wavetable generation and SCPI parsing code can be tested)

host side
=========
```
sudo cp 68-disco-tmc.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
[plug in or reset board here]
virtualenv --system-site-packages .venv
. .venv/bin/activate
pip install -e python-usbtmc
python tmcpoke.py   # should toggle the red led on the board
```
