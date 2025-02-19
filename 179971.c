mono_find_jit_icall_by_addr (gconstpointer addr)
{
	MonoJitICallInfo *info;
	g_assert (jit_icall_hash_addr);

	mono_loader_lock ();
	info = g_hash_table_lookup (jit_icall_hash_addr, (gpointer)addr);
	mono_loader_unlock ();

	return info;
}