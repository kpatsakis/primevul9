void conv_code_converter_destroy(CodeConverter *conv)
{
	g_free(conv->charset_str);
	g_free(conv);
}