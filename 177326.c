static GHashTable *conv_get_charset_from_str_table(void)
{
	static GHashTable *table;
	gint i;

	if (table)
		return table;

	table = g_hash_table_new(str_case_hash, str_case_equal);

	for (i = 0; i < sizeof(charsets) / sizeof(charsets[0]); i++) {
		g_hash_table_insert(table, charsets[i].name,
				    GUINT_TO_POINTER(charsets[i].charset));
	}

	return table;
}