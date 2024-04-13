e_util_strdup_strip (const gchar *string)
{
	gchar *duplicate;

	duplicate = g_strdup (string);
	if (duplicate != NULL) {
		g_strstrip (duplicate);
		if (*duplicate == '\0') {
			g_free (duplicate);
			duplicate = NULL;
		}
	}

	return duplicate;
}