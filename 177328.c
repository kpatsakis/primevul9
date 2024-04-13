static gint conv_anytoutf8(gchar *outbuf, gint outlen, const gchar *inbuf)
{
	gint r = -1;
	switch (conv_guess_ja_encoding(inbuf)) {
	case C_ISO_2022_JP:
		r = conv_jistoutf8(outbuf, outlen, inbuf);
		break;
	case C_SHIFT_JIS:
		r = conv_sjistoutf8(outbuf, outlen, inbuf);
		break;
	case C_EUC_JP:
		r = conv_euctoutf8(outbuf, outlen, inbuf);
		break;
	default:
		r = 0;
		strncpy2(outbuf, inbuf, outlen);
		break;
	}
	
	return r;
}