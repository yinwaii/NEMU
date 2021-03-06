#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  const int reg_num = (sizeof(regsl)) / (sizeof(regsl[0])) + 5;
  printf("The status of registers:\n");
  for (int i = 0; i < reg_num; i++)
  {
    switch (i)
    {
      case reg_num - 1:
        printf("%-15s%-#30x%u\n", "pc", cpu.pc, cpu.pc);
        break;
      case reg_num - 2:
        printf("%-15s%-#30x%u\n", "OF", cpu.eflags.OF, cpu.eflags.OF);
        break;
      case reg_num - 3:
        printf("%-15s%-#30x%u\n", "ZF", cpu.eflags.ZF, cpu.eflags.ZF);
        break;
      case reg_num - 4:
        printf("%-15s%-#30x%u\n", "SF", cpu.eflags.SF, cpu.eflags.SF);
        break;
      case reg_num - 5:
        printf("%-15s%-#30x%u\n", "CF", cpu.eflags.CF, cpu.eflags.CF);
        break;
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        printf("%-15s%-#30x%u\n", regsl[i], reg_l(i), reg_l(i));
        break;
      }
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  if (strcmp(s, "pc") == 0)
	return cpu.pc;
  for (int i = 0; i < 8; i++)
  {
	if (strcmp(s, regsl[i]) == 0)
	  return reg_l(i);
	else if(strcmp(s, regsw[i]) == 0)
	  return reg_w(i);
	else if(strcmp(s, regsb[i]) == 0)
	  return reg_b(i);
  }
  *success = false;
  return 0;
}
