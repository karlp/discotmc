
This is a usbtmc 2 channel function generator on an stm32F4 discovery board.
Bandwidth and all that sort of stuff is limited, we're using the onboard DAC,
but the flexibility can't be beat.

getting started....

```
git submodule update --init
```

Device firmware portion
=======================
```
make -C libopencm3
make -C scpi-parser/libscpi
make -C f4-usbtmc
make -C f4-usbtmc flash  # assumes openocd installed and operational
```

This has some basic support for operating both on the host (as a TCP server)
and on the F4 as a USB device.

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
