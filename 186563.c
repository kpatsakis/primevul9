_read_subprocess_line_argv (const char *progpath,
                            dbus_bool_t path_fallback,
                            const char * const *argv,
                            DBusString *result,
                            DBusError  *error)
{
  int result_pipe[2] = { -1, -1 };
  int errors_pipe[2] = { -1, -1 };
  pid_t pid;
  int ret;
  int status;
  int orig_len;

  dbus_bool_t retval;
  sigset_t new_set, old_set;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  retval = FALSE;

  /* We need to block any existing handlers for SIGCHLD temporarily; they
   * will cause waitpid() below to fail.
   * https://bugs.freedesktop.org/show_bug.cgi?id=21347
   */
  sigemptyset (&new_set);
  sigaddset (&new_set, SIGCHLD);
  sigprocmask (SIG_BLOCK, &new_set, &old_set);

  orig_len = _dbus_string_get_length (result);

#define READ_END        0
#define WRITE_END       1
  if (pipe (result_pipe) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to create a pipe to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to create a pipe to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }
  if (pipe (errors_pipe) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to create a pipe to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to create a pipe to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }

  /* Make sure our output buffers aren't redundantly printed by both the
   * parent and the child */
  fflush (stdout);
  fflush (stderr);

  pid = fork ();
  if (pid < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to fork() to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to fork() to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }

  if (pid == 0)
    {
      /* child process */
      const char *error_str;

      if (!_dbus_ensure_standard_fds (DBUS_FORCE_STDIN_NULL, &error_str))
        {
          int saved_errno = errno;

          /* Try to write details into the pipe, but don't bother
           * trying too hard (no retry loop). */

          if (write (errors_pipe[WRITE_END], error_str, strlen (error_str)) < 0 ||
              write (errors_pipe[WRITE_END], ": ", 2) < 0)
            {
              /* ignore, not much we can do */
            }

          error_str = _dbus_strerror (saved_errno);

          if (write (errors_pipe[WRITE_END], error_str, strlen (error_str)) < 0)
            {
              /* ignore, not much we can do */
            }

          _exit (1);
        }

      /* set-up stdXXX */
      close (result_pipe[READ_END]);
      close (errors_pipe[READ_END]);

      if (dup2 (result_pipe[WRITE_END], 1) == -1) /* setup stdout */
        _exit (1);
      if (dup2 (errors_pipe[WRITE_END], 2) == -1) /* setup stderr */
        _exit (1);

      _dbus_close_all ();

      sigprocmask (SIG_SETMASK, &old_set, NULL);

      /* If it looks fully-qualified, try execv first */
      if (progpath[0] == '/')
        {
          execv (progpath, (char * const *) argv);
          /* Ok, that failed.  Now if path_fallback is given, let's
           * try unqualified.  This is mostly a hack to work
           * around systems which ship dbus-launch in /usr/bin
           * but everything else in /bin (because dbus-launch
           * depends on X11).
           */
          if (path_fallback)
            /* We must have a slash, because we checked above */
            execvp (strrchr (progpath, '/')+1, (char * const *) argv);
        }
      else
        execvp (progpath, (char * const *) argv);

      /* still nothing, we failed */
      _exit (1);
    }

  /* parent process */
  close (result_pipe[WRITE_END]);
  close (errors_pipe[WRITE_END]);
  result_pipe[WRITE_END] = -1;
  errors_pipe[WRITE_END] = -1;

  ret = 0;
  do
    {
      ret = _dbus_read (result_pipe[READ_END], result, 1024);
    }
  while (ret > 0);

  /* reap the child process to avoid it lingering as zombie */
  do
    {
      ret = waitpid (pid, &status, 0);
    }
  while (ret == -1 && errno == EINTR);

  /* We succeeded if the process exited with status 0 and
     anything was read */
  if (!WIFEXITED (status) || WEXITSTATUS (status) != 0 )
    {
      /* The process ended with error */
      DBusString error_message;
      if (!_dbus_string_init (&error_message))
        {
          _DBUS_SET_OOM (error);
          goto out;
        }

      ret = 0;
      do
        {
          ret = _dbus_read (errors_pipe[READ_END], &error_message, 1024);
        }
      while (ret > 0);

      _dbus_string_set_length (result, orig_len);
      if (_dbus_string_get_length (&error_message) > 0)
        dbus_set_error (error, DBUS_ERROR_SPAWN_EXEC_FAILED,
                        "%s terminated abnormally with the following error: %s",
                        progpath, _dbus_string_get_data (&error_message));
      else
        dbus_set_error (error, DBUS_ERROR_SPAWN_EXEC_FAILED,
                        "%s terminated abnormally without any error message",
                        progpath);
      goto out;
    }

  retval = TRUE;

 out:
  sigprocmask (SIG_SETMASK, &old_set, NULL);

  _DBUS_ASSERT_ERROR_XOR_BOOL (error, retval);

  if (result_pipe[0] != -1)
    close (result_pipe[0]);
  if (result_pipe[1] != -1)
    close (result_pipe[1]);
  if (errors_pipe[0] != -1)
    close (errors_pipe[0]);
  if (errors_pipe[1] != -1)
    close (errors_pipe[1]);

  return retval;
}