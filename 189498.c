e_util_strv_equal (gconstpointer v1,
                   gconstpointer v2)
{
	gchar **strv1 = (gchar **) v1;
	gchar **strv2 = (gchar **) v2;
	guint length1, length2, ii;

	if (strv1 == strv2)
		return TRUE;

	if (strv1 == NULL || strv2 == NULL)
		return FALSE;

	length1 = g_strv_length (strv1);
	length2 = g_strv_length (strv2);

	if (length1 != length2)
		return FALSE;

	for (ii = 0; ii < length1; ii++) {
		if (!g_str_equal (strv1[ii], strv2[ii]))
			return FALSE;
	}

	return TRUE;
}