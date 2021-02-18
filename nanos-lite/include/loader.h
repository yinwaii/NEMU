#ifndef __LOADER_H__
#define __LOADER_H__
#include <proc.h>
#include <elf.h>
#include <fs.h>
void naive_uload(PCB *pcb, const char *filename);
#endif