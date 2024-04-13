flatpak_run_allocate_id (int *lock_fd_out)
{
  g_autofree char *user_runtime_dir = flatpak_get_real_xdg_runtime_dir ();
  g_autofree char *base_dir = g_build_filename (user_runtime_dir, ".flatpak", NULL);
  int count;

  g_mkdir_with_parents (base_dir, 0755);

  flatpak_run_gc_ids ();

  for (count = 0; count < 1000; count++)
    {
      g_autofree char *instance_id = NULL;
      g_autofree char *instance_dir = NULL;

      instance_id = g_strdup_printf ("%u", g_random_int ());

      instance_dir = g_build_filename (base_dir, instance_id, NULL);

      /* We use an atomic mkdir to ensure the instance id is unique */
      if (mkdir (instance_dir, 0755) == 0)
        {
          g_autofree char *lock_file = g_build_filename (instance_dir, ".ref", NULL);
          glnx_autofd int lock_fd = -1;
          struct flock l = {
            .l_type = F_RDLCK,
            .l_whence = SEEK_SET,
            .l_start = 0,
            .l_len = 0
          };

          /* Then we take a file lock inside the dir, hold that during
           * setup and in bwrap. Anyone trying to clean up unused
           * directories need to first verify that there is a .ref
           * file and take a write lock on .ref to ensure its not in
           * use. */
          lock_fd = open (lock_file, O_RDWR | O_CREAT | O_CLOEXEC, 0644);
          /* There is a tiny race here between the open creating the file and the lock suceeding.
             We work around that by only gc:ing "old" .ref files */
          if (lock_fd != -1 && fcntl (lock_fd, F_SETLK, &l) == 0)
            {
              *lock_fd_out = glnx_steal_fd (&lock_fd);
              g_debug ("Allocated instance id %s", instance_id);
              return g_steal_pointer (&instance_id);
            }
        }
    }

  return NULL;
}