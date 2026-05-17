#ifndef MOCKOTO_TEST_CHIBI_C_H
#define MOCKOTO_TEST_CHIBI_C_H

#include <stdint.h>

uint8_t twi_start(void);
uint8_t twi_write(uint8_t data);
uint8_t twi_read_nack(uint8_t *data);
void twi_stop(void);
uint8_t twi_write_regs(uint8_t addr7, uint8_t reg, const uint8_t *data,
                       uint8_t len);

#endif
