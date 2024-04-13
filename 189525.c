e_util_utf8_make_valid (const gchar *str)
{
	if (!str)
		return g_strdup ("");

	return e_util_utf8_data_make_valid (str, strlen (str));
}