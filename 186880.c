uncompressgzipped(struct magic_set *ms, const unsigned char *old,
    unsigned char **newch, size_t n)
{
	unsigned char flg = old[3];
	size_t data_start = 10;
	z_stream z;
	int rc;

	if (flg & FEXTRA) {
		if (data_start+1 >= n)
			return 0;
		data_start += 2 + old[data_start] + old[data_start + 1] * 256;
	}
	if (flg & FNAME) {
		while(data_start < n && old[data_start])
			data_start++;
		data_start++;
	}
	if(flg & FCOMMENT) {
		while(data_start < n && old[data_start])
			data_start++;
		data_start++;
	}
	if(flg & FHCRC)
		data_start += 2;

	if (data_start >= n)
		return 0;
	if ((*newch = CAST(unsigned char *, emalloc(HOWMANY + 1))) == NULL) {
		return 0;
	}
	
	/* XXX: const castaway, via strchr */
	z.next_in = (Bytef *)strchr((const char *)old + data_start,
	    old[data_start]);
	z.avail_in = CAST(uint32_t, (n - data_start));
	z.next_out = *newch;
	z.avail_out = HOWMANY;
	z.zalloc = Z_NULL;
	z.zfree = Z_NULL;
	z.opaque = Z_NULL;

	/* LINTED bug in header macro */
	rc = inflateInit2(&z, -15);
	if (rc != Z_OK) {
		file_error(ms, 0, "zlib: %s", z.msg);
		return 0;
	}

	rc = inflate(&z, Z_SYNC_FLUSH);
	if (rc != Z_OK && rc != Z_STREAM_END) {
		file_error(ms, 0, "zlib: %s", z.msg);
		return 0;
	}

	n = (size_t)z.total_out;
	(void)inflateEnd(&z);
	
	/* let's keep the nul-terminate tradition */
	(*newch)[n] = '\0';

	return n;
}