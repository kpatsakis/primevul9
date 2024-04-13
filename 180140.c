mono_create_icall_signature (const char *sigstr)
{
	gchar **parts;
	int i, len;
	gchar **tmp;
	MonoMethodSignature *res;

	mono_loader_lock ();
	res = g_hash_table_lookup (mono_defaults.corlib->helper_signatures, sigstr);
	if (res) {
		mono_loader_unlock ();
		return res;
	}

	parts = g_strsplit (sigstr, " ", 256);

	tmp = parts;
	len = 0;
	while (*tmp) {
		len ++;
		tmp ++;
	}

	res = mono_metadata_signature_alloc (mono_defaults.corlib, len - 1);
	res->pinvoke = 1;

#ifdef PLATFORM_WIN32
	/* 
	 * Under windows, the default pinvoke calling convention is STDCALL but
	 * we need CDECL.
	 */
	res->call_convention = MONO_CALL_C;
#endif

	res->ret = type_from_typename (parts [0]);
	for (i = 1; i < len; ++i) {
		res->params [i - 1] = type_from_typename (parts [i]);
	}

	g_strfreev (parts);

	g_hash_table_insert (mono_defaults.corlib->helper_signatures, (gpointer)sigstr, res);

	mono_loader_unlock ();

	return res;
}