sso_ntlm_response (SoupAuthNTLMPrivate *priv, const char *input, SoupNTLMState conn_state)
{
	ssize_t size;
	char buf[1024];
	char *tmpbuf = buf;
	size_t	len_in = strlen (input), len_out = sizeof (buf);

	while (len_in > 0) {
		int written = write (priv->fd_in, input, len_in);
		if (written == -1) {
			if (errno == EINTR)
				continue;
			/* write failed if other errors happen */
			return NULL;
		}
		input += written;
		len_in -= written;
	}
	/* Read one line */
	while (len_out > 0) {
		size = read (priv->fd_out, tmpbuf, len_out);
		if (size == -1) {
			if (errno == EINTR)
				continue;
			return NULL;
		} else if (size == 0)
			return NULL;
		else if (tmpbuf[size - 1] == '\n') {
			tmpbuf[size - 1] = '\0';
			goto wrfinish;
		}
		tmpbuf += size;
		len_out -= size;
	}
	return NULL;

wrfinish:
	if (g_ascii_strcasecmp (buf, "PW") == 0) {
		/* Samba/winbind installed but not configured */
		return g_strdup ("PW");
	}
	if (conn_state == SOUP_NTLM_NEW &&
	    g_ascii_strncasecmp (buf, "YR ", 3) != 0) {
		/* invalid response for type 1 message */
		return NULL;
	}
	if (conn_state == SOUP_NTLM_RECEIVED_CHALLENGE &&
	    g_ascii_strncasecmp (buf, "KK ", 3) != 0 &&
	    g_ascii_strncasecmp (buf, "AF ", 3) != 0) {
		/* invalid response for type 3 message */
		return NULL;
	}

	return g_strdup_printf ("NTLM %.*s", (int)(size - 4), buf + 3);
}