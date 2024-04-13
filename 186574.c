_dbus_fd_set_close_on_exec (int fd)
{
  int val;

  val = fcntl (fd, F_GETFD, 0);

  if (val < 0)
    return;

  val |= FD_CLOEXEC;

  fcntl (fd, F_SETFD, val);
}