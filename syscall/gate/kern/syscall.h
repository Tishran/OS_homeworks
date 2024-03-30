#ifndef JOS_KERN_SYSCALL_H
#define JOS_KERN_SYSCALL_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

// System call number 1.
void sys_work(void*);

// System call number 2.
void sys_retire();

#endif	// !JOS_KERN_SYSCALL_H

