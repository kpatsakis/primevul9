imapx_uids_desc_cmp (gconstpointer ap,
		     gconstpointer bp)
{
	const gchar *a = (const gchar *) ap;
	const gchar *b = (const gchar *) bp;

	return imapx_refresh_info_uid_cmp (a, b, FALSE);
}