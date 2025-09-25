// License C 2021-2022
// Author: Thomas Unger
#include <unistd.h>
#include <sys/syscall.h>

#include <stdint.h>
#include "L0/sym_lib.h"

// This gives 1 level of toggle prevention.
#ifndef CONFIG_NO_STICKY_ELEVATE
__thread int is_sticky = 0;
#endif

/* Implementation is almost entirely archetecture specific, check arch dir. */
#ifdef CONFIG_X86_64
#include "../../arch/x86_64/L0/sym_lib.h"
#endif

static long sym_do_syscall(int work){
  
#ifndef CONFIG_NO_STICKY_ELEVATE
if(!is_sticky){
#endif

    return syscall(NR_ELEVATE_SYSCALL, work);

#ifndef CONFIG_NO_STICKY_ELEVATE
  }
#endif
  // XXX obviously
  return 42;
}

long sym_mode_shift(uint64_t flags){
  return sym_do_syscall(flags);
}

long sym_elevate(){
  // XXX HACK This assumes we can just clobber user's gsbase...
  // I believe it also assumes no core migration...

  long ret = sym_mode_shift( SYM_ELEVATE_FLAG | SYM_INT_DISABLE_FLAG );
  GET_KERN_GS_CLOBBER_USER_GS;
  return ret;
}

long sym_lower(){
  /* return sym_do_syscall(SYSCALL_LOWER); */
  return sym_mode_shift(SYM_LOWER_FLAG );

}

long sym_check_elevate(){
  /* return sym_do_syscall(SYSCALL_CHECK_ELEVATE_STATUS); */
  return sym_mode_shift(SYM_QUERY_FLAG);
}

