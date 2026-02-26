#ifndef LOTTO_LOGGER_H
#define LOTTO_LOGGER_H

#include <stdio.h>

enum logger_level {
    LOGGER_ERROR,
    LOGGER_WARN,
    LOGGER_INFO,
    LOGGER_DEBUG,
};

void logger_set_level(enum logger_level l);
FILE *logger_fp(void);
void logger(enum logger_level l, FILE *fp);

void logger_fatalf(const char *fmt, ...);
void logger_errorf(const char *fmt, ...);
void logger_warnf(const char *fmt, ...);
void logger_infof(const char *fmt, ...);
void logger_debugf(const char *fmt, ...);
void logger_printf(const char *fmt, ...);

#endif
