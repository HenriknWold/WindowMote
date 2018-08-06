#ifndef __NOKIA_5110_H__
#define __NOKIA_5110_H__

#include <avr/pgmspace.h>
#include <stdint.h>


#define PORT_LCD PORTH
#define DDR_LCD DDRH


#define LCD_SCE PH1
#define LCD_RST PH2
#define LCD_DC PH3
#define LCD_DIN PH4
#define LCD_CLK PH5

#define LCD_CONTRAST 0x40


void nokia_lcd_init(void);


void nokia_lcd_clear(void);


void nokia_lcd_power(uint8_t on);


void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);

void nokia_lcd_write_char(char code, uint8_t scale);


void nokia_lcd_write_string(const char *str, uint8_t scale);

void nokia_lcd_set_cursor(uint8_t x, uint8_t y);


void nokia_lcd_render(void);


#endif
