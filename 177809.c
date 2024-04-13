allocate_space (int fd, off_t offset, off_t len)
{
  static size_t page_size;

  /* Obtain system page size. */
  if (!page_size)
    page_size = sysconf(_SC_PAGESIZE);

  unsigned char buf[page_size];
  memset (buf, 0, page_size);

  while (len > 0)
    {
      off_t to_write = (len < page_size) ? len : page_size;
      if (write (fd, buf, to_write) < to_write)
        return -1;
      len -= to_write;
    }

  return 0;
}