#ifndef __SIMPLE_I2C__
#define __SIMPLE_I2C__

#include <stdint.h>

#define SCL_CLOCK  100000L

void    i2c_init(uint8_t address);
uint8_t i2c_start();
uint8_t i2c_write(uint8_t data);
void    i2c_stop(void);



#endif
