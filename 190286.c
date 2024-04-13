static php_iconv_err_t _php_iconv_substr(smart_str *pretval,
	const char *str, size_t nbytes, int offset, int len, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd1, cd2;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;
	int total_len;

	err = _php_iconv_strlen(&total_len, str, nbytes, enc);
	if (err != PHP_ICONV_ERR_SUCCESS) {
		return err;
	}

	if (len < 0) {
		if ((len += (total_len - offset)) < 0) {
			return PHP_ICONV_ERR_SUCCESS;
		}
	}

	if (offset < 0) {
		if ((offset += total_len) < 0) {
			return PHP_ICONV_ERR_SUCCESS;
		}
	}

	if(len > total_len) {
		len = total_len;
	}


	if (offset >= total_len) {
		return PHP_ICONV_ERR_SUCCESS;
	}

	if ((offset + len) > total_len ) {
		/* trying to compute the length */
		len = total_len - offset;
	}

	if (len == 0) {
		smart_str_appendl(pretval, "", 0);
		smart_str_0(pretval);
		return PHP_ICONV_ERR_SUCCESS;
	}

	cd1 = iconv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd1 == (iconv_t)(-1)) {
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

	cd2 = (iconv_t)NULL;
	errno = 0;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0 && len > 0; ++cnt) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (iconv(cd1, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
				break;
			}
		}

		if (cnt >= (unsigned int)offset) {
			if (cd2 == (iconv_t)NULL) {
				cd2 = iconv_open(enc, GENERIC_SUPERSET_NAME);

				if (cd2 == (iconv_t)(-1)) {
					cd2 = (iconv_t)NULL;
#if ICONV_SUPPORTS_ERRNO
					if (errno == EINVAL) {
						err = PHP_ICONV_ERR_WRONG_CHARSET;
					} else {
						err = PHP_ICONV_ERR_CONVERTER;
					}
#else
					err = PHP_ICONV_ERR_UNKNOWN;
#endif
					break;
				}
			}

			if (_php_iconv_appendl(pretval, buf, sizeof(buf), cd2) != PHP_ICONV_ERR_SUCCESS) {
				break;
			}
			--len;
		}

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
			break;
	}
#endif
	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (cd2 != (iconv_t)NULL) {
			_php_iconv_appendl(pretval, NULL, 0, cd2);
		}
		smart_str_0(pretval);
	}

	if (cd1 != (iconv_t)NULL) {
		iconv_close(cd1);
	}

	if (cd2 != (iconv_t)NULL) {
		iconv_close(cd2);
	}
	return err;
}