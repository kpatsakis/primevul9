file_buffer(struct magic_set *ms, php_stream *stream, const char *inname, const void *buf,
    size_t nb)
{
	int m = 0, rv = 0, looks_text = 0;
	int mime = ms->flags & MAGIC_MIME;
	const unsigned char *ubuf = CAST(const unsigned char *, buf);
	unichar *u8buf = NULL;
	size_t ulen;
	const char *code = NULL;
	const char *code_mime = "binary";
	const char *type = NULL;



	if (nb == 0) {
		if ((!mime || (mime & MAGIC_MIME_TYPE)) &&
		    file_printf(ms, mime ? "application/x-empty" :
		    "empty") == -1)
			return -1;
		return 1;
	} else if (nb == 1) {
		if ((!mime || (mime & MAGIC_MIME_TYPE)) &&
		    file_printf(ms, mime ? "application/octet-stream" :
		    "very short file (no magic)") == -1)
			return -1;
		return 1;
	}

	if ((ms->flags & MAGIC_NO_CHECK_ENCODING) == 0) {
		looks_text = file_encoding(ms, ubuf, nb, &u8buf, &ulen,
		    &code, &code_mime, &type);
	}

#ifdef __EMX__
	if ((ms->flags & MAGIC_NO_CHECK_APPTYPE) == 0 && inname) {
		switch (file_os2_apptype(ms, inname, buf, nb)) {
		case -1:
			return -1;
		case 0:
			break;
		default:
			return 1;
		}
	}
#endif

#if PHP_FILEINFO_UNCOMPRESS
	if ((ms->flags & MAGIC_NO_CHECK_COMPRESS) == 0)
		if ((m = file_zmagic(ms, stream, inname, ubuf, nb)) != 0) {
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void)fprintf(stderr, "zmagic %d\n", m);
			goto done;
		}
#endif
	/* Check if we have a tar file */
	if ((ms->flags & MAGIC_NO_CHECK_TAR) == 0)
		if ((m = file_is_tar(ms, ubuf, nb)) != 0) {
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void)fprintf(stderr, "tar %d\n", m);
			goto done;
		}

	/* Check if we have a CDF file */
	if ((ms->flags & MAGIC_NO_CHECK_CDF) == 0) {
		int fd;
		TSRMLS_FETCH();
		if (stream && SUCCESS == php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&fd, 0)) {
			if ((m = file_trycdf(ms, fd, ubuf, nb)) != 0) {
				if ((ms->flags & MAGIC_DEBUG) != 0)
					(void)fprintf(stderr, "cdf %d\n", m);
				goto done;
			}
		}
	}

	/* try soft magic tests */
	if ((ms->flags & MAGIC_NO_CHECK_SOFT) == 0)
		if ((m = file_softmagic(ms, ubuf, nb, 0, BINTEST,
		    looks_text)) != 0) {
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void)fprintf(stderr, "softmagic %d\n", m);
#ifdef BUILTIN_ELF
			if ((ms->flags & MAGIC_NO_CHECK_ELF) == 0 && m == 1 &&
			    nb > 5 && fd != -1) {
				/*
				 * We matched something in the file, so this
				 * *might* be an ELF file, and the file is at
				 * least 5 bytes long, so if it's an ELF file
				 * it has at least one byte past the ELF magic
				 * number - try extracting information from the
				 * ELF headers that cannot easily * be
				 * extracted with rules in the magic file.
				 */
				if ((m = file_tryelf(ms, fd, ubuf, nb)) != 0)
					if ((ms->flags & MAGIC_DEBUG) != 0)
						(void)fprintf(stderr,
						    "elf %d\n", m);
			}
#endif
			goto done;
		}

	/* try text properties */
	if ((ms->flags & MAGIC_NO_CHECK_TEXT) == 0) {

		if ((m = file_ascmagic(ms, ubuf, nb, looks_text)) != 0) {
			if ((ms->flags & MAGIC_DEBUG) != 0)
				(void)fprintf(stderr, "ascmagic %d\n", m);
			goto done;
		}

		/* try to discover text encoding */
		if ((ms->flags & MAGIC_NO_CHECK_ENCODING) == 0) {
			if (looks_text == 0)
				if ((m = file_ascmagic_with_encoding( ms, ubuf,
				    nb, u8buf, ulen, code, type, looks_text))
				    != 0) {
					if ((ms->flags & MAGIC_DEBUG) != 0)
						(void)fprintf(stderr,
						    "ascmagic/enc %d\n", m);
					goto done;
				}
		}
	}

	/* give up */
	m = 1;
	if ((!mime || (mime & MAGIC_MIME_TYPE)) &&
	    file_printf(ms, mime ? "application/octet-stream" : "data") == -1) {
	    rv = -1;
	}
 done:
	if ((ms->flags & MAGIC_MIME_ENCODING) != 0) {
		if (ms->flags & MAGIC_MIME_TYPE)
			if (file_printf(ms, "; charset=") == -1)
				rv = -1;
		if (file_printf(ms, "%s", code_mime) == -1)
			rv = -1;
	}
	free(u8buf);
	if (rv)
		return rv;

	return m;
}