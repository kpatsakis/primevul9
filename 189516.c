e_util_utf8_remove_accents (const gchar *str)
{
	gchar *res;
	gint i, j;

	if (str == NULL)
		return NULL;

	res = g_utf8_normalize (str, -1, G_NORMALIZE_NFD);
	if (!res)
		return g_strdup (str);

	for (i = 0, j = 0; res[i]; i++) {
		if ((guchar) res[i] != 0xCC || res[i + 1] == 0) {
			res[j] = res[i];
			j++;
		} else {
			i++;
		}
	}

	res[j] = 0;

	return res;
}