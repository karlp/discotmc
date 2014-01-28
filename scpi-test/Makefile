CFLAGS+=-I../scpi-parser/libscpi/inc
LDFLAGS+=-L../scpi-parser/libscpi/dist/x86_64-redhat-linux
LDLIBS+=-lscpi

CFLAGS	+=-g -Wextra -Wshadow -Wredundant-decls -Wmissing-prototypes
CFLAGS	+= -Wstrict-prototypes -Wimplicit-function-declaration
CFLAGS	+=

OBJS=t.o

CC=gcc
LD=gcc

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:


all: ttt.elf

ttt.elf: $(OBJS)
	@#printf "  LD      $(*).elf\n"
	$(Q)$(LD) $(LDFLAGS) $(ARCH_FLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.o: %.c
	@#printf "  CC      $(*).c\n"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(*).o -c $(*).c

clean:
	$(RM) $(OBJS)
	$(RM) ttt.elf
