ves_icall_System_Reflection_Module_ResolveSignature (MonoImage *image, guint32 token, MonoResolveTokenError *error)
{
	int table = mono_metadata_token_table (token);
	int idx = mono_metadata_token_index (token);
	MonoTableInfo *tables = image->tables;
	guint32 sig, len;
	const char *ptr;
	MonoArray *res;

	*error = ResolveTokenError_OutOfRange;

	/* FIXME: Support other tables ? */
	if (table != MONO_TABLE_STANDALONESIG)
		return NULL;

	if (image->dynamic)
		return NULL;

	if ((idx == 0) || (idx > tables [MONO_TABLE_STANDALONESIG].rows))
		return NULL;

	sig = mono_metadata_decode_row_col (&tables [MONO_TABLE_STANDALONESIG], idx - 1, 0);

	ptr = mono_metadata_blob_heap (image, sig);
	len = mono_metadata_decode_blob_size (ptr, &ptr);

	res = mono_array_new (mono_domain_get (), mono_defaults.byte_class, len);
	memcpy (mono_array_addr (res, guint8, 0), ptr, len);
	return res;
}