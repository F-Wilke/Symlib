#ifndef __ARCH_X86_64_SYM_STACK_SWITCH__
#define __ARCH_X86_64_SYM_STACK_SWITCH__

// Assembly interface to do stack switching
// Don't break this up into macros!!!!
// We must ensure compiler does not get in between
// and use the stack or make assumptions
typedef void *(*kcall_thunk)(void *ctx);

void * stack_switch_kcall_asm(unsigned long ktop_offset,
			      kcall_thunk thunk,
			      void  *ctx);

#endif

