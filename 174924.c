archive_read_format_rar_options(struct archive_read *a,
    const char *key, const char *val)
{
  struct rar *rar;
  int ret = ARCHIVE_FAILED;

  rar = (struct rar *)(a->format->data);
  if (strcmp(key, "hdrcharset")  == 0) {
    if (val == NULL || val[0] == 0)
      archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
          "rar: hdrcharset option needs a character-set name");
    else {
      rar->opt_sconv =
          archive_string_conversion_from_charset(
              &a->archive, val, 0);
      if (rar->opt_sconv != NULL)
        ret = ARCHIVE_OK;
      else
        ret = ARCHIVE_FATAL;
    }
    return (ret);
  }

  /* Note: The "warn" return is just to inform the options
   * supervisor that we didn't handle it.  It will generate
   * a suitable error if no one used this option. */
  return (ARCHIVE_WARN);
}