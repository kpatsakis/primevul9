static gint conv_noconv(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	strncpy2(outbuf, inbuf, outlen);
	return 0;
}