mono_free_static_data (gpointer* static_data, gboolean threadlocal)
{
	int i;
	for (i = 1; i < NUM_STATIC_DATA_IDX; ++i) {
		if (!static_data [i])
			continue;
#ifdef HAVE_SGEN_GC
		if (threadlocal)
			g_free (static_data [i]);
		else
			mono_gc_free_fixed (static_data [i]);
#else
		mono_gc_free_fixed (static_data [i]);
#endif
	}
	mono_gc_free_fixed (static_data);
}