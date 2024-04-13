ffi_closure_free (void *ptr)
{
  ffi_closure *closure = ptr;

  pthread_mutex_lock (&ffi_trampoline_lock);

  /* Fetch the table and entry references */
  ffi_trampoline_table *table = closure->trampoline_table;
  ffi_trampoline_table_entry *entry = closure->trampoline_table_entry;

  /* Return the entry to the free list */
  entry->next = table->free_list;
  table->free_list = entry;
  table->free_count++;

  /* If all trampolines within this table are free, and at least one other table exists, deallocate
   * the table */
  if (table->free_count == FFI_TRAMPOLINE_COUNT
      && ffi_trampoline_tables != table)
    {
      ffi_trampoline_table_free (table);
    }
  else if (ffi_trampoline_tables != table)
    {
      /* Otherwise, bump this table to the top of the list */
      table->prev = NULL;
      table->next = ffi_trampoline_tables;
      if (ffi_trampoline_tables != NULL)
	ffi_trampoline_tables->prev = table;

      ffi_trampoline_tables = table;
    }

  pthread_mutex_unlock (&ffi_trampoline_lock);

  /* Free the closure */
  free (closure);
}