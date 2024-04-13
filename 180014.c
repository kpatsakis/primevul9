mono_find_jit_icall_by_name (const char *name)
{
	MonoJitICallInfo *info;
	g_assert (jit_icall_hash_name);

	mono_loader_lock ();
	info = g_hash_table_lookup (jit_icall_hash_name, name);
	mono_loader_unlock ();
	return info;
}