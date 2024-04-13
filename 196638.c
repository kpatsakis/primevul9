verify_assembly_table (MonoImage *image, GSList *list, int level)
{
	MonoTableInfo *t = &image->tables [MONO_TABLE_ASSEMBLY];
	guint32 cols [MONO_ASSEMBLY_SIZE];
	const char *p;

	if (level & MONO_VERIFY_ERROR) {
		if (t->rows > 1)
			ADD_ERROR (list, g_strdup ("Assembly table may only have 0 or 1 rows"));
		mono_metadata_decode_row (t, 0, cols, MONO_ASSEMBLY_SIZE);

		switch (cols [MONO_ASSEMBLY_HASH_ALG]) {
		case ASSEMBLY_HASH_NONE:
		case ASSEMBLY_HASH_MD5:
		case ASSEMBLY_HASH_SHA1:
			break;
		default:
			ADD_ERROR (list, g_strdup_printf ("Hash algorithm 0x%x unknown", cols [MONO_ASSEMBLY_HASH_ALG]));
		}

		if (!is_valid_assembly_flags (cols [MONO_ASSEMBLY_FLAGS]))
			ADD_ERROR (list, g_strdup_printf ("Invalid flags in assembly: 0x%x", cols [MONO_ASSEMBLY_FLAGS]));

		if (!is_valid_blob (image, cols [MONO_ASSEMBLY_PUBLIC_KEY], FALSE))
			ADD_ERROR (list, g_strdup ("Assembly public key is an invalid index"));

		if (!(p = is_valid_string (image, cols [MONO_ASSEMBLY_NAME], TRUE))) {
			ADD_ERROR (list, g_strdup ("Assembly name is invalid"));
		} else {
			if (strpbrk (p, ":\\/."))
				ADD_ERROR (list, g_strdup_printf ("Assembly name `%s' contains invalid chars", p));
		}

		if (!(p = is_valid_string (image, cols [MONO_ASSEMBLY_CULTURE], FALSE))) {
			ADD_ERROR (list, g_strdup ("Assembly culture is an invalid index"));
		} else {
			if (!is_valid_culture (p))
				ADD_ERROR (list, g_strdup_printf ("Assembly culture `%s' is invalid", p));
		}
	}
	return list;
}