_dbus_fd_set_all_close_on_exec (void)
{
  act_on_fds_3_and_up (_dbus_fd_set_close_on_exec);
}