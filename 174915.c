ppmd_read(void *p)
{
  struct archive_read *a = ((IByteIn*)p)->a;
  struct rar *rar = (struct rar *)(a->format->data);
  struct rar_br *br = &(rar->br);
  Byte b;
  if (!rar_br_read_ahead(a, br, 8))
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Truncated RAR file data");
    rar->valid = 0;
    return 0;
  }
  b = rar_br_bits(br, 8);
  rar_br_consume(br, 8);
  return b;
}