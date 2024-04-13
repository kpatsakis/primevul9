_dbus_get_is_errno_eagain_or_ewouldblock (int e)
{
  /* Avoid the -Wlogical-op GCC warning, which can be triggered when EAGAIN and
   * EWOULDBLOCK are numerically equal, which is permitted as described by
   * errno(3).
   */
#if EAGAIN == EWOULDBLOCK
  return e == EAGAIN;
#else
  return e == EAGAIN || e == EWOULDBLOCK;
#endif
}