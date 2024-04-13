e_util_utf8_normalize (const gchar *str)
{
	gchar *valid = NULL;
	gchar *normal, *casefolded = NULL;

	if (str == NULL)
		return NULL;

	if (!g_utf8_validate (str, -1, NULL)) {
		valid = e_util_utf8_make_valid (str);
		str = valid;
	}

	normal = e_util_utf8_remove_accents (str);
	if (normal)
		casefolded = g_utf8_casefold (normal, -1);

	g_free (valid);
	g_free (normal);

	return casefolded;
}