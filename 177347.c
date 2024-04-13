static gint conv_euctoutf8(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	static iconv_t cd = (iconv_t)-1;
	static gboolean iconv_ok = TRUE;
	gchar *tmpstr;

	if (cd == (iconv_t)-1) {
		if (!iconv_ok) {
			strncpy2(outbuf, inbuf, outlen);
			return -1;
		}
		cd = iconv_open(CS_UTF_8, CS_EUC_JP_MS);
		if (cd == (iconv_t)-1) {
			cd = iconv_open(CS_UTF_8, CS_EUC_JP);
			if (cd == (iconv_t)-1) {
				g_warning("conv_euctoutf8(): %s",
					  g_strerror(errno));
				iconv_ok = FALSE;
				strncpy2(outbuf, inbuf, outlen);
				return -1;
			}
		}
	}

	tmpstr = conv_iconv_strdup_with_cd(inbuf, cd);
	if (tmpstr) {
		strncpy2(outbuf, tmpstr, outlen);
		g_free(tmpstr);
		return 0;
	} else {
		strncpy2(outbuf, inbuf, outlen);
		return -1;
	}
}