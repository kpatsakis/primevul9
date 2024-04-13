mono_alloc_static_data (gpointer **static_data_ptr, guint32 offset, gboolean threadlocal)
{
	guint idx = (offset >> 24) - 1;
	int i;

	gpointer* static_data = *static_data_ptr;
	if (!static_data) {
		static void* tls_desc = NULL;
#ifdef HAVE_SGEN_GC
		if (!tls_desc)
			tls_desc = mono_gc_make_root_descr_user (mark_tls_slots);
#endif
		static_data = mono_gc_alloc_fixed (static_data_size [0], threadlocal?tls_desc:NULL);
		*static_data_ptr = static_data;
		static_data [0] = static_data;
	}

	for (i = 1; i <= idx; ++i) {
		if (static_data [i])
			continue;
#ifdef HAVE_SGEN_GC
		static_data [i] = threadlocal?g_malloc0 (static_data_size [i]):mono_gc_alloc_fixed (static_data_size [i], NULL);
#else
		static_data [i] = mono_gc_alloc_fixed (static_data_size [i], NULL);
#endif
	}
}