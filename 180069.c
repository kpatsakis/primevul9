mono_register_jit_icall (gconstpointer func, const char *name, MonoMethodSignature *sig, gboolean is_save)
{
	MonoJitICallInfo *info;
	
	g_assert (func);
	g_assert (name);

	mono_loader_lock ();

	if (!jit_icall_hash_name) {
		jit_icall_hash_name = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_free);
		jit_icall_hash_addr = g_hash_table_new (NULL, NULL);
	}

	if (g_hash_table_lookup (jit_icall_hash_name, name)) {
		g_warning ("jit icall already defined \"%s\"\n", name);
		g_assert_not_reached ();
	}

	info = g_new0 (MonoJitICallInfo, 1);
	
	info->name = name;
	info->func = func;
	info->sig = sig;

	if (is_save) {
		info->wrapper = func;
	} else {
		info->wrapper = NULL;
	}

	g_hash_table_insert (jit_icall_hash_name, (gpointer)info->name, info);
	g_hash_table_insert (jit_icall_hash_addr, (gpointer)func, info);

	mono_loader_unlock ();
	return info;
}