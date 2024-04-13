const gchar *conv_get_charset_str(CharSet charset)
{
	GHashTable *table;

	table = conv_get_charset_to_str_table();
	return g_hash_table_lookup(table, GUINT_TO_POINTER(charset));
}