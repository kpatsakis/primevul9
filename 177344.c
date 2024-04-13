static GHashTable *conv_get_charset_to_str_table(void)
{
	static GHashTable *table;
	gint i;

	if (table)
		return table;

	table = g_hash_table_new(NULL, g_direct_equal);

	for (i = 0; i < sizeof(charsets) / sizeof(charsets[0]); i++) {
		if (g_hash_table_lookup(table, GUINT_TO_POINTER(charsets[i].charset))
		    == NULL) {
			g_hash_table_insert
				(table, GUINT_TO_POINTER(charsets[i].charset),
				 charsets[i].name);
		}
	}

	return table;
}