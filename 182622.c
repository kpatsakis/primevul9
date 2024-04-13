caseless_hash (gconstpointer v)
{
	gchar *lower;
	guint ret;

	lower = g_ascii_strdown (v, -1);
	ret = g_str_hash (lower);
	g_free (lower);

	return ret;
}