ip6_to_str_buf_with_pfx(const ws_in6_addr *addr, gchar *buf, int buf_size, const char *prefix)
{
	int bytes;    /* the number of bytes which would be produced if the buffer was large enough. */
	gchar addr_buf[WS_INET6_ADDRSTRLEN];
	int len;

	if (prefix == NULL)
		prefix = "";
	bytes = g_snprintf(buf, buf_size, "%s%s", prefix, ws_inet_ntop6(addr, addr_buf, sizeof(addr_buf)));
	len = bytes - 1;

	if (len > buf_size - 1) { /* size minus nul terminator */
		len = (int)g_strlcpy(buf, BUF_TOO_SMALL_ERR, buf_size);  /* Let the unexpected value alert user */
	}
	return len;
}