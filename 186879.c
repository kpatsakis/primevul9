file_or_stream(struct magic_set *ms, const char *inname, php_stream *stream)
{
	int	rv = -1;
	unsigned char *buf;
	struct stat	sb;
	ssize_t nbytes = 0;	/* number of bytes read from a datafile */
	int no_in_stream = 0;
	TSRMLS_FETCH();

	if (!inname && !stream) {
		return NULL;
	}

	/*
	 * one extra for terminating '\0', and
	 * some overlapping space for matches near EOF
	 */
#define SLOP (1 + sizeof(union VALUETYPE))
	buf = emalloc(HOWMANY + SLOP);

	if (file_reset(ms) == -1)
		goto done;

	switch (file_fsmagic(ms, inname, &sb, stream)) {
	case -1:		/* error */
		goto done;
	case 0:			/* nothing found */
		break;
	default:		/* matched it and printed type */
		rv = 0;
		goto done;
	}

	errno = 0;

	if (!stream && inname) {
		no_in_stream = 1;
#if PHP_API_VERSION < 20100412
		stream = php_stream_open_wrapper((char *)inname, "rb", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);
#else
		stream = php_stream_open_wrapper((char *)inname, "rb", REPORT_ERRORS, NULL);
#endif
	}

	if (!stream) {
		if (unreadable_info(ms, sb.st_mode, inname) == -1)
			goto done;
		rv = 0;
		goto done;
	}

#ifdef O_NONBLOCK
/* we should be already be in non blocking mode for network socket */
#endif

	/*
	 * try looking at the first HOWMANY bytes
	 */
	if ((nbytes = php_stream_read(stream, (char *)buf, HOWMANY)) < 0) {
		file_error(ms, errno, "cannot read `%s'", inname);
		goto done;
	}

	(void)memset(buf + nbytes, 0, SLOP); /* NUL terminate */
	if (file_buffer(ms, stream, inname, buf, (size_t)nbytes) == -1)
		goto done;
	rv = 0;
done:
	efree(buf);

	if (no_in_stream && stream) {
		php_stream_close(stream);
	}

	close_and_restore(ms, inname, 0, &sb);
	return rv == 0 ? file_getbuffer(ms) : NULL;
}