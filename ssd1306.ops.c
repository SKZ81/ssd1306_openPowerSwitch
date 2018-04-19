#include "ssd1306.ops.h"
#include "simple_i2c.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>

#define SSD1306_DEFAULT_ADDRESS 0x78
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFERSIZE (SSD1306_WIDTH*SSD1306_HEIGHT)/8


#define SSD1306_DEBUG 0

#if SSD1306_DEBUG
#include <stdio.h>
#endif



static const unsigned char font[] PROGMEM;


void ssd1306_send(uint8_t command) {
    i2c_start();
    i2c_write(0x00);
    i2c_write(command);
    i2c_stop();
}


void ssd1306_init() {
    i2c_init(SSD1306_DEFAULT_ADDRESS);
    // Turn display off
    ssd1306_send(SSD1306_DISPLAYOFF);

    ssd1306_send(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_send(0x80);

    ssd1306_send(SSD1306_SETMULTIPLEX);
    ssd1306_send(0x3F);

    ssd1306_send(SSD1306_SETDISPLAYOFFSET);
    ssd1306_send(0x00);

    ssd1306_send(SSD1306_SETSTARTLINE | 0x00);

    // We use internal charge pump
    ssd1306_send(SSD1306_CHARGEPUMP);
    ssd1306_send(0x14);

    // Horizontal memory mode
    ssd1306_send(SSD1306_MEMORYMODE);
    ssd1306_send(0x00);

    ssd1306_send(SSD1306_SEGREMAP | 0x1);

    ssd1306_send(SSD1306_COMSCANDEC);

    ssd1306_send(SSD1306_SETCOMPINS);
    ssd1306_send(0x12);

    // Max contrast
    ssd1306_send(SSD1306_SETCONTRAST);
    ssd1306_send(0xCF);

    ssd1306_send(SSD1306_SETPRECHARGE);
    ssd1306_send(0xF1);

    ssd1306_send(SSD1306_SETVCOMDETECT);
    ssd1306_send(0x40);

    ssd1306_send(SSD1306_DISPLAYALLON_RESUME);

    // Non-inverted display
    ssd1306_send(SSD1306_NORMALDISPLAY);

    // Turn display back on
    ssd1306_send(SSD1306_DISPLAYON);
}

/*
void ssd1306_print(uint8_t line, char* str) {
    ssd1306_clear(line);
    if (line > SSD1306_NB_LINE-1) return;

    ssd1306_send(SSD1306_COLUMNADDR);
    ssd1306_send(0x00);
    ssd1306_send(0x7F);

    ssd1306_send(SSD1306_PAGEADDR);
    ssd1306_send(line);
    ssd1306_send(line+1);

    uint8_t str_len = strlen(str);

    for (uint8_t pixel_line = 0; pixel_line < 8; pixel_line++) {
        uint8_t bytes[16] = {0};
        uint8_t offset =0;
        while (offset < 128) {
            // below str, print spaces
            uint8_t the_char = (offset/6 < str_len) ? str[offset/6] : ' ';

            if (offset%6 == 5) {
                // space between characters
#if SSD1306_DEBUG
                printf(" %c| ", the_char);
#endif
                offset++;
                continue;
            }



            if (pgm_read_byte(font + the_char*5 + offset%6) & (1<<pixel_line)) {
                bytes[offset/8] |= 1<<(7 - (offset%8));
#if SSD1306_DEBUG
                printf("*");
#endif
            }
#if SSD1306_DEBUG
            else {printf(" ");}
#endif
            offset++;
        }
        i2c_start();
        i2c_write(0x40);
        for (uint8_t byte_nb = 0; byte_nb < 16; ++byte_nb) {
            i2c_write(bytes[byte_nb]);
            printf("%02x ", bytes[byte_nb]);
        }
        i2c_stop();

// #if SSD1306_DEBUG
        printf("\n");
// #endif
    }
}
*/


void ssd1306_print(uint8_t line, char* str) {
    if (line > SSD1306_NB_LINE-1) return;

    ssd1306_clear(line);

    ssd1306_send(SSD1306_COLUMNADDR);
    ssd1306_send(0x00);
    ssd1306_send(0x7F);

    ssd1306_send(SSD1306_PAGEADDR);
    ssd1306_send(line);
    ssd1306_send(line+1);

    uint8_t str_len = strlen(str);

    for (uint8_t offset =0; offset < 128; offset++) {
        if ((offset&0x0F) == 0) {
            i2c_start();
            i2c_write(0x40);
        }
        // below str, print spaces
        uint8_t the_char = (offset/6 < str_len) ? str[offset/6] : ' ';
        uint8_t data;
        if (offset%6 == 5) {
            data = 0x00;
        } else {
            data = pgm_read_byte(font + the_char*5 + offset%6);
        }

        i2c_write(data);

        if ((offset&0x0F) == 0x0F) {
            i2c_stop();
        }
    }
}

void ssd1306_clear(uint8_t line) {
    if (line > SSD1306_NB_LINE-1) return;

    ssd1306_send(SSD1306_COLUMNADDR);
    ssd1306_send(0x00);
    ssd1306_send(0x7F);

    ssd1306_send(SSD1306_PAGEADDR);
    ssd1306_send(line);
    ssd1306_send(line+1);

    for (uint8_t pixel_line = 0; pixel_line < 8; pixel_line++) {
        i2c_start();
        i2c_write(0x40);
        for (uint8_t byte = 0; byte < 16; ++byte) {
            i2c_write(0);
        }
        i2c_stop();
    }
}

static const unsigned char ops_logo[];
#define OPS_LOGO_SIZE 768
void openPowerSwitch_logo() {
    ssd1306_clear(0);
    ssd1306_clear(7);

    ssd1306_send(SSD1306_COLUMNADDR);
    ssd1306_send(0x00);
    ssd1306_send(0x7F);

    ssd1306_send(SSD1306_PAGEADDR);
    ssd1306_send(1);
    ssd1306_send(7);

    for (uint16_t offset =0; offset < OPS_LOGO_SIZE; offset++) {
        if ((offset&0x0F) == 0) {
            i2c_start();
            i2c_write(0x40);
        }

        i2c_write(pgm_read_byte(ops_logo + offset));

        if ((offset&0x0F) == 0x0F) {
            i2c_stop();
        }
    }
}


static const unsigned char led_on[];
static const unsigned char led_off[];
#define LED_ICON_SIZE 8

void openPowerSwitch_outlet_status(uint8_t line, uint8_t status) {
    if (line>SSD1306_NB_LINE) return;

    ssd1306_clear(line);

    ssd1306_send(SSD1306_COLUMNADDR);
    ssd1306_send(0x24);
    ssd1306_send(0x5C);

    ssd1306_send(SSD1306_PAGEADDR);
    ssd1306_send(line);
    ssd1306_send(line+1);

    for (uint8_t led = 0; led < 4; led++) {
        i2c_start();
        i2c_write(0x40);

        unsigned char* icon = (status & (1 << led)) ? led_on : led_off;
        uint8_t counter=0;
        for(; counter < LED_ICON_SIZE; counter++) {
            i2c_write(pgm_read_byte(icon + counter));
        }
        while(counter < 16) {
            i2c_write(0);
            counter++;
        }
        i2c_stop();
    }
}

// Standard ASCII 5x7 font
// Copied from Adafruit's GFX lib

static const unsigned char font[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x22, 0x54, 0x54, 0x78, 0x42, // a-umlaut
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0x7D, 0x12, 0x11, 0x12, 0x7D, // A-umlaut
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x3A, 0x44, 0x44, 0x44, 0x3A, // o-umlaut
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x3D, 0x42, 0x42, 0x42, 0x3D, // O-umlaut
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0x55, 0x00, 0x55, 0x00, 0x55, // #176 (25% block) missing in old code
	0xAA, 0x55, 0xAA, 0x55, 0xAA, // 50% block
	0xFF, 0x55, 0xFF, 0x55, 0xFF, // 75% block
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0xFC, 0x4A, 0x4A, 0x4A, 0x34, // sharp-s or beta
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00  // #255 NBSP
};


static const unsigned char ops_logo[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFE,0xFE,0xCF,0x87,0x87,0xCF,0xFE,0xFE,0xFC,
0x00,0x00,0xFF,0xFF,0xFE,0xCE,0x87,0x87,0xCF,0xFF,0xFE,0x7C,0x00,0x38,0xFC,0xFE,
0xFF,0xB7,0x33,0x37,0xBF,0xBE,0xBC,0x00,0x00,0xFF,0xFF,0xFF,0x0E,0x07,0x07,0x0F,
0xFF,0xFE,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0xFC,0x1C,0x1C,0x1C,0x3C,0xFC,0xF8,0xF0,
0xE0,0x00,0x00,0x80,0x80,0x80,0x80,0x01,0x01,0x03,0x03,0x83,0x83,0x81,0x81,0x00,
0x00,0x00,0xFF,0xFF,0xFF,0x81,0x03,0x03,0x03,0x81,0x81,0x80,0x80,0x00,0x00,0x01,
0x81,0x83,0x83,0x83,0x83,0x01,0x01,0x00,0x00,0x83,0x83,0x83,0x00,0x00,0x80,0x80,
0x83,0x83,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x0E,0x0E,0x0E,0x0F,0x0F,0x07,0x03,
0x7F,0xFF,0xFF,0xE7,0xC3,0xC3,0xE7,0xFF,0xFF,0x7E,0x00,0x01,0x0F,0x3F,0xFF,0xFC,
0xF0,0xFC,0xFF,0x3F,0x3F,0xFF,0xFC,0xF0,0xFC,0xFF,0x3F,0x07,0x01,0x1C,0x7E,0xFF,
0xFF,0xDB,0x99,0x9B,0xDF,0xDF,0xDE,0x00,0x00,0xFF,0xFF,0xFF,0x0E,0x07,0x03,0x81,
0xC3,0xC3,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xE0,
0xC0,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x0F,
0x1F,0x1F,0x3F,0x3D,0x38,0x78,0xF8,0xF9,0xF1,0xE0,0x0C,0x3C,0xFC,0xFC,0xF0,0x80,
0xE0,0xF8,0xFC,0xFC,0xFC,0xF0,0x80,0xE0,0xF8,0xFC,0x7C,0x0C,0x00,0xFC,0xFD,0xFD,
0xFC,0x00,0x1C,0x1C,0xFF,0xFF,0xFF,0x1C,0x1C,0x1C,0x00,0xF0,0xF8,0xF8,0x3C,0x1C,
0x1C,0x1C,0x38,0x00,0x00,0xFF,0xFF,0xFF,0x18,0x1C,0x3C,0xFC,0xFC,0xF8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xF8,0xF8,0xC0,0xC0,0xC0,0x80,0x40,0xC0,0xC0,0x00,0x00,
0xC0,0xC0,0x40,0x00,0x00,0xC0,0xE0,0xF0,0xB0,0x30,0x30,0x20,0x00,0xF0,0xF0,0x80,
0xC0,0xE0,0x70,0x30,0x10,0x00,0x30,0x30,0x30,0xB0,0xF0,0xF0,0x70,0x00,0xE0,0xF0,
0xB0,0x30,0xB0,0xF0,0xE0,0x00,0x40,0x60,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x07,
0x0F,0x0E,0x0E,0x0E,0x0E,0x0F,0x07,0x07,0x03,0x01,0x00,0x00,0x01,0x07,0x0F,0x0F,
0x0F,0x07,0x00,0x00,0x07,0x0F,0x0F,0x0F,0x07,0x01,0x00,0x00,0x00,0x0F,0x0F,0x0F,
0x0F,0x00,0x00,0x00,0x07,0x0F,0x0F,0x0E,0x0E,0x0E,0x00,0x03,0x07,0x07,0x0F,0x0E,
0x0E,0x0E,0x07,0x00,0x00,0x0F,0x0F,0x0F,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x18,0x1D,0x1F,0x0F,0x00,0x87,0xCF,0xFC,0x7C,
0x0F,0x07,0x00,0x00,0x00,0x0C,0x19,0x19,0x1B,0x1F,0x0F,0x06,0x00,0x1F,0x1F,0x01,
0x03,0x0F,0x1E,0x1C,0x10,0x00,0x1C,0x1E,0x1F,0x1B,0x19,0x18,0x18,0x00,0x0E,0x1F,
0x1B,0x19,0x1B,0x1F,0x0E,0x00,0x18,0x18,0x1F,0x1F,0x18,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const unsigned char led_on[] PROGMEM = {
0x3C,0x42,0x99,0xB5,0xBD,0x99,0x42,0x3C
};

static const unsigned char led_off[] PROGMEM = {
0x3C,0x42,0x81,0x81,0x81,0x81,0x42,0x3C
};
