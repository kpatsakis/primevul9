check_filesystem (guestfs_h *g, const char *mountable,
                  const struct guestfs_internal_mountable *m,
                  int whole_device)
{
  if (extend_fses (g) == -1)
    return -1;

  int partnum = -1;
  if (!whole_device && m->im_type == MOUNTABLE_DEVICE) {
    guestfs_push_error_handler (g, NULL, NULL);
    partnum = guestfs_part_to_partnum (g, m->im_device);
    guestfs_pop_error_handler (g);
  }

  struct inspect_fs *fs = &g->fses[g->nr_fses-1];

  fs->mountable = safe_strdup (g, mountable);

  /* Optimize some of the tests by avoiding multiple tests of the same thing. */
  int is_dir_etc = guestfs_is_dir (g, "/etc") > 0;
  int is_dir_bin = guestfs_is_dir (g, "/bin") > 0;
  int is_dir_share = guestfs_is_dir (g, "/share") > 0;

  /* Grub /boot? */
  if (guestfs_is_file (g, "/grub/menu.lst") > 0 ||
      guestfs_is_file (g, "/grub/grub.conf") > 0 ||
      guestfs_is_file (g, "/grub2/grub.cfg") > 0)
    ;
  /* FreeBSD root? */
  else if (is_dir_etc &&
           is_dir_bin &&
           guestfs_is_file (g, "/etc/freebsd-update.conf") > 0 &&
           guestfs_is_file (g, "/etc/fstab") > 0) {
    /* Ignore /dev/sda1 which is a shadow of the real root filesystem
     * that is probably /dev/sda5 (see:
     * http://www.freebsd.org/doc/handbook/disk-organization.html)
     */
    if (m->im_type == MOUNTABLE_DEVICE &&
        match (g, m->im_device, re_first_partition))
      return 0;

    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED;
    if (guestfs___check_freebsd_root (g, fs) == -1)
      return -1;
  }
  else if (is_dir_etc &&
           is_dir_bin &&
           guestfs_is_file (g, "/etc/fstab") > 0 &&
           guestfs_is_file (g, "/etc/release") > 0) {
    /* Ignore /dev/sda1 which is a shadow of the real root filesystem
     * that is probably /dev/sda5 (see:
     * http://www.freebsd.org/doc/handbook/disk-organization.html)
     */
    if (m->im_type == MOUNTABLE_DEVICE &&
        match (g, m->im_device, re_first_partition))
      return 0;

    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED;
    if (guestfs___check_netbsd_root (g, fs) == -1)
      return -1;
  }
  /* Hurd root? */
  else if (guestfs_is_file (g, "/hurd/console") > 0 &&
           guestfs_is_file (g, "/hurd/hello") > 0 &&
           guestfs_is_file (g, "/hurd/null") > 0) {
    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED; /* XXX could be more specific */
    if (guestfs___check_hurd_root (g, fs) == -1)
      return -1;
  }
  /* Linux root? */
  else if (is_dir_etc &&
           (is_dir_bin ||
            (guestfs_is_symlink (g, "/bin") > 0 &&
             guestfs_is_dir (g, "/usr/bin") > 0)) &&
           guestfs_is_file (g, "/etc/fstab") > 0) {
    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED;
    if (guestfs___check_linux_root (g, fs) == -1)
      return -1;
  }
  /* Linux /usr/local? */
  else if (is_dir_etc &&
           is_dir_bin &&
           is_dir_share &&
           guestfs_exists (g, "/local") == 0 &&
           guestfs_is_file (g, "/etc/fstab") == 0)
    ;
  /* Linux /usr? */
  else if (is_dir_etc &&
           is_dir_bin &&
           is_dir_share &&
           guestfs_exists (g, "/local") > 0 &&
           guestfs_is_file (g, "/etc/fstab") == 0)
    ;
  /* Linux /var? */
  else if (guestfs_is_dir (g, "/log") > 0 &&
           guestfs_is_dir (g, "/run") > 0 &&
           guestfs_is_dir (g, "/spool") > 0)
    ;
  /* Windows root? */
  else if (guestfs___has_windows_systemroot (g) >= 0) {
    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED;
    if (guestfs___check_windows_root (g, fs) == -1)
      return -1;
  }
  /* Windows volume with installed applications (but not root)? */
  else if (guestfs___is_dir_nocase (g, "/System Volume Information") > 0 &&
           guestfs___is_dir_nocase (g, "/Program Files") > 0)
    ;
  /* Windows volume (but not root)? */
  else if (guestfs___is_dir_nocase (g, "/System Volume Information") > 0)
    ;
  /* FreeDOS? */
  else if (guestfs___is_dir_nocase (g, "/FDOS") > 0 &&
           guestfs___is_file_nocase (g, "/FDOS/FREEDOS.BSS") > 0) {
    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLED;
    fs->type = OS_TYPE_DOS;
    fs->distro = OS_DISTRO_FREEDOS;
    /* FreeDOS is a mix of 16 and 32 bit, but assume it requires a
     * 32 bit i386 processor.
     */
    fs->arch = safe_strdup (g, "i386");
  }
  /* Install CD/disk?
   *
   * Note that we checked (above) for an install ISO, but there are
   * other types of install image (eg. USB keys) which that check
   * wouldn't have picked up.
   *
   * Skip these checks if it's not a whole device (eg. CD) or the
   * first partition (eg. bootable USB key).
   */
  else if ((whole_device || partnum == 1) &&
           (guestfs_is_file (g, "/isolinux/isolinux.cfg") > 0 ||
            guestfs_is_dir (g, "/EFI/BOOT") > 0 ||
            guestfs_is_file (g, "/images/install.img") > 0 ||
            guestfs_is_dir (g, "/.disk") > 0 ||
            guestfs_is_file (g, "/.discinfo") > 0 ||
            guestfs_is_file (g, "/i386/txtsetup.sif") > 0 ||
            guestfs_is_file (g, "/amd64/txtsetup.sif") > 0 ||
            guestfs_is_file (g, "/freedos/freedos.ico") > 0)) {
    fs->is_root = 1;
    fs->format = OS_FORMAT_INSTALLER;
    if (guestfs___check_installer_root (g, fs) == -1)
      return -1;
  }

  /* The above code should have set fs->type and fs->distro fields, so
   * we can now guess the package management system.
   */
  guestfs___check_package_format (g, fs);
  guestfs___check_package_management (g, fs);

  return 0;
}