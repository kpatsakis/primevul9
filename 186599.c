_dbus_ensure_standard_fds (DBusEnsureStandardFdsFlags   flags,
                           const char                 **error_str_p)
{
  static int const relevant_flag[] = { DBUS_FORCE_STDIN_NULL,
      DBUS_FORCE_STDOUT_NULL,
      DBUS_FORCE_STDERR_NULL };
  /* Should always get replaced with the real error before use */
  const char *error_str = "Failed mysteriously";
  int devnull = -1;
  int saved_errno;
  /* This function relies on the standard fds having their POSIX values. */
  _DBUS_STATIC_ASSERT (STDIN_FILENO == 0);
  _DBUS_STATIC_ASSERT (STDOUT_FILENO == 1);
  _DBUS_STATIC_ASSERT (STDERR_FILENO == 2);
  int i;

  for (i = STDIN_FILENO; i <= STDERR_FILENO; i++)
    {
      /* Because we rely on being single-threaded, and we want the
       * standard fds to not be close-on-exec, we don't set it
       * close-on-exec. */
      if (devnull < i)
        devnull = open ("/dev/null", O_RDWR);

      if (devnull < 0)
        {
          error_str = "Failed to open /dev/null";
          goto out;
        }

      /* We already opened all fds < i, so the only way this assertion
       * could fail is if another thread closed one, and we document
       * this function as not safe for multi-threading. */
      _dbus_assert (devnull >= i);

      if (devnull != i && (flags & relevant_flag[i]) != 0)
        {
          if (dup2 (devnull, i) < 0)
            {
              error_str = "Failed to dup2 /dev/null onto a standard fd";
              goto out;
            }
        }
    }

  error_str = NULL;

out:
  saved_errno = errno;

  if (devnull > STDERR_FILENO)
    close (devnull);

  if (error_str_p != NULL)
    *error_str_p = error_str;

  errno = saved_errno;
  return (error_str == NULL);
}