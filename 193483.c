mono_alloc_special_static_data (guint32 static_type, guint32 size, guint32 align, uintptr_t *bitmap, int max_set)
{
	guint32 offset;
	if (static_type == SPECIAL_STATIC_THREAD) {
		MonoThreadDomainTls *item;
		mono_threads_lock ();
		item = search_tls_slot_in_freelist (&thread_static_info, size, align);
		/*g_print ("TLS alloc: %d in domain %p (total: %d), cached: %p\n", size, mono_domain_get (), thread_static_info.offset, item);*/
		if (item) {
			offset = item->offset;
			g_free (item);
		} else {
			offset = mono_alloc_static_data_slot (&thread_static_info, size, align);
		}
		update_tls_reference_bitmap (offset, bitmap, max_set);
		/* This can be called during startup */
		if (threads != NULL)
			mono_g_hash_table_foreach (threads, alloc_thread_static_data_helper, GUINT_TO_POINTER (offset));
		mono_threads_unlock ();
	} else {
		g_assert (static_type == SPECIAL_STATIC_CONTEXT);
		mono_contexts_lock ();
		offset = mono_alloc_static_data_slot (&context_static_info, size, align);
		mono_contexts_unlock ();
		offset |= 0x80000000;	/* Set the high bit to indicate context static data */
	}
	return offset;
}