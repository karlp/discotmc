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

CFLAGS += -std=c99 -ggdb3
CFLAGS+=-I../core -I../../scpi-parser/libscpi/inc
LDFLAGS+=-L../../scpi-parser/libscpi/dist/arm-none-eabi
LDFLAGS+= --specs=nano.specs
LDLIBS+=-lscpi -lm

VPATH += ../core

LDSCRIPT = stm32f4-discovery.ld

include Makefile.include

