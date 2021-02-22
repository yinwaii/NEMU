#----|------------entry------------|---irq id---|-----handler-----|
.globl __am_vecsys;    __am_vecsys: pushl $0x80; jmp __am_asm_trap
.globl __am_vectrap;  __am_vectrap: pushl $0x81; jmp __am_asm_trap
.globl __am_irq0;        __am_irq0: pushl   $32; jmp __am_asm_trap
.globl __am_vecnull;  __am_vecnull: pushl   $-1; jmp __am_asm_trap

.globl __amkcontext_start
__amkcontext_start:
  // eax = arg, ebx = entry
  pushl %eax
  pushl $__am_panic_on_return
  jmpl *%ebx

__am_asm_trap:
  pushal

  pushl $0

  pushl %esp
  call __am_irq_handle

  movl %eax, %esp
  addl $4, %esp

  addl $4, %esp
  popal
  addl $4, %esp

  iret
