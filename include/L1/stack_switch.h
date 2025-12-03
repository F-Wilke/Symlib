#ifndef __SYM_STACK_SWITCH__
#define __SYM_STACK_SWITCH__

#ifdef CONFIG_X86_64
#include "../../arch/x86_64/L1/stack_switch.h"
#endif

// These (ON_KERN, ON_USER) must be used as a pair
#define SYM_ON_KERN_STACK() \
    sym_elevate(); \
    uint64_t user_stack; \
    SYM_PRESERVE_USER_STACK(user_stack); \
    SYM_SWITCH_TO_KERN_STACK();

#define SYM_ON_KERN_STACK_DYNSYM(ktos)		\
  sym_elevate();				\
  uint64_t user_stack;				\
  SYM_PRESERVE_USER_STACK(user_stack);		\
  SYM_SWITCH_TO_KERN_STACK_OFF(ktos);

#define SYM_ON_USER_STACK() \
    SYM_RESTORE_USER_STACK(user_stack); \
    sym_lower();

// Combine the two above so we don't have to remember to call both
// but put all of user code inbetween
#define SYM_ON_KERN_STACK_DO(fn) \
    SYM_ON_KERN_STACK(); \
    fn; \
    SYM_ON_USER_STACK();

#define SYM_ON_KERN_STACK_DYNSYM_DO(ktos,fn)		\
  SYM_ON_KERN_STACK_DYNSYM(ktos);				\
  fn;							\
  SYM_ON_USER_STACK();

#endif
