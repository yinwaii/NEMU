.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %esp
  pushl %eax
  call call_main
