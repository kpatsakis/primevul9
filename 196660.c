verify_moduleref_table (MonoImage *image, GSList *list, int level)
{
	MonoTableInfo *t = &image->tables [MONO_TABLE_MODULEREF];
	MonoTableInfo *tfile = &image->tables [MONO_TABLE_FILE];
	guint32 cols [MONO_MODULEREF_SIZE];
	const char *p, *pf;
	guint32 found, i, j, value;
	GHashTable *dups = g_hash_table_new (g_str_hash, g_str_equal);

	for (i = 0; i < t->rows; ++i) {
		mono_metadata_decode_row (t, i, cols, MONO_MODULEREF_SIZE);
		if (!(p = is_valid_string (image, cols [MONO_MODULEREF_NAME], TRUE))) {
			if (level & MONO_VERIFY_ERROR)
				ADD_ERROR (list, g_strdup_printf ("Invalid name in ModuleRef row %d", i + 1));
		} else {
			if (level & MONO_VERIFY_ERROR) {
				if (!is_valid_filename (p))
					ADD_ERROR (list, g_strdup_printf ("Invalid name '%s` in ModuleRef row %d", p, i + 1));
				else if (g_hash_table_lookup (dups, p)) {
					ADD_WARN (list, MONO_VERIFY_WARNING, g_strdup_printf ("Duplicate name '%s` in ModuleRef row %d", p, i + 1));
					g_hash_table_insert (dups, (gpointer)p, (gpointer)p);
					found = 0;
					for (j = 0; j < tfile->rows; ++j) {
						value = mono_metadata_decode_row_col (tfile, j, MONO_FILE_NAME);
						if ((pf = is_valid_string (image, value, TRUE)))
							if (strcmp (p, pf) == 0) {
								found = 1;
								break;
							}
					}
					if (!found)
						ADD_ERROR (list, g_strdup_printf ("Name '%s` in ModuleRef row %d doesn't have a match in File table", p, i + 1));
				}
			}
		}
	}
	g_hash_table_destroy (dups);
	return list;
}