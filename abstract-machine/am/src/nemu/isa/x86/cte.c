#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

void __amkcontext_start();
void __am_panic_on_return() { halt(0); }

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->irq) {
      case 0x32:
        ev.event = EVENT_IRQ_TIMER;
        break;
      case 0x80:
        ev.event = EVENT_SYSCALL;
        break;
      case 0x81:
        ev.event = EVENT_YIELD;
        break;
      default:
        ev.event = EVENT_ERROR;
        break;
    }
    // printf("%p\n", c->this_esp);
    // printf("%p, %p, %p, %p, %p, %p, %p, %p\n", c->edi, c->esi, c->ebp, c->esp, c->ebx, c->edx, c->ecx, c->eax);
    // printf("%p\n", c->irq);
    // printf("%p, %p, %p\n", c->eip, c->cs, c->eflags);

    c = user_handler(ev, c);
    assert(c != NULL);
    __am_switch(c);
  }
  return c;
}

bool cte_init(Context*(*handler)(Event, Context*)) {
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0,    DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys,  DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = (Context *)kstack.end - 1;
  memset(context, 0, sizeof(context));
  context->cs = 0x8;
  context->eip = (uintptr_t)__amkcontext_start;
  context->esp = (uintptr_t)kstack.end;
  context->cr3 = NULL;
  context->irq = 0x81;
  context->GPR1 = (uintptr_t)arg;
  context->GPR2 = (uintptr_t)entry;
  return context;
}

void yield() {
  asm volatile("int $0x81");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
