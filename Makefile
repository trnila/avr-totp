PROGRAMMER=usbasp
PROGRAMMER_PARAMS=-B 10
DEVICE=atmega8

CFLAGS=-mmcu=$(DEVICE) -Wall -Werror -Os -Wl,-gc-sections -ffunction-sections -D BAUD=2400 -D F_CPU=1000000

all: main.hex

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.elf: main.o uart.o
	avr-gcc $(CFLAGS) -o $@ $^

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f main.{hex,elf} *.o

flash: main.hex
	avrdude -e -D -p atmega8 -c $(PROGRAMMER) -U flash:w:$< $(PROGRAMMER_PARAMS)

