#ifndef JOS_KERN_SYSGATE_H
#define JOS_KERN_SYSGATE_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

// Some useful x86_64 definitions
#define IA32_EFER  0xC0000080
#define IA32_STAR  0xC0000081
#define IA32_LSTAR  0xC0000082
#define IA32_FMASK  0xC0000084


// System call 
void sysgate();

#endif	// !JOS_KERN_SYSGATE_H
