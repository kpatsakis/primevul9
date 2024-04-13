static php_iconv_err_t _php_iconv_strpos(unsigned int *pretval,
	const char *haystk, size_t haystk_nbytes,
	const char *ndl, size_t ndl_nbytes,
	int offset, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;

	char *ndl_buf;
	const char *ndl_buf_p;
	size_t ndl_buf_len, ndl_buf_left;

	unsigned int match_ofs;

	*pretval = (unsigned int)-1;

	err = php_iconv_string(ndl, ndl_nbytes,
		&ndl_buf, &ndl_buf_len, GENERIC_SUPERSET_NAME, enc);

	if (err != PHP_ICONV_ERR_SUCCESS) {
		if (ndl_buf != NULL) {
			efree(ndl_buf);
		}
		return err;
	}

	cd = iconv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
		if (ndl_buf != NULL) {
			efree(ndl_buf);
		}
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

	ndl_buf_p = ndl_buf;
	ndl_buf_left = ndl_buf_len;
	match_ofs = (unsigned int)-1;

	for (in_p = haystk, in_left = haystk_nbytes, cnt = 0; in_left > 0; ++cnt) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
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

					default:
						err = PHP_ICONV_ERR_UNKNOWN;
						break;
				}
#endif
				break;
			}
		}
		if (offset >= 0) {
			if (cnt >= (unsigned int)offset) {
				if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
					if (match_ofs == (unsigned int)-1) {
						match_ofs = cnt;
					}
					ndl_buf_p += GENERIC_SUPERSET_NBYTES;
					ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
					if (ndl_buf_left == 0) {
						*pretval = match_ofs;
						break;
					}
				} else {
					unsigned int i, j, lim;

					i = 0;
					j = GENERIC_SUPERSET_NBYTES;
					lim = (unsigned int)(ndl_buf_p - ndl_buf);

					while (j < lim) {
						if (_php_iconv_memequal(&ndl_buf[j], &ndl_buf[i],
						           GENERIC_SUPERSET_NBYTES)) {
							i += GENERIC_SUPERSET_NBYTES;
						} else {
							j -= i;
							i = 0;
						}
						j += GENERIC_SUPERSET_NBYTES;
					}

					if (_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
						match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
						i += GENERIC_SUPERSET_NBYTES;
						ndl_buf_p = &ndl_buf[i];
						ndl_buf_left = ndl_buf_len - i;
					} else {
						match_ofs = (unsigned int)-1;
						ndl_buf_p = ndl_buf;
						ndl_buf_left = ndl_buf_len;
					}
				}
			}
		} else {
			if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
				if (match_ofs == (unsigned int)-1) {
					match_ofs = cnt;
				}
				ndl_buf_p += GENERIC_SUPERSET_NBYTES;
				ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
				if (ndl_buf_left == 0) {
					*pretval = match_ofs;
					ndl_buf_p = ndl_buf;
					ndl_buf_left = ndl_buf_len;
					match_ofs = -1;
				}
			} else {
				unsigned int i, j, lim;

				i = 0;
				j = GENERIC_SUPERSET_NBYTES;
				lim = (unsigned int)(ndl_buf_p - ndl_buf);

				while (j < lim) {
					if (_php_iconv_memequal(&ndl_buf[j], &ndl_buf[i],
							   GENERIC_SUPERSET_NBYTES)) {
						i += GENERIC_SUPERSET_NBYTES;
					} else {
						j -= i;
						i = 0;
					}
					j += GENERIC_SUPERSET_NBYTES;
				}

				if (_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
					match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
					i += GENERIC_SUPERSET_NBYTES;
					ndl_buf_p = &ndl_buf[i];
					ndl_buf_left = ndl_buf_len - i;
				} else {
					match_ofs = (unsigned int)-1;
					ndl_buf_p = ndl_buf;
					ndl_buf_left = ndl_buf_len;
				}
			}
		}
	}

	if (ndl_buf) {
		efree(ndl_buf);
	}

	iconv_close(cd);

	return err;
}