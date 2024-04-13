CharSet conv_get_charset_from_str(const gchar *charset)
{
	GHashTable *table;

	if (!charset) return C_AUTO;

	table = conv_get_charset_from_str_table();
	return GPOINTER_TO_UINT(g_hash_table_lookup(table, charset));
}