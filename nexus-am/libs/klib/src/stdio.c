#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/* pa2.2
 * 2020-12-6
 * reference the implement of 'printf' in glibc
 * web: http://www.gnu.org/software/libc/
 */
int printf(const char *fmt, ...) {
  va_list arg;
  int done;
  char str[1024];

  va_start(arg, fmt);
  done = vsprintf(str, fmt, arg);
  va_end(arg);

  int i = 0;
  while (str[i] != '\0')
  {
    _putc(str[i++]);
  }
  
  return done;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str = out;
  int ret = 0;
  for ( ; *fmt != '\0'; fmt++) {
    if(*fmt != '%' || *(fmt + 1) == '%') {
      *str++ = *fmt;
      ret++;
      continue;
    }
    fmt++;
    switch(*fmt) {
      case 's':
      {
        const char* t = va_arg(ap, char*);
        int len = strlen(t);
        int i = 0;
        for (; i < len; i++) {
          *str++ = *t++;
        }
        ret += len;
        break;
      }
      case 'd':
      {
        int num = va_arg(ap,int);
        if (num == 0) {
          *str++ = '0';
        }
        else {
          if (num < 0) {
            *str++ = '-';
            num = -num;
          }
          /* in this step, we get a reversed string,
           * e.g: "1234" will be transfer "4321"
           */
          int len = 0;
          while (num != 0) {
            *str++ = num % 10 + '0';
            len++;
            num /= 10;
          }
          ret += len;
          // reverse the string
          int i = 1;
          int mid = len / 2;
          char tmp;
          for ( ; i <= mid; i++) {
            tmp = *(str - (len - i + 1));
            *(str - (len - i + 1)) = *(str - i);
            *(str - i) = tmp;
          }
        }
        break;
      }
      case 'c':
      {
        *str++ = (char)va_arg(ap,int);
        ret++;
        break;
      }
      case 'x':
      case 'X':
      case 'p':
      {
        *str++ = '0';
        *str++ = 'x';
        uint32_t hex = va_arg(ap,uint32_t);
        if (hex == 0) {
          *str++ = '0';
          ret += 3;
        }
        else {
          /* in this step, we get a reversed string,
           * e.g: "abcd" will be transfer "dcba"
           */
          int len = 0;
          while (hex != 0) {
            uint32_t tmp = hex % 0x10;
            if (tmp < 0xa) {
              *str++ = tmp + '0';
            }
            else {
              *str++ = tmp + 'a' - 0xa;
            }
            len++;
            hex /= 16;
          }
          // reverse the string
          int i = 1;
          int mid = len / 2;
          char tmp;
          for ( ; i <= mid; i++) {
            tmp = *(str - (len - i + 1));
            *(str - (len - i + 1)) = *(str - i);
            *(str - i) = tmp;
          }
          ret += (len + 2);
        }
        break;
      }  
      default:
        ret = -1;
        break;
    }
  }
  *str = '\0';
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list arg;
  int done;

  va_start(arg, fmt);
  done = vsprintf(out, fmt, arg);
  va_end(arg);

  return done;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
