#include <inc/mmu.h>

# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.set PROT_MODE_CSEG, 0x8         # kernel code segment selector
.set PROT_MODE_DSEG, 0x10        # kernel data segment selector
.set CR0_PE_ON,      0x1         # protected mode enable flag
.set CR0_PG_ON,      (1 << 31)
.set CR4_PAE_ON,      (1 << 5)
.set IA32_EFER_SCE, 1         # SYSCALL Enable: IA32_EFER.SCE (R/W) Enables SYSCALL/SYSRET instructions in 64-bit mode.
.set IA32_EFER_LME, (1 << 8)         # IA-32e Mode Enable: IA32_EFER.LME (R/W) Enables IA-32e mode operation.
.set IA32_EFER_LMA, (1 << 10)        # IA-32e Mode Active: IA32_EFER.LMA (R) Indicates IA-32e mode is active when set.
.set IA32_EFER_NXE, (1 << 11)        # Execute Disable Bit Enable: IA32_EFER.NXE (R/W)
.set IA32_EFER,  0xC0000080    # Extended Feature Enables MSR

.set PAGE_ROOT, 0x8000
.set PAGE_SIZE, (1<<12)
.set PDPT_ADDR, (PAGE_ROOT + PAGE_SIZE)
.set PDPT_ADDR2, (PDPT_ADDR + PAGE_SIZE)

.globl start
start:
  .code16                     # Assemble for 16-bit mode
  cli                         # Disable interrupts
  cld                         # String operations increment

  # Set up the important data segment registers (DS, ES, SS).
  xorw    %ax,%ax             # Segment number zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment

  # Enable A20:
  #   For backwards compatibility with the earliest PCs, physical
  #   address line 20 is tied low, so that addresses higher than
  #   1MB wrap around to zero by default.  This code undoes this.
  inb    $0x92, %al
  orb    $0x02, %al
  outb   %al, $0x92

  # Switch from real to protected mode, using a bootstrap GDT
  # and segment translation that makes virtual addresses 
  # identical to their physical addresses, so that the 
  # effective memory map does not change during the switch.
  lgdt    gdtdesc
  movl    %cr0, %eax
  orl     $CR0_PE_ON, %eax
  movl    %eax, %cr0
  
  # Jump to next instruction, but in 32-bit code segment.
  # Switches processor into 32-bit mode.
  ljmp    $PROT_MODE_CSEG, $protcseg

  .code32                     # Assemble for 32-bit mode
protcseg:
  # Set up the protected-mode data segment registers
  movw    $PROT_MODE_DSEG, %ax    # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment

  movl %cr4, %eax
  orl $CR4_PAE_ON, %eax
  movl %eax, %cr4

  movl $(7 + (1<<7)), %eax
  movl $PDPT_ADDR + 1, PAGE_ROOT
  movl %eax, PDPT_ADDR
  movl $PDPT_ADDR2 + 7, PAGE_ROOT + PAGE_SIZE - 8
  movl %eax, PDPT_ADDR2 + PAGE_SIZE - 8

  movl $PAGE_ROOT, %eax
  movl %eax, %cr3

  movl $IA32_EFER, %ecx
  rdmsr
  orl $IA32_EFER_LME, %eax
  wrmsr

  movl    %cr0, %eax
  orl     $CR0_PG_ON, %eax
  movl    %eax, %cr0

  ljmp $24, $seg64

.code64
seg64:

  # Set up the stack pointer and call into C.
  movq    $start, %rsp
  call bootmain

  # If bootmain returns (it shouldn't), loop.
#spin:
#  jmp spin

# Bootstrap GDT
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULL				# null seg
  SEG(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
  SEG(STA_W, 0x0, 0xffffffff)	        # data seg
  SEGL(STA_X|STA_R, 0x0, 0xffffffff)	# code seg

gdtdesc:
  .word   0x1f                            # sizeof(gdt) - 1
  .long   gdt                             # address gdt


#.org 512 - 2
#.word 0xAA55
