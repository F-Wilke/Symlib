// License C 2021-2022
// Author: Thomas Unger
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>

#include <stdint.h>
#include "L0/sym_lib.h"

// This gives 1 level of toggle prevention.
// __thread int is_sticky = 0;

/* Implementation is almost entirely archetecture specific, check arch dir. */
#ifdef CONFIG_X86_64
#include "../../arch/x86_64/L0/sym_lib.h"
#endif

static long sym_do_syscall(int work){
  // if(!is_sticky){
    return syscall(NR_ELEVATE_SYSCALL, work);
  // }
  // XXX obviously
  return 42;
}

long sym_mode_shift(uint64_t flags){
  return sym_do_syscall(flags);
}

#ifdef CONFIG_X86_64
__attribute__((aligned(4096)))
long sym_elevate(){ //this needs to be page aligned. Otherwise we can suffer a page fault between elevate and gs fixup
  // Integrated version that bypasses intermediate functions and libc syscall
  // XXX HACK This assumes we can just clobber user's gsbase...
  // I believe it also assumes no core migration...

  long ret;
  long syscall = NR_ELEVATE_SYSCALL;
  uint64_t flags = SYM_ELEVATE_FLAG | SYM_INT_DISABLE_FLAG | 
                                          SYM_NOSMEP_FLAG | SYM_NOSMAP_FLAG | 
                                          SYM_TOGGLE_SMEP_FLAG | SYM_TOGGLE_SMAP_FLAG;
  
  // Integrated syscall assembly - avoids libc syscall wrapper
  __asm__ __volatile__ (
    "syscall;"                   // invoke syscall
    "movl $0xc0000102, %%ecx;" 
    "rdmsr;"                   
    "movl $0xc0000101, %%ecx;" 
    "wrmsr;"                   
    "sti;"                     
    : "=a" (ret)                 // output: rax contains return value
    : "a" (syscall), "D" (flags)       // input: rdi contains flags, rax contains syscall number
    : "%rcx", "%r11", "%edx", "%ecx", "memory"   // syscall clobbers rcx, r11, and memory
  );
  return ret;
}
#else
long sym_elevate(){
  // XXX HACK This assumes we can just clobber user's gsbase...
  // I believe it also assumes no core migration...

  long ret = sym_mode_shift( SYM_ELEVATE_FLAG | SYM_INT_DISABLE_FLAG | SYM_NOSMEP_FLAG | SYM_NOSMAP_FLAG | SYM_TOGGLE_SMEP_FLAG | SYM_TOGGLE_SMAP_FLAG);
  GET_KERN_GS_CLOBBER_USER_GS;
  return ret;
}
#endif

#ifdef CONFIG_X86_64
int symbi_fast_lower() {
  int ret;
  __asm__ __volatile__ (
    "push %%rcx;"                
    "push %%r11;"                
    "call symbi_fast_lower_sysret;"
    "pop %%r11;"
    "pop %%rcx;"
    : "=a" (ret)                 // output: rax contains return value
    :                            // no input
    : "memory"  // clobbers memory, we manually restore rcx and r11
  );
  return ret;

}
#endif


long sym_lower(){
  /* return sym_do_syscall(SYSCALL_LOWER); */
  assert(0); // unsafe poisoned - need to remove due to race on gs switch
  RESET_KERN_GS_USER_GS;
  return sym_mode_shift(SYM_LOWER_FLAG );

}

long sym_check_elevate(){
  /* return sym_do_syscall(SYSCALL_CHECK_ELEVATE_STATUS); */
  return sym_mode_shift(SYM_QUERY_FLAG);
}


__attribute__((naked)) int sym_iret() {
  RESET_KERN_GS_USER_GS_CLI;
  DO_IRET_LOWER;
}
