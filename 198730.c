flatpak_run_gc_ids (void)
{
  g_autofree char *base_dir = g_build_filename (g_get_user_runtime_dir (), ".flatpak", NULL);
  g_auto(GLnxDirFdIterator) iter = { 0 };
  struct dirent *dent;

  /* Clean up unused instances */
  if (!glnx_dirfd_iterator_init_at (AT_FDCWD, base_dir, FALSE, &iter, NULL))
    return;

  while (TRUE)
    {
      if (!glnx_dirfd_iterator_next_dent_ensure_dtype (&iter, &dent, NULL, NULL))
        break;

      if (dent == NULL)
        break;

      if (dent->d_type == DT_DIR)
        {
          g_autofree char *ref_file = g_strconcat (dent->d_name, "/.ref", NULL);
          struct stat statbuf;
          struct flock l = {
            .l_type = F_WRLCK,
            .l_whence = SEEK_SET,
            .l_start = 0,
            .l_len = 0
          };
          glnx_autofd int lock_fd = openat (iter.fd, ref_file, O_RDWR | O_CLOEXEC);
          if (lock_fd != -1 &&
              fstat (lock_fd, &statbuf) == 0 &&
              /* Only gc if created at least 3 secs ago, to work around race mentioned in flatpak_run_allocate_id() */
              statbuf.st_mtime + 3 < time (NULL) &&
              fcntl (lock_fd, F_GETLK, &l) == 0 &&
              l.l_type == F_UNLCK)
            {
              /* The instance is not used, remove it */
              g_debug ("Cleaning up unused container id %s", dent->d_name);
              glnx_shutil_rm_rf_at (iter.fd, dent->d_name, NULL, NULL);
            }
        }
    }
}