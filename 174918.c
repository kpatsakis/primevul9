read_data_stored(struct archive_read *a, const void **buff, size_t *size,
                 int64_t *offset)
{
  struct rar *rar;
  ssize_t bytes_avail;

  rar = (struct rar *)(a->format->data);
  if (rar->bytes_remaining == 0 &&
    !(rar->main_flags & MHD_VOLUME && rar->file_flags & FHD_SPLIT_AFTER))
  {
    *buff = NULL;
    *size = 0;
    *offset = rar->offset;
    if (rar->file_crc != rar->crc_calculated) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "File CRC error");
      return (ARCHIVE_FATAL);
    }
    rar->entry_eof = 1;
    return (ARCHIVE_EOF);
  }

  *buff = rar_read_ahead(a, 1, &bytes_avail);
  if (bytes_avail <= 0)
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Truncated RAR file data");
    return (ARCHIVE_FATAL);
  }

  *size = bytes_avail;
  *offset = rar->offset;
  rar->offset += bytes_avail;
  rar->offset_seek += bytes_avail;
  rar->bytes_remaining -= bytes_avail;
  rar->bytes_unconsumed = bytes_avail;
  /* Calculate File CRC. */
  rar->crc_calculated = crc32(rar->crc_calculated, *buff,
    (unsigned)bytes_avail);
  return (ARCHIVE_OK);
}