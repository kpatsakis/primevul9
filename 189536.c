e_util_utf8_data_make_valid (const gchar *data,
                             gsize data_bytes)
{
	/* almost identical copy of glib's _g_utf8_make_valid() */
	GString *string;
	const gchar *remainder, *invalid;
	gint remaining_bytes, valid_bytes;

	if (!data)
		return g_strdup ("");

	string = NULL;
	remainder = (gchar *) data,
	remaining_bytes = data_bytes;

	while (remaining_bytes != 0) {
		if (g_utf8_validate (remainder, remaining_bytes, &invalid))
			break;
		valid_bytes = invalid - remainder;

		if (string == NULL)
			string = g_string_sized_new (remaining_bytes);

		g_string_append_len (string, remainder, valid_bytes);
		/* append U+FFFD REPLACEMENT CHARACTER */
		g_string_append (string, "\357\277\275");

		remaining_bytes -= valid_bytes + 1;
		remainder = invalid + 1;
	}

	if (string == NULL)
		return g_strndup ((gchar *) data, data_bytes);

	g_string_append (string, remainder);

	g_warn_if_fail (g_utf8_validate (string->str, -1, NULL));

	return g_string_free (string, FALSE);
}