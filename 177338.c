gint conv_convert(CodeConverter *conv, gchar *outbuf, gint outlen,
		  const gchar *inbuf)
{
	if (conv->code_conv_func != conv_noconv)
		return conv->code_conv_func(outbuf, outlen, inbuf);
	else {
		gchar *str;

		str = conv_iconv_strdup(inbuf, conv->charset_str, NULL);
		if (!str)
			return -1;
		else {
			strncpy2(outbuf, str, outlen);
			g_free(str);
		}
	}

	return 0;
}