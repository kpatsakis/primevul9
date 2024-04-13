ffi_trampoline_table_alloc (void)
{
  ffi_trampoline_table *table;
  vm_address_t config_page;
  vm_address_t trampoline_page;
  vm_address_t trampoline_page_template;
  vm_prot_t cur_prot;
  vm_prot_t max_prot;
  kern_return_t kt;
  uint16_t i;

  /* Allocate two pages -- a config page and a placeholder page */
  config_page = 0x0;
  kt = vm_allocate (mach_task_self (), &config_page, PAGE_MAX_SIZE * 2,
		    VM_FLAGS_ANYWHERE);
  if (kt != KERN_SUCCESS)
    return NULL;

  /* Remap the trampoline table on top of the placeholder page */
  trampoline_page = config_page + PAGE_MAX_SIZE;
  trampoline_page_template = (vm_address_t)&ffi_closure_trampoline_table_page;
#ifdef __arm__
  /* ffi_closure_trampoline_table_page can be thumb-biased on some ARM archs */
  trampoline_page_template &= ~1UL;
#endif
  kt = vm_remap (mach_task_self (), &trampoline_page, PAGE_MAX_SIZE, 0x0,
		 VM_FLAGS_OVERWRITE, mach_task_self (), trampoline_page_template,
		 FALSE, &cur_prot, &max_prot, VM_INHERIT_SHARE);
  if (kt != KERN_SUCCESS)
    {
      vm_deallocate (mach_task_self (), config_page, PAGE_MAX_SIZE * 2);
      return NULL;
    }

  /* We have valid trampoline and config pages */
  table = calloc (1, sizeof (ffi_trampoline_table));
  table->free_count = FFI_TRAMPOLINE_COUNT;
  table->config_page = config_page;
  table->trampoline_page = trampoline_page;

  /* Create and initialize the free list */
  table->free_list_pool =
    calloc (FFI_TRAMPOLINE_COUNT, sizeof (ffi_trampoline_table_entry));

  for (i = 0; i < table->free_count; i++)
    {
      ffi_trampoline_table_entry *entry = &table->free_list_pool[i];
      entry->trampoline =
	(void *) (table->trampoline_page + (i * FFI_TRAMPOLINE_SIZE));

      if (i < table->free_count - 1)
	entry->next = &table->free_list_pool[i + 1];
    }

  table->free_list = table->free_list_pool;

  return table;
}