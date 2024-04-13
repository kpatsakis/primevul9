CodeConverter *conv_code_converter_new(const gchar *src_charset)
{
	CodeConverter *conv;

	src_charset = conv_get_fallback_for_private_encoding(src_charset);

	conv = g_new0(CodeConverter, 1);
	conv->code_conv_func = conv_get_code_conv_func(src_charset, NULL);
	conv->charset_str = g_strdup(src_charset);
	conv->charset = conv_get_charset_from_str(src_charset);

	return conv;
}