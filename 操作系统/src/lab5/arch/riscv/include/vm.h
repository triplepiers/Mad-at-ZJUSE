#include "string.h"  // memset()
#include "defs.h"    // PGSIZE
#include "mm.h"      // kalloc()
#include "printk.h"

extern unsigned long swapper_pg_dir[512];
extern char _srodata[];
extern char _stext[];
extern char _sdata[];

void setup_vm();

void setup_vm_final();

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);