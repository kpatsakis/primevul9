const gchar *conv_get_locale_charset_str_no_utf8(void)
{
	static const gchar *codeset = NULL;

	if (!codeset)
		codeset = conv_get_charset_str(conv_get_locale_charset_no_utf8());

	return codeset ? codeset : CS_INTERNAL;
}