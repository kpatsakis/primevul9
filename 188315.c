int git_path_iconv(git_path_iconv_t *ic, const char **in, size_t *inlen)
{
	char *nfd = (char*)*in, *nfc;
	size_t nfdlen = *inlen, nfclen, wantlen = nfdlen, alloclen, rv;
	int retry = 1;

	if (!ic || ic->map == (iconv_t)-1 ||
		!git_path_has_non_ascii(*in, *inlen))
		return 0;

	git_buf_clear(&ic->buf);

	while (1) {
		GIT_ERROR_CHECK_ALLOC_ADD(&alloclen, wantlen, 1);
		if (git_buf_grow(&ic->buf, alloclen) < 0)
			return -1;

		nfc    = ic->buf.ptr   + ic->buf.size;
		nfclen = ic->buf.asize - ic->buf.size;

		rv = iconv(ic->map, &nfd, &nfdlen, &nfc, &nfclen);

		ic->buf.size = (nfc - ic->buf.ptr);

		if (rv != (size_t)-1)
			break;

		/* if we cannot convert the data (probably because iconv thinks
		 * it is not valid UTF-8 source data), then use original data
		 */
		if (errno != E2BIG)
			return 0;

		/* make space for 2x the remaining data to be converted
		 * (with per retry overhead to avoid infinite loops)
		 */
		wantlen = ic->buf.size + max(nfclen, nfdlen) * 2 + (size_t)(retry * 4);

		if (retry++ > 4)
			goto fail;
	}

	ic->buf.ptr[ic->buf.size] = '\0';

	*in    = ic->buf.ptr;
	*inlen = ic->buf.size;

	return 0;

fail:
	git_error_set(GIT_ERROR_OS, "unable to convert unicode path data");
	return -1;
}