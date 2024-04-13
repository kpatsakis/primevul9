static gint conv_sjistodisp(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	return conv_sjistoutf8(outbuf, outlen, inbuf);
}