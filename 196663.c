verify_file_table (MonoImage *image, GSList *list, int level)
{
	MonoTableInfo *t = &image->tables [MONO_TABLE_FILE];
	guint32 cols [MONO_FILE_SIZE];
	const char *p;
	guint32 i;
	GHashTable *dups = g_hash_table_new (g_str_hash, g_str_equal);

	for (i = 0; i < t->rows; ++i) {
		mono_metadata_decode_row (t, i, cols, MONO_FILE_SIZE);
		if (level & MONO_VERIFY_ERROR) {
			if (cols [MONO_FILE_FLAGS] != FILE_CONTAINS_METADATA && cols [MONO_FILE_FLAGS] != FILE_CONTAINS_NO_METADATA)
				ADD_ERROR (list, g_strdup_printf ("Invalid flags in File row %d", i + 1));
			if (!is_valid_blob (image, cols [MONO_FILE_HASH_VALUE], TRUE))
				ADD_ERROR (list, g_strdup_printf ("File hash value in row %d is invalid or not null and empty", i + 1));
		}
		if (!(p = is_valid_string (image, cols [MONO_FILE_NAME], TRUE))) {
			if (level & MONO_VERIFY_ERROR)
				ADD_ERROR (list, g_strdup_printf ("Invalid name in File row %d", i + 1));
		} else {
			if (level & MONO_VERIFY_ERROR) {
				if (!is_valid_filename (p))
					ADD_ERROR (list, g_strdup_printf ("Invalid name '%s` in File row %d", p, i + 1));
				else if (g_hash_table_lookup (dups, p)) {
					ADD_ERROR (list, g_strdup_printf ("Duplicate name '%s` in File row %d", p, i + 1));
				}
				g_hash_table_insert (dups, (gpointer)p, (gpointer)p);
			}
		}
		/*
		 * FIXME: I don't understand what this means:
		 * If this module contains a row in the Assembly table (that is, if this module "holds the manifest") 
		 * then there shall not be any row in the File table for this module - i.e., no self-reference  [ERROR]
		 */

	}
	if (level & MONO_VERIFY_WARNING) {
		if (!t->rows && image->tables [MONO_TABLE_EXPORTEDTYPE].rows)
			ADD_WARN (list, MONO_VERIFY_WARNING, g_strdup ("ExportedType table should be empty if File table is empty"));
	}
	g_hash_table_destroy (dups);
	return list;
}