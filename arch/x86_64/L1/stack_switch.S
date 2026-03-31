/*
 We use this code, in a conformat way, to switch to our tasks
 kernel stack and make a call on it and then switch back to the
 original stack.  Note if we might want to factor into a version
 that does not assume ktop

 LLM Note: used claude to help with .cfi (call frame info) directives
	
 See  System V AMD64 ABI abi.md for constaints and
 conventions we must conform to and can exploit

   rdi = kenel per CPU top of stack offset gs relative
         — first byte *past* the new stack region is at gs:(%rdi)
   rsi = thunk
         — void (*)(void *ctx)
   rdx = ctx
         — forwarded verbatim to thunk as rdi

Stack alignment note from ABI.md:

  "The end of the input argument area shall be aligned on a 16 (32 or
  64, if __m256 or __m512 is passed on stack) byte boundary.(see
  footnote 11) **In other words, the stack needs to be 16 (32 or 64)
  byte aligned immediately before the call instruction is
  executed**. Once control has been transferred to the function entry
  point, i.e. immediately after the return address has been pushed,
  %rsp points to the return address, and the value of (%rsp + 8) is a
  multiple of 16 (32 or 64)."
*/
	
#define DEBUG
	
        .text
        .globl stack_switch_kcall_asm
	.type  stack_switch_kcall_asm, @function
stack_switch_kcall_asm:	
        .cfi_startproc
	// Given that callee's must save rbx we will use it to
	// save original stack pointer
        pushq   %rbx
        .cfi_adjust_cfa_offset   8
        .cfi_rel_offset          %rbx, 0
        movq    %rsp, %rbx	
        .cfi_def_cfa_register    %rbx

	// switch to kernel stack
        movq    %gs:(%rdi), %rsp      // we assume it is page aligned so
	                              // so will be 16 bit aligned
#ifdef DEBUG
        test    %rsp, 0xf        
        jz      .Lrsp_aligned     
	xorq    %rax, %rax
	mov     (%rax), %rax
.Lrsp_aligned:			
#endif	
        movq    %rdx, %rdi           // ctx pointer in rdx move thunk arg (rdi)  
        callq   *%rsi                // call thunk(ctx) on kernel stack
	
      	// rax wil have thunk return -- leave it alone
	// rbx should be good to go with our original rsp
        movq    %rbx, %rsp
        .cfi_def_cfa_register    %rsp

	// restore our callers original rbx value
        popq    %rbx
        .cfi_adjust_cfa_offset  -8
        .cfi_restore             %rbx

        retq
        .cfi_endproc

	.size stack_switch_kcall_asm, .-stack_switch_kcall_asm
