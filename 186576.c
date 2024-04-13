_dbus_restore_socket_errno (int saved_errno)
{
  errno = saved_errno;
}