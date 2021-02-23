#ifndef __LOADER_H__
#define __LOADER_H__
#include <proc.h>
#include <elf.h>
#include <fs.h>
void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry, void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
#endif