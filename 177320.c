void conv_utf8todisp(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	if (g_utf8_validate(inbuf, -1, NULL) == TRUE)
		strncpy2(outbuf, inbuf, outlen);
	else
		conv_ustodisp(outbuf, outlen, inbuf);
}