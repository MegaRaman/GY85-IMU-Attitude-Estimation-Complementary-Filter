#ifndef INC_STRPRINT_H_
#define INC_STRPRINT_H_

#include <stdarg.h>

/*
 * strprint - Analog of sprintf that supports %d and %f printing formats.
 * @str - Pointer to buffer where formatted message will be copied
 * @format - Format string, analogous to that in regular printf/sprintf
 *
 * Returns the length of copied message
 * */
int strprint(char *str, const char *format, ...);

#endif /* INC_STRPRINT_H_ */
