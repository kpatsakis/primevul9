compare_cookies (gconstpointer a, gconstpointer b, gpointer jar)
{
	SoupCookie *ca = (SoupCookie *)a;
	SoupCookie *cb = (SoupCookie *)b;
	SoupCookieJarPrivate *priv = soup_cookie_jar_get_instance_private (jar);
	int alen, blen;
	guint aserial, bserial;

	/* "Cookies with longer path fields are listed before cookies
	 * with shorter path field."
	 */
	alen = ca->path ? strlen (ca->path) : 0;
	blen = cb->path ? strlen (cb->path) : 0;
	if (alen != blen)
		return blen - alen;

	/* "Among cookies that have equal length path fields, cookies
	 * with earlier creation dates are listed before cookies with
	 * later creation dates."
	 */
	aserial = GPOINTER_TO_UINT (g_hash_table_lookup (priv->serials, ca));
	bserial = GPOINTER_TO_UINT (g_hash_table_lookup (priv->serials, cb));
	return aserial - bserial;
}