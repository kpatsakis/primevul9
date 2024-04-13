conv_get_fallback_for_private_encoding(const gchar *encoding)
{
	if (encoding && (encoding[0] == 'X' || encoding[0] == 'x') &&
	    encoding[1] == '-') {
		if (!g_ascii_strcasecmp(encoding, CS_X_MACCYR))
			return CS_MACCYR;
		if (!g_ascii_strcasecmp(encoding, CS_X_GBK))
			return CS_GBK;
	}

	return encoding;
}