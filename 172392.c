imapx_refresh_info_uid_cmp (gconstpointer ap,
                            gconstpointer bp,
                            gboolean ascending)
{
	guint av, bv;

	av = g_ascii_strtoull ((const gchar *) ap, NULL, 10);
	bv = g_ascii_strtoull ((const gchar *) bp, NULL, 10);

	if (av < bv)
		return ascending ? -1 : 1;
	else if (av > bv)
		return ascending ? 1 : -1;
	else
		return 0;
}