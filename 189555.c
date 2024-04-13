e_util_utf8_strcasecmp (const gchar *s1,
                        const gchar *s2)
{
	gchar *folded_s1, *folded_s2;
	gint retval;

	g_return_val_if_fail (s1 != NULL && s2 != NULL, -1);

	if (strcmp (s1, s2) == 0)
		return 0;

	folded_s1 = g_utf8_casefold (s1, -1);
	folded_s2 = g_utf8_casefold (s2, -1);

	retval = g_utf8_collate (folded_s1, folded_s2);

	g_free (folded_s2);
	g_free (folded_s1);

	return retval;
}