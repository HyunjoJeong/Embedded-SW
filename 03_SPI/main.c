#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define MAX_CS_DDR    DDRB
#define MAX_CS_PORT   PORTB
#define MAX_CS_BIT    5

// Bit-Bang 방식
static void SpiTx_BB (uint8_t data) {
  for (uint8_t i=0; i<8; i++) {
    uint8_t bit = (data>>(7-i)) & 1;  // MSB 부터 차례로 1 bit 추출
    PORTB &= ~(1 << 6);               // PB6 핀에 0 출력
    PORTB |= (bit << 6);              // PB6 핀에 bit 값 출력
    PORTB |= (1 << 7);                // PB7 핀에 1 출력
    PORTB &= ~(1 << 7);               // PB7 핀에 0 출력
  }
}

// USI 방식
static void SpiTx_USI (uint8_t data) {
  USIDR = data;
  for (uint8_t i=0; i<8; i++) {
    USICR = _BV(USIWM0) | _BV(USITC);
    USICR = _BV(USIWM0) | _BV(USICLK) | _BV(USITC);
  }
}

static void max7219_write (uint8_t addr, uint8_t data) {
  MAX_CS_PORT &= ~(1 << MAX_CS_BIT);  // CS 핀에 0 출력 (전송 시작)
  SpiTx_BB(addr);                     // 8-bit addr 전송
  SpiTx_BB(data);                     // 8-bit data 전송
  MAX_CS_PORT |= (1 << MAX_CS_BIT);   // CS 핀에 1 출력 (전송 종료)
}

static void max7219_init (void) {
  const uint8_t initData[] = { 0x09, 0xff, 0x0a, 0x01, 0x0b, 0x07, 0x0c, 0x01, 0x0f, 0x00 };

  DDRB |= (1 << 6); // PB6 핀을 출력 모드로 설정
  DDRB |= (1 << 7); // PB7 핀을 출력 모드로 설정
  MAX_CS_DDR |= (1 << MAX_CS_BIT);  // CS 핀을 출력 모드로 설정
  MAX_CS_PORT |= (1 << MAX_CS_BIT); // CS 핀에 1 출력

  // initData[] 값을 data와 addr의 piar로 max7219_write 함수에 보냄
  for (unsigned i=0; i<sizeof(initData); i+=2) {
    max7219_write(initData[i], initData[i+1]);
  }
}

int main (void) {
  max7219_init();
  while (1) {
    for (uint8_t number=0; number<10; number++) {
      for (uint8_t digit=0; digit<8; digit ++) {
        max7219_write((7-digit)+1, (number+digit)%10);
      }
      _delay_ms(500);
    }
  }
}