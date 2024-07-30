#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "strprint.h"

#define FLOAT_PRECISION 1e3       /* Number of digits after the decimal part */

/*
 * Convert int to string, copy it to str and return length
 * */
int copy_int(int i, char *str) {
    int len = 0;
    bool is_negative = false;
    if (i < 0) {
        is_negative = true;
        i = -i;
    }
    do {
        char digit = (i % 10) + '0';
        if (!isdigit(digit))
            return -1;
        str[len++]= digit;
    } while ((i /= 10) > 0);

    if (is_negative)
        str[len++] = '-';

    // reverse string
    for (int j = 0, k = len - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
    return len;
}

int copy_float(float f, char *str) {
    int len = 0;
    int temp = 0;
    if (f < 0) {
        str[len++] = '-';
        f = -f;
    }
    int ipart = (int)f;
    float fpart = f - ipart;
    temp = copy_int(ipart, str + len);

    if (temp < 0)
        return -1;

    len += temp;

    str[len++] = '.';

    int formatted_fpart = fpart * FLOAT_PRECISION;
    if (formatted_fpart < 10) {
        str[len++] = '0';
        str[len++] = '0';
    }
    else if (formatted_fpart < 100) {
        str[len++] = '0';
    }
    temp = copy_int(formatted_fpart, str + len);

    if (temp < 0)
        return -1;

    len += temp;
    return len;
}

int strprint(char *str, const char *format, ...) {
    int ret = 0;
    int len, strl;
    int i;
    float f;
    char *s;
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd':
                    i = va_arg(args, int);
                    len = copy_int(i, str);
                    if (len < 0)
                        goto err;
                    str += len;
                    ret += len;
                    break;
                case 's':
                    strl = strlen(s);
                    s = va_arg(args, char*);
                    memcpy(str, s, strl);
                    str += strl;
                    ret += strl;
                    break;
                case 'f':
                    f = va_arg(args, double);
                    len = copy_float(f, str);
                    if (len < 0)
                        goto err;
                    str += len;
                    ret += len;
                    break;
                default:
                    goto err;
            }
        }
        else {
            *str = *p;
            str++;
            ret++;
        }
    }
    va_end(args);
    return ret;

err:
    va_end(args);
    return -1;
}

