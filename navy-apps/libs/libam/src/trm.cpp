#include <am.h>
#include <NDL.h>
#include <stdio.h>

Area heap;

void putch(char ch) {
	putchar(ch);
}

void halt(int code) {
	NDL_Quit();
}
