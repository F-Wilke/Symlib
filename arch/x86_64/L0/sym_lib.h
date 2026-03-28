#ifndef __ARCH_X86_64_SYM_LIB__
#define __ARCH_X86_64_SYM_LIB__

/* #include <unistd.h> */
/* #include <sys/syscall.h> */

/* #include "L0/sym_lib.h" */
//TODO: make this save for migrated processes. As it stands a migrated symbiote will have a gs base pointer into the wrong core

#define GET_KERN_GS_CLOBBER_USER_GS                           \
  __asm__ __volatile__ (        \
    "movl $0xc0000102, %%ecx;" \
    "rdmsr;"                   \
    "movl $0xc0000101, %%ecx;" \
    "wrmsr;"                   \
    "sti;"                     \
    :: :"%rax", "%edx", "%ecx" \
    );

//restore user gs base back to 0 
#define RESET_KERN_GS_USER_GS                           \
  __asm__ __volatile__ (				\
    "movl $0x0, %%edx;" \
    "movl $0x0, %%eax;" \
    "movl $0xc0000101, %%ecx;" \
    "wrmsr;"                   \
    "sti;"                     \
    :: :"%rax", "%edx", "%ecx" \
    );

#define RESET_KERN_GS_USER_GS_CLI                           \
  __asm__ __volatile__ (        \
    "cli;" \
    "movl $0x0, %%edx;" \
    "movl $0x0, %%eax;" \
    "movl $0xc0000101, %%ecx;" \
    "wrmsr;"                   \
    :: :"%rax", "%edx", "%ecx" \
    );
    
    #define DO_IRET_LOWER \
    __asm__ __volatile__ ( \
      "lea 8(%%rsp), %%rax;" \
      "pushq $0x2b;" \
      "pushq %%rax;" \
      "pushq $0x202;"\
      "pushq $0x33;"\
      "pushq -8(%%rax);"\
      "movq $0x0, %%rax;" \
      "iretq;" \
      ::: "memory" \
    ); 
    
    
#endif //__ARCH_X86_64_SYM_LIB__
