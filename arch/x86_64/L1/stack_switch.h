#ifndef __ARCH_X86_64_SYM_STACK_SWITCH__
#define __ARCH_X86_64_SYM_STACK_SWITCH__

// Var better be in memory
#define SYM_PRESERVE_USER_STACK(var) \
asm volatile("mov %%rsp, %0" : "=m"(var) : : "memory"); \

// Assumes on a user stack in a symbiote. Switches to the default kernel stack
#define SYM_SWITCH_TO_KERN_STACK() \
asm volatile("mov %gs:0xffffffff83d19010, %rsp"); //this probably depends a lot on the kernel version
// asm volatile("mov %gs:0x17b90, %rsp"); //this changed between 5.14 and 6.16 

#define SYM_RESTORE_USER_STACK(var) \
  asm volatile("mov %0, %%rsp" : : "m"(var));

#endif
