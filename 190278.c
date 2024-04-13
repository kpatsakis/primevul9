static php_iconv_err_t _php_iconv_appendl(smart_str *d, const char *s, size_t l, iconv_t cd)
{
	const char *in_p = s;
	size_t in_left = l;
	char *out_p;
	size_t out_left = 0;
	size_t buf_growth = 128;
#if !ICONV_SUPPORTS_ERRNO
	size_t prev_in_left = in_left;
#endif

	if (in_p != NULL) {
		while (in_left > 0) {
			out_left = buf_growth - out_left;
			{
				size_t newlen;
				smart_str_alloc((d), out_left, 0);
			}

			out_p = (d)->c + (d)->len;

			if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
				switch (errno) {
					case EINVAL:
						return PHP_ICONV_ERR_ILLEGAL_CHAR;

					case EILSEQ:
						return PHP_ICONV_ERR_ILLEGAL_SEQ;

					case E2BIG:
						break;

					default:
						return PHP_ICONV_ERR_UNKNOWN;
				}
#else
				if (prev_in_left == in_left) {
					return PHP_ICONV_ERR_UNKNOWN;
				}
#endif
			}
#if !ICONV_SUPPORTS_ERRNO
			prev_in_left = in_left;
#endif
			(d)->len += (buf_growth - out_left);
			buf_growth <<= 1;
		}
	} else {
		for (;;) {
			out_left = buf_growth - out_left;
			{
				size_t newlen;
				smart_str_alloc((d), out_left, 0);
			}

			out_p = (d)->c + (d)->len;

			if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)0) {
				(d)->len += (buf_growth - out_left);
				break;
			} else {
#if ICONV_SUPPORTS_ERRNO
				if (errno != E2BIG) {
					return PHP_ICONV_ERR_UNKNOWN;
				}
#else
				if (out_left != 0) {
					return PHP_ICONV_ERR_UNKNOWN;
				}
#endif
			}
			(d)->len += (buf_growth - out_left);
			buf_growth <<= 1;
		}
	}
	return PHP_ICONV_ERR_SUCCESS;
}