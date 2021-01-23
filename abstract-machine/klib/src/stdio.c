#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list sArgv;
  va_start(sArgv, fmt);
  int p_out = 0;
  for (int i = 0; fmt[i] != '\0'; i++)
  {
    if (fmt[i] != '%')
    {
      out[p_out] = fmt[i];
      p_out++;
    }
    else
    {
      assert(fmt[i + 1] != '\0');
      i++;
      if (fmt[i] == 'd')
      {
        int num_dec = va_arg(sArgv, int);
        char num_str[10];
        int p_num_str = 0;
        while (num_dec != 0)
        {
          num_str[p_num_str] = '0' + (num_dec % 10);
          p_num_str++;
          num_dec /= 10;
        }
        for (int j = p_num_str - 1; j >= 0; j--)
        {
          out[p_out] = num_str[j];
          p_out++;
        }
      }
      else if (fmt[i] == 's')
      {
        char *str = va_arg(sArgv, char *);
        strcat(out, str);
        p_out += strlen(str);
      }
    }
  }
  out[p_out] = '\0';
  va_end(sArgv);
  return p_out;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
