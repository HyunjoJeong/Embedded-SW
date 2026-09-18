#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

/* HD44780 연결: PB0~PB7=D0~D7, E=PD4, RS=PD5 (R/W=GND)
 * 4-bit 모드에서는 D4~D7, 즉 PB4~PB7만 사용한다.
 */
#define LCD_EN_DDR   DDRD
#define LCD_EN_PORT  PORTD
#define LCD_EN_BIT   4

#define LCD_RS_DDR   DDRD
#define LCD_RS_PORT  PORTD
#define LCD_RS_BIT   5

#define LCD_DB_DDR   DDRB
#define LCD_DB_PORT  PORTB
#define LCD_DB_MASK  0xF0

static void lcd_write_nibble(uint8_t rs, uint8_t data)
{
  if (rs) {
    LCD_RS_PORT |= _BV(LCD_RS_BIT);
  } else {
    LCD_RS_PORT &= (uint8_t)~_BV(LCD_RS_BIT);
  }

  LCD_DB_PORT = (LCD_DB_PORT & (uint8_t)~LCD_DB_MASK) |
                ((data << 4) & LCD_DB_MASK);
  _delay_us(1);
  LCD_EN_PORT |= _BV(LCD_EN_BIT);
  _delay_us(1);
  LCD_EN_PORT &= (uint8_t)~_BV(LCD_EN_BIT);
  _delay_us(1);
}

static void lcd_write_byte(uint8_t rs, uint8_t data)
{
  lcd_write_nibble(rs, data >> 4);  /* 상위 4 bit 먼저 전송 */
  lcd_write_nibble(rs, data & 0x0F);
}

static void lcd_init(void)
{
  LCD_EN_DDR |= _BV(LCD_EN_BIT);
  LCD_RS_DDR |= _BV(LCD_RS_BIT);
  LCD_DB_DDR |= LCD_DB_MASK;

  LCD_EN_PORT &= (uint8_t)~_BV(LCD_EN_BIT);
  LCD_RS_PORT &= (uint8_t)~_BV(LCD_RS_BIT);

  _delay_ms(20);
  lcd_write_nibble(0, 0x03);       /* 8-bit 초기화 단계 */
  _delay_ms(5);
  lcd_write_nibble(0, 0x03);
  _delay_us(100);
  lcd_write_nibble(0, 0x03);
  _delay_us(40);
  lcd_write_nibble(0, 0x02);       /* 4-bit 모드로 전환 */
  _delay_us(40);

  lcd_write_byte(0, 0x28);         /* 4-bit, 2-line, 5x8 font */
  _delay_us(40);
  lcd_write_byte(0, 0x08);         /* Display off */
  _delay_us(40);
  lcd_write_byte(0, 0x01);         /* Display clear */
  _delay_ms(2);
  lcd_write_byte(0, 0x06);         /* Entry mode: increment */
  _delay_us(40);
  lcd_write_byte(0, 0x0C);         /* Display on */
  _delay_us(40);
}

static void lcd_put_line(uint8_t address, const char *str)
{
  uint8_t i;

  lcd_write_byte(0, address);
  _delay_us(40);
  for (i = 0; i < 16; i++) {
    if (*str != '\0') {
      lcd_write_byte(1, *str++);
    } else {
      lcd_write_byte(1, ' ');
    }
    _delay_us(40);
  }
}

/* CGRAM 문자 0: 하트, 문자 1: 웃는 얼굴 */
static const uint8_t custom5x8[] = {
  0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00,
  0x00, 0x00, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00
};

static void lcd_load_custom_fonts(void)
{
  uint8_t i;

  lcd_write_byte(0, 0x40);         /* CGRAM address 0x00 */
  _delay_us(40);
  for (i = 0; i < sizeof(custom5x8); i++) {
    lcd_write_byte(1, custom5x8[i]);
    _delay_us(40);
  }
}

static void lcd_show_frame(uint8_t position)
{
  uint8_t i;

  lcd_put_line(0x80, "4bit CGRAM Demo");
  lcd_write_byte(0, 0xC0);
  _delay_us(40);
  for (i = 0; i < 16; i++) {
    if (i == position) {
      lcd_write_byte(1, 0);         /* 사용자 문자 0: 하트 */
    } else if (i == 15 - position) {
      lcd_write_byte(1, 1);         /* 사용자 문자 1: 웃는 얼굴 */
    } else {
      lcd_write_byte(1, ' ');
    }
    _delay_us(40);
  }
}

int main(void)
{
  uint8_t position = 0;

  lcd_init();
  lcd_load_custom_fonts();

  while (1) {
    lcd_show_frame(position);
    position = (position + 1) % 16;
    _delay_ms(250);
  }
}
