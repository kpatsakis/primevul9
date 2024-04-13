e_util_unicode_get_utf8 (const gchar *text,
                         gunichar *out)
{
	g_return_val_if_fail (text != NULL, NULL);
	g_return_val_if_fail (out != NULL, NULL);

	*out = g_utf8_get_char (text);
	return (*out == (gunichar) -1) ? NULL : g_utf8_next_char (text);
}