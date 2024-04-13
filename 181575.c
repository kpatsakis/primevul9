guestfs___parse_major_minor (guestfs_h *g, struct inspect_fs *fs)
{
  char *major, *minor;

  if (match2 (g, fs->product_name, re_major_minor, &major, &minor)) {
    fs->major_version = guestfs___parse_unsigned_int (g, major);
    free (major);
    if (fs->major_version == -1) {
      free (minor);
      return -1;
    }
    fs->minor_version = guestfs___parse_unsigned_int (g, minor);
    free (minor);
    if (fs->minor_version == -1)
      return -1;
  }
  return 0;
}