mono_alloc_special_static_data_free (GHashTable *special_static_fields)
{
	mono_threads_lock ();
	g_hash_table_foreach (special_static_fields, do_free_special, NULL);
	mono_threads_unlock ();
}