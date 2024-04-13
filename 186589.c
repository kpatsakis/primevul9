_dbus_disable_sigpipe (void)
{
  signal (SIGPIPE, SIG_IGN);
}