e_util_safe_free_string (gchar *str)
{
	if (!str)
		return;

	if (*str)
		memset (str, 0, sizeof (gchar) * strlen (str));

	g_free (str);
}