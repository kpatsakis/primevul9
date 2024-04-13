copy_from_lzss_window(struct archive_read *a, const void **buffer,
                        int64_t startpos, int length)
{
  int windowoffs, firstpart;
  struct rar *rar = (struct rar *)(a->format->data);

  if (!rar->unp_buffer)
  {
    if ((rar->unp_buffer = malloc(rar->unp_buffer_size)) == NULL)
    {
      archive_set_error(&a->archive, ENOMEM,
                        "Unable to allocate memory for uncompressed data.");
      return (ARCHIVE_FATAL);
    }
  }

  windowoffs = lzss_offset_for_position(&rar->lzss, startpos);
  if(windowoffs + length <= lzss_size(&rar->lzss)) {
    memcpy(&rar->unp_buffer[rar->unp_offset], &rar->lzss.window[windowoffs],
           length);
  } else if (length <= lzss_size(&rar->lzss)) {
    firstpart = lzss_size(&rar->lzss) - windowoffs;
    if (firstpart < 0) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "Bad RAR file data");
      return (ARCHIVE_FATAL);
    }
    if (firstpart < length) {
      memcpy(&rar->unp_buffer[rar->unp_offset],
             &rar->lzss.window[windowoffs], firstpart);
      memcpy(&rar->unp_buffer[rar->unp_offset + firstpart],
             &rar->lzss.window[0], length - firstpart);
    } else {
      memcpy(&rar->unp_buffer[rar->unp_offset],
             &rar->lzss.window[windowoffs], length);
    }
  } else {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "Bad RAR file data");
      return (ARCHIVE_FATAL);
  }
  rar->unp_offset += length;
  if (rar->unp_offset >= rar->unp_buffer_size)
    *buffer = rar->unp_buffer;
  else
    *buffer = NULL;
  return (ARCHIVE_OK);
}