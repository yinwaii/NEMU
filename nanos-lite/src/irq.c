#include <common.h>
void do_syscall(Context *c);
Context *schedule(Context *prev);

static Context *do_event(Event e, Context *c) {
  switch (e.event) {
    case EVENT_IRQ_TIMER:
      // Log("Timer Interrupt!");
      return schedule(c);
      break;
    case EVENT_YIELD:
      // Log("Yield!");
      return schedule(c);
      break;
    case EVENT_SYSCALL:
      do_syscall(c);
      return c;
      break;
    case EVENT_IRQ_IODEV:
      Log("IO Interrupt!");
      return c;
      break;
    default:
      panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
