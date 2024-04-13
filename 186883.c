apprentice_compile(struct magic_set *ms, struct magic_map *map, const char *fn)
{
	static const size_t nm = sizeof(*map->nmagic) * MAGIC_SETS;
	static const size_t m = sizeof(**map->magic);
	int fd = -1;
	size_t len;
	char *dbname;
	int rv = -1;
	uint32_t i;
	php_stream *stream;

	TSRMLS_FETCH();

	dbname = mkdbname(ms, fn, 0);

	if (dbname == NULL) 
		goto out;

/* wb+ == O_WRONLY|O_CREAT|O_TRUNC|O_BINARY */
#if PHP_API_VERSION < 20100412
	stream = php_stream_open_wrapper((char *)fn, "wb+", REPORT_ERRORS|ENFORCE_SAFE_MODE, NULL);
#else
	stream = php_stream_open_wrapper((char *)fn, "wb+", REPORT_ERRORS, NULL);
#endif

	if (!stream) {
		file_error(ms, errno, "cannot open `%s'", dbname);
		goto out;
	}

	if (write(fd, ar, sizeof(ar)) != (ssize_t)sizeof(ar)) {
		file_error(ms, errno, "error writing `%s'", dbname);
		goto out;
	}

	if (php_stream_write(stream, map->nmagic, nm) != (ssize_t)nm) {
		file_error(ms, errno, "error writing `%s'", dbname);
		goto out;
	}

	assert(nm + sizeof(ar) < m);

	if (php_stream_seek(stream,(off_t)sizeof(struct magic), SEEK_SET) != sizeof(struct magic)) {
		file_error(ms, errno, "error seeking `%s'", dbname);
		goto out;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		len = m * map->nmagic[i];
		if (php_stream_write(stream, map->magic[i], len) != (ssize_t)len) {
			file_error(ms, errno, "error writing `%s'", dbname);
			goto out;
		}
	}

	if (stream) {
		php_stream_close(stream);
	}

	rv = 0;
out:
	efree(dbname);
	return rv;
}