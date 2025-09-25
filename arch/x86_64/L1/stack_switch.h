#ifndef __ARCH_X86_64_SYM_STACK_SWITCH__
#define __ARCH_X86_64_SYM_STACK_SWITCH__

// Var better be in memory
#define SYM_PRESERVE_USER_STACK(var) \
asm volatile("mov %%rsp, %0" : "=m"(var) : : "memory"); \

// Assumes on a user stack in a symbiote. Switches to the default kernel stack
#define SYM_SWITCH_TO_KERN_STACK() \
asm volatile("mov %gs:0x17b90, %rsp"); 

#define SYM_RESTORE_USER_STACK(var) \
  asm volatile("mov %0, %%rsp" : : "m"(var));
  
#define SYM_GET_KERNEL_STACK(var) \
  asm volatile (    "movq %%gs:0x17b90, %0"    : "=r"(var)      :    : "memory");

  #endif


