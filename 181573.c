guestfs___check_for_filesystem_on (guestfs_h *g, const char *mountable)
{
  CLEANUP_FREE char *vfs_type = NULL;
  int is_swap, r;
  struct inspect_fs *fs;
  CLEANUP_FREE_INTERNAL_MOUNTABLE struct guestfs_internal_mountable *m = NULL;
  int whole_device = 0;

  /* Get vfs-type in order to check if it's a Linux(?) swap device.
   * If there's an error we should ignore it, so to do that we have to
   * temporarily replace the error handler with a null one.
   */
  guestfs_push_error_handler (g, NULL, NULL);
  vfs_type = guestfs_vfs_type (g, mountable);
  guestfs_pop_error_handler (g);

  is_swap = vfs_type && STREQ (vfs_type, "swap");
  debug (g, "check_for_filesystem_on: %s (%s)",
         mountable, vfs_type ? vfs_type : "failed to get vfs type");

  if (is_swap) {
    if (extend_fses (g) == -1)
      return -1;
    fs = &g->fses[g->nr_fses-1];
    fs->mountable = safe_strdup (g, mountable);
    return 0;
  }

  m = guestfs_internal_parse_mountable (g, mountable);
  if (m == NULL)
    return -1;

  /* If it's a whole device, see if it is an install ISO. */
  if (m->im_type == MOUNTABLE_DEVICE) {
    whole_device = guestfs_is_whole_device (g, m->im_device);
    if (whole_device == -1) {
      return -1;
    }
  }

  if (whole_device) {
    if (extend_fses (g) == -1)
      return -1;
    fs = &g->fses[g->nr_fses-1];

    r = guestfs___check_installer_iso (g, fs, m->im_device);
    if (r == -1) {              /* Fatal error. */
      g->nr_fses--;
      return -1;
    }
    if (r > 0)                  /* Found something. */
      return 0;

    /* Didn't find anything.  Fall through ... */
    g->nr_fses--;
  }

  /* Try mounting the device.  As above, ignore errors. */
  guestfs_push_error_handler (g, NULL, NULL);
  if (vfs_type && STREQ (vfs_type, "ufs")) { /* Hack for the *BSDs. */
    /* FreeBSD fs is a variant of ufs called ufs2 ... */
    r = guestfs_mount_vfs (g, "ro,ufstype=ufs2", "ufs", mountable, "/");
    if (r == -1)
      /* while NetBSD and OpenBSD use another variant labeled 44bsd */
      r = guestfs_mount_vfs (g, "ro,ufstype=44bsd", "ufs", mountable, "/");
  } else {
    r = guestfs_mount_ro (g, mountable, "/");
  }
  guestfs_pop_error_handler (g);
  if (r == -1)
    return 0;

  /* Do the rest of the checks. */
  r = check_filesystem (g, mountable, m, whole_device);

  /* Unmount the filesystem. */
  if (guestfs_umount_all (g) == -1)
    return -1;

  return r;
}