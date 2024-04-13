mono_icall_cleanup (void)
{
	g_hash_table_destroy (icall_hash);
	g_hash_table_destroy (jit_icall_hash_name);
	g_hash_table_destroy (jit_icall_hash_addr);
}