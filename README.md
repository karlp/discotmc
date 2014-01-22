
This is a usbtmc 2 channel function generator on an stm32F4 discovery board.
Bandwidth and all that sort of stuff is limited, we're using the onboard DAC,
but the flexibility can't be beat.

getting started....

```
make -C libopencm3
make -C f4-usbtmc
make -C f4-usbtmc flash  # assumes openocd installed and operational
```

host side......

```
virtualenv --use-site-packages .venv
. .venv/bin/activate
pip install -e python-usbtmc
python tmcpoke.py   # should toggle the red led on the board
```
