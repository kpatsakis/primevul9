guint32_to_str_buf(guint32 u, gchar *buf, int buf_len)
{
	int str_len = guint32_to_str_buf_len(u)+1;

	gchar *bp = &buf[str_len];

	if (buf_len < str_len) {
		g_strlcpy(buf, BUF_TOO_SMALL_ERR, buf_len);	/* Let the unexpected value alert user */
		return;
	}

	*--bp = '\0';

	uint_to_str_back(bp, u);
}