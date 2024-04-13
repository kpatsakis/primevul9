guestfs___parse_unsigned_int_ignore_trailing (guestfs_h *g, const char *str)
{
  long ret;
  int r = xstrtol (str, NULL, 10, &ret, NULL);
  if (r != LONGINT_OK) {
    error (g, _("could not parse integer in version number: %s"), str);
    return -1;
  }
  return ret;
}