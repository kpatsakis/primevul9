static gint conv_anytodisp(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gint r = 0;
	if (conv_anytoutf8(outbuf, outlen, inbuf) < 0)
		r = -1;
	if (g_utf8_validate(outbuf, -1, NULL) != TRUE)
		conv_unreadable_8bit(outbuf);
	return r;
}