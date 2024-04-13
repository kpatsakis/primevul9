void conv_localetodisp(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gchar *tmpstr;

	codeconv_set_strict(TRUE);
	tmpstr = conv_iconv_strdup(inbuf, conv_get_locale_charset_str(),
				   CS_INTERNAL);
	codeconv_set_strict(FALSE);
	if (tmpstr && g_utf8_validate(tmpstr, -1, NULL)) {
		strncpy2(outbuf, tmpstr, outlen);
		g_free(tmpstr);
		return;
	} else if (tmpstr && !g_utf8_validate(tmpstr, -1, NULL)) {
		g_free(tmpstr);
		codeconv_set_strict(TRUE);
		tmpstr = conv_iconv_strdup(inbuf, 
				conv_get_locale_charset_str_no_utf8(),
				CS_INTERNAL);
		codeconv_set_strict(FALSE);
	}
	if (tmpstr && g_utf8_validate(tmpstr, -1, NULL)) {
		strncpy2(outbuf, tmpstr, outlen);
		g_free(tmpstr);
		return;
	} else {
		g_free(tmpstr);
		conv_utf8todisp(outbuf, outlen, inbuf);
	}
}