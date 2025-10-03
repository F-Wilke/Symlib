#include <stdio.h>
#include "LIDK/idk.h"
#include <stdint.h>
#include <string.h>

#include "L1/stack_switch.h"
#include "L2/sym_lib_page_fault.h"
#include "L2/sym_probe.h"


typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
static kallsyms_lookup_name_t kallsyms_lookup_name = NULL;

void parse_system_map() {
	char kernel_version[128] = {0};
	// Bit bigger to avoid compiler warnings.
	char system_map_path[2*128] = {0};

	FILE* fp_version = fopen("/proc/version", "r");
	if (fp_version == NULL) {
		fprintf(stderr, "Could not open /proc/version\n");
	}

	fscanf(fp_version, "Linux version %s", kernel_version);
	fclose(fp_version);

	// Use kernel version to get system map path
	sprintf(system_map_path, "/boot/System.map-%s", kernel_version);

	unsigned long long addr;
	char type;
	char symbol[128];

	FILE* fp = fopen(system_map_path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Could not open system map: %s\n", system_map_path);
	}

	while (fscanf(fp, "%llx %c %s", &addr, &type, symbol) != EOF) {
		if (strcmp(symbol, "kallsyms_lookup_name") == 0) {
			kallsyms_lookup_name = (kallsyms_lookup_name_t)addr;
			break;
		}
	}

	fclose(fp);
}

void* sym_get_fn_address(char *symbol) {
  // Initialization step: parse system map to get
  // the address of the kallsyms_lookup_name routine.
  if (kallsyms_lookup_name == NULL) {
	parse_system_map();
  }

  // Elevate to be able to call kallsyms_lookup_name
  sym_elevate();
  // TODO: clean this up  
  uint64_t user_stack;
  // SYM_PRESERVE_USER_STACK(user_stack);
  // SYM_SWITCH_TO_KERN_STACK();
  void* result = (void*)kallsyms_lookup_name(symbol);
  // SYM_RESTORE_USER_STACK(user_stack);
  // Don't forget to lower
  sym_lower();

  return (void*)result;
}

void sym_l2_init(){
  printf("sym_l2_init\n");
  sym_lib_page_fault_init();
  sym_probe_init();
}
