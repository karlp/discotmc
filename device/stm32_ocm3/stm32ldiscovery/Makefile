##
## This file is part of the libopencm3 project.
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

BINARY = main
# Core objects
OBJS += dscpi.o funcgen.o
# platform support
OBJS += usb_tmc.o scpi_platform.o funcgen-plat.o
# Arch support
OBJS+=arch.o funcgen-plat-arch.o

CFLAGS += -std=c99 -ggdb3
CFLAGS+=-I../../core -I../../../scpi-parser/libscpi/inc
CFLAGS+=-I.. -I.
# FIXME - this is totally going to fail!
LDFLAGS+=-L../../../scpi-parser/libscpi/dist/arm-none-eabi
LDFLAGS+= --specs=nano.specs -u _printf_float
LDLIBS+=-lscpi -lm

VPATH += ../../core
VPATH += ..

OPENCM3_DIR=../../../libopencm3
LDSCRIPT= $(OPENCM3_DIR)/lib/stm32/l1/stm32l15xxb.ld

include Makefile.include

