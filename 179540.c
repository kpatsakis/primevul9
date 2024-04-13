ip_to_str_buf(const guint8 *ad, gchar *buf, const int buf_len)
{
	register gchar const *p;
	register gchar *b=buf;

	if (buf_len < WS_INET_ADDRSTRLEN) {
		g_strlcpy(buf, BUF_TOO_SMALL_ERR, buf_len);  /* Let the unexpected value alert user */
		return;
	}

	p=fast_strings[*ad++];
	do {
		*b++=*p;
		p++;
	} while(*p);
	*b++='.';

	p=fast_strings[*ad++];
	do {
		*b++=*p;
		p++;
	} while(*p);
	*b++='.';

	p=fast_strings[*ad++];
	do {
		*b++=*p;
		p++;
	} while(*p);
	*b++='.';

	p=fast_strings[*ad];
	do {
		*b++=*p;
		p++;
	} while(*p);
	*b=0;
}