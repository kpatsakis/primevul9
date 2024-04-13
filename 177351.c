static gint conv_utf8tojis(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gchar *eucstr;

	Xalloca(eucstr, outlen, return -1);

	if (conv_utf8toeuc(eucstr, outlen, inbuf) < 0)
		return -1;
	if (conv_euctojis(outbuf, outlen, eucstr) < 0)
		return -1;
		
	return 0;
}