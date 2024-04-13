PHP_ICONV_API php_iconv_err_t php_iconv_string(const char *in_p, size_t in_len,
							char **out, size_t *out_len,
							const char *out_charset, const char *in_charset)
{
#if !ICONV_SUPPORTS_ERRNO
	size_t in_size, out_size, out_left;
	char *out_buffer, *out_p;
	iconv_t cd;
	size_t result;

	*out = NULL;
	*out_len = 0;

	/*
	  This is not the right way to get output size...
	  This is not space efficient for large text.
	  This is also problem for encoding like UTF-7/UTF-8/ISO-2022 which
	  a single char can be more than 4 bytes.
	  I added 15 extra bytes for safety. <yohgaki@php.net>
	*/
	out_size = in_len * sizeof(int) + 15;
	out_left = out_size;

	in_size = in_len;

	cd = iconv_open(out_charset, in_charset);

	if (cd == (iconv_t)(-1)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}

	out_buffer = (char *) emalloc(out_size + 1);
	out_p = out_buffer;

#ifdef NETWARE
	result = iconv(cd, (char **) &in_p, &in_size, (char **)
#else
	result = iconv(cd, (const char **) &in_p, &in_size, (char **)
#endif
				&out_p, &out_left);

	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	if (out_left < 8) {
		size_t pos = out_p - out_buffer;
		out_buffer = (char *) safe_erealloc(out_buffer, out_size, 1, 8);
		out_p = out_buffer+pos;
		out_size += 7;
		out_left += 7;
	}

	/* flush the shift-out sequences */
	result = iconv(cd, NULL, NULL, &out_p, &out_left);

	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	*out_len = out_size - out_left;
	out_buffer[*out_len] = '\0';
	*out = out_buffer;

	iconv_close(cd);

	return PHP_ICONV_ERR_SUCCESS;

#else
	/*
	  iconv supports errno. Handle it better way.
	*/
	iconv_t cd;
	size_t in_left, out_size, out_left;
	char *out_p, *out_buf, *tmp_buf;
	size_t bsz, result = 0;
	php_iconv_err_t retval = PHP_ICONV_ERR_SUCCESS;
	int ignore_ilseq = _php_check_ignore(out_charset);

	*out = NULL;
	*out_len = 0;

	cd = iconv_open(out_charset, in_charset);

	if (cd == (iconv_t)(-1)) {
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
	}
	in_left= in_len;
	out_left = in_len + 32; /* Avoid realloc() most cases */
	out_size = 0;
	bsz = out_left;
	out_buf = (char *) emalloc(bsz+1);
	out_p = out_buf;

	while (in_left > 0) {
		result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
		out_size = bsz - out_left;
		if (result == (size_t)(-1)) {
			if (ignore_ilseq && errno == EILSEQ) {
				if (in_left <= 1) {
					result = 0;
				} else {
					errno = 0;
					in_p++;
					in_left--;
					continue;
				}
			}

			if (errno == E2BIG && in_left > 0) {
				/* converted string is longer than out buffer */
				bsz += in_len;

				tmp_buf = (char*) erealloc(out_buf, bsz+1);
				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
				continue;
			}
		}
		break;
	}

	if (result != (size_t)(-1)) {
		/* flush the shift-out sequences */
		for (;;) {
		   	result = iconv(cd, NULL, NULL, (char **) &out_p, &out_left);
			out_size = bsz - out_left;

			if (result != (size_t)(-1)) {
				break;
			}

			if (errno == E2BIG) {
				bsz += 16;
				tmp_buf = (char *) erealloc(out_buf, bsz);

				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
			} else {
				break;
			}
		}
	}

	iconv_close(cd);

	if (result == (size_t)(-1)) {
		switch (errno) {
			case EINVAL:
				retval = PHP_ICONV_ERR_ILLEGAL_CHAR;
				break;

			case EILSEQ:
				retval = PHP_ICONV_ERR_ILLEGAL_SEQ;
				break;

			case E2BIG:
				/* should not happen */
				retval = PHP_ICONV_ERR_TOO_BIG;
				break;

			default:
				/* other error */
				retval = PHP_ICONV_ERR_UNKNOWN;
				efree(out_buf);
				return PHP_ICONV_ERR_UNKNOWN;
		}
	}
	*out_p = '\0';
	*out = out_buf;
	*out_len = out_size;
	return retval;
#endif
}