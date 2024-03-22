#include "backtrace.h"

#include <elf.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>

char *AddrToName(void *addr) {
    static const char *fname = "/proc/self/exe";
    static struct stat st;
    if (stat(fname, &st) != 0) {
        perror("stat");
        return NULL;
    }

    int proc_exe_fd = open(fname, 0);
    void *proc_addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, proc_exe_fd, 0);
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) proc_addr;
    Elf64_Shdr *shdr = (Elf64_Shdr *) (proc_addr + ehdr->e_shoff);
    int shnum = ehdr->e_shnum;

    char* functionName = NULL;
    for (int i = 0; i < shnum; ++i) {
        if (shdr[i].sh_type == SHT_SYMTAB) {
            Elf64_Sym *symtab = (Elf64_Sym *) (proc_addr + shdr[i].sh_offset);
            int symcount = shdr[i].sh_size / sizeof(Elf64_Sym);

            Elf64_Addr best_addr = (Elf64_Addr) NULL;
            int best_j = 0;
            for (int j = 0; j < symcount; ++j) {
                if ((void *) symtab[j].st_value <= addr && symtab[j].st_value > best_addr) {
                    best_addr = symtab[j].st_value;
                    best_j = j;
                }
            }

            functionName = (proc_addr + shdr[shdr[i].sh_link].sh_offset + symtab[best_j].st_name);
            break;
        }
    }

    return functionName;
}

int Backtrace(void *backtrace[], int limit) {
    int size = 1;

    void *ret_addr;
    void *rbp_val;

    asm("mov %%rbp, %0" : "=r" (rbp_val));

    for (int i = 0; i <= limit; ++i) {
        asm("mov %2, %%rax\n\t"
            "mov (%%rax), %1\n\t"
            "mov 8(%%rax), %0\n\t"
                : "=r" (ret_addr), "=r" (rbp_val)
                : "rm" (rbp_val)
                : "%rax");

        backtrace[i] = ret_addr;
        if (strcmp(AddrToName(ret_addr), "main") == 0) {
            break;
        }

        ++size;
    }

    return size;
}

void PrintBt() {
    void* bt[256];

    int sz = Backtrace(bt, 256);
    for (int i = 0; i < sz; ++i) {
        printf("%p %s\n", bt[i], AddrToName(bt[i]));
    }
}
