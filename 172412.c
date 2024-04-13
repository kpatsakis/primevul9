imapx_server_check_is_broken_cyrus (const gchar *response_text,
				    gboolean *inout_is_cyrus)
{
	const gchar *pp, *from;
	gint vermajor = 0, verminor = 0, vermicro = 0;

	g_return_val_if_fail (inout_is_cyrus != NULL, FALSE);

	/* If already known that this is cyrus server, then it had been
	   identified as a good server, thus just return here. */
	if (*inout_is_cyrus)
		return FALSE;

	if (!response_text || !*response_text)
		return FALSE;

	/* Expects "Cyrus IMAP v1.2.3", eventually "Cyrus IMAP 4.5.6" (with or without 'v' prefix) */
	pp = response_text;
	while (pp = camel_strstrcase (pp, "cyrus"), pp) {
		/* It's a whole word */
		if ((pp == response_text || g_ascii_isspace (pp[-1])) && g_ascii_isspace (pp[5]))
			break;
		pp++;
	}

	if (!pp)
		return FALSE;

	from = pp;

	/* In case there is the 'cyrus' word multiple times */
	while (pp = from, pp && *pp) {
		#define skip_word() \
			while (*pp && *pp != ' ') {	\
				pp++;			\
			}				\
							\
			if (!*pp)			\
				return TRUE;		\
							\
			pp++;

		/* Skip the 'Cyrus' word */
		skip_word ();

		/* Skip the 'IMAP' word */
		skip_word ();

		#undef skip_word

		/* Now is at version with or without 'v' prefix */
		if (*pp == 'v')
			pp++;

		if (sscanf (pp, "%d.%d.%d", &vermajor, &verminor, &vermicro) == 3) {
			*inout_is_cyrus = TRUE;
			break;
		}

		vermajor = 0;

		pp = from + 1;
		from = NULL;

		while (pp = camel_strstrcase (pp, "cyrus"), pp) {
			/* It's a whole word */
			if (g_ascii_isspace (pp[-1]) && g_ascii_isspace (pp[5])) {
				from = pp;
				break;
			}

			pp++;
		}
	}

	/* The 2.5.11, inclusive, has the issue fixed, thus check for that version. */
	return !(vermajor > 2 || (vermajor == 2 && (verminor > 5 || (verminor == 5 && vermicro >= 11))));
}