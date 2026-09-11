#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

/* HD44780 연결: DB0~DB7=PB0~PB7, E=PD4, RS=PD5 (R/W=GND) */
#define LCD_EN_DDR   DDRD
#define LCD_EN_PORT  PORTD
#define LCD_EN_BIT   4

#define LCD_RS_DDR   DDRD
#define LCD_RS_PORT  PORTD
#define LCD_RS_BIT   5

#define LCD_DB_DDR   DDRB
#define LCD_DB_PORT  PORTB

static void lcd_write_byte(uint8_t rs, uint8_t data)
{
  if (rs) {
    LCD_RS_PORT |= _BV(LCD_RS_BIT);
  } else {
    LCD_RS_PORT &= (uint8_t)~_BV(LCD_RS_BIT);
  }

  LCD_DB_PORT = data;
  LCD_EN_PORT |= _BV(LCD_EN_BIT);
  LCD_EN_PORT &= (uint8_t)~_BV(LCD_EN_BIT);
}

static void lcd_init(void)
{
  LCD_EN_DDR |= _BV(LCD_EN_BIT);
  LCD_RS_DDR |= _BV(LCD_RS_BIT);
  LCD_DB_DDR = 0xFF;

  LCD_EN_PORT &= (uint8_t)~_BV(LCD_EN_BIT);
  LCD_RS_PORT &= (uint8_t)~_BV(LCD_RS_BIT);

  _delay_ms(20);                 /* Power-on reset 완료 대기 */
  lcd_write_byte(0, 0x38);       /* 8-bit, 2-line, 5x8 font */
  _delay_us(40);
  lcd_write_byte(0, 0x0C);       /* Display on, Cursor/Blink off */
  _delay_us(40);
  lcd_write_byte(0, 0x06);       /* 입력 뒤 DDRAM 주소 증가 */
  _delay_us(40);
  lcd_write_byte(0, 0x01);       /* Display clear */
  _delay_ms(2);
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

static void lcd_puts(const char *line1, const char *line2)
{
  lcd_put_line(0x80, line1);     /* 첫째 줄 */
  lcd_put_line(0xC0, line2);     /* 둘째 줄 */
}

int main(void)
{
  uint8_t seconds = 0;
  char message[] = "00 seconds";

  lcd_init();
  lcd_puts("Timer Start!", "");
  _delay_ms(1000);

  lcd_write_byte(0, 0x01);
  _delay_ms(2);

  while (seconds < 100) {
    message[0] = (seconds / 10) + '0';
    message[1] = (seconds % 10) + '0';
    lcd_puts("Running...", message);

    seconds++;
    _delay_ms(1000);
  }

  lcd_write_byte(0, 0x01);
  _delay_ms(2);
  lcd_puts("Times UP!", "");

  while (1) {
  }
}
