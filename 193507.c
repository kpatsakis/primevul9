mono_thread_free_local_slot_values (int slot, MonoBoolean thread_local)
{
	MonoDomain *domain;
	LocalSlotID sid;
	sid.slot = slot;
	if (thread_local) {
		void *addr = NULL;
		if (!local_slots) {
			local_slots = mono_class_get_field_from_name (mono_defaults.thread_class, "local_slots");
			if (!local_slots) {
				g_warning ("local_slots field not found in Thread class");
				return;
			}
		}
		domain = mono_domain_get ();
		mono_domain_lock (domain);
		if (domain->special_static_fields)
			addr = g_hash_table_lookup (domain->special_static_fields, local_slots);
		mono_domain_unlock (domain);
		if (!addr)
			return;
		/*g_print ("freeing slot %d at %p\n", slot, addr);*/
		sid.offset = GPOINTER_TO_UINT (addr);
		sid.offset &= 0x7fffffff;
		sid.idx = (sid.offset >> 24) - 1;
		mono_threads_lock ();
		mono_g_hash_table_foreach (threads, clear_local_slot, &sid);
		mono_threads_unlock ();
	} else {
		/* FIXME: clear the slot for MonoAppContexts, too */
	}
}