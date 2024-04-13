static php_iconv_err_t _php_iconv_strlen(unsigned int *pretval, const char *str, size_t nbytes, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES*2];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;

	*pretval = (unsigned int)-1;

	cd = iconv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
#else
		return PHP_ICONV_ERR_UNKNOWN;
#endif
	}

	errno = out_left = 0;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0; cnt+=2) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
				break;
			}
		}
	}

	if (out_left > 0) {
		cnt -= out_left / GENERIC_SUPERSET_NBYTES;
	}

#if ICONV_SUPPORTS_ERRNO
	switch (errno) {
		case EINVAL:
			err = PHP_ICONV_ERR_ILLEGAL_CHAR;
			break;

		case EILSEQ:
			err = PHP_ICONV_ERR_ILLEGAL_SEQ;
			break;

		case E2BIG:
		case 0:
			*pretval = cnt;
			break;

		default:
			err = PHP_ICONV_ERR_UNKNOWN;
			break;
	}
#else
	*pretval = cnt;
#endif

	iconv_close(cd);

	return err;
}