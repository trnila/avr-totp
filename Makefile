PROGRAMMER=usbasp
PROGRAMMER_PARAMS=-B 10
DEVICE=atmega8

CFLAGS=-mmcu=$(DEVICE) -Wall -Werror -Os -Wl,-gc-sections -ffunction-sections -D BAUD=2400 -D F_CPU=8000000 -D TIME=`date +%s`

all: main.hex

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

main.elf: main.o lcd.o uart.o hmac-sha1.o sha1.o base32.o i2c.o DS1307.o
	avr-gcc $(CFLAGS) -o $@ $^

%.o: %.c
	avr-gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f main.{hex,elf} *.o

flash: main.hex
	avrdude -e -D -p atmega8 -c $(PROGRAMMER) -U flash:w:$< $(PROGRAMMER_PARAMS)

fuse:
	avrdude -p atmega8 -c $(PROGRAMMER) -U lfuse:w:0xe4:m -U hfuse:w:0xd9:m $(PROGRAMMER_PARAMS)