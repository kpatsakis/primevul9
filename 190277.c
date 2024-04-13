static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, unsigned int max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;
	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);
	unsigned int char_cnt = 0;
	size_t out_charset_len;
	size_t lfchars_len;
	char *buf = NULL;
	char *encoded = NULL;
	size_t encoded_len;
	const char *in_p;
	size_t in_left;
	char *out_p;
	size_t out_left;
	static int qp_table[256] = {
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x00 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x10 */
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x20 */
		1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 3, 1, 3, /* 0x30 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x40 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, /* 0x50 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x60 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, /* 0x70 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x80 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x90 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xA0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xB0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xC0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xD0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xE0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3  /* 0xF0 */
	};

	out_charset_len = strlen(out_charset);
	lfchars_len = strlen(lfchars);

	if ((fname_nbytes + 2) >= max_line_len
		|| (out_charset_len + 12) >= max_line_len) {
		/* field name is too long */
		err = PHP_ICONV_ERR_TOO_BIG;
		goto out;
	}

	cd_pl = iconv_open(ICONV_ASCII_ENCODING, enc);
	if (cd_pl == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	cd = iconv_open(out_charset, enc);
	if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	buf = safe_emalloc(1, max_line_len, 5);

	char_cnt = max_line_len;

	_php_iconv_appendl(pretval, fname, fname_nbytes, cd_pl);
	char_cnt -= fname_nbytes;
	smart_str_appendl(pretval, ": ", sizeof(": ") - 1);
	char_cnt -= 2;

	in_p = fval;
	in_left = fval_nbytes;

	do {
		size_t prev_in_left;
		size_t out_size;

		if (char_cnt < (out_charset_len + 12)) {
			/* lfchars must be encoded in ASCII here*/
			smart_str_appendl(pretval, lfchars, lfchars_len);
			smart_str_appendc(pretval, ' ');
			char_cnt = max_line_len - 1;
		}

		smart_str_appendl(pretval, "=?", sizeof("=?") - 1);
		char_cnt -= 2;
		smart_str_appendl(pretval, out_charset, out_charset_len);
		char_cnt -= out_charset_len;
		smart_str_appendc(pretval, '?');
		char_cnt --;

		switch (enc_scheme) {
			case PHP_ICONV_ENC_SCHEME_BASE64: {
				size_t ini_in_left;
				const char *ini_in_p;
				size_t out_reserved = 4;
				int dummy;

				smart_str_appendc(pretval, 'B');
				char_cnt--;
				smart_str_appendc(pretval, '?');
				char_cnt--;

				prev_in_left = ini_in_left = in_left;
				ini_in_p = in_p;

				out_size = (char_cnt - 2) / 4 * 3;

				for (;;) {
					out_p = buf;

					if (out_size <= out_reserved) {
						err = PHP_ICONV_ERR_TOO_BIG;
						goto out;
					}

					out_left = out_size - out_reserved;

					if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						switch (errno) {
							case EINVAL:
								err = PHP_ICONV_ERR_ILLEGAL_CHAR;
								goto out;

							case EILSEQ:
								err = PHP_ICONV_ERR_ILLEGAL_SEQ;
								goto out;

							case E2BIG:
								if (prev_in_left == in_left) {
									err = PHP_ICONV_ERR_TOO_BIG;
									goto out;
								}
								break;

							default:
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
						}
#else
						if (prev_in_left == in_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					out_left += out_reserved;

					if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						if (errno != E2BIG) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#else
						if (out_left != 0) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					} else {
						break;
					}

					if (iconv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
						err = PHP_ICONV_ERR_UNKNOWN;
						goto out;
					}

					out_reserved += 4;
					in_left = ini_in_left;
					in_p = ini_in_p;
				}

				prev_in_left = in_left;

				encoded = (char *) php_base64_encode((unsigned char *) buf, (int)(out_size - out_left), &dummy);
				encoded_len = (size_t)dummy;

				if (char_cnt < encoded_len) {
					/* something went wrong! */
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

				smart_str_appendl(pretval, encoded, encoded_len);
				char_cnt -= encoded_len;
				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				efree(encoded);
				encoded = NULL;
			} break; /* case PHP_ICONV_ENC_SCHEME_BASE64: */

			case PHP_ICONV_ENC_SCHEME_QPRINT: {
				size_t ini_in_left;
				const char *ini_in_p;
				const unsigned char *p;
				size_t nbytes_required;

				smart_str_appendc(pretval, 'Q');
				char_cnt--;
				smart_str_appendc(pretval, '?');
				char_cnt--;

				prev_in_left = ini_in_left = in_left;
				ini_in_p = in_p;

				for (out_size = (char_cnt - 2) / 3; out_size > 0;) {
					size_t prev_out_left;

					nbytes_required = 0;

					out_p = buf;
					out_left = out_size;

					if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						switch (errno) {
							case EINVAL:
								err = PHP_ICONV_ERR_ILLEGAL_CHAR;
								goto out;

							case EILSEQ:
								err = PHP_ICONV_ERR_ILLEGAL_SEQ;
								goto out;

							case E2BIG:
								if (prev_in_left == in_left) {
									err = PHP_ICONV_ERR_UNKNOWN;
									goto out;
								}
								break;

							default:
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
						}
#else
						if (prev_in_left == in_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					prev_out_left = out_left;
					if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						if (errno != E2BIG) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#else
						if (out_left == prev_out_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					for (p = (unsigned char *)buf; p < (unsigned char *)out_p; p++) {
						nbytes_required += qp_table[*p];
					}

					if (nbytes_required <= char_cnt - 2) {
						break;
					}

					out_size -= ((nbytes_required - (char_cnt - 2)) + 1) / 3;
					in_left = ini_in_left;
					in_p = ini_in_p;
				}

				for (p = (unsigned char *)buf; p < (unsigned char *)out_p; p++) {
					if (qp_table[*p] == 1) {
						smart_str_appendc(pretval, *(char *)p);
						char_cnt--;
					} else {
						static char qp_digits[] = "0123456789ABCDEF";
						smart_str_appendc(pretval, '=');
						smart_str_appendc(pretval, qp_digits[(*p >> 4) & 0x0f]);
						smart_str_appendc(pretval, qp_digits[(*p & 0x0f)]);
						char_cnt -= 3;
					}
				}

				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				if (iconv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

			} break; /* case PHP_ICONV_ENC_SCHEME_QPRINT: */
		}
	} while (in_left > 0);

	smart_str_0(pretval);

out:
	if (cd != (iconv_t)(-1)) {
		iconv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		iconv_close(cd_pl);
	}
	if (encoded != NULL) {
		efree(encoded);
	}
	if (buf != NULL) {
		efree(buf);
	}
	return err;
}