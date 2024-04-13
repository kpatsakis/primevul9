skip_sfx(struct archive_read *a)
{
  const void *h;
  const char *p, *q;
  size_t skip, total;
  ssize_t bytes, window;

  total = 0;
  window = 4096;
  while (total + window <= (1024 * 128)) {
    h = __archive_read_ahead(a, window, &bytes);
    if (h == NULL) {
      /* Remaining bytes are less than window. */
      window >>= 1;
      if (window < 0x40)
      	goto fatal;
      continue;
    }
    if (bytes < 0x40)
      goto fatal;
    p = h;
    q = p + bytes;

    /*
     * Scan ahead until we find something that looks
     * like the RAR header.
     */
    while (p + 7 < q) {
      if (memcmp(p, RAR_SIGNATURE, 7) == 0) {
      	skip = p - (const char *)h;
      	__archive_read_consume(a, skip);
      	return (ARCHIVE_OK);
      }
      p += 0x10;
    }
    skip = p - (const char *)h;
    __archive_read_consume(a, skip);
	total += skip;
  }
fatal:
  archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
      "Couldn't find out RAR header");
  return (ARCHIVE_FATAL);
}