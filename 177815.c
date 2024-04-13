ffi_closure_alloc (size_t size, void **code)
{
  /* Create the closure */
  ffi_closure *closure = malloc (size);
  if (closure == NULL)
    return NULL;

  pthread_mutex_lock (&ffi_trampoline_lock);

  /* Check for an active trampoline table with available entries. */
  ffi_trampoline_table *table = ffi_trampoline_tables;
  if (table == NULL || table->free_list == NULL)
    {
      table = ffi_trampoline_table_alloc ();
      if (table == NULL)
	{
	  pthread_mutex_unlock (&ffi_trampoline_lock);
	  free (closure);
	  return NULL;
	}

      /* Insert the new table at the top of the list */
      table->next = ffi_trampoline_tables;
      if (table->next != NULL)
	table->next->prev = table;

      ffi_trampoline_tables = table;
    }

  /* Claim the free entry */
  ffi_trampoline_table_entry *entry = ffi_trampoline_tables->free_list;
  ffi_trampoline_tables->free_list = entry->next;
  ffi_trampoline_tables->free_count--;
  entry->next = NULL;

  pthread_mutex_unlock (&ffi_trampoline_lock);

  /* Initialize the return values */
  *code = entry->trampoline;
  closure->trampoline_table = table;
  closure->trampoline_table_entry = entry;

  return closure;
}