#ifndef MOCKOTO_TEST_CHIBI_BYTES_H
#define MOCKOTO_TEST_CHIBI_BYTES_H

typedef unsigned char uint8_t;

int read_byte(uint8_t *out);
int read_text(char *out);
int write_bytes(const uint8_t *buf, uint8_t len);
int write_text(const char *s);

#endif
