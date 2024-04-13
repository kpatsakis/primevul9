e_util_ensure_gdbus_string (const gchar *str,
                            gchar **gdbus_str)
{
	g_return_val_if_fail (gdbus_str != NULL, NULL);

	*gdbus_str = NULL;

	if (!str || !*str)
		return "";

	if (g_utf8_validate (str, -1, NULL))
		return str;

	*gdbus_str = e_util_utf8_make_valid (str);

	return *gdbus_str;
}