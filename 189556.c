e_strftime (gchar *string,
            gsize max,
            const gchar *fmt,
            const struct tm *tm)
{
#ifndef HAVE_LKSTRFTIME
	gchar *c, *ffmt, *ff;
#endif
	gsize ret;

	g_return_val_if_fail (string != NULL, 0);
	g_return_val_if_fail (fmt != NULL, 0);
	g_return_val_if_fail (tm != NULL, 0);

#ifdef HAVE_LKSTRFTIME
	ret = strftime (string, max, fmt, tm);
#else
	ffmt = g_strdup (fmt);
	ff = ffmt;
	while ((c = strstr (ff, "%l")) != NULL) {
		c[1] = 'I';
		ff = c;
	}

	ff = ffmt;
	while ((c = strstr (ff, "%k")) != NULL) {
		c[1] = 'H';
		ff = c;
	}

#ifdef G_OS_WIN32
	/* The Microsoft strftime () doesn't have %e either */
	ff = ffmt;
	while ((c = strstr (ff, "%e")) != NULL) {
		c[1] = 'd';
		ff = c;
	}
#endif

	ret = strftime (string, max, ffmt, tm);
	g_free (ffmt);
#endif

	if (ret == 0 && max > 0)
		string[0] = '\0';

	return ret;
}