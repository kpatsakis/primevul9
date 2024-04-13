file_ascmagic(struct magic_set *ms, const unsigned char *buf, size_t nbytes,
	int text)
{
	unichar *ubuf = NULL;
	size_t ulen;
	int rv = 1;

	const char *code = NULL;
	const char *code_mime = NULL;
	const char *type = NULL;

	if (ms->flags & MAGIC_APPLE)
		return 0;

	nbytes = trim_nuls(buf, nbytes);

	/* If file doesn't look like any sort of text, give up. */
	if (file_encoding(ms, buf, nbytes, &ubuf, &ulen, &code, &code_mime,
	    &type) == 0)
		rv = 0;
        else
		rv = file_ascmagic_with_encoding(ms, buf, nbytes, ubuf, ulen, code,
						 type, text);

	free(ubuf);

	return rv;
}