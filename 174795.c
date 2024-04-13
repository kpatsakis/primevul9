soup_ntlm_nt_hash (const char *password, guchar hash[21])
{
	unsigned char *buf, *p;

	p = buf = g_malloc (strlen (password) * 2);

	while (*password) {
		*p++ = *password++;
		*p++ = '\0';
	}

	md4sum (buf, p - buf, hash);
	memset (hash + 16, 0, 5);

	g_free (buf);
}