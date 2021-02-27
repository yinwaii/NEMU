#include <isa.h>
#include <common.h>

void dev_raise_intr() {
	cpu.INTR = true;
}
