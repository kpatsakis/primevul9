static gint conv_jistoutf8(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gchar *eucstr;

	Xalloca(eucstr, outlen, return -1);

	if (conv_jistoeuc(eucstr, outlen, inbuf) <0)
		return -1;
	if (conv_euctoutf8(outbuf, outlen, eucstr) < 0)
		return -1;
	return 0;
}