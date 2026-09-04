#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  DDRB |= 0b00000010;

  while (1) {
    PORTB |= 0b00000010;
    _delay_ms(500);
    
    PORTB &= 0b11111101;
    _delay_ms(500);
  }
}