check_string (uint32_t option, char *buf, uint32_t len, uint32_t maxlen,
              const char *name)
{
  if (len > NBD_MAX_STRING || len > maxlen) {
    nbdkit_error ("%s: %s too long", name_of_nbd_opt (option), name);
    return -1;
  }
  if (strnlen (buf, len) != len) {
    nbdkit_error ("%s: %s may not include NUL bytes",
                  name_of_nbd_opt (option), name);
    return -1;
  }
  /* TODO: Check for valid UTF-8? */
  return 0;
}