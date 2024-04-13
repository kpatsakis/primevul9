static gint conv_sjistoutf8(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gchar *tmpstr;

	tmpstr = conv_iconv_strdup(inbuf, CS_SHIFT_JIS, CS_UTF_8);
	if (tmpstr) {
		strncpy2(outbuf, tmpstr, outlen);
		g_free(tmpstr);
		return 0;
	} else {
		strncpy2(outbuf, inbuf, outlen);
		return -1;
	}
}