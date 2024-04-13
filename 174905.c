archive_read_format_rar_read_data_skip(struct archive_read *a)
{
  struct rar *rar;
  int64_t bytes_skipped;
  int ret;

  rar = (struct rar *)(a->format->data);

  if (rar->bytes_unconsumed > 0) {
      /* Consume as much as the decompressor actually used. */
      __archive_read_consume(a, rar->bytes_unconsumed);
      rar->bytes_unconsumed = 0;
  }

  if (rar->bytes_remaining > 0) {
    bytes_skipped = __archive_read_consume(a, rar->bytes_remaining);
    if (bytes_skipped < 0)
      return (ARCHIVE_FATAL);
  }

  /* Compressed data to skip must be read from each header in a multivolume
   * archive.
   */
  if (rar->main_flags & MHD_VOLUME && rar->file_flags & FHD_SPLIT_AFTER)
  {
    ret = archive_read_format_rar_read_header(a, a->entry);
    if (ret == (ARCHIVE_EOF))
      ret = archive_read_format_rar_read_header(a, a->entry);
    if (ret != (ARCHIVE_OK))
      return ret;
    return archive_read_format_rar_read_data_skip(a);
  }

  return (ARCHIVE_OK);
}