mono_register_jit_icall_wrapper (MonoJitICallInfo *info, gconstpointer wrapper)
{
	mono_loader_lock ();
	g_hash_table_insert (jit_icall_hash_addr, (gpointer)wrapper, info);
	mono_loader_unlock ();
}