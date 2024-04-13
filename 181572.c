guestfs___is_dir_nocase (guestfs_h *g, const char *path)
{
  CLEANUP_FREE char *p = NULL;
  int r;

  p = guestfs___case_sensitive_path_silently (g, path);
  if (!p)
    return 0;
  r = guestfs_is_dir (g, p);
  return r > 0;
}