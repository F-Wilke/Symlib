#ifndef __SYM_LIB_ALL__
#define __SYM_LIB_ALL__

// TODO: Per level sym_alls, e.g. L0/sym_all.h
#include "LIDK/idk.h"
#include "L0/sym_lib.h"
#include "L0/sym_lib_hacks.h"
#include "L0/sym_structs.h"
#include "L1/sym_interrupts.h"
#include "L1/stack_switch.h"
#include "L2/sym_lib_page_fault.h"
#include "L2/sym_probe.h"
#include "L2/sym_get_addr.h"
#include "L3/mitigate.h"

// OK these aren't needed to build the symlib on its own, but they
// are needed to build external tools like idt_tool. This is a HACK
// We should resolve this somehow, such as having a configure step or something.
// Basically we need to define the arch specific stuff, CONFIG_X86_64.
#include "../arch/x86_64/L2/common.h"
#include "../arch/x86_64/L1/stack_switch.h"
#include "../arch/x86_64/L0/sym_lib.h"

#include "LINF/init.h"
#endif
