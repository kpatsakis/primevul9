const gchar *conv_get_outgoing_charset_str(void)
{
	CharSet out_charset;
	const gchar *str;

	out_charset = conv_get_outgoing_charset();
	str = conv_get_charset_str(out_charset);

	return str ? str : CS_UTF_8;
}