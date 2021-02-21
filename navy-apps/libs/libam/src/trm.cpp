#include <am.h>
#include <NDL.h>
#include <stdio.h>

Area heap;

void putch(char ch) {
	// printf("Enter am: putch!\n");
	putchar(ch);
}

void halt(int code) {
	// printf("Enter am: halt!\n");
	NDL_Quit();
}
