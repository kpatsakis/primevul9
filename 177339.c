static CharSet conv_guess_ja_encoding(const gchar *str)
{
	const guchar *p = str;
	CharSet guessed = C_US_ASCII;

	while (*p != '\0') {
		if (*p == ESC && (*(p + 1) == '$' || *(p + 1) == '(')) {
			if (guessed == C_US_ASCII)
				return C_ISO_2022_JP;
			p += 2;
		} else if (IS_ASCII(*p)) {
			p++;
		} else if (iseuckanji(*p) && iseuckanji(*(p + 1))) {
			if (*p >= 0xfd && *p <= 0xfe)
				return C_EUC_JP;
			else if (guessed == C_SHIFT_JIS) {
				if ((issjiskanji1(*p) &&
				     issjiskanji2(*(p + 1))) ||
				    issjishwkana(*p))
					guessed = C_SHIFT_JIS;
				else
					guessed = C_EUC_JP;
			} else
				guessed = C_EUC_JP;
			p += 2;
		} else if (issjiskanji1(*p) && issjiskanji2(*(p + 1))) {
			if (iseuchwkana1(*p) && iseuchwkana2(*(p + 1)))
				guessed = C_SHIFT_JIS;
			else
				return C_SHIFT_JIS;
			p += 2;
		} else if (issjishwkana(*p)) {
			guessed = C_SHIFT_JIS;
			p++;
		} else {
			p++;
		}
	}

	return guessed;
}