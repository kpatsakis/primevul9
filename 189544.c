e_util_strcmp0 (const gchar *str1,
		const gchar *str2)
{
	if (str1 && !*str1)
		str1 = NULL;

	if (str2 && !*str2)
		str2 = NULL;

	return g_strcmp0 (str1, str2);
}