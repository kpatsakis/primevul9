const gchar *conv_get_locale_charset_str(void)
{
	static const gchar *codeset = NULL;

	if (!codeset)
		codeset = conv_get_charset_str(conv_get_locale_charset());

	return codeset ? codeset : CS_INTERNAL;
}