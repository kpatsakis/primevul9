_dbus_connect_exec (const char     *path,
                    char *const    argv[],
                    DBusError      *error)
{
  int fds[2];
  pid_t pid;
  int retval;
  dbus_bool_t cloexec_done = 0;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _dbus_verbose ("connecting to process %s\n", path);

#ifdef SOCK_CLOEXEC
  retval = socketpair (AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0, fds);
  cloexec_done = (retval >= 0);

  if (retval < 0 && (errno == EINVAL || errno == EPROTOTYPE))
#endif
    {
      retval = socketpair (AF_UNIX, SOCK_STREAM, 0, fds);
    }

  if (retval < 0)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (errno),
                      "Failed to create socket pair: %s",
                      _dbus_strerror (errno));
      return -1;
    }

  if (!cloexec_done)
    {
      _dbus_fd_set_close_on_exec (fds[0]);
      _dbus_fd_set_close_on_exec (fds[1]);
    }

  /* Make sure our output buffers aren't redundantly printed by both the
   * parent and the child */
  fflush (stdout);
  fflush (stderr);

  pid = fork ();
  if (pid < 0)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (errno),
                      "Failed to fork() to call %s: %s",
                      path, _dbus_strerror (errno));
      close (fds[0]);
      close (fds[1]);
      return -1;
    }

  if (pid == 0)
    {
      /* child */
      close (fds[0]);

      dup2 (fds[1], STDIN_FILENO);
      dup2 (fds[1], STDOUT_FILENO);

      if (fds[1] != STDIN_FILENO &&
          fds[1] != STDOUT_FILENO)
        close (fds[1]);

      /* Inherit STDERR and the controlling terminal from the
         parent */

      _dbus_close_all ();

      execvp (path, (char * const *) argv);

      fprintf (stderr, "Failed to execute process %s: %s\n", path, _dbus_strerror (errno));

      _exit(1);
    }

  /* parent */
  close (fds[1]);

  if (!_dbus_set_fd_nonblocking (fds[0], error))
    {
      _DBUS_ASSERT_ERROR_IS_SET (error);

      close (fds[0]);
      return -1;
    }

  return fds[0];
}