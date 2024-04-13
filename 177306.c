gchar *conv_codeset_strdup(const gchar *inbuf,
			   const gchar *src_code, const gchar *dest_code)
{
	gchar *buf;
	size_t len;
	CodeConvFunc conv_func;

	if (!strcmp2(src_code, dest_code)) {
		CharSet dest_charset = conv_get_charset_from_str(dest_code);
		if (strict_mode && dest_charset == C_UTF_8) {
			/* ensure valid UTF-8 if target is UTF-8 */
			if (!g_utf8_validate(inbuf, -1, NULL)) {
				return NULL;
			}
		}
		/* otherwise, try for a lucky day */
		return g_strdup(inbuf);
	}

	src_code = conv_get_fallback_for_private_encoding(src_code);
	conv_func = conv_get_code_conv_func(src_code, dest_code);
	if (conv_func == conv_ustodisp && strict_mode && !is_ascii_str(inbuf))
		return NULL;

	if (conv_func != conv_noconv) {
		len = (strlen(inbuf) + 1) * 3;
		buf = g_malloc(len);
		if (!buf) return NULL;

		if (conv_func(buf, len, inbuf) == 0 || !strict_mode)
			return g_realloc(buf, strlen(buf) + 1);
		else {
			g_free(buf);
			return NULL;
		}
	}

	return conv_iconv_strdup(inbuf, src_code, dest_code);
}