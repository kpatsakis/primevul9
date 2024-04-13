gchar *conv_unmime_header(const gchar *str, const gchar *default_encoding,
			   gboolean addr_field)
{
	gchar buf[BUFFSIZE];

	if (is_ascii_str(str))
		return unmime_header(str, addr_field);

	if (default_encoding) {
		gchar *utf8_buf;

		utf8_buf = conv_codeset_strdup
			(str, default_encoding, CS_INTERNAL);
		if (utf8_buf) {
			gchar *decoded_str;

			decoded_str = unmime_header(utf8_buf, addr_field);
			g_free(utf8_buf);
			return decoded_str;
		}
	}

	if (conv_is_ja_locale())
		conv_anytodisp(buf, sizeof(buf), str);
	else
		conv_localetodisp(buf, sizeof(buf), str);

	return unmime_header(buf, addr_field);
}