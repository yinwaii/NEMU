#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0

// reg

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
union {
	union {
    uint32_t _32;
    uint16_t _16;
    uint8_t _8[2];
	} gpr[8];

  /* Do NOT change the order of the GPRs' definitions. */

  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
	struct{
		rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	};
};
  vaddr_t pc;
  union{
    struct {
      word_t CF : 1;
      unsigned : 1;
      word_t PF : 1;
      unsigned : 1;
      word_t AF : 1;
      unsigned : 1;
      word_t ZF : 1;
      word_t SF : 1;
      word_t TF : 1;
      word_t IF : 1;
      word_t DF : 1;
      word_t OF : 1;
      word_t IOPL : 1;
      word_t NT : 1;
      unsigned : 1;
      word_t RF : 1;
      word_t VM : 1;
      unsigned : 15;
    };
    word_t val;
  } eflags;
  uint16_t CS, SS, DS, ES, FS, GS;
  union {
    struct {
      uint16_t limit : 16;
      vaddr_t base : 32;
    };
    uint64_t val;
  } IDTR, GDTR;
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
