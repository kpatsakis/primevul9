mono_add_internal_call (const char *name, gconstpointer method)
{
	mono_loader_lock ();

	g_hash_table_insert (icall_hash, g_strdup (name), (gpointer) method);

	mono_loader_unlock ();
}