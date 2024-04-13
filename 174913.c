read_symlink_stored(struct archive_read *a, struct archive_entry *entry,
                    struct archive_string_conv *sconv)
{
  const void *h;
  const char *p;
  struct rar *rar;
  int ret = (ARCHIVE_OK);

  rar = (struct rar *)(a->format->data);
  if ((h = rar_read_ahead(a, (size_t)rar->packed_size, NULL)) == NULL)
    return (ARCHIVE_FATAL);
  p = h;

  if (archive_entry_copy_symlink_l(entry,
      p, (size_t)rar->packed_size, sconv))
  {
    if (errno == ENOMEM)
    {
      archive_set_error(&a->archive, ENOMEM,
                        "Can't allocate memory for link");
      return (ARCHIVE_FATAL);
    }
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "link cannot be converted from %s to current locale.",
                      archive_string_conversion_charset_name(sconv));
    ret = (ARCHIVE_WARN);
  }
  __archive_read_consume(a, rar->packed_size);
  return ret;
}