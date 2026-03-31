#ifndef __SYM_STACK_SWITCH__
#define __SYM_STACK_SWITCH__

#include "../../arch/x86_64/L1/stack_switch.h"

// Just a wrapper should not add any rreal code 
static inline void *
stack_switch_kcall(unsigned long ktop_offset,
		   kcall_thunk thunk,
		   void  *ctx)
{
    return stack_switch_kcall_asm(ktop_offset, thunk, ctx);
}


#endif
