MCU=attiny4313
F_CPU=8000000UL
PROG=usbasp
TARGET=main

.PHONY: all flash unflash fuse rdfuse clean

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -o $@ $<

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@
	avr-size --mcu=$(MCU) -C $<

# 실행파일 업로드
flash: $(TARGET).hex
	avrdude -c $(PROG) -p t4313 -U flash:w:$(TARGET).hex:i

# 업로드된 실행파일 제거
unflash:
	avrdude -c $(PROG) -p t4313 -e

# 퓨즈 설정 Write (8Mhz로 동작하게)
fuse:
	avrdude -c $(PROG) -p t4313 -U lfuse:w:0xe4:m

# 퓨즈 설정 Read
rdfuse:
	avrdude -c $(PROG) -p t4313 -U lfuse:r:-:h

clean:
	rm -f $(TARGET).elf $(TARGET).hex
