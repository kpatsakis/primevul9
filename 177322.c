static CodeConvFunc conv_get_code_conv_func(const gchar *src_charset_str,
				     const gchar *dest_charset_str)
{
	CodeConvFunc code_conv = conv_noconv;
	CharSet src_charset;
	CharSet dest_charset;

	if (!src_charset_str)
		src_charset = conv_get_locale_charset();
	else
		src_charset = conv_get_charset_from_str(src_charset_str);

	/* auto detection mode */
	if (!src_charset_str && !dest_charset_str) {
		if (conv_is_ja_locale())
			return conv_anytodisp;
		else
			return conv_noconv;
	}

	dest_charset = conv_get_charset_from_str(dest_charset_str);

	if (dest_charset == C_US_ASCII)
		return conv_ustodisp;

	switch (src_charset) {
	case C_US_ASCII:
	case C_ISO_8859_1:
	case C_ISO_8859_2:
	case C_ISO_8859_3:
	case C_ISO_8859_4:
	case C_ISO_8859_5:
	case C_ISO_8859_6:
	case C_ISO_8859_7:
	case C_ISO_8859_8:
	case C_ISO_8859_9:
	case C_ISO_8859_10:
	case C_ISO_8859_11:
	case C_ISO_8859_13:
	case C_ISO_8859_14:
	case C_ISO_8859_15:
		break;
	case C_ISO_2022_JP:
	case C_ISO_2022_JP_2:
	case C_ISO_2022_JP_3:
		if (dest_charset == C_AUTO)
			code_conv = conv_jistodisp;
		else if (dest_charset == C_EUC_JP)
			code_conv = conv_jistoeuc;
		else if (dest_charset == C_UTF_8)
			code_conv = conv_jistoutf8;
		break;
	case C_SHIFT_JIS:
		if (dest_charset == C_AUTO)
			code_conv = conv_sjistodisp;
		else if (dest_charset == C_EUC_JP)
			code_conv = conv_sjistoeuc;
		else if (dest_charset == C_UTF_8)
			code_conv = conv_sjistoutf8;
		break;
	case C_EUC_JP:
		if (dest_charset == C_AUTO)
			code_conv = conv_euctodisp;
		else if (dest_charset == C_ISO_2022_JP   ||
			 dest_charset == C_ISO_2022_JP_2 ||
			 dest_charset == C_ISO_2022_JP_3)
			code_conv = conv_euctojis;
		else if (dest_charset == C_UTF_8)
			code_conv = conv_euctoutf8;
		break;
	case C_UTF_8:
		if (dest_charset == C_EUC_JP)
			code_conv = conv_utf8toeuc;
		else if (dest_charset == C_ISO_2022_JP   ||
			 dest_charset == C_ISO_2022_JP_2 ||
			 dest_charset == C_ISO_2022_JP_3)
			code_conv = conv_utf8tojis;
		break;
	default:
		break;
	}

	return code_conv;
}