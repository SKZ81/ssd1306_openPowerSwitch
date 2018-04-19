#include "simple_i2c.h"
#include <util/twi.h>


static uint8_t address;
#define twi_status_register (TW_STATUS & 0xF8)


void i2c_init(uint8_t slave_address) {
    address = slave_address;
    TWSR = 0;
    TWBR = ((F_CPU/SCL_CLOCK)-16)/2;
}

uint8_t i2c_start() {
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));

    if ((twi_status_register != TW_START) && (twi_status_register != TW_REP_START)) {
        return 1;
    }

    TWDR = address;
    TWCR = (1<<TWINT) | (1<<TWEN);

    while(!(TWCR & (1<<TWINT)));

    if ((twi_status_register != TW_MT_SLA_ACK) && (twi_status_register != TW_MR_SLA_ACK)) {
        return 1;
    }

    return 0;
}

uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);

    while(!(TWCR & (1<<TWINT)));

    if (twi_status_register != TW_MT_DATA_ACK) {
        return 1;
    } else {
        return 0;
    }
}

void i2c_stop(void) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    while(TWCR & (1<<TWSTO));
}
