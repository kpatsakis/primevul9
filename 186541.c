_dbus_close_all (void)
{
  act_on_fds_3_and_up (close_ignore_error);
}