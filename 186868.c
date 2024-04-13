file_zmagic(struct magic_set *ms, int fd, const char *name,
    const unsigned char *buf, size_t nbytes)
{
	unsigned char *newbuf = NULL;
	size_t i, nsz;
	int rv = 0;
	int mime = ms->flags & MAGIC_MIME;
	size_t ncompr;

	if ((ms->flags & MAGIC_COMPRESS) == 0)
		return 0;

	ncompr = sizeof(compr) / sizeof(compr[0]);

	for (i = 0; i < ncompr; i++) {
		if (nbytes < compr[i].maglen)
			continue;
		if (memcmp(buf, compr[i].magic, compr[i].maglen) == 0 &&
		    (nsz = uncompressbuf(ms, fd, i, buf, &newbuf,
		    nbytes)) != NODATA) {
			ms->flags &= ~MAGIC_COMPRESS;
			rv = -1;
			if (file_buffer(ms, -1, name, newbuf, nsz) == -1)
				goto error;

			if (mime == MAGIC_MIME || mime == 0) {
				if (file_printf(ms, mime ?
				    " compressed-encoding=" : " (") == -1)
					goto error;
			}

			if ((mime == 0 || mime & MAGIC_MIME_ENCODING) &&
			    file_buffer(ms, -1, NULL, buf, nbytes) == -1)
				goto error;

			if (!mime && file_printf(ms, ")") == -1)
				goto error;
			rv = 1;
			break;
		}
	}
error:
	if (newbuf)
		efree(newbuf);
	ms->flags |= MAGIC_COMPRESS;
	return rv;
}