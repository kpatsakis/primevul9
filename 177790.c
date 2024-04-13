ffi_trampoline_table_free (ffi_trampoline_table *table)
{
  /* Remove from the list */
  if (table->prev != NULL)
    table->prev->next = table->next;

  if (table->next != NULL)
    table->next->prev = table->prev;

  /* Deallocate pages */
  vm_deallocate (mach_task_self (), table->config_page, PAGE_MAX_SIZE * 2);

  /* Deallocate free list */
  free (table->free_list_pool);
  free (table);
}