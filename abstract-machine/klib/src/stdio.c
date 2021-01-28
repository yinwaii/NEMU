#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

struct param {
  char uc : 1;
  char sign;
  char blank;
  int length;
  unsigned int base;
  char *str;
};

typedef void (*printer)(void *, char);

struct outstream {
  char *dest;
  size_t num_chars;
};

static void vsprintf_printer(void *outstream, char c) {
  struct outstream *data = (struct outstream *)outstream;
  data->dest[data->num_chars] = c;
  data->num_chars++;
}

static void printf_printer(void *outstream, char c) {
  putch(c);
}

static void printstr(void *outstream, printer printer, struct param *p)
{
  int str_length = strlen(p->str);
  if (p->sign != '\0')
    str_length++;
  if (p->blank != '\0')
  {
    int blank_length = p->length - str_length;
    while (blank_length > 0)
    {
      printer(outstream, p->blank);
      blank_length--;
    }
  }
  if (p->sign != '\0')
    printer(outstream, p->sign);
  for (int i = 0; p->str[i] != '\0'; i++)
    printer(outstream, p->str[i]);
  p->blank = '\0';
}

static void ui2a(unsigned int num, struct param *p) {
  int n = 0;
  unsigned int d = 1;
  int p_str = 0;
  while (num / d >= p->base)
    d *= p->base;
  while (d != 0)
  {
    int dgt = num / d;
    num %= d;
    d /= p->base;
    if (n || dgt > 0 || d == 0)
    {
      p->str[p_str] = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
      p_str++;
      n++;
    }
  }
  p->str[p_str] = '\0';
}

static void i2a(int num, struct param *p) {
  if (num < 0)
  {
    num = -num;
    p->sign = '-';
  }
  ui2a(num, p);
}

void my_format(void *outstream, printer printer, const char *fmt, va_list va) {
  struct param p;
  char str[12];
  p.str = str;
  p.blank = '\0';
  bool control = false;
  for (int i = 0; fmt[i] != '\0'; i++)
  {
    if (control) {
      p.sign = '\0';
      switch (fmt[i])
      {
      case '0':
        p.blank = '0';
        i++;
        assert(fmt[i] != '\0');
        p.length = fmt[i] - '0';
        break;
      case 'd':
        p.base = 10;
        p.uc = 0;
        i2a(va_arg(va, int), &p);
        printstr(outstream, printer, &p);
        control = false;
        break;
      case 'x':
        p.base = 16;
        p.uc = 0;
        i2a(va_arg(va, int), &p);
        printstr(outstream, printer, &p);
        control = false;
        break;
      case 'p':
        printf("0x");
        p.blank = '0';
        p.length = 8;
        p.base = 16;
        p.uc = 0;
        ui2a((unsigned)va_arg(va, void *), &p);
        printstr(outstream, printer, &p);
        control = false;
        break;
      case 's':
        p.str = va_arg(va, char *);
        printstr(outstream, printer, &p);
        control = false;
        p.str = str;
        break;
      case 'c':
        p.str[0] = '\0' + (va_arg(va, int));
        p.str[1] = '\0';
        printstr(outstream, printer, &p);
        control = false;
        break;
      case '%':
        p.str[0] = '%';
        p.str[1] = '\0';
        printstr(outstream, printer, &p);
        control = false;
        break;
      default:
        control = false;
        printf("\33[1;31m please implement %%%c in printf! \33[0m", fmt[i]);
        break;
      }
    }
    else {
      if (fmt[i] == '%')
        control = true;
      else
        printer(outstream, fmt[i]);
    }
  }
}

int printf(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  // char *out = malloc((strlen(fmt) + 1) * sizeof(char));
  my_format(NULL, printf_printer, fmt, ap);
  // memset((void *)out, (int)'\0', strlen(out));
  // free(out);
  va_end(ap);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  struct outstream data;
  data.dest = out;
  data.num_chars = 0;
  my_format(&data, vsprintf_printer, fmt, ap);
  data.dest[data.num_chars] = '\0';
  return data.num_chars;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vsprintf(out, fmt, ap);
  va_end(ap);
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int res = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return res;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  // char *tmp_out = malloc((strlen(fmt) + 1) * sizeof(char));
  // maybe wrong
  printf("please fix the vsnprintf!\n");
  char tmp_out[100];
  int res = vsprintf(tmp_out, fmt, ap);
  strncpy(out, tmp_out, n - 1);
  out[n - 1] = '\0';
  // free(tmp_out);
  if (res >= n)
    return n - 1;
  else
    return res;
}

#endif
